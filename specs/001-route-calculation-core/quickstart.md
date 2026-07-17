# Quickstart: Route Calculation Core

**Feature**: Route Calculation Core  
**Date**: 2026-07-16

This guide provides runnable validation scenarios for the Mapas route-calculation feature.

## Prerequisites

*   GCC installed and available in the system PATH.
*   Make available.
*   Unity testing framework source files placed under `tests/unity/`.

## Build the Application

From the repository root:

```bash
make
```

On Windows without Make:

```bat
cmd /c build.bat
```

Expected result: the executable `build/mapas` is created without compiler warnings.

## Run the Unit Tests

```bash
make test
```

Expected result: all tests pass and the final line reports success.

## Validate the CLI End-to-End

1. Create a route file `data/routes_example.txt`:

```text
Lugares
Heladeria
Ferreteria
Parque
RutaA -> Heladeria:Ferreteria = P:2.5; B:0; C:0
RutaB -> Heladeria:Ferreteria = P:2.5; B:13; C:10
RutaC -> Ferreteria:Parque = P:1; B:2; C:0.5
```

2. Create a weather file `data/weather_example.txt`:

```text
RutaB = P:0; B:1; C:1.3
```

3. Run the application:

```bash
./build/mapas
```

4. At the prompts, enter:

*   `Ruta del archivo de rutas:` → `data/routes_example.txt`
*   `Ruta del archivo de clima (opcional, presione Enter para omitir):` → `data/weather_example.txt`
*   `Lugar de origen:` → `Heladeria`
*   `Lugar de destino:` → `Parque`

5. Expected result: a table showing the optimal and alternative routes for each mode.

   *   `A pie` optimal route should be `Heladeria -> Ferreteria -> Parque` with cost `3.50`.
   *   `Bus` optimal route should be `Heladeria -> Ferreteria -> Parque` with cost `15.00`.
   *   `Carro` optimal route should be `Heladeria -> Ferreteria -> Parque` with cost `13.50` (10 * 1.3 + 0.5).

6. At the next-action prompt, press `s` to exit.

Expected result: the program terminates cleanly without errors.

## Validate Error Handling

1. Run the application.
2. At the route file prompt, enter a non-existent path.

Expected result: an error message in Spanish and a repeat of the route file prompt.

3. Enter a valid route file, then enter an origin that does not exist in the map.

Expected result: an error message in Spanish identifying the unknown place and a repeat of the origin prompt.

## Validate the Re-run Loop

1. Complete a query with the example route and weather files.
2. At the next-action prompt, press `n`.

Expected result: the program prompts for a new origin and destination.

3. Enter a different origin and destination.

Expected result: the program displays new results without restarting.

## Validate Without Weather File

1. Run the application.
2. At the route file prompt, enter `data/routes_example.txt`.
3. At the weather file prompt, press Enter.

Expected result: the program uses base costs for all routes.
