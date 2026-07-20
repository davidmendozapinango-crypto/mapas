# [PARSER][Alta] Parseo de archivo inválido no muestra mensaje claro

Descripción: Al cargar un archivo de rutas malformado la aplicación no muestra un mensaje claro (línea/razón) o hace crash.

Posibles salidas (ejemplos en texto plano):

1) Mensaje claro indicado línea y error

```
Error: línea 12: token inesperado 'B:13; C:10' — formato esperado 'P:<num>; B:<num>; C:<num>'
Carga cancelada. Corrija el archivo e intente de nuevo.
```

2) Mensaje genérico sin detalle

```
Error: archivo de rutas inválido.
```

3) Crash por fallo de parseo

```
Segmentation fault (core dumped)
```

Evidencia recomendada: archivo de entrada usado, salida completa de la consola, versión del binario.
