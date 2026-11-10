/*
 * validaciones.c - Modulo de Validaciones e Interfaz
 * Todas las lecturas leen la linea completa con fgets y despues la analizan.
 */
#include <stdio.h>    /* printf, fgets, getchar, feof, fflush, stdin */
#include <stdlib.h>   /* strtol, strtod */
#include <string.h>   /* strchr */
#include <ctype.h>    /* isspace, toupper */
#include <errno.h>    /* errno, ERANGE */
#include <math.h>     /* isnan, isinf */
#include "validaciones.h"

/* ---- Funciones privadas de este archivo ---- */
static int leerLinea(char linea[], int tamanio);
static int soloEspacios(const char *texto);
static int pedirRealInterno(const char *mensaje, double minimo, double maximo,
                            int permitirVacio, double valorActual, double *valor);

/* ================= LECTURA DE UNA LINEA ================= */

/*
 * Lee UNA linea completa en 'linea'.
 * Devuelve LECTURA_OK, LECTURA_FIN_ENTRADA o LECTURA_DESBORDE.
 */
static int leerLinea(char linea[], int tamanio)
{
    int caracter;

    if (fgets(linea, tamanio, stdin) == NULL) {
        return LECTURA_FIN_ENTRADA;
    }

    /* Si no entro el '\n', la linea era mas larga que el arreglo:
       se descarta el resto para que no ensucie la proxima lectura. */
    if (strchr(linea, '\n') == NULL && !feof(stdin)) {
        while ((caracter = getchar()) != '\n' && caracter != EOF) {
            /* descartar */
        }
        return LECTURA_DESBORDE;
    }
    return LECTURA_OK;
}

/* Devuelve 1 si desde 'texto' hasta el final solo hay espacios (o nada). */
static int soloEspacios(const char *texto)
{
    while (isspace((unsigned char)*texto)) {
        texto++;
    }
    return *texto == '\0';
}

/* ================= CONVERSIONES ================= */

int leerEntero(long *valor)
{
    char  linea[TAMANIO_LINEA];
    char *fin;
    long  numero;
    int   estado;

    estado = leerLinea(linea, TAMANIO_LINEA);
    if (estado != LECTURA_OK) {
        return estado;
    }
    if (soloEspacios(linea)) {
        return LECTURA_VACIA;
    }

    errno = 0;
    numero = strtol(linea, &fin, 10);

    if (fin == linea) {
        return LECTURA_NO_NUMERICA;
    }
    if (errno == ERANGE) {
        return LECTURA_DESBORDE;
    }
    if (!soloEspacios(fin)) {
        return LECTURA_TEXTO_EXTRA;
    }

    *valor = numero;
    return LECTURA_OK;
}

int leerReal(double *valor)
{
    char   linea[TAMANIO_LINEA];
    char  *fin;
    double numero;
    int    estado;

    estado = leerLinea(linea, TAMANIO_LINEA);
    if (estado != LECTURA_OK) {
        return estado;
    }
    if (soloEspacios(linea)) {
        return LECTURA_VACIA;
    }

    errno = 0;
    numero = strtod(linea, &fin);

    if (fin == linea) {
        return LECTURA_NO_NUMERICA;
    }
    if (isnan(numero)) {          /* strtod acepta "nan" como texto valido */
        return LECTURA_NAN;
    }
    if (errno == ERANGE) {        /* ej: 1e999 */
        return LECTURA_DESBORDE;
    }
    if (isinf(numero)) {          /* "inf", "infinity" */
        return LECTURA_INFINITO;
    }
    if (*fin == ',') {            /* "6,5" en lugar de "6.5" */
        return LECTURA_COMA_DECIMAL;
    }
    if (!soloEspacios(fin)) {
        return LECTURA_TEXTO_EXTRA;
    }

    *valor = numero;
    return LECTURA_OK;
}

int leerCaracter(char *valor)
{
    char  linea[TAMANIO_LINEA];
    char *actual;
    int   estado;

    estado = leerLinea(linea, TAMANIO_LINEA);
    if (estado != LECTURA_OK) {
        return estado;
    }
    if (soloEspacios(linea)) {
        return LECTURA_VACIA;
    }

    actual = linea;
    while (isspace((unsigned char)*actual)) {    /* saltea espacios iniciales */
        actual++;
    }
    if (!soloEspacios(actual + 1)) {             /* despues del caracter no puede haber mas */
        return LECTURA_VARIOS_CARACTERES;
    }

    *valor = *actual;
    return LECTURA_OK;
}

/* ================= MENSAJES ================= */

void mostrarErrorLectura(int estado)
{
    switch (estado) {
        case LECTURA_VACIA:
            printf("Error: no escribiste nada.\n");
            break;
        case LECTURA_NO_NUMERICA:
            printf("Error: eso no es un numero.\n");
            break;
        case LECTURA_TEXTO_EXTRA:
            printf("Error: hay texto pegado al numero. Ingresa solo el numero.\n");
            break;
        case LECTURA_DESBORDE:
            printf("Error: numero fuera de lo representable o entrada demasiado larga.\n");
            break;
        case LECTURA_COMA_DECIMAL:
            printf("Error: usa punto decimal, no coma (ej: 6.5).\n");
            break;
        case LECTURA_NAN:
            printf("Error: 'nan' no es un numero valido.\n");
            break;
        case LECTURA_INFINITO:
            printf("Error: el infinito no es un valor valido.\n");
            break;
        case LECTURA_VARIOS_CARACTERES:
            printf("Error: escribi una sola letra.\n");
            break;
        default:
            printf("Error de lectura desconocido.\n");
            break;
    }
}

/* ================= PREGUNTAS COMPLETAS ================= */

/*
 * Pregunta hasta obtener un real con minimo < valor < maximo.
 * Si permitirVacio es 1, Enter vacio devuelve valorActual.
 */
static int pedirRealInterno(const char *mensaje, double minimo, double maximo,
                            int permitirVacio, double valorActual, double *valor)
{
    int    estado;
    double leido = 0.0;

    do {
        printf("%s", mensaje);
        fflush(stdout);
        estado = leerReal(&leido);

        if (estado == LECTURA_VACIA && permitirVacio) {
            leido  = valorActual;
            estado = LECTURA_OK;
        } else if (estado == LECTURA_OK && (leido <= minimo || leido >= maximo)) {
            printf("Error: debe ser mayor que %g y menor que %g.\n", minimo, maximo);
            estado = LECTURA_FUERA_RANGO;
        } else if (estado != LECTURA_OK && estado != LECTURA_FIN_ENTRADA) {
            mostrarErrorLectura(estado);
        }
    } while (estado != LECTURA_OK && estado != LECTURA_FIN_ENTRADA);

    if (estado == LECTURA_OK) {
        *valor = leido;
    }
    return estado;
}

int pedirReal(const char *mensaje, double minimo, double maximo, double *valor)
{
    return pedirRealInterno(mensaje, minimo, maximo, 0, 0.0, valor);
}

int pedirParametro(const char *nombre, const char *unidad,
                   double minimo, double maximo,
                   double valorActual, double *valor)
{
    char mensaje[128];

    /* snprintf arma el texto de la pregunta sin pasarse del tamanio del arreglo */
    snprintf(mensaje, sizeof mensaje, "%s [%s] (actual: %g, Enter conserva): ",
             nombre, unidad, valorActual);
    return pedirRealInterno(mensaje, minimo, maximo, 1, valorActual, valor);
}

int pedirConfirmacion(const char *mensaje, int *confirma)
{
    int  estado;
    char respuesta = ' ';

    do {
        printf("%s", mensaje);
        fflush(stdout);
        estado = leerCaracter(&respuesta);

        if (estado == LECTURA_OK) {
            respuesta = (char)toupper((unsigned char)respuesta);
            if (respuesta != 'S' && respuesta != 'N') {
                printf("Error: responde S o N.\n");
                estado = LECTURA_FUERA_RANGO;
            }
        } else if (estado != LECTURA_FIN_ENTRADA) {
            mostrarErrorLectura(estado);
        }
    } while (estado != LECTURA_OK && estado != LECTURA_FIN_ENTRADA);

    if (estado == LECTURA_OK) {
        *confirma = (respuesta == 'S');
    }
    return estado;
}
