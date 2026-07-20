# [RUN][Alta] Loop interactivo crashea o muestra leak tras múltiples ejecuciones

Descripción: Al usar la funcionalidad de re-ejecución interactiva, la aplicación crashea o consume memoria crecientemente.

Posibles salidas (ejemplos en texto plano):

1) Crash después de N ejecuciones

```
... (ejecuciones 1..49)
Segmentation fault (core dumped)
```

2) Uso de memoria creciente (ejemplo de `top` snapshots)

```
Antes: PID 1234 mapas  %MEM 1.2
Despues 30 ejec: PID 1234 mapas  %MEM 12.5
```

3) Mensaje de error al liberar recursos

```
Error: intento de liberar memoria no inicializada en graph_free()
```

Evidencia recomendada: logs de ejecución, snapshots de `top`/`ps`, valgrind (o equivalente) si es posible.
