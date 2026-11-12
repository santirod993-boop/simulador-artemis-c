/* simulador.c - Modulo de Logica y Calculos Fisicos */
#include <stdio.h>
#include <math.h>
#include "simulador.h"
#include "validaciones.h"

static int  eventoEvaluarAngulo(ResultadoMision *resultado, double angulo);
static void eventoFrenadoAtmosferico(ResultadoMision *resultado);
static void eventoAperturaParacaidas(ResultadoMision *resultado);
static void eventoAmerizaje(ResultadoMision *resultado);
static void mostrarReporteFallo(const ResultadoMision *resultado);
static void mostrarReporteExito(const ResultadoMision *resultado);
static const char *describirClasificacion(int clasificacion);
static void mostrarParametros(const char *titulo, const Configuracion *configuracion);
static void mostrarFilaParametro(const char *nombre, double valor, double referencia);
static const char *describirRendimiento(double puntajePromedio);

/* ---- CALCULOS FISICOS ---- */

double calcularTrabajoRozamiento(double masa, double velocidadInicial, double velocidadFinal, double alturaInicial, double alturaFinal)
{
    double variacionCinetica = 0.5 * masa * (velocidadFinal * velocidadFinal - velocidadInicial * velocidadInicial);
    double variacionPotencial = masa * G * (alturaFinal - alturaInicial);
    return variacionCinetica + variacionPotencial; /* Trabajo no conservativo = dEc + dEp */
}

double calcularDesaceleracionMedia(double velocidadInicial, double velocidadFinal, double tiempo)
{
    return (velocidadFinal - velocidadInicial) / tiempo;
}

double convertirAG(double aceleracion)
{
    return fabs(aceleracion) / G;
}

double calcularFuerzaAgua(double masa, double velocidadImpacto, double profundidad)
{
    double peso = masa * G;
    double fuerzaFrenado = (masa * velocidadImpacto * velocidadImpacto) / (2.0 * profundidad);
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
    if (angulo < ANGULO_MINIMO_GRADOS) return REINGRESO_REBOTE;
    if (angulo > ANGULO_MAXIMO_GRADOS) return REINGRESO_EXCESIVO;
    return REINGRESO_SEGURO;
}

/* ---- CONFIGURACION ---- */

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

static void mostrarParametros(const char *titulo, const Configuracion *configuracion)
{
    Configuracion referencia;
    inicializarConfiguracion(&referencia);

    printf("\n------ %s ------\n", titulo);
    printf("%-36s %10s   %10s\n", "Parametro", "Valor", "Referencia");
    mostrarFilaParametro("Masa de la capsula [kg]", configuracion->masa, referencia.masa);
    mostrarFilaParametro("Velocidad inicial [m/s]", configuracion->velocidadInicial, referencia.velocidadInicial);
    mostrarFilaParametro("Velocidad al abrir paracaidas [m/s]", configuracion->velocidadParacaidas, referencia.velocidadParacaidas);
    mostrarFilaParametro("Velocidad de impacto [m/s]", configuracion->velocidadImpacto, referencia.velocidadImpacto);
    mostrarFilaParametro("Altura inicial [m]", configuracion->alturaInicial, referencia.alturaInicial);
    mostrarFilaParametro("Altura de despliegue [m]", configuracion->alturaDespliegue, referencia.alturaDespliegue);
    mostrarFilaParametro("Tiempo de frenado [s]", configuracion->tiempoFrenado, referencia.tiempoFrenado);
    mostrarFilaParametro("Profundidad en el agua [m]", configuracion->profundidadAgua, referencia.profundidadAgua);
}

static void mostrarFilaParametro(const char *nombre, double valor, double referencia)
{
    printf("%-36s %10.2f   %10.2f %s\n", nombre, valor, referencia, (valor != referencia) ? "*" : "");
}

void mostrarConfiguracion(const Configuracion *configuracion)
{
    mostrarParametros("DATOS ACTUALES DE LA MISION (SI)", configuracion);
    printf("(*) modificado respecto de la consigna\n");
    printf("Gravedad (constante) ........... %.2f m/s2\n", G);
    printf("Ventana segura de angulo ....... %.1f a %.1f grados (incluidos)\n", ANGULO_MINIMO_GRADOS, ANGULO_MAXIMO_GRADOS);
}

int configurarParametros(Configuracion *configuracion)
{
    Configuracion nueva = *configuracion;
    int confirma = 0;
    int estado;

    printf("\n------ CONFIGURAR PARAMETROS ------\n");
    estado = pedirConfirmacion("Restaurar los valores de la consigna? (S/N): ", &confirma);
    if (estado == LECTURA_OK && confirma) {
        inicializarConfiguracion(configuracion);
        printf("Se restauraron los valores de la consigna.\n");
        return LECTURA_OK;
    }

    if (estado == LECTURA_OK) {
        printf("Usa punto decimal. Enter vacio conserva el valor actual.\n");
        estado = pedirParametro("Masa de la capsula", "kg", 0.0, MASA_MAXIMA_KG, nueva.masa, &nueva.masa);
    }
    if (estado == LECTURA_OK) {
        estado = pedirParametro("Velocidad inicial de reingreso", "m/s", 0.0, VELOCIDAD_INICIAL_MAXIMA_MS, nueva.velocidadInicial, &nueva.velocidadInicial);
    }
    if (estado == LECTURA_OK) {
        estado = pedirParametro("Velocidad al abrir paracaidas", "m/s", 0.0, nueva.velocidadInicial, nueva.velocidadParacaidas, &nueva.velocidadParacaidas);
    }
    if (estado == LECTURA_OK) {
        estado = pedirParametro("Velocidad de impacto en el agua", "m/s", 0.0, nueva.velocidadParacaidas, nueva.velocidadImpacto, &nueva.velocidadImpacto);
    }
    if (estado == LECTURA_OK) {
        estado = pedirParametro("Altura inicial de reingreso", "m", 0.0, ALTURA_INICIAL_MAXIMA_M, nueva.alturaInicial, &nueva.alturaInicial);
    }
    if (estado == LECTURA_OK) {
        estado = pedirParametro("Altura de despliegue de paracaidas", "m", 0.0, nueva.alturaInicial, nueva.alturaDespliegue, &nueva.alturaDespliegue);
    }
    if (estado == LECTURA_OK) {
        estado = pedirParametro("Tiempo de frenado", "s", 0.0, TIEMPO_MAXIMO_S, nueva.tiempoFrenado, &nueva.tiempoFrenado);
    }
    if (estado == LECTURA_OK) {
        estado = pedirParametro("Profundidad de frenado en el agua", "m", 0.0, PROFUNDIDAD_MAXIMA_M, nueva.profundidadAgua, &nueva.profundidadAgua);
    }
    if (estado == LECTURA_OK) {
        mostrarParametros("NUEVOS VALORES (todavia sin guardar)", &nueva);
        estado = pedirConfirmacion("Guardar los cambios? (S/N): ", &confirma);
    }

    if (estado == LECTURA_FIN_ENTRADA) {
        printf("\nCarga interrumpida: se conserva la configuracion anterior.\n");
        return estado;
    }

    if (confirma) {
        *configuracion = nueva;
        printf("Configuracion guardada.\n");
    } else {
        printf("Cambios descartados: se conserva la configuracion anterior.\n");
    }
    return LECTURA_OK;
}

/* ---- MISION Y EVENTOS ---- */

int iniciarMision(double angulo, const Configuracion *configuracion, ResultadoMision *resultado)
{
    ResultadoMision vacio = {0};
    *resultado = vacio;
    resultado->configuracionUsada = *configuracion;
    resultado->exito = MISION_FALLIDA;

    printf("\n=========== SIMULACION DE REINGRESO ===========\n");

    if (eventoEvaluarAngulo(resultado, angulo) != REINGRESO_SEGURO) {
        mostrarReporteFallo(resultado);
        return MISION_FALLIDA;
    }

    eventoFrenadoAtmosferico(resultado);
    eventoAperturaParacaidas(resultado);
    eventoAmerizaje(resultado);

    resultado->exito = MISION_EXITOSA;
    resultado->puntaje = calcularPuntaje(angulo, resultado->clasificacion);
    mostrarReporteExito(resultado);
    return MISION_EXITOSA;
}

int ejecutarMision(const Configuracion *configuracion, ResultadoMision historial[], int *cantidad)
{
    ResultadoMision resultado;
    double angulo = 0.0;
    int estado;

    printf("\nIniciando mision...\n");
    estado = pedirReal("Angulo de reingreso en grados (usa punto decimal, ej: 6.5): ", ANGULO_FISICO_MINIMO, ANGULO_FISICO_MAXIMO, &angulo);
    if (estado == LECTURA_FIN_ENTRADA) return estado;

    iniciarMision(angulo, configuracion, &resultado);

    if (guardarResultado(historial, cantidad, &resultado)) {
        printf("Intento guardado en el historial (%d/%d).\n", *cantidad, MAX_HISTORIAL);
    } else {
        printf("AVISO: historial lleno (%d/%d). Este intento no se guardo.\n", *cantidad, MAX_HISTORIAL);
    }
    return LECTURA_OK;
}

static int eventoEvaluarAngulo(ResultadoMision *resultado, double angulo)
{
    resultado->angulo = angulo;
    resultado->clasificacion = clasificarAngulo(angulo);

    printf("[Evento 1/%d] Evaluacion del angulo de reingreso: %g grados -> ", TOTAL_EVENTOS, angulo);
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
    resultado->trabajoRozamiento = calcularTrabajoRozamiento(config->masa, config->velocidadInicial, config->velocidadParacaidas, config->alturaInicial, config->alturaDespliegue);
    resultado->aceleracionMedia = calcularDesaceleracionMedia(config->velocidadInicial, config->velocidadParacaidas, config->tiempoFrenado);
    resultado->aceleracionEnG = convertirAG(resultado->aceleracionMedia);

    printf("[Evento 2/%d] Frenado atmosferico: de %.1f m/s a %.1f m/s en %.1f s\n", TOTAL_EVENTOS, config->velocidadInicial, config->velocidadParacaidas, config->tiempoFrenado);
    resultado->eventosCompletados = 2;
}

static void eventoAperturaParacaidas(ResultadoMision *resultado)
{
    const Configuracion *config = &resultado->configuracionUsada;
    printf("[Evento 3/%d] Apertura de paracaidas a %.1f m de altura con %.1f m/s\n", TOTAL_EVENTOS, config->alturaDespliegue, config->velocidadParacaidas);
    resultado->eventosCompletados = 3;
}

static void eventoAmerizaje(ResultadoMision *resultado)
{
    const Configuracion *config = &resultado->configuracionUsada;
    resultado->fuerzaAgua = calcularFuerzaAgua(config->masa, config->velocidadImpacto, config->profundidadAgua);
    resultado->pesoCapsula = calcularPeso(config->masa);
    resultado->multiploPeso = calcularMultiploDelPeso(resultado->fuerzaAgua, config->masa);

    printf("[Evento 4/%d] Amerizaje: impacto a %.1f m/s, hundimiento de %.2f m\n", TOTAL_EVENTOS, config->velocidadImpacto, config->profundidadAgua);
    resultado->eventosCompletados = 4;
}

/* ---- REPORTES E HISTORIAL ---- */

static const char *describirClasificacion(int clasificacion)
{
    switch (clasificacion) {
        case REINGRESO_REBOTE: return "angulo muy bajo -> rebote atmosferico (skip-out)";
        case REINGRESO_EXCESIVO: return "angulo muy alto -> desaceleracion y calentamiento excesivos";
        case REINGRESO_SEGURO: return "angulo dentro de la ventana segura";
        default: return "clasificacion desconocida";
    }
}

static void mostrarReporteFallo(const ResultadoMision *resultado)
{
    printf("\n------------- REPORTE: MISION FALLIDA -------------\n");
    printf("Angulo de reingreso ........... %g grados\n", resultado->angulo);
    printf("Ventana segura ................ %.1f a %.1f grados\n", ANGULO_MINIMO_GRADOS, ANGULO_MAXIMO_GRADOS);
    printf("Causa: %s\n", describirClasificacion(resultado->clasificacion));
    printf("Riesgo para la tripulacion .... CRITICO\n");
    printf("Eventos completados ........... %d de %d (fallo en el evento 1)\n", resultado->eventosCompletados, TOTAL_EVENTOS);
    printf("PUNTAJE ....................... 0 / %d\n", PUNTAJE_MAXIMO);
    printf("---------------------------------------------------\n");
}

static void mostrarReporteExito(const ResultadoMision *resultado)
{
    const Configuracion *config = &resultado->configuracionUsada;
    printf("\n------------- REPORTE: MISION EXITOSA -------------\n");
    printf("Angulo de reingreso ........... %g grados (seguro)\n", resultado->angulo);
    printf("Trabajo del rozamiento ........ %.4e J\n", resultado->trabajoRozamiento);
    printf("Aceleracion media ............. %.4f m/s2 (%.4f g)\n", resultado->aceleracionMedia, resultado->aceleracionEnG);
    printf("Fuerza promedio del agua ...... %.2f N (%.4f x peso)\n", resultado->fuerzaAgua, resultado->multiploPeso);
    printf("Eventos completados ........... %d de %d\n", resultado->eventosCompletados, TOTAL_EVENTOS);
    printf("PUNTAJE ....................... %d / %d\n", resultado->puntaje, PUNTAJE_MAXIMO);
    printf("---------------------------------------------------\n");
}

int guardarResultado(ResultadoMision historial[], int *cantidad, const ResultadoMision *resultado)
{
    if (*cantidad >= MAX_HISTORIAL) return 0;
    historial[*cantidad] = *resultado;
    historial[*cantidad].numeroIntento = *cantidad + 1;
    (*cantidad)++;
    return 1;
}

void mostrarHistorial(const ResultadoMision historial[], int cantidad)
{
    int exitosas = 0;
    printf("\n------------- HISTORIAL DE INTENTOS (%d/%d) -------------\n", cantidad, MAX_HISTORIAL);
    if (cantidad == 0) {
        printf("Todavia no hay intentos registrados.\n");
        return;
    }
    printf("%-4s %-8s %-10s %-7s %-4s %s\n", "Nro", "Angulo", "Masa[kg]", "Estado", "Pts", "Detalle");
    for (int i = 0; i < cantidad; i++) {
        printf("%-4d %-8g %-10.1f ", historial[i].numeroIntento, historial[i].angulo, historial[i].configuracionUsada.masa);
        if (historial[i].exito == MISION_EXITOSA) {
            exitosas++;
            printf("%-7s %-4d %.2f g media, agua %.2f N\n", "EXITO", historial[i].puntaje, historial[i].aceleracionEnG, historial[i].fuerzaAgua);
        } else {
            printf("%-7s %-4d %s\n", "FALLO", historial[i].puntaje, describirClasificacion(historial[i].clasificacion));
        }
    }
    printf("Exitosas: %d   Fallidas: %d\n", exitosas, cantidad - exitosas);
}

int calcularPuntaje(double angulo, int clasificacion)
{
    if (clasificacion != REINGRESO_SEGURO) return 0;
    double distancia = fabs(angulo - ANGULO_CENTRO_GRADOS);
    double precision = 1.0 - distancia / ANGULO_SEMIANCHO_GRADOS;
    return (int)(PUNTAJE_MINIMO_EXITO + (PUNTAJE_MAXIMO - PUNTAJE_MINIMO_EXITO) * precision + 0.5);
}

void calcularEstadisticas(const ResultadoMision historial[], int cantidad, EstadisticasMision *estadisticas)
{
    EstadisticasMision vacio = {0};
    *estadisticas = vacio;
    estadisticas->total = cantidad;
    estadisticas->indiceMejor = -1;

    for (int i = 0; i < cantidad; i++) {
        estadisticas->puntajeTotal += historial[i].puntaje;
        if (historial[i].exito == MISION_EXITOSA) {
            estadisticas->exitosas++;
            if (estadisticas->indiceMejor == -1 || historial[i].puntaje > historial[estadisticas->indiceMejor].puntaje) {
                estadisticas->indiceMejor = i;
            }
            if (historial[i].aceleracionEnG > estadisticas->maximaAceleracionEnG) {
                estadisticas->maximaAceleracionEnG = historial[i].aceleracionEnG;
            }
            if (historial[i].multiploPeso > estadisticas->maximoMultiploPeso) {
                estadisticas->maximoMultiploPeso = historial[i].multiploPeso;
            }
        } else if (historial[i].clasificacion == REINGRESO_REBOTE) {
            estadisticas->rebotes++;
        } else {
            estadisticas->excesivos++;
        }
    }

    if (cantidad > 0) {
        estadisticas->porcentajeExito = 100.0 * estadisticas->exitosas / cantidad;
        estadisticas->puntajePromedio = (double)estadisticas->puntajeTotal / cantidad;
    }
}

static const char *describirRendimiento(double puntajePromedio)
{
    if (puntajePromedio >= 90.0) return "EXCELENTE - comandante de mision";
    if (puntajePromedio >= 75.0) return "MUY BUENO - piloto experto";
    if (puntajePromedio >= 60.0) return "BUENO - piloto";
    if (puntajePromedio >= 50.0) return "REGULAR - piloto en entrenamiento";
    return "INSUFICIENTE - repetir el entrenamiento";
}

void mostrarReporteFinal(const ResultadoMision historial[], int cantidad)
{
    EstadisticasMision estadisticas;
    calcularEstadisticas(historial, cantidad, &estadisticas);

    printf("\n============ REPORTE FINAL DE DESEMPENO ============\n");
    if (estadisticas.total == 0) {
        printf("No se registraron misiones.\n");
        printf("====================================================\n");
        return;
    }
    printf("Intentos registrados ......... %d\n", estadisticas.total);
    printf("Misiones exitosas ............ %d (%.1f %%)\n", estadisticas.exitosas, estadisticas.porcentajeExito);
    printf("Puntaje promedio ............. %.1f por intento\n", estadisticas.puntajePromedio);
    printf("Rendimiento .................. %s\n", describirRendimiento(estadisticas.puntajePromedio));
    printf("====================================================\n");
}
