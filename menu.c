#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#define MASA_KG                  10400.0
#define VELOCIDAD_INICIAL_MS     11000.0
#define VELOCIDAD_PARACAIDAS_MS  25.0
#define ALTURA_INICIAL_M         120000.0
#define ALTURA_DESPLIEGUE_M      2500.0
#define TIEMPO_FRENADO_S         400.0
#define VELOCIDAD_IMPACTO_MS     8.0
#define PROFUNDIDAD_AGUA_M       1.50
#define GRAVEDAD_MS2             9.81
#define ANGULO_MINIMO_GRADOS     5.5
#define ANGULO_MAXIMO_GRADOS     7.0

#define OPCION_INICIAR    1
#define OPCION_VER_DATOS  2
#define OPCION_SALIR      3

#define LECTURA_OK           0
#define LECTURA_FIN_ENTRADA  1
#define LECTURA_VACIA        2
#define LECTURA_NO_NUMERICA  3
#define LECTURA_TEXTO_EXTRA  4
#define LECTURA_DESBORDE     5

#define TAMANIO_LINEA 64

void mostrar_menu(void);
void mostrar_datos_orion(void);
int  leer_entero(long *valor);
void mostrar_error_lectura(int estado);

int main(void)
{
    long opcion = 0;
    int  estado;
    int  seguir = 1;

    do {
        mostrar_menu();
        estado = leer_entero(&opcion);

        if (estado == LECTURA_FIN_ENTRADA) {
            printf("\nFin de la entrada. Cerrando el programa.\n");
            seguir = 0;
        } else if (estado != LECTURA_OK) {
            mostrar_error_lectura(estado);
        } else {
            switch (opcion) {
                case OPCION_INICIAR:
                    printf("\nIniciando mision...\n");
                    break;
                case OPCION_VER_DATOS:
                    mostrar_datos_orion();
                    break;
                case OPCION_SALIR:
                    printf("\nFin del programa. Hasta luego.\n");
                    seguir = 0;
                    break;
                default:
                    printf("\nError: la opcion %ld no existe. Elegi 1, 2 o 3.\n", opcion);
                    break;
            }
        }
    } while (seguir);

    return 0;
}

void mostrar_menu(void)
{
    printf("\n========= ORION - REINGRESO =========\n");
    printf("  1. Iniciar mision\n");
    printf("  2. Ver datos de Orion\n");
    printf("  3. Salir\n");
    printf("=====================================\n");
    printf("Opcion: ");
    fflush(stdout);
}

void mostrar_datos_orion(void)
{
    printf("\n------ DATOS DE REFERENCIA (SI) ------\n");
    printf("Masa de la capsula ............ %10.2f kg\n",   MASA_KG);
    printf("Velocidad inicial ............. %10.2f m/s\n",  VELOCIDAD_INICIAL_MS);
    printf("Velocidad al abrir paracaidas . %10.2f m/s\n",  VELOCIDAD_PARACAIDAS_MS);
    printf("Altura inicial ................ %10.2f m\n",    ALTURA_INICIAL_M);
    printf("Altura de despliegue .......... %10.2f m\n",    ALTURA_DESPLIEGUE_M);
    printf("Tiempo de frenado ............. %10.2f s\n",    TIEMPO_FRENADO_S);
    printf("Velocidad de impacto .......... %10.2f m/s\n",  VELOCIDAD_IMPACTO_MS);
    printf("Profundidad en el agua ........ %10.2f m\n",    PROFUNDIDAD_AGUA_M);
    printf("Gravedad (constante) .......... %10.2f m/s2\n", GRAVEDAD_MS2);
    printf("Ventana segura de angulo ...... %.1f a %.1f grados (incluidos)\n",
           ANGULO_MINIMO_GRADOS, ANGULO_MAXIMO_GRADOS);
}

int leer_entero(long *valor)
{
    char  linea[TAMANIO_LINEA];
    char *inicio;
    char *fin;
    long  numero;
    int   caracter;

    if (fgets(linea, sizeof linea, stdin) == NULL) {
        return LECTURA_FIN_ENTRADA;
    }

    if (strchr(linea, '\n') == NULL && !feof(stdin)) {
        while ((caracter = getchar()) != '\n' && caracter != EOF) {
        }
        return LECTURA_DESBORDE;
    }

    inicio = linea;
    while (isspace((unsigned char)*inicio)) {
        inicio++;
    }
    if (*inicio == '\0') {
        return LECTURA_VACIA;
    }

    errno = 0;
    numero = strtol(inicio, &fin, 10);

    if (fin == inicio) {
        return LECTURA_NO_NUMERICA;
    }
    if (errno == ERANGE) {
        return LECTURA_DESBORDE;
    }

    while (isspace((unsigned char)*fin)) {
        fin++;
    }
    if (*fin != '\0') {
        return LECTURA_TEXTO_EXTRA;
    }

    *valor = numero;
    return LECTURA_OK;
}

void mostrar_error_lectura(int estado)
{
    switch (estado) {
        case LECTURA_VACIA:
            printf("\nError: no escribiste nada. Ingresa un numero.\n");
            break;
        case LECTURA_NO_NUMERICA:
            printf("\nError: eso no es un numero. Ingresa 1, 2 o 3.\n");
            break;
        case LECTURA_TEXTO_EXTRA:
            printf("\nError: hay texto pegado al numero. Ingresa solo el numero.\n");
            break;
        case LECTURA_DESBORDE:
            printf("\nError: numero demasiado grande o entrada demasiado larga.\n");
            break;
        default:
            printf("\nError de lectura desconocido.\n");
            break;
    }
}
