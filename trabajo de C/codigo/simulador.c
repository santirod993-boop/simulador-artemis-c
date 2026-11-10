/*
 * simulador.c - Modulo de Logica
 * Calculos fisicos, configuracion, simulacion por eventos,
 * reportes e historial del reingreso de Orion (Artemis, escenario 4).
 * Las funciones 'static' son privadas de este archivo: no estan en el .h.
 */
#include <stdio.h>        
#include <math.h>          
#include "simulador.h"
#include "validaciones.h"  

/* ---- Funciones privadas de este archivo ---- */
static int  eventoEvaluarAngulo(ResultadoMision *resultado, double angulo);
static void eventoFrenadoAtmosferico(ResultadoMision *resultado);
static void eventoAperturaParacaidas(ResultadoMision *resultado);
static void eventoAmerizaje(ResultadoMision *resultado);
static void mostrarReporteFallo(const ResultadoMision *resultado);
static void mostrarReporteExito(const ResultadoMision *resultado);
static const char *describirClasificacion(int clasificacion);

/* ---------------- CALCULOS FISICOS ---------------- */

double calcularTrabajoRozamiento(double masa,
                                 double velocidadInicial, double velocidadFinal,
                                 double alturaInicial,    double alturaFinal)
{
    double variacionCinetica;
    double variacionPotencial;

    variacionCinetica  = 0.5 * masa * (velocidadFinal * velocidadFinal
                                       - velocidadInicial * velocidadInicial);
    variacionPotencial = masa * G * (alturaFinal - alturaInicial);

    return variacionCinetica + variacionPotencial;
}

double calcularDesaceleracionMedia(double velocidadInicial, double velocidadFinal,
                                   double tiempo)
{
    return (velocidadFinal - velocidadInicial) / tiempo;
}

double convertirAG(double aceleracion)
{
    return fabs(aceleracion) / G;
}

double calcularFuerzaAgua(double masa, double velocidadImpacto, double profundidad)
{
    double peso;
    double fuerzaFrenado;

    peso          = masa * G;                                              /* sostener la capsula */
    fuerzaFrenado = masa * velocidadImpacto * velocidadImpacto / (2.0 * profundidad); /* frenarla */

    return peso + fuerzaFrenado;
}

double calcularPeso(double masa)
{
    return masa * G;
}

double calcularMultiploDelPeso(double fuerza, double masa)
{
    return fuerza / calcularPeso(masa);
}

int clasificarAngulo(double angulo)
{
    int resultado;

    if (angulo < ANGULO_MINIMO_GRADOS) {
        resultado = REINGRESO_REBOTE;
    } else if (angulo > ANGULO_MAXIMO_GRADOS) {
        resultado = REINGRESO_EXCESIVO;
    } else {
        resultado = REINGRESO_SEGURO;
    }
    return resultado;
}

/* ---------------- CONFIGURACION ---------------- */

void inicializarConfiguracion(Configuracion *configuracion)
{
    configuracion->masa                = MASA_REFERENCIA_KG;
    configuracion->velocidadInicial    = VELOCIDAD_INICIAL_REFERENCIA_MS;
    configuracion->velocidadParacaidas = VELOCIDAD_PARACAIDAS_REFERENCIA_MS;
    configuracion->alturaInicial       = ALTURA_INICIAL_REFERENCIA_M;
    configuracion->alturaDespliegue    = ALTURA_DESPLIEGUE_REFERENCIA_M;
    configuracion->tiempoFrenado       = TIEMPO_FRENADO_REFERENCIA_S;
    configuracion->velocidadImpacto    = VELOCIDAD_IMPACTO_REFERENCIA_MS;
    configuracion->profundidadAgua     = PROFUNDIDAD_AGUA_REFERENCIA_M;
}

void mostrarConfiguracion(const Configuracion *configuracion)
{
    printf("\n------ DATOS ACTUALES DE LA MISION (SI) ------\n");
    printf("Masa de la capsula ............ %10.2f kg   (editable)\n", configuracion->masa);
    printf("Velocidad inicial ............. %10.2f m/s\n",  configuracion->velocidadInicial);
    printf("Velocidad al abrir paracaidas . %10.2f m/s\n",  configuracion->velocidadParacaidas);
    printf("Altura inicial ................ %10.2f m\n",    configuracion->alturaInicial);
    printf("Altura de despliegue .......... %10.2f m\n",    configuracion->alturaDespliegue);
    printf("Tiempo de frenado ............. %10.2f s    (editable)\n", configuracion->tiempoFrenado);
    printf("Velocidad de impacto .......... %10.2f m/s  (editable)\n", configuracion->velocidadImpacto);
    printf("Profundidad en el agua ........ %10.2f m    (editable)\n", configuracion->profundidadAgua);
    printf("Gravedad (constante) .......... %10.2f m/s2\n", G);
    printf("Ventana segura de angulo ...... %.1f a %.1f grados (incluidos)\n",
           ANGULO_MINIMO_GRADOS, ANGULO_MAXIMO_GRADOS);
}

/*
 * Trabaja sobre una COPIA: *configuracion solo cambia si todo se cargo
 * bien y el usuario confirma con S. Si la carga se interrumpe, queda como estaba.
 */
int configurarParametros(Configuracion *configuracion)
{
    Configuracion nueva = *configuracion;   /* copia de trabajo */
    int confirma = 0;
    int estado;

    printf("\n------ CONFIGURAR PARAMETROS ------\n");
    printf("Usa punto decimal. Enter vacio conserva el valor actual.\n");

    estado = pedirParametro("Masa de la capsula", "kg", 0.0, MASA_MAXIMA_KG,
                            configuracion->masa, &nueva.masa);
    if (estado == LECTURA_OK) {
        estado = pedirParametro("Tiempo de frenado", "s", 0.0, TIEMPO_MAXIMO_S,
                                configuracion->tiempoFrenado, &nueva.tiempoFrenado);
    }
    if (estado == LECTURA_OK) {
        /* [DECISION DE DISENO] el impacto debe ser menor que la velocidad
           con la que se abren los paracaidas (los paracaidas frenan) */
        estado = pedirParametro("Velocidad de impacto", "m/s", 0.0,
                                configuracion->velocidadParacaidas,
                                configuracion->velocidadImpacto, &nueva.velocidadImpacto);
    }
    if (estado == LECTURA_OK) {
        estado = pedirParametro("Profundidad de frenado en el agua", "m", 0.0,
                                PROFUNDIDAD_MAXIMA_M,
                                configuracion->profundidadAgua, &nueva.profundidadAgua);
    }
    if (estado == LECTURA_OK) {
        printf("\nNuevos valores: masa %g kg, tiempo %g s, impacto %g m/s, profundidad %g m\n",
               nueva.masa, nueva.tiempoFrenado, nueva.velocidadImpacto, nueva.profundidadAgua);
        estado = pedirConfirmacion("Guardar los cambios? (S/N): ", &confirma);
    }

    if (estado == LECTURA_FIN_ENTRADA) {
        printf("\nCarga interrumpida: se conserva la configuracion anterior.\n");
        return estado;
    }

    if (confirma) {
        *configuracion = nueva;             /* recien aca cambia la configuracion de main */
        printf("Configuracion guardada.\n");
    } else {
        printf("Cambios descartados: se conserva la configuracion anterior.\n");
    }
    return LECTURA_OK;
}

/* ---------------- MISION ---------------- */

int iniciarMision(double angulo, const Configuracion *configuracion,
                  ResultadoMision *resultado)
{
    ResultadoMision vacio = {0};   /* todos los campos en 0 */

    *resultado = vacio;                              /* borra datos de un intento anterior */
    resultado->configuracionUsada = *configuracion;  /* foto de los parametros usados */
    resultado->exito = MISION_FALLIDA;               /* se vuelve EXITOSA solo al final */

    printf("\n=========== SIMULACION DE REINGRESO ===========\n");

    /* Evento 1: si el angulo no es seguro, el intento termina aca */
    if (eventoEvaluarAngulo(resultado, angulo) != REINGRESO_SEGURO) {
        mostrarReporteFallo(resultado);
        return MISION_FALLIDA;
    }

    /* Eventos 2, 3 y 4 en orden */
    eventoFrenadoAtmosferico(resultado);
    eventoAperturaParacaidas(resultado);
    eventoAmerizaje(resultado);

    resultado->exito = MISION_EXITOSA;
    mostrarReporteExito(resultado);
    return MISION_EXITOSA;
}

/*
 * Opcion 1 del menu: pide el angulo (modulo validaciones), simula
 * con iniciarMision y guarda el intento en el historial.
 */
int ejecutarMision(const Configuracion *configuracion,
                   ResultadoMision historial[], int *cantidad)
{
    ResultadoMision resultado;
    double angulo = 0.0;
    int    estado;

    printf("\nIniciando mision...\n");
    estado = pedirReal("Angulo de reingreso en grados (usa punto decimal, ej: 6.5): ",
                       ANGULO_FISICO_MINIMO, ANGULO_FISICO_MAXIMO, &angulo);
    if (estado == LECTURA_FIN_ENTRADA) {
        return estado;                      /* no hubo intento: no se guarda nada */
    }

    iniciarMision(angulo, configuracion, &resultado);

    if (guardarResultado(historial, cantidad, &resultado)) {
        printf("Intento guardado en el historial (%d/%d).\n", *cantidad, MAX_HISTORIAL);
    } else {
        printf("AVISO: historial lleno (%d/%d). Este intento no se guardo.\n",
               *cantidad, MAX_HISTORIAL);
    }
    return LECTURA_OK;
}

/* ---------------- EVENTOS ---------------- */

static int eventoEvaluarAngulo(ResultadoMision *resultado, double angulo)
{
    resultado->angulo        = angulo;
    resultado->clasificacion = clasificarAngulo(angulo);

    printf("[Evento 1/%d] Evaluacion del angulo de reingreso: %g grados -> ",
           TOTAL_EVENTOS, angulo);

    if (resultado->clasificacion == REINGRESO_SEGURO) {
        printf("SEGURO\n");
        resultado->eventosCompletados = 1;
    } else {
        printf("FUERA DE LA VENTANA\n");
    }
    return resultado->clasificacion;
}

static void eventoFrenadoAtmosferico(ResultadoMision *resultado)
{
    const Configuracion *config = &resultado->configuracionUsada;

    resultado->trabajoRozamiento = calcularTrabajoRozamiento(config->masa,
                                       config->velocidadInicial, config->velocidadParacaidas,
                                       config->alturaInicial, config->alturaDespliegue);
    resultado->aceleracionMedia  = calcularDesaceleracionMedia(config->velocidadInicial,
                                       config->velocidadParacaidas, config->tiempoFrenado);
    resultado->aceleracionEnG    = convertirAG(resultado->aceleracionMedia);

    printf("[Evento 2/%d] Frenado atmosferico: de %.1f m/s a %.1f m/s en %.1f s\n",
           TOTAL_EVENTOS, config->velocidadInicial, config->velocidadParacaidas,
           config->tiempoFrenado);
    resultado->eventosCompletados = 2;
}

static void eventoAperturaParacaidas(ResultadoMision *resultado)
{
    const Configuracion *config = &resultado->configuracionUsada;

    printf("[Evento 3/%d] Apertura de paracaidas a %.1f m de altura con %.1f m/s\n",
           TOTAL_EVENTOS, config->alturaDespliegue, config->velocidadParacaidas);
    resultado->eventosCompletados = 3;
}

static void eventoAmerizaje(ResultadoMision *resultado)
{
    const Configuracion *config = &resultado->configuracionUsada;

    resultado->fuerzaAgua   = calcularFuerzaAgua(config->masa, config->velocidadImpacto,
                                                 config->profundidadAgua);
    resultado->pesoCapsula  = calcularPeso(config->masa);
    resultado->multiploPeso = calcularMultiploDelPeso(resultado->fuerzaAgua, config->masa);

    printf("[Evento 4/%d] Amerizaje: impacto a %.1f m/s, hundimiento de %.2f m\n",
           TOTAL_EVENTOS, config->velocidadImpacto, config->profundidadAgua);
    resultado->eventosCompletados = 4;
}

/* ---------------- REPORTES ---------------- */

/* Devuelve un texto fijo que describe la clasificacion del angulo. */
static const char *describirClasificacion(int clasificacion)
{
    const char *texto;

    switch (clasificacion) {
        case REINGRESO_REBOTE:
            texto = "angulo muy bajo -> rebote atmosferico (skip-out)";
            break;
        case REINGRESO_EXCESIVO:
            texto = "angulo muy alto -> desaceleracion y calentamiento excesivos";
            break;
        case REINGRESO_SEGURO:
            texto = "angulo dentro de la ventana segura";
            break;
        default:
            texto = "clasificacion desconocida";
            break;
    }
    return texto;
}

static void mostrarReporteFallo(const ResultadoMision *resultado)
{
    printf("\n------------- REPORTE: MISION FALLIDA -------------\n");
    printf("Angulo de reingreso ........... %g grados\n", resultado->angulo);
    printf("Ventana segura ................ %.1f a %.1f grados\n",
           ANGULO_MINIMO_GRADOS, ANGULO_MAXIMO_GRADOS);
    printf("Causa: %s\n", describirClasificacion(resultado->clasificacion));

    if (resultado->clasificacion == REINGRESO_REBOTE) {
        printf("El aire es poco denso, el arrastre no alcanza y la capsula vuelve al espacio.\n");
    } else {
        printf("La capsula llega al aire denso a muy alta velocidad: picos de g y de calor.\n");
    }
    printf("Riesgo para la tripulacion .... CRITICO\n");
    printf("Eventos completados ........... %d de %d (fallo en el evento 1)\n",
           resultado->eventosCompletados, TOTAL_EVENTOS);
    printf("No se ejecutaron: frenado, paracaidas ni amerizaje.\n");
    printf("---------------------------------------------------\n");
}

static void mostrarReporteExito(const ResultadoMision *resultado)
{
    const Configuracion *config = &resultado->configuracionUsada;

    printf("\n------------- REPORTE: MISION EXITOSA -------------\n");
    printf("Angulo de reingreso ........... %g grados (seguro)\n", resultado->angulo);
    printf("Masa / tiempo de frenado ...... %.2f kg / %.2f s\n",
           config->masa, config->tiempoFrenado);
    printf("Trabajo del rozamiento ........ %.4e J\n", resultado->trabajoRozamiento);
    printf("Aceleracion media ............. %.4f m/s2\n", resultado->aceleracionMedia);
    printf("Aceleracion media en g ........ %.4f g\n", resultado->aceleracionEnG);
    printf("Paracaidas .................... %.1f m de altura, %.1f m/s\n",
           config->alturaDespliegue, config->velocidadParacaidas);
    printf("Impacto / hundimiento ......... %.2f m/s / %.2f m\n",
           config->velocidadImpacto, config->profundidadAgua);
    printf("Fuerza promedio del agua ...... %.2f N\n", resultado->fuerzaAgua);
    printf("Peso de la capsula ............ %.2f N\n", resultado->pesoCapsula);
    printf("Fuerza del agua / peso ........ %.4f\n", resultado->multiploPeso);
    printf("Eventos completados ........... %d de %d\n",
           resultado->eventosCompletados, TOTAL_EVENTOS);
    printf("---------------------------------------------------\n");
}

/* ---------------- HISTORIAL ---------------- */

int guardarResultado(ResultadoMision historial[], int *cantidad,
                     const ResultadoMision *resultado)
{
    if (*cantidad >= MAX_HISTORIAL) {
        return 0;                                    /* lleno: no se escribe nada */
    }

    historial[*cantidad] = *resultado;               /* copia el registro completo */
    historial[*cantidad].numeroIntento = *cantidad + 1;
    (*cantidad)++;                                   /* actualiza el contador de main */
    return 1;
}

void mostrarHistorial(const ResultadoMision historial[], int cantidad)
{
    int i;
    int exitosas = 0;

    printf("\n------------- HISTORIAL DE INTENTOS (%d/%d) -------------\n",
           cantidad, MAX_HISTORIAL);

    if (cantidad == 0) {
        printf("Todavia no hay intentos registrados.\n");
        return;
    }

    printf("%-4s %-8s %-10s %-7s %s\n", "Nro", "Angulo", "Masa[kg]", "Estado", "Detalle");

    for (i = 0; i < cantidad; i++) {
        printf("%-4d %-8g %-10.1f ", historial[i].numeroIntento, historial[i].angulo,
               historial[i].configuracionUsada.masa);

        if (historial[i].exito == MISION_EXITOSA) {
            exitosas++;
            printf("%-7s %.2f g media, agua %.2f N (%.2f x peso)\n", "EXITO",
                   historial[i].aceleracionEnG, historial[i].fuerzaAgua,
                   historial[i].multiploPeso);
        } else {
            printf("%-7s %s\n", "FALLO",
                   describirClasificacion(historial[i].clasificacion));
        }
    }

    printf("Exitosas: %d   Fallidas: %d\n", exitosas, cantidad - exitosas);
    if (cantidad == MAX_HISTORIAL) {
        printf("AVISO: el historial esta lleno; los proximos intentos no se guardaran.\n");
    }
}