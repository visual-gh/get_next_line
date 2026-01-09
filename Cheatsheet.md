# Get Next Line - Complete Study Cheatsheet

## 🎯 The Big Picture

**Goal:** Read a file line by line, one call at a time.

**Challenge:** The `read()` function doesn't read line by line - it reads in chunks (BUFFER_SIZE). We need to:
1. Store leftover data between function calls
2. Accumulate chunks until we find a newline
3. Return one line at a time

**Solution:** Use a **static variable** called `stash` to remember data between calls.

---

## 📊 Program Flow Diagram

```
Call 1: get_next_line(fd)
    ↓
    Read chunks → Stash: "Hello\nWo"
    ↓
    Extract: "Hello\n"
    ↓
    Clean stash: "Wo"
    ↓
    Return: "Hello\n"

Call 2: get_next_line(fd)
    ↓
    Read chunks → Stash: "World\nGo"
    ↓
    Extract: "World\n"
    ↓
    Clean stash: "Go"
    ↓
    Return: "World\n"

Call 3: get_next_line(fd)
    ↓
    Read chunks → Stash: "Goodbye" (EOF)
    ↓
    Extract: "Goodbye"
    ↓
    Clean stash: NULL (no more data)
    ↓
    Return: "Goodbye"

Call 4: get_next_line(fd)
    ↓
    Stash is empty
    ↓
    Return: NULL (end of file)
```

---

## 🔑 Key Concepts You Need to Know

### **1. Static Variables**
```c
static char *stash;
```
- **Lives between function calls** (unlike normal variables)
- **Initialized to NULL** automatically
- **One per file descriptor** (in basic version)
- **Retains its value** when function returns

**Example:**
```c
void count_calls(void)
{
    static int counter = 0;  // Only initialized ONCE
    counter++;
    printf("Call #%d\n", counter);
}

count_calls();  // Prints: Call #1
count_calls();  // Prints: Call #2
count_calls();  // Prints: Call #3
```

---

### **2. The read() Function**
```c
ssize_t read(int fd, void *buf, size_t count);
```

**Parameters:**
- `fd` - File descriptor (which file to read)
- `buf` - Buffer to store data
- `count` - Maximum bytes to read

**Return value:**
- `n > 0` - Successfully read n bytes
- `n = 0` - EOF (end of file)
- `n < 0` - Error occurred

**Key behavior:** It doesn't read lines, it reads BYTES!

**Example:**
```c
char buffer[10];
int n = read(fd, buffer, 10);
// Might read: "Hello\nWor" (10 bytes)
// Does NOT stop at \n!
```

---

### **3. Pointer Arithmetic**
```c
stash + i + 1
```

**What this means:**
- Start at `stash`
- Move forward `i + 1` positions
- Return pointer to that position

**Example:**
```c
char *str = "Hello\nWorld";
//            012345 678901
char *ptr = str + 6;
printf("%s", ptr);  // Prints: "World"
```

---

### **4. The Comma Operator**
```c
return (free(stash), NULL);
```

**Executes left to right:**
1. Execute `free(stash)`
2. Return `NULL`

**Same as:**
```c
free(stash);
return (NULL);
```

---

## 📝 Function-by-Function Breakdown

---

## FUNCTION 1: `get_next_line()`
### The Main Controller

```c
char *get_next_line(int fd)
{
    static char *stash;
    char        *line;

    if (fd < 0 || BUFFER_SIZE <= 0)
        return (NULL);
    if (!stash)
        stash = ft_strdup("");
    stash = read_to_stash(fd, stash);
    if (!stash)
        return (NULL);
    line = extract_line(stash);
    stash = clean_stash(stash);
    return (line);
}
```

### Step-by-Step:

**Step 1: Validate input**
```c
if (fd < 0 || BUFFER_SIZE <= 0)
    return (NULL);
```
- Check for bad file descriptor (negative)
- Check for invalid buffer size (0 or negative)

---

**Step 2: Initialize stash on first call**
```c
if (!stash)
    stash = ft_strdup("");
```
- First call: `stash` is NULL (static initialization)
- Create empty string to start accumulating data
- **Why empty string?** So `ft_strchr` and `ft_strjoin` work on first read

---

**Step 3: Read and accumulate data**
```c
stash = read_to_stash(fd, stash);
if (!stash)
    return (NULL);
```
- Read chunks from file
- Append to stash until we find `\n` or reach EOF
- If error, return NULL

---

**Step 4: Extract one line**
```c
line = extract_line(stash);
```
- Take everything up to (and including) `\n`
- Or take everything if no `\n` found (last line)

---

**Step 5: Clean up stash**
```c
stash = clean_stash(stash);
```
- Keep only the leftover data after the extracted line
- If nothing left, stash becomes NULL

---

**Step 6: Return the line**
```c
return (line);
```
- User gets one complete line
- Stash keeps the rest for next call

---

## FUNCTION 2: `read_to_stash()`
### The Data Accumulator

```c
static char *read_to_stash(int fd, char *stash)
{
    char    *buffer;
    char    *temp;
    ssize_t n;

    buffer = malloc(BUFFER_SIZE + 1);
    if (!buffer)
        return (free(stash), NULL);
    n = 1;
    while (!ft_strchr(stash, '\n') && n > 0)
    {
        n = read(fd, buffer, BUFFER_SIZE);
        if (n < 0)
            return (free(stash), free(buffer), NULL);
        buffer[n] = '\0';
        temp = ft_strjoin(stash, buffer);
        free(stash);
        stash = temp;
        if (!stash)
            return (free(buffer), NULL);
    }
    return (free(buffer), stash);
}
```

### Step-by-Step:

**Step 1: Allocate temporary buffer**
```c
buffer = malloc(BUFFER_SIZE + 1);
if (!buffer)
    return (free(stash), NULL);
```
- Create space for reading chunks
- `+1` for null terminator
- If malloc fails, clean up existing stash

---

**Step 2: Initialize read counter**
```c
n = 1;
```
- Set to 1 to enter the while loop
- Will be overwritten by actual read() result

---

**Step 3: Read until newline found or EOF**
```c
while (!ft_strchr(stash, '\n') && n > 0)
```
- **Condition 1:** `!ft_strchr(stash, '\n')` - No newline in stash yet
- **Condition 2:** `n > 0` - Last read was successful
- **Loop continues:** Keep reading until we find `\n` or hit EOF

---

**Step 4: Read chunk from file**
```c
n = read(fd, buffer, BUFFER_SIZE);
if (n < 0)
    return (free(stash), free(buffer), NULL);
```
- Read up to BUFFER_SIZE bytes
- Store in buffer
- If error (n < 0), clean up and return NULL

---

**Step 5: Null-terminate buffer**
```c
buffer[n] = '\0';
```
- **CRITICAL:** `read()` doesn't add `\0`
- We need it for string functions
- Place it right after the bytes we read

---

**Step 6: Append to stash (Memory Dance)**
```c
temp = ft_strjoin(stash, buffer);  // Create: old + new
free(stash);                        // Free: old
stash = temp;                       // Keep: new combined
if (!stash)
    return (free(buffer), NULL);
```

**Visual:**
```
Before:
stash  → "Hello"
buffer → "Wor"

After ft_strjoin:
temp   → "HelloWor"

After free(stash):
stash  → (freed)

After stash = temp:
stash  → "HelloWor"
```

---

**Step 7: Clean up and return**
```c
return (free(buffer), stash);
```
- Free temporary buffer (no longer needed)
- Return accumulated stash

---

### **Real Example Trace:**

**File content:** "Hi\nBye" with BUFFER_SIZE=2

**First call to read_to_stash:**
```
Initial: stash = ""

Loop 1:
  read(fd, buffer, 2) → buffer = "Hi", n = 2
  buffer[2] = '\0'
  temp = "" + "Hi" = "Hi"
  stash = "Hi"
  No \n found, continue

Loop 2:
  read(fd, buffer, 2) → buffer = "\nB", n = 2
  buffer[2] = '\0'
  temp = "Hi" + "\nB" = "Hi\nB"
  stash = "Hi\nB"
  \n found! Exit loop

Return: "Hi\nB"
```

---

## FUNCTION 3: `extract_line()`
### The Line Extractor

```c
static char *extract_line(char *stash)
{
    char    *line;
    size_t  i;

    if (!stash || !stash[0])
        return (NULL);
    i = 0;
    while (stash[i] && stash[i] != '\n')
        i++;
    line = malloc(i + 2);
    if (!line)
        return (NULL);
    i = 0;
    while (stash[i] && stash[i] != '\n')
    {
        line[i] = stash[i];
        i++;
    }
    if (stash[i] == '\n')
        line[i++] = '\n';
    line[i] = '\0';
    return (line);
}
```

### Step-by-Step:

**Step 1: Safety checks**
```c
if (!stash || !stash[0])
    return (NULL);
```
- `!stash` - Stash is NULL (shouldn't happen, defensive)
- `!stash[0]` - Stash is empty string (EOF reached)

---

**Step 2: Count to newline**
```c
i = 0;
while (stash[i] && stash[i] != '\n')
    i++;
```
**Example:**
```
stash = "Hello\nWorld"
         01234 5

After loop: i = 5
```

---

**Step 3: Allocate line**
```c
line = malloc(i + 2);
```
- `i` characters before newline
- `+1` for potential `\n`
- `+1` for `\0`

**Example:** For "Hello\n":
- i = 5
- malloc(7) = space for "Hello\n\0"

---

**Step 4: Reset counter and copy**
```c
i = 0;
while (stash[i] && stash[i] != '\n')
{
    line[i] = stash[i];
    i++;
}
```
**Why reset i?** We already used it for counting, now use it for indexing.

**Copy process:**
```
stash: "Hello\nWorld"
line:  "Hello"
       01234
```

---

**Step 5: Include newline if present**
```c
if (stash[i] == '\n')
    line[i++] = '\n';
```
**Post-increment magic:**
- Copy `\n` to `line[i]`
- Then increment `i`

---

**Step 6: Null terminate and return**
```c
line[i] = '\0';
return (line);
```
Final result: `"Hello\n\0"` or `"Hello\0"` (if no newline)

---

## FUNCTION 4: `clean_stash()`
### The Leftover Keeper

```c
static char *clean_stash(char *stash)
{
    char    *new;
    size_t  i;

    i = 0;
    while (stash[i] && stash[i] != '\n')
        i++;
    if (!stash[i])
        return (free(stash), NULL);
    new = ft_strdup(stash + i + 1);
    free(stash);
    return (new);
}
```

### Step-by-Step:

**Step 1: Find the newline**
```c
i = 0;
while (stash[i] && stash[i] != '\n')
    i++;
```
**Example:**
```
stash = "Hello\nWorld"
         01234 5

After loop: i = 5 (at '\n')
```

---

**Step 2: Check if newline exists**
```c
if (!stash[i])
    return (free(stash), NULL);
```
- If `stash[i]` is `\0`, we didn't find `\n`
- This means we returned the last line
- Nothing left to keep, free everything

---

**Step 3: Duplicate remainder**
```c
new = ft_strdup(stash + i + 1);
```

**Pointer arithmetic breakdown:**
```
stash     → "Hello\nWorld"
             01234 5 678901

stash + 5 → points to '\n'
stash + 5 + 1 → points to 'W'

ft_strdup("World") → creates new string "World"
```

---

**Step 4: Clean up and return**
```c
free(stash);
return (new);
```
- Free old stash (already extracted)
- Return new stash (for next call)

---

## 🎮 Complete Execution Example

**File:** "AB\nCD\nE"
**BUFFER_SIZE:** 2

### Call 1: `get_next_line(fd)`

1. **Initialize:** `stash = ""`
2. **read_to_stash:**
   - Read "AB" → stash = "AB"
   - Read "\nC" → stash = "AB\nC" (found \n, stop)
3. **extract_line:** Return "AB\n"
4. **clean_stash:** stash = "C"
5. **Return:** "AB\n"

### Call 2: `get_next_line(fd)`

1. **stash = "C"** (from previous call)
2. **read_to_stash:**
   - Read "D\n" → stash = "CD\n" (found \n, stop)
3. **extract_line:** Return "CD\n"
4. **clean_stash:** stash = ""
5. **Return:** "CD\n"

### Call 3: `get_next_line(fd)`

1. **stash = ""** (from previous call)
2. **read_to_stash:**
   - Read "E" → stash = "E"
   - Read 0 bytes (EOF) → stop
3. **extract_line:** Return "E"
4. **clean_stash:** stash = NULL (no \n found)
5. **Return:** "E"

### Call 4: `get_next_line(fd)`

1. **stash = NULL** (from previous call)
2. **Initialize:** stash = ""
3. **read_to_stash:** Read 0 bytes (EOF) → stash = ""
4. **extract_line:** stash[0] is '\0' → Return NULL
5. **Return:** NULL

---

## 🐛 Common Pitfalls & Why They Don't Happen

### **1. Memory Leaks**
❌ **Problem:** Forgetting to free allocated memory
✅ **Solution:** Every malloc has a corresponding free:
- `buffer` freed in `read_to_stash` before return
- Old `stash` freed before replacing with `temp`
- `stash` freed in `clean_stash` before return

---

### **2. Use-After-Free**
❌ **Problem:** Using memory after freeing it
✅ **Solution:**
```c
temp = ft_strjoin(stash, buffer);
free(stash);           // Free old
stash = temp;          // Immediately reassign
```
Never use `stash` between free and reassignment!

---

### **3. Reading Past Buffer**
❌ **Problem:** String functions reading garbage
✅ **Solution:**
```c
buffer[n] = '\0';  // Always null-terminate after read()
```

---

### **4. Undefined Behavior**
❌ **Problem:** `line[i] = stash[i++]` - modifying i twice
✅ **Solution:** Use separate statements:
```c
line[i] = stash[i];
i++;
```

---

## 🧠 Memory Map Visualization

```
STACK:
┌─────────────────────┐
│ get_next_line       │
│   stash (static) ──────────┐
│   line           ──────┐   │
└─────────────────────┘  │   │
                         │   │
HEAP:                    │   │
┌─────────────────────┐  │   │
│ "Hello\nWorld"  ◄───────┘   │ (pointed by stash)
└─────────────────────┘      │
┌─────────────────────┐      │
│ "Hello\n"       ◄───────────┘ (pointed by line)
└─────────────────────┘
```

---

## 📌 Quick Reference

### **When to free:**
- ✅ `buffer` in `read_to_stash` before every return
- ✅ Old `stash` before assigning new combined version
- ✅ `stash` in `clean_stash` before return
- ❌ Never free `line` - user must free it!

### **When to return NULL:**
- ✅ Invalid fd or BUFFER_SIZE
- ✅ Malloc failure (after cleaning up)
- ✅ Read error (after cleaning up)
- ✅ EOF and stash is empty

### **Critical null terminators:**
- `buffer[n] = '\0'` after read()
- `line[i] = '\0'` after copying
- `ft_strdup` adds it automatically

---

## 🎯 Key Takeaways

1. **Static variables** preserve data between function calls
2. **read()** doesn't care about lines, only bytes
3. **Stash** accumulates data until we find `\n`
4. **Memory dance:** create new, free old, reassign pointer
5. **Always null-terminate** after `read()`
6. **Pointer arithmetic** `(stash + i + 1)` skips to position
7. **Comma operator** for cleanup: `(free(x), NULL)`

---

## ✅ Test Your Understanding

**Q1:** Why do we need `n = 1` before the while loop?
**A:** To enter the loop initially (n > 0 condition)

**Q2:** Why `malloc(i + 2)` in extract_line?
**A:** i chars + potential \n + null terminator

**Q3:** What happens if file doesn't end with \n?
**A:** Last line returned without \n, then clean_stash returns NULL

**Q4:** Why is stash static?
**A:** To remember leftover data between function calls

**Q5:** What if BUFFER_SIZE is 1000000 but file is 10 bytes?
**A:** Works fine! read() returns 10, we only process 10 bytes

---

## 💡 Pro Tips

- Test with BUFFER_SIZE = 1 (worst case)
- Test with files not ending in \n
- Test with empty files
- Test with multiple \n in a row
- Use valgrind to check for leaks
- Draw memory diagrams when confused

---

**You've got this! 🚀**
