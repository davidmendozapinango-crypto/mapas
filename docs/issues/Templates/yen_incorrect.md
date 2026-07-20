# [ALGOR][Alta] Yen no devuelve la segunda mejor ruta correctamente

Descripción: Para un grafo con alternativas, la segunda mejor ruta generada por Yen no es la segunda menor o es idéntica a la óptima.

Posibles salidas (ejemplos en texto plano):

1) Segunda ruta igual a la óptima

```
Mejor: A -> B -> D (Coste: 5.0)
Segunda: A -> B -> D (Coste: 5.0)
```

2) Reporta inexistente cuando sí existe

```
Segunda mejor ruta: No disponible
```

3) Segunda ruta con coste menor que la óptima (error claro)

```
Mejor: A -> C -> D (Coste: 6.0)
Segunda: A -> B -> D (Coste: 4.0)  # coste < óptimo
```

Evidencia recomendada: salida, archivo de rutas y explicación del orden correcto de rutas.
