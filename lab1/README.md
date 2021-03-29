# For easier testing

## Task 1

```bash
gcc prosti_brojevi.c -lm -o lab1.o
kill -SIGTERM "$(pgrep lab1)"
```

## Task 2

Processing code

```bash
gcc obrada.c -o obrada.o && ./obrada.o
```

Signal generator

```bash
gcc generator_prekida.c -o gen.o && ./gen.o "$(pgrep obrada.o)"
```
