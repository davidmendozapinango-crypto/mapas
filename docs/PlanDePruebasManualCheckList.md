# Checklist — Plan de Pruebas Manual (Mapas)

Proyecto: Mapas — Verificar que la aplicación cumple `docs/01 ProjectGoals.md`.

Uso: ejecutar en WSL (Debian). Comandos relevantes: `make`, `make test`, `./build/mapas`, `make coverage`.

Preparación
- [ ] Abrir WSL y situarse en la raíz del repo (`/mnt/c/.../mapas`).
- [ ] Instalar dependencias: `sudo apt update && sudo apt install -y build-essential make gcov dos2unix`.
- [ ] Verificar existencia de archivos de prueba: `data/routes_example.txt`, `data/weather_example.txt`, `tests/fixtures/*`.
- [ ] Ejecutar `dos2unix Makefile` si hubo problemas con CRLF.

Compilación (Make / WSL)
- [ ] `make` ejecuta sin errores y produce `build/mapas`.
- [ ] `make -B` o `make clean && make` reconstruye desde cero si es necesario.
- [ ] Compilar con flags estrictas: `make CFLAGS="-std=c11 -g -O0 -Wall -Iinclude -Itests/unity"` para debug.

Arranque y prueba básica
- [ ] Ejecutar `./build/mapas` y confirmar que muestra prompts o ayuda (no crash).
- [ ] Cargar `data/routes_example.txt` y comprobar mensaje de carga OK.

Parseo de archivos
- [ ] Cargar `tests/fixtures/valid_routes.txt` → archivo parseado correctamente.
- [ ] Cargar `tests/fixtures/invalid_routes.txt` → error claro con línea/razón; aplicación no crash.
- [ ] Cargar `tests/fixtures/duplicate_routes.txt` → duplicados detectados/reportados según especificación.
- [ ] Cargar `tests/fixtures/valid_weather.txt` → weather parseado correctamente.
- [ ] Cargar `tests/fixtures/invalid_weather.txt` → error claro o opción para omitir; no crash.

Construcción del grafo
- [ ] Verificar que las aristas creadas coinciden con el archivo de rutas (nodos y direcciones).
- [ ] Verificar pesos por modo (P, B, C) contra los valores del archivo.
- [ ] Consultar ruta entre nodos desconectados → mensaje "ruta no encontrada" por modo.

Algoritmos (correctitud)
- [ ] Dijkstra: para un origen/destino conocido, confirmar que la ruta óptima y coste coinciden con la suma manual.
- [ ] Yen: confirmar segunda mejor ruta distinta de la óptima y coste >= óptimo cuando hay alternativas.
- [ ] Mensaje correcto si no existe segunda ruta.

Integración clima (weather)
- [ ] Aplicar `data/weather_example.txt` y verificar costos ajustados (base * multiplier).
- [ ] Probar multiplicador 0 en una arista crítica → arista intransitable; alternativa o mensaje de no transitables.
- [ ] Ejecutar sin weather → resultados igual que sin aplicar multiplicadores.

Salida y formato
- [ ] Salida muestra tablas legibles con columnas: modo, tipo (Mejor/2ª), camino (lista de nodos), coste total.
- [ ] Formato numérico y redondeo consistente (documentado en README).
- [ ] Mensajes CLI/prompt claros para cargar archivos, origen y destino.

Loop interactivo y estabilidad
- [ ] Desde el loop, cambiar origen/destino y re-ejecutar varias veces → resultados actualizados.
- [ ] Ejecutar 50 consultas consecutivas en loop → uso de memoria estable (sin leaks aparentes).
- [ ] Salir desde el loop → proceso finaliza con código 0.

Errores y validaciones
- [ ] Indicar archivo inexistente → mensaje con path y opción a reintentar.
- [ ] Origen/destino inválido → prompt para reingresar sin crash.
- [ ] Valores anómalos (negativos, NaN, overflow) → rechazados con mensaje claro.

Performance
- [ ] Ejecutar con `tests/fixtures/performance_routes.txt` → tiempo razonable (definir umbral local, p. ej. <5s) y sin crash.
- [ ] Observación manual de uso de memoria/CPU durante pruebas de carga.

Tests automáticos y cobertura
- [ ] `make test` corre y pasa los tests unitarios/integración incluidos.
- [ ] `make coverage` genera resultados de cobertura (gcov) sin errores.

Documentación y entregables
- [ ] README incluye instrucciones WSL/Make y ejemplos de ejecución.
- [ ] Informe final (<=5 páginas) preparado con diseño y resultados.

Criterios de aceptación
- [ ] App compila en WSL (Debian) y no produce crash en casos críticos.
- [ ] Parseo y validaciones con mensajes claros.
- [ ] Dijkstra y Yen verificados en casos manuales.
- [ ] Weather aplicado correctamente; multiplicador 0 marca intransitabilidad.
- [ ] Loop interactivo estable y reproducible.

Formato de registro de defectos (usar al encontrar fallas)
- ID (ej: DEF-01)
- Caso de prueba (ej: A1)
- Pasos para reproducir
- Resultado esperado
- Resultado obtenido
- Severidad (Baja/Media/Alta/Crítica)
- Evidencia (captura / log / output file)

Referencias rápidas
- Especificación: `docs/01 ProjectGoals.md`
- Formatos: `specs/001-route-calculation-core/contracts/route-file-format.md`, `specs/001-route-calculation-core/contracts/weather-file-format.md`, `specs/001-route-calculation-core/contracts/cli-interaction.md`
- Archivos de prueba: `data/*.txt`, `tests/fixtures/*`

Checklist generado en `docs/PlanDePruebasManualCheckList.md`.
