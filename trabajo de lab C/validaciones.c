/* validaciones.c - Modulo de Validaciones e Interfaz */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <math.h>
#include "validaciones.h"

static int leerLinea(char linea[], int tamanio);
static int soloEspacios(const char *texto);
static int pedirRealInterno(const char *mensaje, double minimo, double maximo, int permitirVacio, double valorActual, double *valor);

static int leerLinea(char linea[], int tamanio)
{
    int caracter;
    if (fgets(linea, tamanio, stdin) == NULL) return LECTURA_FIN_ENTRADA;

    if (strchr(linea, '\n') == NULL && !feof(stdin)) {
        while ((caracter = getchar()) != '\n' && caracter != EOF); /* Limpia búfer excedido */
        return LECTURA_DESBORDE;
    }
    return LECTURA_OK;
}

static int soloEspacios(const char *texto)
{
    while (isspace((unsigned char)*texto)) texto++;
    return *texto == '\0';
}

int leerEntero(long *valor)
{
    char linea[TAMANIO_LINEA];
    char *fin;
    long numero;
    int estado = leerLinea(linea, TAMANIO_LINEA);

    if (estado != LECTURA_OK) return estado;
    if (soloEspacios(linea)) return LECTURA_VACIA;

    errno = 0;
    numero = strtol(linea, &fin, 10);

    if (fin == linea) return LECTURA_NO_NUMERICA;
    if (errno == ERANGE) return LECTURA_DESBORDE;
    if (!soloEspacios(fin)) return LECTURA_TEXTO_EXTRA;

    *valor = numero;
    return LECTURA_OK;
}

int leerReal(double *valor)
{
    char linea[TAMANIO_LINEA];
    char *fin;
    double numero;
    int estado = leerLinea(linea, TAMANIO_LINEA);

    if (estado != LECTURA_OK) return estado;
    if (soloEspacios(linea)) return LECTURA_VACIA;

    errno = 0;
    numero = strtod(linea, &fin);

    if (fin == linea) return LECTURA_NO_NUMERICA;
    if (isnan(numero)) return LECTURA_NAN;
    if (errno == ERANGE) return LECTURA_DESBORDE;
    if (isinf(numero)) return LECTURA_INFINITO;
    if (*fin == ',') return LECTURA_COMA_DECIMAL;
    if (!soloEspacios(fin)) return LECTURA_TEXTO_EXTRA;

    *valor = numero;
    return LECTURA_OK;
}

int leerCaracter(char *valor)
{
    char linea[TAMANIO_LINEA];
    char *actual;
    int estado = leerLinea(linea, TAMANIO_LINEA);

    if (estado != LECTURA_OK) return estado;
    if (soloEspacios(linea)) return LECTURA_VACIA;

    actual = linea;
    while (isspace((unsigned char)*actual)) actual++;
    if (!soloEspacios(actual + 1)) return LECTURA_VARIOS_CARACTERES;

    *valor = *actual;
    return LECTURA_OK;
}

void mostrarErrorLectura(int estado)
{
    switch (estado) {
        case LECTURA_VACIA: printf("Error: no escribiste nada.\n"); break;
        case LECTURA_NO_NUMERICA: printf("Error: eso no es un numero.\n"); break;
        case LECTURA_TEXTO_EXTRA: printf("Error: hay texto pegado al numero.\n"); break;
        case LECTURA_DESBORDE: printf("Error: numero fuera de rango o entrada muy larga.\n"); break;
        case LECTURA_COMA_DECIMAL: printf("Error: usa punto decimal, no coma (ej: 6.5).\n"); break;
        case LECTURA_NAN: printf("Error: 'nan' no es un numero valido.\n"); break;
        case LECTURA_INFINITO: printf("Error: el infinito no es valido.\n"); break;
        case LECTURA_VARIOS_CARACTERES: printf("Error: escribi una sola letra.\n"); break;
        default: printf("Error de lectura desconocido.\n"); break;
    }
}

static int pedirRealInterno(const char *mensaje, double minimo, double maximo, int permitirVacio, double valorActual, double *valor)
{
    int estado, usoValorActual;
    double leido = 0.0;

    do {
        printf("%s", mensaje);
        fflush(stdout);
        estado = leerReal(&leido);
        usoValorActual = 0;

        if (estado == LECTURA_VACIA && permitirVacio) {
            leido = valorActual;
            estado = LECTURA_OK;
            usoValorActual = 1;
        }

        if (estado == LECTURA_OK && (leido <= minimo || leido >= maximo)) {
            if (usoValorActual) printf("Error: el valor actual (%g) ya no es valido.\n", valorActual);
            printf("Error: debe ser mayor que %g y menor que %g.\n", minimo, maximo);
            estado = LECTURA_FUERA_RANGO;
        } else if (estado != LECTURA_OK && estado != LECTURA_FIN_ENTRADA) {
            mostrarErrorLectura(estado);
        }
    } while (estado != LECTURA_OK && estado != LECTURA_FIN_ENTRADA);

    if (estado == LECTURA_OK) *valor = leido;
    return estado;
}

int pedirReal(const char *mensaje, double minimo, double maximo, double *valor)
{
    return pedirRealInterno(mensaje, minimo, maximo, 0, 0.0, valor);
}

int pedirParametro(const char *mensaje, const char *unidad, double minimo, double maximo, double valorActual, double *valor)
{
    char textoMensaje[128];
    snprintf(textoMensaje, sizeof textoMensaje, "%s [%s] (actual: %g, Enter conserva): ", mensaje, unidad, valorActual);
    return pedirRealInterno(textoMensaje, minimo, maximo, 1, valorActual, valor);
}

int pedirConfirmacion(const char *mensaje, int *confirma)
{
    int estado;
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

    if (estado == LECTURA_OK) *confirma = (respuesta == 'S');
    return estado;
}