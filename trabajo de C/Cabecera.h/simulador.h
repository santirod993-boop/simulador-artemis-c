/*
 * simulador.h - Modulo de Cabecera
 * Constantes, tipos de datos (struct/typedef) y prototipos publicos
 * de la simulacion del reingreso de Orion (Artemis, escenario 4).
 */
#ifndef SIMULADOR_H
#define SIMULADOR_H

/* ================= CONSTANTES FISICAS ================= */

/* Aceleracion de la gravedad [m/s2], constante en todo el descenso */
#define G 9.81

/* Ventana segura del angulo de reingreso [grados], extremos incluidos */
#define ANGULO_MINIMO_GRADOS  5.5
#define ANGULO_MAXIMO_GRADOS  7.0

/* [DECISION DE DISENO] rango fisicamente aceptado al ingresar el angulo: 0 < angulo < 90 */
#define ANGULO_FISICO_MINIMO  0.0
#define ANGULO_FISICO_MAXIMO  90.0

/* Resultados de clasificarAngulo */
#define REINGRESO_REBOTE      1
#define REINGRESO_SEGURO      2
#define REINGRESO_EXCESIVO    3

/* ================= VALORES DE REFERENCIA (consigna, SI) ================= */
#define MASA_REFERENCIA_KG                 10400.0
#define VELOCIDAD_INICIAL_REFERENCIA_MS    11000.0
#define VELOCIDAD_PARACAIDAS_REFERENCIA_MS 25.0
#define ALTURA_INICIAL_REFERENCIA_M        120000.0
#define ALTURA_DESPLIEGUE_REFERENCIA_M     2500.0
#define TIEMPO_FRENADO_REFERENCIA_S        400.0
#define VELOCIDAD_IMPACTO_REFERENCIA_MS    8.0
#define PROFUNDIDAD_AGUA_REFERENCIA_M      1.50

/* [DECISION DE DISENO] maximos de los parametros editables (minimo: mayor que 0) */
#define MASA_MAXIMA_KG        100000.0
#define TIEMPO_MAXIMO_S       3600.0
#define PROFUNDIDAD_MAXIMA_M  10.0

/* ================= MISION E HISTORIAL ================= */
#define MISION_EXITOSA  1
#define MISION_FALLIDA  0

#define TOTAL_EVENTOS   4

/* [DECISION DE DISENO] el historial guarda como maximo 10 intentos */
#define MAX_HISTORIAL   10

/* ================= TIPOS DE DATOS ================= */

/* Parametros fisicos de la mision */
typedef struct {
    double masa;                 /* [kg]   editable */
    double velocidadInicial;     /* [m/s] */
    double velocidadParacaidas;  /* [m/s] */
    double alturaInicial;        /* [m] */
    double alturaDespliegue;     /* [m] */
    double tiempoFrenado;        /* [s]    editable */
    double velocidadImpacto;     /* [m/s]  editable */
    double profundidadAgua;      /* [m]    editable */
} Configuracion;

/* Todo lo que produce un intento de mision */
typedef struct {
    int           numeroIntento;       /* 1, 2, 3... (lo asigna guardarResultado) */
    int           exito;               /* MISION_EXITOSA o MISION_FALLIDA */
    double        angulo;              /* [grados] */
    int           clasificacion;       /* REINGRESO_REBOTE / _SEGURO / _EXCESIVO */
    int           eventosCompletados;  /* 0 a TOTAL_EVENTOS */
    Configuracion configuracionUsada;  /* copia de los parametros de este intento */
    double        trabajoRozamiento;   /* [J]   (negativo) */
    double        aceleracionMedia;    /* [m/s2] (negativa) */
    double        aceleracionEnG;      /* [g]   (magnitud) */
    double        fuerzaAgua;          /* [N] */
    double        pesoCapsula;         /* [N] */
    double        multiploPeso;        /* adimensional */
} ResultadoMision;

/* ================= CALCULOS FISICOS ================= */
/* No validan: quien llama debe cumplir las precondiciones. */

/* Wroz = 0.5*m*(vf^2 - vi^2) + m*G*(hf - hi)   [J]
   Precondicion: masa > 0, vi > vf >= 0, hi > hf >= 0. Resultado negativo. */
double calcularTrabajoRozamiento(double masa,
                                 double velocidadInicial, double velocidadFinal,
                                 double alturaInicial,    double alturaFinal);

/* a = (vf - vi) / tiempo   [m/s2]   Precondicion: tiempo > 0. */
double calcularDesaceleracionMedia(double velocidadInicial, double velocidadFinal,
                                   double tiempo);

/* |aceleracion| / G   (adimensional, >= 0) */
double convertirAG(double aceleracion);

/* F = m*G + m*v^2 / (2*d)   [N]   Precondicion: masa > 0, profundidad > 0. */
double calcularFuerzaAgua(double masa, double velocidadImpacto, double profundidad);

/* Peso = m*G   [N] */
double calcularPeso(double masa);

/* fuerza / peso   (adimensional)   Precondicion: masa > 0. */
double calcularMultiploDelPeso(double fuerza, double masa);

/* Devuelve REINGRESO_REBOTE, REINGRESO_SEGURO o REINGRESO_EXCESIVO. */
int clasificarAngulo(double angulo);

/* ================= CONFIGURACION ================= */

/* Carga los valores de referencia de la consigna. */
void inicializarConfiguracion(Configuracion *configuracion);

/* Muestra los parametros actuales. */
void mostrarConfiguracion(const Configuracion *configuracion);

/*
 * Edita los parametros editables. Trabaja sobre una copia y solo
 * modifica *configuracion si el usuario confirma.
 * Devuelve LECTURA_OK o LECTURA_FIN_ENTRADA.
 */
int configurarParametros(Configuracion *configuracion);

/* ================= MISION ================= */

/*
 * Simula la mision por eventos con un angulo YA VALIDADO.
 * No lee del teclado. Escribe todo en *resultado y muestra el reporte.
 * Devuelve MISION_EXITOSA o MISION_FALLIDA.
 */
int iniciarMision(double angulo, const Configuracion *configuracion,
                  ResultadoMision *resultado);

/*
 * Opcion 1 del menu: pide el angulo, simula y guarda el intento.
 * Devuelve LECTURA_OK o LECTURA_FIN_ENTRADA.
 */
int ejecutarMision(const Configuracion *configuracion,
                   ResultadoMision historial[], int *cantidad);

/* ================= HISTORIAL ================= */

/* Copia *resultado en historial[*cantidad] y suma 1 a *cantidad.
   Si esta lleno no escribe nada. Devuelve 1 si guardo, 0 si estaba lleno. */
int guardarResultado(ResultadoMision historial[], int *cantidad,
                     const ResultadoMision *resultado);

/* Muestra los intentos guardados. */
void mostrarHistorial(const ResultadoMision historial[], int cantidad);

#endif /* SIMULADOR_H */
