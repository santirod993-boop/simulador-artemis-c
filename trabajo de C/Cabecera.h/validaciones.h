/*
 * validaciones.h - Modulo de Validaciones e Interfaz
 * Ingreso seguro de enteros, reales y caracteres, y mensajes de error.
 */
#ifndef VALIDACIONES_H
#define VALIDACIONES_H

/* ---- Resultados posibles de las funciones de lectura ---- */
#define LECTURA_OK                 0
#define LECTURA_FIN_ENTRADA        1   /* Ctrl+D (Linux/Mac) o Ctrl+Z + Enter (Windows) */
#define LECTURA_VACIA              2
#define LECTURA_NO_NUMERICA        3
#define LECTURA_TEXTO_EXTRA        4
#define LECTURA_DESBORDE           5
#define LECTURA_COMA_DECIMAL       6
#define LECTURA_NAN                7
#define LECTURA_INFINITO           8
#define LECTURA_FUERA_RANGO        9
#define LECTURA_VARIOS_CARACTERES  10

#define TAMANIO_LINEA 64

/* ---- Lectura de UNA linea y conversion (no repiten la pregunta) ---- */

/* Lee un entero. Guarda en *valor solo si devuelve LECTURA_OK. */
int leerEntero(long *valor);

/* Lee un real (punto decimal). Rechaza texto, coma, NaN e infinito. */
int leerReal(double *valor);

/* Lee exactamente un caracter que no sea espacio. */
int leerCaracter(char *valor);

/* Muestra el mensaje que corresponde a un codigo LECTURA_... */
void mostrarErrorLectura(int estado);

/* ---- Preguntas completas (repiten hasta tener un dato valido) ---- */

/*
 * Pide un real con  minimo < valor < maximo.
 * Devuelve LECTURA_OK (valor en *valor) o LECTURA_FIN_ENTRADA.
 */
int pedirReal(const char *mensaje, double minimo, double maximo, double *valor);

/*
 * Igual que pedirReal, pero Enter vacio conserva valorActual.
 * Se usa para editar parametros de la configuracion.
 */
int pedirParametro(const char *nombre, const char *unidad,
                   double minimo, double maximo,
                   double valorActual, double *valor);

/*
 * Pide S o N (mayuscula o minuscula).
 * Deja *confirma en 1 si respondio S y en 0 si respondio N.
 * Devuelve LECTURA_OK o LECTURA_FIN_ENTRADA.
 */
int pedirConfirmacion(const char *mensaje, int *confirma);

#endif /* VALIDACIONES_H */
