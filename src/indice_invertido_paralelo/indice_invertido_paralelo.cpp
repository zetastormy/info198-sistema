#include "../include/util.h"
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <filesystem>
#include <sstream>
#include <chrono>
#include <sys/types.h>
#include <unistd.h>
#include <string>
#include <thread>
#include <queue>
#include <condition_variable>
#include <mutex>
#include "../include/dotenv.h"
using namespace std;
using namespace std::chrono;
namespace fs = filesystem;

dotenv env(".env");
int n_lote = stoi(env.get("N_LOTE"));
int n_hilos = stoi(env.get("N_THREADS"));
string rutaMapa = env.get("MAPA_LIBROS");

bool crearIndiceInvertido(string nombreIndice, string rutaLibros);
wchar_t normalizar(wchar_t c);
string obtenerNombreArchivo (int id);
void crearIndiceParalelo(const string &rutaLibros, const string &nombreIndice, int n_hilos, int n_lote);
void guardarLog(log nuevoLog, string archivoLogs);

int main(int argc, char** argv) {
   	if (argc != 3) {
		cout << "(ERROR) Debe ejecutarse como ./bin/indice_invertido <nombre archivo> \"<ruta directorio libros>\"" << endl;
		exit(EXIT_FAILURE);
	}

	string nombreIndice = argv[1];
	string rutaLibros = argv[2];


	cout << "---= CREACIÓN ÍNDICE INVERTIDO (PID: " << getpid() << ") =---" << endl;
	cout << "Por favor, espere unos segundos mientras se crea el índice invertido..." << endl;

	auto start = high_resolution_clock::now();

	crearIndiceParalelo(rutaLibros, nombreIndice, n_hilos, n_lote);

    auto end = high_resolution_clock::now();
    auto duration = duration_cast<seconds>(end - start);
    cout << "Se ha creado el índice invertido en " << duration.count() << " segundos." << endl;

    return 0;
}

void crearIndiceParalelo(const string &rutaLibros, const string &nombreIndice, int n_hilos, int n_lote) {
    if (!fs::exists(rutaLibros)) {
        cerr << "(ERROR) No existe la ruta de libros: " << rutaLibros << endl;
        return;
    }

    string nombreRutaIndice = "data/" + nombreIndice + ".idx";
    fs::path rutaIndice(nombreRutaIndice);
    if (!fs::exists(rutaIndice.parent_path()))
        fs::create_directories(rutaIndice.parent_path());

    ofstream archivoIndice(nombreRutaIndice);
    if (!archivoIndice.is_open()) {
        cerr << "(ERROR) No se pudo crear el índice: " << nombreRutaIndice << endl;
        return;
    }

    vector<int> idsLibros;
    for (const auto &entry : fs::directory_iterator(rutaLibros)) {
        if (entry.is_regular_file() && entry.path().extension() == ".txt") {
            string filename = entry.path().filename().string();
            idsLibros.push_back(idsLibros.size() + 1);
        }
    }

    int totalLibros = idsLibros.size();
    cout << "(INFO) Se detectaron " << totalLibros << " libros." << endl;

    queue<pair<int, int>> colaLotes;
    for (int inicio = 1; inicio <= totalLibros; inicio += n_lote) {
        int fin = min(inicio + n_lote - 1, totalLibros);
        colaLotes.push({inicio, fin});
    }

    mutex mtxCola;
    mutex mtxIndice;
    condition_variable cv;

    unordered_map<string, unordered_map<int, int>> palabraContador;

    auto worker = [&]() {
        while (true) {
            pair<int, int> lote;
            {
                unique_lock<mutex> lock(mtxCola);
                if (colaLotes.empty()) break;
                lote = colaLotes.front();
                colaLotes.pop();
            }

            unordered_map<string, unordered_map<int, int>> localContador;

            for (int i = lote.first; i <= lote.second; i++) {
                string nombreLibro = obtenerNombreArchivo(i);
                ifstream libro(rutaLibros + "/" + nombreLibro);
                if (!libro.is_open()) {
                    cerr << "(ADVERTENCIA) No se pudo abrir: " << nombreLibro << endl;
                    continue;
                }

                // 🔹 Log: inicio del procesamiento
                log registro;
                registro.idThread = hash<thread::id>{}(this_thread::get_id()); // convierte el id del hilo a un entero
                registro.idLibro = i;
                registro.inicio = time(nullptr);

                int contadorPalabras = 0;

                string linea;
                while (getline(libro, linea)) {
                    istringstream iss(linea);
                    string palabra;
                    while (iss >> palabra) {
                        string palabraNormalizada;
                        for (wchar_t caracter : palabra) {
                            char c = normalizar(caracter);
                            if (isalpha(caracter)) palabraNormalizada += c;
                        }
                        if (!palabraNormalizada.empty()) {
                            localContador[palabraNormalizada][i]++;
                            contadorPalabras++;
                        }
                    }
                }

                // 🔹 Log: término del procesamiento
                registro.cantPalabras = contadorPalabras;
                registro.fin = time(nullptr);

                // 🔹 Guardar log (usa tu función)
                {
                    lock_guard<mutex> lock(mtxIndice); // proteger escritura de logs
                    guardarLog(registro, "data/logs.txt");
                }
            }

            // 🔹 Actualizar mapa global
            {
                lock_guard<mutex> lock(mtxIndice);
                for (auto &par : localContador) {
                    for (auto &libro : par.second) {
                        palabraContador[par.first][libro.first] += libro.second;
                    }
                }
            }
        }
    };

    vector<thread> hilos;
    for (int i = 0; i < n_hilos; i++)
        hilos.emplace_back(worker);

    for (auto &h : hilos)
        h.join();

    for (const auto &palabra : palabraContador) {
        archivoIndice << palabra.first << ";";

        int i = 0;
        for (const auto &libro : palabra.second) {
            archivoIndice << "(" << libro.first << "," << libro.second << ")";
            if (i < (int)palabra.second.size() - 1) archivoIndice << ";";
            i++;
        }
        archivoIndice << endl;
    }
}

void guardarLog(log nuevoLog, string archivoLogs) {
    auto start = high_resolution_clock::now();

    ofstream archivo(archivoLogs, ios::app);
    if (!archivo.is_open()) {
        cerr << "(ERROR) No se pudo abrir el archivo de logs: " << archivoLogs << endl;
        return;
    }

    archivo << "ThreadID: " << nuevoLog.idThread
            << ", LibroID: " << nuevoLog.idLibro
            << ", Palabras: " << nuevoLog.cantPalabras
            << ", Inicio: " << nuevoLog.inicio
            << ", Fin: " << nuevoLog.fin
            << endl;

    archivo.close();

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << "(INFO) Log guardado en " << duration.count() << " microsegundos." << endl;
}


string obtenerNombreArchivo (int id){
    ifstream ReadFile(rutaMapa);
    if (!ReadFile.is_open()) {
            cout << "(ERROR) No se pudo abrir el archivo mapaLibros." << endl;
            return "";
        }
    string contenido;
    while(getline (ReadFile, contenido)){
        size_t pos = contenido.find(';');
        if (pos != string::npos){
            int idArchivo = stoi(contenido.substr(0,pos));

            if (idArchivo == id){
                string nombreArchivo = contenido.substr (pos+1) + ".txt";
                ReadFile.close();
                return nombreArchivo;
            }
        }
    }
    ReadFile.close();
    cout << "(ERROR) No se encontró el ID especificado." << endl;
    return "";
}

wchar_t normalizar(wchar_t c) {
    switch (c) {
        case L'á': case L'Á': return L'a';
        case L'é': case L'É': return L'e';
        case L'í': case L'Í': return L'i';
        case L'ó': case L'Ó': return L'o';
        case L'ú': case L'Ú': return L'u';
        default: return tolower(c);
    }
}
