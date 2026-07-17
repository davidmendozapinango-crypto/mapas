# Weather File Format Contract

**Feature**: Route Calculation Core  
**Date**: 2026-07-16

This contract defines the plain text format for optional weather configuration files.

## File Structure

```text
nombre_ruta = P:coeficiente; B:coeficiente; C:coeficiente
```

Each line contains one weather entry. Empty lines are ignored.

## Format

*   `nombre_ruta` is the identifier of the route to which the multipliers apply.
*   `coeficiente` is a non-negative decimal number.
*   A coefficient of zero makes the route impassable for that mode.

## Example

```text
RutaB = P:0; B:1; C:1.3
RutaC = P:1; B:1.2; C:1
```

## Validation Rules

*   Each line MUST follow the exact format above.
*   Invalid lines produce a warning with the line number and are skipped.
*   References to unknown routes produce a warning and are ignored.
*   If no weather file is provided, all routes use their base costs.
