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

Requiere GCC y Make.

```bash
make
```

En Windows sin Make:

```bat
build.bat
```

El ejecutable se genera en `build/mapas`.

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
