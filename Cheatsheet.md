# get_next_line — Complete Study Sheet

This document is a **defense-ready explanation** of my `get_next_line` implementation.
It explains the algorithm, the role of each function, memory behavior, and all important
edge cases. This file is meant to be read before evaluations and can be dropped directly
into the repository.

---

## 1. Core concept

`get_next_line()` reads a file descriptor **line by line**, returning **exactly one line**
per call, including the terminating `\n` if it exists.

The function relies on a **static buffer (`stash`)** that persists across calls in order
to store leftover data between reads.

At a high level, each call follows this pipeline:

1. **Read** data from the file descriptor into the stash until a full line exists or EOF is reached.
2. **Extract** one line from the stash and return it.
3. **Clean** the stash so only the remaining unread data is kept for the next call.

---

## 2. High-level execution flow

```c
char *get_next_line(int fd)
{
    static char *stash;
    char        *line;

    if (fd < 0 || BUFFER_SIZE <= 0)
        return (NULL);

    stash = read_to_stash(fd, stash);
    if (!stash)
        return (NULL);

    line = extract_line(stash);
    stash = clean_stash(stash);

    return (line);
}
```

### Key observations
- `stash` is static → it survives between function calls.
- The function always returns **one line max**.
- Memory is allocated only for returned lines and stash updates.

---

## 3. `read_to_stash()` — reading until a line exists

### Purpose
Read from `fd` and append to `stash` until:
- a `\n` is found,
- EOF is reached (`read() == 0`),
- or an error occurs (`read() == -1`).

### Important behaviors

#### Stash initialization
If `stash` is `NULL`, it is initialized to an empty string:
```c
if (!stash)
    stash = ft_strdup("");
```

This guarantees `stash` is always a valid C-string.

#### Read loop condition
```c
while (!ft_strchr(stash, '\n') && n > 0)
```
- Stops early as soon as one line is available.
- Avoids unnecessary reads.

#### Buffer handling
- A buffer of `BUFFER_SIZE + 1` is allocated.
- `read()` fills the buffer.
- `buffer[n] = '\0'` ensures safe string operations.

#### Appending logic
```c
temp = ft_strjoin(stash, buffer);
free(stash);
stash = temp;
```
- Old stash is freed.
- New combined string becomes the stash.

#### Error handling
If `read()` fails:
```c
free(stash);
free(buffer);
return (NULL);
```

This prevents leaks and resets the internal state.

---

## 4. `extract_line()` — returning one line

### Purpose
Allocate and return **exactly one line** from the beginning of `stash`.

### Behavior
- Copies characters until `\n` or `\0`.
- Includes `\n` if present.
- Always null-terminates the returned string.

### Memory allocation
```c
malloc(i + 2);
```
Why `+2`?
- One character for a possible `\n`
- One for `\0`

### Return values
- Returns `NULL` if stash is empty.
- Returns the final line even if it does not end with `\n`.

---

## 5. `clean_stash()` — preserving leftovers

### Purpose
Remove the returned line from `stash` and keep the remaining data for the next call.

### Key cases

#### Case 1: Newline exists
- Allocate a new string for everything **after** the newline.
- Free the old stash.
- Return the new stash.

#### Case 2: No newline found
```c
free(stash);
return (NULL);
```
- The stash is fully consumed.
- Next call starts fresh.

---

## 6. Helper: `free_and_null()`

A small utility used when `read()` fails:
```c
free(stash);
free(buffer);
return (NULL);
```

Ensures no memory leak and consistent error propagation.

---

## 7. Memory management summary

- Every `malloc` has a matching `free`.
- Returned lines are always heap-allocated.
- Stash is freed:
  - on read error,
  - when fully consumed,
  - at program termination.

There are **no intentional leaks**.

---

## 8. Edge cases handled

### Invalid arguments
- `fd < 0` → `NULL`
- `BUFFER_SIZE <= 0` → `NULL`

### Empty file
- First read returns `0`
- `stash` stays empty
- `get_next_line()` returns `NULL` immediately

### File with only `\n`
- Returns `"\n"` once
- Then returns `NULL`

### File without trailing newline
Example: `"abc"`
- Returns `"abc"`
- Next call returns `NULL`

### `BUFFER_SIZE = 1`
- Reads one byte at a time
- Still works correctly

### Very large `BUFFER_SIZE`
- Reads larger chunks
- Fewer concatenations

### Read error (`read() == -1`)
- Frees stash and buffer
- Returns `NULL`
- Internal state is reset

---

## 9. Limitations (by design)

### Single file descriptor
- Uses one static stash
- Does **not** support multiple FDs simultaneously
- This is expected in the mandatory part

### Binary files
- Uses C-string logic (`\0`-terminated)
- Undefined behavior if file contains `\0`

### Performance
- Repeated string joins cause O(n²) behavior in worst cases
- Accepted trade-off for simplicity and correctness

---

## 10. Why this implementation is correct

- Reading stops as soon as a line is complete.
- Each call returns **exactly one line**.
- Leftover data is preserved correctly.
- EOF and error conditions are cleanly handled.

This guarantees correct behavior across all valid inputs required by the subject.

---

## 11. One-sentence defense summary

> “My get_next_line uses a static stash to accumulate reads until a newline or EOF,
> extracts exactly one line per call, and preserves leftovers safely for subsequent calls.”
