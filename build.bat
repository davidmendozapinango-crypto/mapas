@echo off
setlocal enabledelayedexpansion

if not exist build mkdir build
if not exist build\tests mkdir build\tests

set CC=gcc
set CFLAGS=-std=c11 -Wall -Wextra -Wpedantic -Iinclude -Itests/unity
set LDFLAGS=-lm

set SRC=src\main.c src\cli.c src\parser.c src\graph.c src\dijkstra.c src\yen.c src\weather.c src\output.c
set OBJ=
for %%f in (%SRC%) do (
    set OBJ=!OBJ! build\%%~nf.o
    %CC% %CFLAGS% -c -o build\%%~nf.o %%f
    if errorlevel 1 exit /b 1
)

%CC% %CFLAGS% -o build\mapas.exe %OBJ% %LDFLAGS%
if errorlevel 1 exit /b 1

echo Build successful: build\mapas.exe
