#include "../include/analisis_rendimiento.h"
#include "../include/dotenv.h"
#include <chrono>
#include <unistd.h>
#include <sys/wait.h>
#include <string>
#include <sys/types.h>
#include <unistd.h>
using namespace std;

#define CANT_THREADS {1, 2, 3, 4, 5}

dotenv env (".env");
string rutaLog = env.get("LOG_RENDIMIENTO");
string binIndiceInvertidoParalelo = env.get("INDICE_INVET_PARALELO");

void analizar_rendimiento();
void ejecutar_medir(int nThreads, string bin, string rutaLibros);
void saveLog(string archivoLogs, int cantThreads, std::chrono::milliseconds duracion );


void analizar_rendimiento(){
    for (int numThreads : CANT_THREADS){
        cout<<"Indice invertido con "<<numThreads<<" hilo(s)"<<endl;
        ejecutar_medir (numThreads, binIndiceInvertidoParalelo,"./resources/libros");
        cout<<endl;
    }
    ofstream archivo(rutaLog, ios::app);
    archivo<<endl;
}

void ejecutar_medir(int nThreads, string bin, string rutaLibros){
    string nombreIndiceTemp = "test_" + to_string(nThreads);
    string comando = "./" + bin + " " + nombreIndiceTemp + " " + rutaLibros + " " + to_string(nThreads) ;
    auto start = std::chrono::high_resolution_clock::now();

    pid_t pid = fork();

    if (pid == 0) {
        system(comando.c_str());
        exit(0); // Terminar el proceso hijo
    } else if (pid > 0) {
        int status;
        waitpid(pid, &status, 0); // Espera a que el hijo termine
    } else {
        cerr << "(ERROR) Falló el fork para ejecutar la prueba." << endl;
        return;
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duracion_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    saveLog(rutaLog, nThreads, duracion_ms);

}

void saveLog(string archivoLogs, int cantThreads, std::chrono::milliseconds duracion ) {
    ofstream archivo(archivoLogs, ios::app);
    if (!archivo.is_open()) {
        cerr << "(ERROR) No se pudo abrir el archivo de logs: " << archivoLogs << endl;
        return;
    }

    archivo << "N_THREADS: " << cantThreads<< ", TIME: " << duracion.count()<< endl;
    archivo.close();
}
