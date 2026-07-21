/**
 * @file utils.h
 * @brief Utilidades generales usadas por varios módulos.
 *
 * Este archivo contiene funciones auxiliares pequeñas que no encajan en un
 * módulo específico. La función xstrdup permite duplicar cadenas de forma
 * segura, manejando correctamente los punteros NULL.
 */

#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <string.h>

/**
 * @brief Duplica una cadena en memoria dinámica.
 *
 * Reserva exactamente el espacio necesario para copiar la cadena, incluyendo
 * el carácter nulo terminador. Si la cadena de entrada es NULL, devuelve NULL
 * sin reservar memoria. Si la reserva falla, también devuelve NULL.
 *
 * @param s [in] Cadena a duplicar (puede ser NULL).
 * @return Puntero a la nueva cadena, o NULL si s es NULL o falla malloc.
 */
static char *xstrdup(const char *s)
{
    if (s == NULL) {
        return NULL;
    }
    size_t n = strlen(s) + 1;
    char *p = malloc(n);
    if (p != NULL) {
        memcpy(p, s, n);
    }
    return p;
}

#endif
