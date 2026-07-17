# CLI Interaction Contract

**Feature**: Route Calculation Core  
**Date**: 2026-07-16

This contract defines the interactive command-line interface between the user and the Mapas application.

## Session Flow

1. Program starts and prints a welcome message.
2. Program prompts for the route file path.
3. Program validates the file and loads the map. On failure, it prints an error and returns to step 2.
4. Program prompts for an optional weather file path.
5. If a path is provided, the program validates and loads the file. Invalid weather lines produce warnings but do not abort.
6. Program prompts for the origin place.
7. Program validates that the origin exists. On failure, it prints an error and returns to step 6.
8. Program prompts for the destination place.
9. Program validates that the destination exists and is different from the origin. On failure, it prints an error and returns to step 8.
10. Program computes and displays up to six routes in a table.
11. Program prompts for the next action.
    *   `n` → query a new origin and destination with the same files (return to step 6).
    *   `c` → load different files (return to step 2).
    *   `s` → exit the program.

## Prompts

| Step | Spanish Prompt | Expected Input |
|------|----------------|----------------|
| Route file | `Ruta del archivo de rutas:` | Path string |
| Weather file | `Ruta del archivo de clima (opcional, presione Enter para omitir):` | Path string or empty |
| Origin | `Lugar de origen:` | Place name |
| Destination | `Lugar de destino:` | Place name |
| Next action | `Elija una accion: [n]ueva ruta, [c]ambiar archivos, [s]alir` | `n`, `c`, or `s` |

## Error Handling

All errors are printed to standard output on their own line. The program never terminates unexpectedly due to invalid user input; it always returns to the appropriate prompt.

## Quit Signal

The program exits cleanly when the user selects `s` from the next-action prompt.
