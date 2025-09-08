#include "../include/util.h"
#include <iostream>
#include <chrono>
#include <filesystem>
#include <vector>
#include <cstring>
#include <fstream>
using namespace std;
using namespace std::chrono;

void esperarTecla() {
    cout << endl;
    cout << "Presiona ENTER para volver al menú principal...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

void cargarUsuarios(vector<usuario>& usuarios, string archivoUsuarios) {
    auto start = high_resolution_clock::now();

    filesystem::path ruta(archivoUsuarios);

    if (!exists(ruta.parent_path())) {
        cout << "(INFO) El directorio no existía. Se creará: " << ruta.parent_path() << endl;
        create_directories(ruta.parent_path());
    }

    if (!exists(ruta)) {
        cout << "(INFO) El archivo no existía. Se creará: " << archivoUsuarios << endl;
        ofstream nuevoArchivo(archivoUsuarios);
        nuevoArchivo.close();
    }

    ifstream archivo(archivoUsuarios, ios::binary);

    if (!archivo.is_open()) {
        cout << "(ERROR) No se pudo abrir el archivo de usuarios." << endl;
        esperarTecla();
        return;
    }

    usuario u;
    while (archivo.read((char*)&u, sizeof(usuario))) {
        if (strcmp(u.nombre, "empty") == 0) {
            break;
        }
        usuarios.push_back(u);
    }

    archivo.close();
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);

    cout << "(INFO) Los usuarios se cargaron en: " << duration.count() << " microsegundos" << endl;
}

bool esEntero(string id) {
    if (id.empty()) {
        return false;
    }

    for (size_t i = 0; i < id.length(); ++i) {
        if (!std::isdigit(id[i])) {
            return false;
        }
    }

    return true;
}
