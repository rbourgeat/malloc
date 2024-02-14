# MALLOC 42

My own `malloc()`, `realloc()`, `free()` and `show_alloc_mem()` functions.

## Usage

```bash
make
```

- Method 1:

```bash
./run.sh main
```

- Method 2:

```bash
export LD_LIBRARY_PATH=.:$LD_LIBRARY_PATH
gcc main.c -L. -lft_malloc
./main
```

> Info: the `show_alloc_mem()` function only work with the second method.

## Bonus

- [x] Manage the use of your malloc in a multi-threaded program (so to be “thread safe”
using the pthread lib).

