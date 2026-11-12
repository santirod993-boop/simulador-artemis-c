@echo off
REM compilar.bat - Compila y ejecuta el Simulador Artemis en Windows (cmd)
REM Uso: doble clic, o escribir  compilar  en una ventana de cmd abierta en esta carpeta.

REM Si gcc no esta en el PATH, se prueba con el MinGW que trae Code::Blocks
where gcc >nul 2>nul
if errorlevel 1 set "PATH=C:\Program Files\CodeBlocks\MinGW\bin;%PATH%"

where gcc >nul 2>nul
if errorlevel 1 (
    echo No se encontro gcc. Instala Code::Blocks con MinGW o agrega gcc al PATH.
    pause
    exit /b 1
)

gcc -std=c99 -Wall -Wextra -pedantic main.c simulador.c validaciones.c -o artemis.exe -lm
if errorlevel 1 (
    echo.
    echo Error de compilacion. Revisa los mensajes de arriba.
    pause
    exit /b 1
)

echo Compilacion exitosa. Ejecutando...
echo.
artemis.exe
pause
