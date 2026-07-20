# [WEATHER][Alta] Multiplicadores de clima no aplicados correctamente

Descripción: Al cargar `data/weather_example.txt`, los multiplicadores no ajustan los costes correctamente o multiplicador 0 no marca ruta como intransitable.

Posibles salidas (ejemplos en texto plano):

1) Coste esperado vs obtenido

```
Ruta: RutaB, Modo: C
Coste base: 10.0
Multiplicador weather: 1.3
Esperado: 13.0
Obtenido: 10.0
```

2) Multiplicador 0 no inhabilita arista

```
Ruta: RutaB, Modo: P
Multiplicador weather: 0.0
Esperado: arista intransitable para modo P
Obtenido: arista considerada (transitable)
```

Evidencia recomendada: archivos `routes` y `weather` usados, salida completa y cálculo manual.
