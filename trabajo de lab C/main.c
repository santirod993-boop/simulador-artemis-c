/* main.c - Modulo Principal (Simulador Artemis - Escenario 4) */
#include <stdio.h>
#include "simulador.h"
#include "validaciones.h"

#define OPCION_INICIAR     1
#define OPCION_VER_DATOS   2
#define OPCION_CONFIGURAR  3
#define OPCION_HISTORIAL   4
#define OPCION_SALIR       5

static void mostrarMenu(void);

int main(void)
{
    Configuracion   configuracion;
    ResultadoMision historial[MAX_HISTORIAL];
    int             cantidadResultados = 0;
    long            opcion = 0;
    int             estado;
    int             seguir = 1;

    inicializarConfiguracion(&configuracion);

    do {
        mostrarMenu();
        estado = leerEntero(&opcion);

        if (estado == LECTURA_OK) {
            switch (opcion) {
                case OPCION_INICIAR:
                    estado = ejecutarMision(&configuracion, historial, &cantidadResultados);
                    break;
                case OPCION_VER_DATOS:
                    mostrarConfiguracion(&configuracion);
                    break;
                case OPCION_CONFIGURAR:
                    estado = configurarParametros(&configuracion);
                    break;
                case OPCION_HISTORIAL:
                    mostrarHistorial(historial, cantidadResultados);
                    break;
                case OPCION_SALIR:
                    printf("\nFin del programa. Hasta luego.\n");
                    seguir = 0;
                    break;
                default:
                    printf("Error: la opcion %ld no existe. Elegi del 1 al 5.\n", opcion);
                    break;
            }
        } else if (estado != LECTURA_FIN_ENTRADA) {
            mostrarErrorLectura(estado);
        }

        if (estado == LECTURA_FIN_ENTRADA) {
            printf("\nFin de la entrada. Cerrando el programa.\n");
            seguir = 0;
        }
    } while (seguir);

    mostrarReporteFinal(historial, cantidadResultados);
    return 0;
}

static void mostrarMenu(void)
{
    printf("\n========= ARTEMIS - REINGRESO DE ORION =========\n");
    printf("  1. Iniciar mision\n");
    printf("  2. Ver datos de la mision\n");
    printf("  3. Configurar parametros\n");
    printf("  4. Ver historial\n");
    printf("  5. Salir\n");
    printf("================================================\n");
    printf("Opcion: ");
    fflush(stdout);
}
