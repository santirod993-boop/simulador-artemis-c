#ifndef SIMULADOR_H
#define SIMULADOR_H

////// Constantes físicas globales de la simulación (tipo double)
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

////// Valores numéricos de referencia inicial en sistema internacional (tipo double)
#define MASA_REFERENCIA_KG                 10400.0
#define VELOCIDAD_INICIAL_REFERENCIA_MS    11000.0
#define VELOCIDAD_PARACAIDAS_REFERENCIA_MS 25.0
#define ALTURA_INICIAL_REFERENCIA_M        120000.0
#define ALTURA_DESPLIEGUE_REFERENCIA_M     2500.0
#define TIEMPO_FRENADO_REFERENCIA_S        400.0
#define VELOCIDAD_IMPACTO_REFERENCIA_MS    8.0
#define PROFUNDIDAD_AGUA_REFERENCIA_M      1.50

////// Límites máximos permitidos para los parámetros editables del sistema (tipo double)
#define MASA_MAXIMA_KG        100000.0
#define TIEMPO_MAXIMO_S       3600.0
#define PROFUNDIDAD_MAXIMA_M  10.0

////// Indicadores de estado de la misión y gestión de eventos e historiales (tipo int)
#define MISION_EXITOSA  1
#define MISION_FALLIDA  0
#define TOTAL_EVENTOS   4
#define MAX_HISTORIAL   10

////// Estructuras de datos personalizadas (Tipos: Configuracion y ResultadoMision compuestos por double e int)
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
} ResultadoMision;

////// Prototipos de funciones operativas y de cálculo (retornos tipo double, int y void)
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

#endif
