# Makefile - Simulador de Misiones Artemis (escenario 4: reingreso de Orion)
#
# Uso:
#   make          compila el programa
#   make run      compila (si hace falta) y ejecuta
#   make clean    borra los archivos generados
#
# En Windows con MinGW (Code::Blocks) usar mingw32-make desde cmd.

CC        = gcc
CFLAGS    = -std=c99 -Wall -Wextra -pedantic
LDLIBS    = -lm

FUENTES   = main.c simulador.c validaciones.c
CABECERAS = simulador.h validaciones.h
OBJETOS   = $(FUENTES:.c=.o)

ifeq ($(OS),Windows_NT)
    EJECUTABLE = artemis.exe
    EJECUTAR   = artemis.exe
    BORRAR     = del /Q
else
    EJECUTABLE = artemis
    EJECUTAR   = ./artemis
    BORRAR     = rm -f
endif

all: $(EJECUTABLE)

# Enlaza los .o en el ejecutable
$(EJECUTABLE): $(OBJETOS)
	$(CC) $(CFLAGS) -o $@ $(OBJETOS) $(LDLIBS)

# Compila cada .c en su .o (se recompila si cambia el .c o algun .h)
%.o: %.c $(CABECERAS)
	$(CC) $(CFLAGS) -c $<

run: $(EJECUTABLE)
	$(EJECUTAR)

clean:
	$(BORRAR) $(OBJETOS) $(EJECUTABLE)

.PHONY: all run clean
