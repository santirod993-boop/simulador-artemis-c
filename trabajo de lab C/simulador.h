#ifndef SIMULADOR_H
#define SIMULADOR_H

////// Constante física de aceleración de gravedad (tipo double)
#define G 9.81

////// Restricciones y rangos numéricos para los ángulos de reingreso (tipo double)
#define ANGULO_MINIMO_GRADOS  5.5
#define ANGULO_MAXIMO_GRADOS  7.0
#define ANGULO_FISICO_MINIMO  0.0
#define ANGULO_FISICO_MAXIMO  90.0

////// Códigos de estado enteros para la clasificación del ángulo de reingreso (tipo int)
#define REINGRESO_REBOTE      1
#define REINGRESO_SEGURO      2
#define REINGRESO_EXCESIVO    3

////// Valores numéricos de referencia iniciales según la consigna (tipo double)
#define MASA_REFERENCIA_KG                 10400.0
#define VELOCIDAD_INICIAL_REFERENCIA_MS    11000.0
#define VELOCIDAD_PARACAIDAS_REFERENCIA_MS 25.0
#define ALTURA_INICIAL_REFERENCIA_M        120000.0
#define ALTURA_DESPLIEGUE_REFERENCIA_M     2500.0
#define TIEMPO_FRENADO_REFERENCIA_S        400.0
#define VELOCIDAD_IMPACTO_REFERENCIA_MS    8.0
#define PROFUNDIDAD_AGUA_REFERENCIA_M      1.50

////// Límites máximos permitidos para los parámetros editables del sistema (tipo double)
#define MASA_MAXIMA_KG               100000.0
#define VELOCIDAD_INICIAL_MAXIMA_MS  20000.0
#define ALTURA_INICIAL_MAXIMA_M      200000.0
#define TIEMPO_MAXIMO_S              3600.0
#define PROFUNDIDAD_MAXIMA_M         10.0

////// Indicadores de estado de la misión y gestión de eventos e historiales (tipo int)
#define MISION_EXITOSA  1
#define MISION_FALLIDA  0
#define TOTAL_EVENTOS   4
#define MAX_HISTORIAL   10

////// Constantes matemáticas y de puntaje para la evaluación del juego (tipo double e int)
#define ANGULO_CENTRO_GRADOS     ((ANGULO_MINIMO_GRADOS + ANGULO_MAXIMO_GRADOS) / 2.0)
#define ANGULO_SEMIANCHO_GRADOS  ((ANGULO_MAXIMO_GRADOS - ANGULO_MINIMO_GRADOS) / 2.0)
#define PUNTAJE_MAXIMO           100
#define PUNTAJE_MINIMO_EXITO     50

////// Estructuras de datos personalizadas (Tipos: Configuracion, ResultadoMision y EstadisticasMision con double e int)
typedef struct {
    double masa;
    double velocidadInicial;
    double velocidadParacaidas;
    double alturaInicial;
    double alturaDespliegue;
    double tiempoFrenado;
    double velocidadImpacto;
    double profundidadAgua;
} Configuracion;

typedef struct {
    int           numeroIntento;
    int           exito;
    double        angulo;
    int           clasificacion;
    int           eventosCompletados;
    Configuracion configuracionUsada;
    double        trabajoRozamiento;
    double        aceleracionMedia;
    double        aceleracionEnG;
    double        fuerzaAgua;
    double        pesoCapsula;
    double        multiploPeso;
    int           puntaje;
} ResultadoMision;

typedef struct {
    int    total;
    int    exitosas;
    int    rebotes;
    int    excesivos;
    int    puntajeTotal;
    double porcentajeExito;
    double puntajePromedio;
    int    indiceMejor;
    double maximaAceleracionEnG;
    double maximoMultiploPeso;
} EstadisticasMision;

////// Prototipos de funciones operativas, cálculos físicos y gestión de puntajes/reportes (retornos tipo double, int y void)
double calcularTrabajoRozamiento(double masa,
                                 double velocidadInicial, double velocidadFinal,
                                 double alturaInicial,    double alturaFinal);

double calcularDesaceleracionMedia(double velocidadInicial, double velocidadFinal,
                                   double tiempo);

double convertirAG(double aceleracion);

double calcularFuerzaAgua(double masa, double velocidadImpacto, double profundidad);

double calcularPeso(double masa);

double calcularMultiploDelPeso(double fuerza, double masa);

int clasificarAngulo(double angulo);

void inicializarConfiguracion(Configuracion *configuracion);

void mostrarConfiguracion(const Configuracion *configuracion);

int configurarParametros(Configuracion *configuracion);

int iniciarMision(double angulo, const Configuracion *configuracion,
                  ResultadoMision *resultado);

int ejecutarMision(const Configuracion *configuracion,
                   ResultadoMision historial[], int *cantidad);

int guardarResultado(ResultadoMision historial[], int *cantidad,
                     const ResultadoMision *resultado);

void mostrarHistorial(const ResultadoMision historial[], int cantidad);

int calcularPuntaje(double angulo, int clasificacion);

void calcularEstadisticas(const ResultadoMision historial[], int cantidad,
                          EstadisticasMision *estadisticas);

void mostrarReporteFinal(const ResultadoMision historial[], int cantidad);

#endif