# Output Format Contract

**Feature**: Route Calculation Core  
**Date**: 2026-07-16

This contract defines how the application presents route results to the user.

## Result Table

After a successful query, the program prints a table with one row per computed route.

| Column | Description |
|--------|-------------|
| Modo | Transport mode: `A pie`, `Bus`, or `Carro`. |
| Ruta | Sequence of places from origin to destination, separated by ` -> `. |
| Costo total | Total cost for that mode, rounded to two decimal places. |
| Tipo | `Optima` for the best route, `Alternativa` for the second-best route. |

## Example

```text
Modo      | Ruta                              | Costo total | Tipo
----------|-----------------------------------|-------------|------------
A pie     | Heladeria -> Ferreteria -> Parque | 3.50        | Optima
A pie     | No hay ruta disponible            | -           | Alternativa
Bus       | Heladeria -> Ferreteria -> Parque | 15.00       | Optima
Bus       | Heladeria -> Ferreteria -> Parque | 16.00       | Alternativa
Carro     | Heladeria -> Ferreteria -> Parque | 1.50        | Optima
Carro     | No hay ruta disponible            | -           | Alternativa
```

## Rules

*   The table MUST contain at most six rows: two per transport mode.
*   If a route is unavailable for a mode, the `Ruta` column shows `No hay ruta disponible` and the `Costo total` column shows `-`.
*   Costs are displayed with exactly two decimal places.
*   The table header is printed once before the rows.
