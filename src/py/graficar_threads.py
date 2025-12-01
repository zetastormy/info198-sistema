#!/usr/bin/env python3
import os
from pathlib import Path

import matplotlib.pyplot as plt
from dotenv import load_dotenv
from matplotlib.ticker import (
    MaxNLocator,  # para forzar que los valores del ejex sean enteros
)

load_dotenv()

entrada = Path(os.getenv("LOG_RENDIMIENTO"))
salida = Path(os.getenv("CARPETA_GRAFICOS"))
nombre_archivo = "tiempo_vs_threads.png"


def leer_registros(
    ruta_logs: Path,
):  # leer el archivo de logs y devolver una lista de tuplas (hilos, tiempos) para el gráfico
    ejecuciones = []
    hilos = []
    tiempos = []

    for linea_original in ruta_logs.read_text(encoding="utf-8").splitlines():
        linea = linea_original.strip()

        if not linea:
            if hilos:
                ejecuciones.append((hilos, tiempos))
                hilos, tiempos = [], []
            continue

        try:
            parte_hilos, parte_tiempo = linea.split(",")
            valor_hilos = parte_hilos.split(":")[1].strip()
            valor_tiempo = parte_tiempo.split(":")[1].strip()
            hilos.append(int(valor_hilos))
            tiempos.append(int(valor_tiempo))
        except (IndexError, ValueError) as error:
            raise ValueError(f"Formato inválido en la línea: '{linea}'") from error

    if hilos:
        ejecuciones.append((hilos, tiempos))

    if not ejecuciones:
        raise ValueError("El archivo no contiene datos válidos")
    return ejecuciones


def dibujar_grafico(ejecuciones, ruta_salida: Path):
    plt.figure()
    for indice, (hilos, tiempos) in enumerate(ejecuciones, start=1):
        etiqueta = f"Ejecución {indice}" if len(ejecuciones) > 1 else None
        plt.plot(hilos, tiempos, marker="o", label=etiqueta)
    plt.xlabel("Hilos")
    plt.ylabel("Tiempo (ms)")
    plt.title("Tiempo vs Threads")
    plt.grid(True, linestyle="--", alpha=0.5)
    plt.gca().xaxis.set_major_locator(
        MaxNLocator(integer=True)
    )  # forzar que los valores del ejex sean enteros

    if len(ejecuciones) > 1:
        plt.legend()
    plt.tight_layout()
    ruta_salida.parent.mkdir(parents=True, exist_ok=True)
    plt.savefig(ruta_salida, bbox_inches="tight")
    plt.close()


def main():
    pid = os.getpid()

    print(
        f"---= GENERADOR DE GRÁFICO DE RENDIMIENTO DE ÍNDICE INVERTIDO PARALELO (PID: {pid}) =---"
    )

    print("Generando gráfico de tiempo vs threads...")
    ruta_salida = (
        salida / nombre_archivo
    )  # junta la ruta de salida con el nombre del archivo

    if not entrada.exists():
        raise FileNotFoundError(f"No se encontró el archivo: {entrada}")

    ejecuciones = leer_registros(entrada)
    dibujar_grafico(ejecuciones, ruta_salida)
    print(f"Gráfico guardado en {ruta_salida}")


if __name__ == "__main__":
    main()
