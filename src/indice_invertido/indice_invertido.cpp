#include "../include/util.h"
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <filesystem>
#include <sstream>
#include <chrono>
#include <sys/types.h>
#include <unistd.h>

using namespace std;
using namespace std::chrono;

bool crearIndiceInvertido(string nombreIndice, string rutaLibros);
wchar_t normalizar(wchar_t c);

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

    if (!crearIndiceInvertido(nombreIndice, rutaLibros)) {
        esperarTecla();
        exit(EXIT_FAILURE);
    }

    auto end = high_resolution_clock::now();
    auto duration = duration_cast<seconds>(end - start);
    cout << "Se ha creado el índice invertido en " << duration.count() << " segundos." << endl;

    return 0;
}

bool crearIndiceInvertido(string nombreIndice, string rutaLibros) {
    if (!filesystem::exists(rutaLibros)) {
        cout << "(ERROR) La ruta especificada hacia los libros no existe." << endl;
        return false;
    }

    string nombreRutaIndice = "data/" + nombreIndice + ".idx";

    filesystem::path rutaIndice(nombreRutaIndice);

    if (!exists(rutaIndice.parent_path())) {
        cout << "(INFO) El directorio no existía. Se creará: " << rutaIndice.parent_path() << endl;
        create_directories(rutaIndice.parent_path());
    }

    ofstream archivoIndice(nombreRutaIndice);

    if (!archivoIndice.is_open()) {
        cout << "(ERROR) Ocurrió un error al abrir el archivo de índice invertido." << endl;
        esperarTecla();
        return false;
    }

    unordered_map<string, unordered_map<int, int>> palabraContador;
    int contadorL = 1;
    for (const auto& rutaLibro : filesystem::directory_iterator(rutaLibros)) {
        ifstream libro(rutaLibro.path());

        string linea;
        while (getline(libro, linea)) {
            istringstream iss(linea);
            string palabra;

            while (iss >> palabra){
                string palabraNormalizada;

                for (wchar_t caracter : palabra) {
                    char c = normalizar(caracter);

                    if (isalpha(caracter)) {
                        palabraNormalizada += c;
                    }
                }

                string nombreArchivoLibro = rutaLibro.path().filename().replace_extension();

                cout << "Leyendo libro: " << nombreArchivoLibro << "\r";
                cout << flush;

                palabraContador[palabraNormalizada][contadorL]++;
            }
        }

        libro.close();
        contadorL++;
    }

    for (const auto& palabra : palabraContador) {
        archivoIndice << palabra.first << ";";

        int i = 0;

        for (const auto& libro : palabra.second) {
            archivoIndice << "(" << libro.first << "," << libro.second;

            if (i == palabra.second.size() - 1) {
                archivoIndice << ")";
                break;
            }

            archivoIndice << ");";
            i++;
        }

        archivoIndice << endl;
    }

    return true;
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
