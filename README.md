*This project has been created as part of the 42 curriculum by Visual.*

# get_next_line

## Description

A function that reads and returns one line at a time from a file descriptor. Handles multiple consecutive calls to read an entire file line by line, using static variables to preserve state between calls.

## Instructions

Compile with your code:
```bash
cc -Wall -Wextra -Werror -D BUFFER_SIZE=42 get_next_line.c get_next_line_utils.c your_file.c
```

Usage:
```c
#include "get_next_line.h"

int fd = open("file.txt", O_RDONLY);
char *line;

while ((line = get_next_line(fd)))
{
    printf("%s", line);
    free(line);
}
close(fd);
```

The `BUFFER_SIZE` can be set at compilation to any value (tested with 1, 42, 9999, 10000000).

## Function Behavior

| Scenario | Return Value |
|----------|--------------|
| Line read successfully | Pointer to line (with `\n` if present) |
| End of file reached | `NULL` |
| Error occurred | `NULL` |
| Empty file | `NULL` |

**Important:** Each returned line must be freed by the caller.

## How It Works

1. **Read** - Read `BUFFER_SIZE` bytes from fd into buffer
2. **Stash** - Accumulate data in static variable until `\n` found
3. **Extract** - Copy line from stash (including `\n` if present)
4. **Clean** - Keep remaining data in stash for next call
5. **Return** - Return extracted line, repeat on next call

Uses a static `stash` to persist data between function calls. Continues reading until a newline is found or EOF is reached.

## Algorithm Overview

```
┌─────────────────┐
│  get_next_line  │
└────────┬────────┘
         │
    ┌────▼────────────────────────┐
    │ stash empty? Initialize ""  │
    └────┬────────────────────────┘
         │
    ┌────▼────────────────────────┐
    │ read_to_stash:              │
    │ - Read BUFFER_SIZE chunks   │
    │ - Append to stash           │
    │ - Stop when \n found or EOF │
    └────┬────────────────────────┘
         │
    ┌────▼────────────────────────┐
    │ extract_line:               │
    │ - Copy from stash to line   │
    │ - Include \n if present     │
    └────┬────────────────────────┘
         │
    ┌────▼────────────────────────┐
    │ clean_stash:                │
    │ - Keep data after \n        │
    │ - Free old stash            │
    └────┬────────────────────────┘
         │
    ┌────▼────────────────────────┐
    │ Return line                 │
    └─────────────────────────────┘
```

## Resources

- [42-Cursus Gitbook - get_next_line](https://42-cursus.gitbook.io/guide/rank-01/get_next_line)
- ["understanding get_next_line" (nikito)](https://www.youtube.com/watch?v=8E9siq7apUU)
- Man pages: `read`, `malloc`, `free`

## AI Usage

AI was used to break down the logic into clear, manageable steps when the overall flow felt overwhelming. Also consulted for debugging malloc-related errors and memory leaks that were difficult to track down manually.
