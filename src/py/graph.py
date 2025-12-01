import sys
import os
import re
import pandas as pd
import matplotlib.pyplot as plt
from datetime import datetime
import traceback

# --- CONFIGURACIÓN ---
NOMBRE_JUGADOR_OBJETIVO = "mayra"  # El nombre a buscar (substring)
ARCHIVO_OBJETIVO = "data/historial_global.txt" 

def log_debug(mensaje):
    """Log de depuración en data/debug_python.txt"""
    try:
        base_dir = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
        carpeta_data = os.path.join(base_dir, "data")
        if not os.path.exists(carpeta_data): os.makedirs(carpeta_data)
        
        archivo_debug = os.path.join(carpeta_data, "debug_python.txt")
        timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        with open(archivo_debug, "a", encoding="utf-8") as f:
            f.write(f"[{timestamp}] {mensaje}\n")
    except:
        pass

def leer_log_robust(ruta_log):
    """Lee el archivo arreglando líneas rotas por concurrencia"""
    datos_limpios = []
    patron_fecha = re.compile(r'^\d{4}-\d{2}-\d{2}') 
    buffer_linea = ""
    
    if not os.path.exists(ruta_log): return pd.DataFrame()

    with open(ruta_log, 'r', encoding='utf-8', errors='replace') as f:
        for linea in f:
            linea = linea.strip()
            if not linea or linea.startswith("----"): continue 
            
            if patron_fecha.match(linea):
                if buffer_linea:
                    datos_limpios.append(procesar_linea_raw(buffer_linea))
                buffer_linea = linea
            else:
                buffer_linea += " " + linea
        if buffer_linea:
            datos_limpios.append(procesar_linea_raw(buffer_linea))
            
    df = pd.DataFrame(datos_limpios, columns=["Timestamp", "EventType", "Details"])
    return df

def procesar_linea_raw(texto):
    partes = texto.split(',', 2) 
    if len(partes) < 3:
        return [partes[0] if len(partes)>0 else "", "UNKNOWN", ""]
    return [partes[0].strip(), partes[1].strip(), partes[2].strip()]

def procesar_log(ruta_log):
    log_debug("========================================")
    log_debug(f"GENERANDO ESTADÍSTICAS GLOBALES. Ruta: {ruta_log}")

    base_dir = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
    ruta_default = os.path.join(base_dir, "data/graphs/")
    output_dir = os.getenv('CARPETA_GRAFICOS', ruta_default)
    log_debug(f"Carpeta de salida configurada: {output_dir}")
    if not os.path.exists(output_dir): 
        try:
            os.makedirs(output_dir)
        except Exception as e:
            log_debug(f"ERROR: No se pudo crear la carpeta {output_dir}: {e}")
            return
    
    try:
        df = leer_log_robust(ruta_log)
        if df.empty:
            log_debug("El historial está vacío.")
            return
        log_debug(f"Total de eventos históricos: {len(df)}")
    except Exception as e:
        log_debug(f"Error leyendo historial: {e}")
        return

    # --- EXTRACCIÓN DE DATOS ---
    turnos_df = df[df['EventType'] == 'TURNO'].copy()
    datos_jugadas = []
    patron_turno = r"Turno de:\s*(.+?)\s*\((.+?)\).*?lanzó un\s*(\d+).*?Total del equipo.+?:\s*(\d+)"

    for idx, row in turnos_df.iterrows():
        match = re.search(patron_turno, str(row['Details']))
        if match:
            datos_jugadas.append({
                'Jugador': match.group(1).strip(),
                'Equipo': match.group(2).strip(),
                'Dado': int(match.group(3)),
                'Puntaje': int(match.group(4)),
                'Original_Index': idx 
            })

    df_jugadas = pd.DataFrame(datos_jugadas)
    
    # --- GRÁFICOS ---

    # 1. MVP Histórico (Top 5)
    if not df_jugadas.empty:
        try:
            plt.figure(figsize=(10, 6))
            top_mvp = df_jugadas.groupby('Jugador')['Dado'].sum().sort_values(ascending=False).head(5)
            colores = ['gold', 'silver', '#CD7F32', 'skyblue', 'lightblue'] 
            top_mvp.plot(kind='bar', color=colores[:len(top_mvp)], edgecolor='black')
            plt.title('Top 5 MVP Histórico (Distancia Total)')
            plt.ylabel('Suma de Dados')
            plt.xticks(rotation=45)
            plt.tight_layout()
            plt.savefig(os.path.join(output_dir, 'top5_mvp_historico.png'))
            plt.close()
        except Exception as e:
            log_debug(f"Error graficando MVP: {e}")

    # 2. Distribución Dado (1d6)
    if not df_jugadas.empty:
        try:
            plt.figure(figsize=(8, 5))
            conteos = df_jugadas['Dado'].value_counts().sort_index().reindex(range(1, 7), fill_value=0)
            colores = ['red' if x==1 else 'green' if x==6 else 'gray' for x in conteos.index]
            plt.bar(conteos.index, conteos.values, color=colores, edgecolor='black')
            plt.title('Distribución Histórica del Dado (1d6)')
            plt.xlabel('Cara del Dado')
            plt.xticks(range(1, 7))
            plt.savefig(os.path.join(output_dir, 'distribucion_dado_d6.png'))
            plt.close()
        except Exception as e:
            log_debug(f"Error graficando Dados: {e}")

    # 3. Carrera (Última Partida)
    indices_inicio = df[df['EventType'] == 'INICIO'].index
    if not indices_inicio.empty and not df_jugadas.empty:
        try:
            ultimo_inicio = indices_inicio[-1]
            df_carrera = df_jugadas[df_jugadas['Original_Index'] > ultimo_inicio].copy()
            if not df_carrera.empty:
                df_carrera['N_Turno'] = range(1, len(df_carrera) + 1)
                plt.figure(figsize=(10, 6))
                for eq in df_carrera['Equipo'].unique():
                    d = df_carrera[df_carrera['Equipo'] == eq]
                    plt.plot(d['N_Turno'], d['Puntaje'], marker='o', label=eq, linewidth=2)
                plt.title('Carrera (Última Partida)')
                plt.xlabel('Turnos')
                plt.ylabel('Puntaje')
                plt.legend()
                plt.savefig(os.path.join(output_dir, 'carrera_ultima_partida.png'))
                plt.close()
        except Exception as e:
            log_debug(f"Error graficando Carrera: {e}")

    # 4. Resumen Estadístico (GRÁFICO DE BARRAS)
    try:
        total_partidas = len(indices_inicio)
        total_turnos = len(df_jugadas)
        desconexiones = df['Details'].str.contains('desconectado', case=False, na=False).sum()
        
        # Conteo inteligente de veces que ganó la Mayra
        wins_mayra = 0
        obj = NOMBRE_JUGADOR_OBJETIVO.lower()
        for det in df['Details']:
            det_l = det.lower()
            if "victoria_player" in det_l and obj in det_l:
                wins_mayra += 1
            # Fallback para game_over
            elif "game_over" in det_l and obj in det_l: 
                wins_mayra += 1

        # Generar gráfico de barras
        categorias = ['Partidas', 'Turnos', 'Desconexiones', f'Victorias\n"{NOMBRE_JUGADOR_OBJETIVO}"']
        valores = [total_partidas, total_turnos, desconexiones, wins_mayra]
        
        plt.figure(figsize=(10, 6))
        bars = plt.bar(categorias, valores, color=['#3498db', '#e67e22', '#e74c3c', '#2ecc71'], edgecolor='black')
        
        plt.title('Resumen Estadístico Global', fontsize=14, weight='bold')
        plt.ylabel('Cantidad')
        plt.grid(axis='y', linestyle='--', alpha=0.7)
        
        # Etiquetas de valor encima de las barras
        for bar in bars:
            height = bar.get_height()
            plt.text(bar.get_x() + bar.get_width()/2., height + 0.1,
                     f'{int(height)}',
                     ha='center', va='bottom', fontsize=12, weight='bold')
        
        plt.tight_layout()
        plt.savefig(os.path.join(output_dir, 'resumen_estadistico.png'))
        plt.close()
    except Exception as e:
        log_debug(f"Error graficando Resumen: {e}")

    log_debug("¡GENERACIÓN COMPLETA!")

if __name__ == "__main__":
    try:
        arch = "data/historial_global.txt" 
        if len(sys.argv) > 1: arch = sys.argv[1]
        procesar_log(arch)
    except Exception as e:
        log_debug(f"CRASH: {e}")
        log_debug(traceback.format_exc())