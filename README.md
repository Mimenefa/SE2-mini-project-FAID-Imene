# os2-mini-project

Mini project for Operating Systems 2 - L3 IS 2025/2026

## Structure

- `exercise1/main.c` - parallel player threads (no sync)
- `exercise2/main.c` - synchronized playstation access using semaphores

## Exercise 1

Creates 16 threads total (8 football, 4 supermario, 4 nfs), all launched before any join is called. Each thread prints when it starts and finishes, and sleeps randomly between 1 and 3 seconds.

Compile:
```
gcc exercise1/main.c -o ex1 -lpthread
./ex1
```

## Exercise 2

Same threads but now with synchronization constraints:
- The playstation can only run one game type at a time
- Football: max 4 players at once
- Super Mario: max 2 players at once  
- NFS: max 1 player at once
- No starvation, equal priority

Used semaphores: one mutex to protect shared state, and one semaphore per game type to block waiting players.

Compile:
```
gcc exercise2/main.c -o ex2 -lpthread
./ex2
```
