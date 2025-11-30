#include "../include/analisis_rendimiento.h"
#include "../include/dotenv.h"
#include <chrono>
#include <unistd.h>
#include <sys/wait.h>
#include <string>
#include <sys/types.h>
#include <unistd.h>
using namespace std;

void analizar_rendimiento();
void ejecutar_medir(int nThreads, string bin, string rutaLibros);
void saveLog(string archivoLogs, int cantThreads, std::chrono::milliseconds duracion );


void analizar_rendimiento(){
    dotenv env(".env");
    string binIndiceInvertidoParalelo = env.get("INDICE_INVET_PARALELO");

    int CANT_THREADS [5] = {1, 2, 3, 4, 5};
    int size = sizeof (CANT_THREADS)/sizeof(CANT_THREADS[0]);
    for (int i = 0 ; i < size ; i++){
        cout<<"Indice invertido con "<<CANT_THREADS[i]<<" hilo(s)"<<endl;
        int numThreads = CANT_THREADS[i];
        ejecutar_medir (numThreads, binIndiceInvertidoParalelo,"./resources/libros");
        cout<<endl;
    }
    ofstream archivo("data/logs2.txt", ios::app);
    archivo<<endl;
}

void ejecutar_medir(int nThreads, string bin, string rutaLibros){
    string nombreIndiceTemp = "test_perf_" + to_string(nThreads);
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
    cout << "(DEBUG) Duración en MS (bruto): " << duracion_ms.count() << " ms" << endl;
    saveLog("data/logs2.txt", nThreads, duracion_ms);

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
