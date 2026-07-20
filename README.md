# Mapas

El siguiente proyecto consiste en la elaboración de un sistema capaz de calcular rutas de viaje óptimas conforme un mapa dado, considerando distintos factores como el medio de transporte empleado, las condiciones climáticas actuales, etc.

La idea consiste en que el sistema recibirá un archivo de texto con el mapa de la ciudad en el que se especificarán los distintos puntos (sitios) existentes, las rutas que comunican dichos lugares y los costos asociados a dichas rutas si se viaja en carro propio, transporte público o a pie.

Asimismo, el usuario podrá (si lo desea) cargar un archivo adicional en el cual se contendrá información sobre el clima actual. Estos datos servirán para modificar los costos asociados a las rutas previamente dadas. Así pues, el clima podrá aumentar los costos de una ruta haciendo que el camino óptimo sea otro o bien anulando incluso caminos que antes eran transitables.

## Entradas y Requerimientos

- Las rutas serán dadas en un archivo de configuración con el siguiente formato:

```
Lugares
nombre_lugar_1 (solo caracteres alfanuméricos y guión bajo) nombre_ruta -> lugar_partida:lugar_llegada = P:costo; B:costo; C:costo
```

Donde las rutas describen el camino y sentido entre un sitio y otro, y los coeficientes asociados al costo de transitar dicha ruta a pie, en bus o en carro. Por ejemplo:

```
RutaA -> Heladeria:Ferreteria = P:2.5; B:0; C:0
RutaB -> Heladeria:Ferreteria = P:2.5; B:13; C:10
```

`RutaA` indica que hay un camino desde la heladería hacia la ferretería (en ese sentido) que tarda dos unidades y media de tiempo en transitarse a pie, una en carro y que no es transitable en bus (lo indica el coeficiente cero). `RutaB` indica que hay otro camino paralelo entre esos dos mismos lugares.

- El clima (opcional) será dado en un archivo de configuración con el siguiente formato:

```
nombre_ruta = P:coeficiente; B:coeficiente; C:coeficiente
```

Donde se dan los coeficientes que serán multiplicados a los costos a pie, bus y carro respectivamente de la ruta dada. Por ejemplo:

```
RutaB = P:0; B:1; C:1.3
```

Indica que el clima actual afecta a la `RutaB` de tal forma que el camino a pie se vuelve intransitable, el costo del bus sigue siendo igual (`13 * 1`) y el del carro pasa a ser 13 (`10 * 1.3`).

- Se deberá solicitar el archivo de rutas y opcionalmente el de clima. De igual forma se solicitará el punto de partida y fin de la ruta que se desea hallar.
- Se mostrará la ruta óptima y la segunda mejor entre los puntos dados ya sea viajando a pie, bus o carro. Por ende el sistema mostrará a lo sumo seis rutas (2 a pie, 2 en bus y 2 en carro).
- Al finalizar los cálculos se debe continuar con la ejecución preguntando si se desea cambiar alguno de los archivos previamente dados y cuáles serán los nuevos puntos a calcular.
- Los resultados deben mostrarse de forma amigable al usuario ya sea mediante gráficos o tablas que indiquen todas las rutas a tomar para cada uno de los 6 caminos encontrados.

> Nota: Leer sobre algoritmo de Dijkstra y algoritmo de Yen.

La entrega del proyecto constará de un informe escrito no mayor a 5 páginas que contenga el diseño de la solución implementada. También deberá entregar el código fuente y una serie de resultados que se detallarán con mayor precisión el día de la defensa.

## Compilación

Recomendado: compilar en WSL (Debian) usando GCC y Make. A continuación se muestra un flujo paso a paso y soluciones comunes.

Usando Make en WSL (Debian)

1) Preparar WSL y dependencias:

```bash
# abrir WSL y situarse en la raíz del repo (ej: /mnt/c/Users/<tu_usuario>/source/repos/mapas)
sudo apt update
sudo apt install -y build-essential make gcov dos2unix
```

2) Compilar con Make (desde la raíz donde está el `Makefile`):

```bash
make
# binario generado: build/mapas
```

3) Ejecutar la aplicación:

```bash
./build/mapas
```

4) Ejecutar tests unitarios/integración:

```bash
make test
```

5) Cobertura (opcional):

```bash
make coverage
```

6) Limpiar artefactos de compilación:

```bash
make clean
```

Compilación directa con GCC (sin Make)

```bash
gcc -std=c11 -O2 -Wall -Wextra -pedantic -Iinclude -Itests/unity -o build/mapas src/*.c
```

Opciones avanzadas

- Compilar con flags de depuración:

```bash
make CFLAGS="-std=c11 -g -O0 -Wall -Iinclude -Itests/unity"
```

- Convertir finales de línea si hay problemas con Make o scripts (CRLF -> LF):

```bash
dos2unix Makefile
```

Problemas comunes y soluciones rápidas

- "make: command not found": instalar `make` (`sudo apt install make`).
- Errores de include: confirmar que `include/` existe y que `CFLAGS` contiene `-Iinclude -Itests/unity`.
- Errores de linking a `-lm`: normalmente no ocurre en Debian; asegúrate de tener `build-essential` instalado.
- Permisos: si el binario no es ejecutable `chmod +x build/mapas`.

En Windows sin Make (alternativa):

```bat
build.bat
```

## Ejecutar pruebas

```bash
make test
```

## Ejecutar la aplicación

```bash
./build/mapas
```

El programa solicitará:
1. Ruta del archivo de rutas (obligatorio)
2. Ruta del archivo de clima (opcional, presionar Enter para omitir)
3. Lugar de origen
4. Lugar de destino

Luego muestra una tabla con la ruta óptima y la segunda mejor para cada modo de transporte.

## Estructura del proyecto

- `src/` - archivos de implementación
- `include/` - archivos de encabezado
- `tests/` - pruebas unitarias e integración
- `data/` - archivos de ejemplo
- `build/` - archivos generados por la compilación

*El plagio de código fuente o bien del contenido del informe implica la anulación del proyecto.*
