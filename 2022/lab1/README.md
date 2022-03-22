# For easier testing

## Setup

```bash
./clean.sh
```

## Running

```bash
gcc signali.c -lm -o signali.o && ./signali.o
```

## Test

### Run interrupts

```bash
kill -SIGUSR1 "$(pgrep signali.o)" # print
kill -SIGTERM "$(pgrep signali.o)" # terminate
kill -SIGINT "$(pgrep signali.o)" # interrupt and terminate without saving
```

### Read data

```bash
tail -f ./obrada.txt
```

## Deploy

```bash
./pack.sh
```
