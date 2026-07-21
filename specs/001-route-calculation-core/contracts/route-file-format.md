# Route File Format Contract

**Feature**: Route Calculation Core  
**Date**: 2026-07-16

This contract defines the plain text format for route configuration files.

## File Structure

```text
Lugares
nombre_lugar_1
nombre_lugar_2
...
nombre_lugar_N
Rutas
nombre_ruta -> lugar_partida:lugar_llegada = P:costo; B:costo; C:costo
```

## Sections

### Header

The first line MUST be exactly:

```text
Lugares
```

### Routes Header

After the last place name, the file MUST contain a line exactly:

```text
Rutas
```

This header separates the place list from the route list.

### Place Names

After the `Lugares` header, each subsequent line until the `Rutas` header contains a single place name. Place names:

*   MUST contain only ASCII letters, digits, and underscores (`A-Z`, `a-z`, `0-9`, `_`).
*   MUST be unique within the file.
*   ARE case-sensitive.

### Route Definitions

After the `Rutas` header, each route definition occupies one line and follows this exact format:

```text
nombre_ruta -> lugar_partida:lugar_llegada = P:costo; B:costo; C:costo
```

Where:

*   `nombre_ruta` is a unique route identifier (alphanumeric plus underscore).
*   `lugar_partida` is the name of the origin place.
*   `lugar_llegada` is the name of the destination place.
*   `costo` is a non-negative decimal number.
*   A cost of zero means the route is impassable for that mode.

## Example

```text
Lugares
Heladeria
Ferreteria
Parque
Rutas
RutaA -> Heladeria:Ferreteria = P:2.5; B:0; C:1
RutaB -> Heladeria:Ferreteria = P:2.5; B:13; C:10
RutaC -> Ferreteria:Parque = P:1; B:2; C:0.5
```

## Validation Rules

*   The file MUST begin with the `Lugares` header.
*   The `Rutas` header MUST appear after the place list and before any route definition.
*   At least one place and one route MUST be defined.
*   Every route MUST reference places that appear in the place list.
*   Route names MUST be unique within the file.
*   Duplicate place names are ignored after the first occurrence, with a warning.
*   Invalid route lines cause the loader to abort and report the line number.
