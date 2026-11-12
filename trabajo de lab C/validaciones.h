#ifndef VALIDACIONES_H
#define VALIDACIONES_H

////// Códigos enteros de resultado y estados para las funciones de lectura y validación (tipo int)
#define LECTURA_OK                 0
#define LECTURA_FIN_ENTRADA        1
#define LECTURA_VACIA              2
#define LECTURA_NO_NUMERICA        3
#define LECTURA_TEXTO_EXTRA        4
#define LECTURA_DESBORDE           5
#define LECTURA_COMA_DECIMAL       6
#define LECTURA_NAN                7
#define LECTURA_INFINITO           8
#define LECTURA_FUERA_RANGO        9
#define LECTURA_VARIOS_CARACTERES  10

////// Parámetro de formato para el tamaño máximo de la línea de entrada (tipo int)
#define TAMANIO_LINEA 64

////// Prototipos de funciones para lectura segura de datos y manejo de interfaz (retornos tipo int, long, double y void)
int leerEntero(long *valor);

int leerReal(double *valor);

int leerCaracter(char *valor);

void mostrarErrorLectura(int estado);

int pedirReal(const char *mensaje, double minimo, double maximo, double *valor);

int pedirParametro(const char *nombre, const char *unidad,
                   double minimo, double maximo,
                   double valorActual, double *valor);

int pedirConfirmacion(const char *mensaje, int *confirma);

#endif