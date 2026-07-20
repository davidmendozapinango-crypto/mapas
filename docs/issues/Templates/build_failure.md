# [BUILD][Crítica] Compilación falla en WSL (Debian)

Descripción: Compilación con `make` falla o `make` indica "Nothing to be done for 'all'" pero el binario no está presente/ejecutable.

Posibles salidas (ejemplos en texto plano):

1) `make` indica nada por hacer pero no hay binario

```
$ make
make: Nothing to be done for 'all'.

$ ls -l build
total 0
# build/mapas no aparece
```

2) Error de compilación (falta de include o símbolo)

```
$ make
gcc -std=c11 -Wall -Wextra -Wpedantic -Iinclude -Itests/unity -o build/mapas build/main.o build/cli.o ... -lm
/usr/bin/ld: build/main.o: in function `main':
main.c:(.text+0x1f0): undefined reference to `parse_routes'
collect2: error: ld returned 1 exit status
make: *** [Makefile:19: build/mapas] Error 1
```

3) Error por CRLF en Makefile (comportamiento inesperado)

```
$ make
/bin/sh: 1: Syntax error: Unterminated quoted string
# Solución: dos2unix Makefile
```

Evidencia recomendada: salida completa de `make`, `ls -l build`, `stat build/mapas`, `gcc --version`, `make --version`.
