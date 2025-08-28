#include "include/dotenv.h"
#include <algorithm>
#include <cstdio>
#include <iostream>
#include <limits>
#include <string>
#include <vector>
#include <sstream>
#include <filesystem>
#include <chrono>
using namespace std;
using namespace std::chrono;

struct usuario {
    int id;
    string nombre;
    string username;
    string password;
    string perfil;
};

void cargarDatos(vector<usuario>& usuarios, string archivoUsuarios);
void guardarUsuario(vector<usuario>& usuarios, string archivoUsuarios);
void eliminarUsuarioGuardado(vector<usuario>& usuarios, int id, string archivoUsuarios);
int solicitarOpcion();
string serializarUsuarios(const vector<usuario>& usuarios);
void crearUsuario(vector<usuario>& usuarios, string archivoUsuarios);
void listarUsuarios(const vector<usuario>& usuarios);
void eliminarUsuario(vector<usuario>& usuarios, string archivoUsuarios);
bool esEntero(string id);
void esperarTecla();

int main(int argn, char* argv[]) {
    dotenv env(".env");
    vector<usuario> usuarios;
    string archivoUsuarios = env.get("USER_FILE");

    cargarDatos(usuarios, archivoUsuarios);

    while (true) {
        cout << endl;
        cout << "---= SISTEMA DE USUARIOS =---" << endl;
        cout << "0) Salir" << endl;
        cout << "1) Crear usuario" << endl;
        cout << "2) Listar usuarios" << endl;
        cout << "3) Eliminar usuario" << endl;
        cout << endl;

        int opcionInt = solicitarOpcion();

        switch (opcionInt) {
            case 1:
                crearUsuario(usuarios, archivoUsuarios);
                break;
            case 2:
                listarUsuarios(usuarios);
                break;
            case 3:
                eliminarUsuario(usuarios, archivoUsuarios);
                break;
            case 0:
                cout << endl;
                cout << "¡Hasta pronto!" << endl;
                return 0;
            default:
                cout << "(ERROR) ¡Opción inválida!" << endl;
                esperarTecla();
                break;
        }
    }

    return 0;
}

void cargarDatos(vector<usuario>& usuarios, string archivoUsuarios) {
    auto start = high_resolution_clock::now();

    filesystem::path ruta(archivoUsuarios);

    if (!exists(ruta.parent_path())) {
        cout << "(INFO) El directorio no existía. Se creará: " << ruta.parent_path() << endl;
        filesystem::create_directories(ruta.parent_path());
    }

    if (!exists(ruta)) {
        cout << "(INFO) El archivo no existía. Se creará: " << archivoUsuarios << endl;
        ofstream nuevoArchivo(archivoUsuarios);
        nuevoArchivo.close();
    }

    ifstream archivo(archivoUsuarios);

    if (!archivo.is_open()) {
        cout << "(ERROR) No se pudo abrir el archivo de usuarios." << endl;
        esperarTecla();
        return;
    }

    string linea;
    while (getline(archivo, linea)) {
        usuario u;
        stringstream ss(linea); // Lee la línea actual y separa el stream por espacios
        ss >> u.id >> u.nombre >> u.username >> u.password >> u.perfil;
        usuarios.push_back(u);
    }

    archivo.close();
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);

    cout << "(INFO) Los usuarios se cargaron en: " << duration.count() << " microsegundos" << endl;
}

void guardarUsuario(usuario nuevoUsuario, string archivoUsuarios) {
    auto start = high_resolution_clock::now();
    ofstream archivo(archivoUsuarios, ios::app);

    if (!archivo.is_open()) {
        cout << "(ERROR) No se pudo abrir el archivo de usuarios." << endl;
        esperarTecla();
        return;
    }

    archivo << nuevoUsuario.id << " " << nuevoUsuario.nombre << " " << nuevoUsuario.username << " " << nuevoUsuario.password << " " << nuevoUsuario.perfil << endl;

    archivo.close();
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);

    cout << "(INFO) El usuario se guardó en: " << duration.count() << " microsegundos" << endl;
}

void eliminarUsuarioGuardado(vector<usuario>& usuarios, int id, string archivoUsuarios) {
    auto start = high_resolution_clock::now();
    ofstream archivo(archivoUsuarios);

    if (!archivo.is_open()) {
        cout << "(ERROR) No se pudo abrir el archivo de usuarios." << endl;
        esperarTecla();
        return;
    }

    for (const usuario& u : usuarios) {
        if (u.id != id) {
            archivo << u.id << " " << u.nombre << " " << u.username << " " << u.password << " " << u.perfil << endl;
        }
    }

    archivo.close();
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);

    cout << "(INFO) El usuario se eliminó en: " << duration.count() << " microsegundos" << endl;
}

int solicitarOpcion() {
    string opcion = "0";

    while (true) {
        cout << "Ingrese opción (debe ser número): ";
        cin >> opcion;

        if (esEntero(opcion)) {
            break;
        }

        cout << "(ERROR) Opción inválida. Intente nuevamente." << endl;
    }

    return std::stoi(opcion);
}

void crearUsuario(vector<usuario>& usuarios, string archivoUsuarios) {
    cout << endl;
    cout << "---= CREACIÓN DE USUARIO =---" << endl;

    usuario nuevoUsuario;
    int idUsuario = usuarios.empty() ? 1 : usuarios.back().id + 1;

    nuevoUsuario.id = idUsuario;

    cout << "Ingrese nombre: ";
    cin >> nuevoUsuario.nombre;

    cout << "Ingrese nombre de usuario: ";
    cin >> nuevoUsuario.username;

    cout << "Ingrese contraseña: ";
    cin >> nuevoUsuario.password;

    while (true) {
        string perfilUsuario = "";

        cout << "Ingrese perfil (GENERAL o ADMIN): ";
        cin >> perfilUsuario;

        if (perfilUsuario == "GENERAL" || perfilUsuario == "ADMIN") {
            nuevoUsuario.perfil = perfilUsuario;
            break;
        }

        cout << "(ERROR) Perfil inválido. Intente nuevamente." << endl;
    }

    cout << endl;
    cout << "---= RESUMEN USUARIO NUEVO =---" << endl;
    cout << "ID: " << nuevoUsuario.id << endl;
    cout << "Nombre: " << nuevoUsuario.nombre << endl;
    cout << "Nombre de usuario: " << nuevoUsuario.username << endl;
    cout << "Contraseña: " << nuevoUsuario.password << endl;
    cout << "Perfil: " << nuevoUsuario.perfil << endl;
    cout << endl;
    cout << "1) Guardar usuario" << endl;
    cout << "2) Cancelar" << endl;
    cout << "Seleccione una opción: ";

    int opcion = solicitarOpcion();

    switch (opcion) {
        case 1:
            guardarUsuario(nuevoUsuario, archivoUsuarios);
            usuarios.push_back(nuevoUsuario);
            break;
        case 2:
            cout << "Operación cancelada." << endl;
            break;
        default:
            cout << "(ERROR) ¡Opción inválida!" << endl;
            break;
    }
}

void listarUsuarios(const vector<usuario>& usuarios) {
    cout << endl;
    cout << "---= LISTADO DE USUARIOS =---" << endl;
    cout << "ID | Nombre | Perfil" << endl;

    if (usuarios.empty()) {
        cout << "No hay usuarios registrados." << endl;
        esperarTecla();
        return;
    }

    for (const usuario& u : usuarios) {
        cout << u.id << " | " << u.nombre << " | " << u.perfil << endl;
    }

    esperarTecla();
}

void eliminarUsuario(vector<usuario>& usuarios, string archivoUsuarios) {
    if (usuarios.empty()) {
        cout << "No hay usuarios registrados para eliminar." << endl;
        esperarTecla();
        return;
    }

    cout << endl;
    cout << "---= ELIMINACIÓN DE USUARIO =---" << endl;

    string idUsuario = "";

    do {
        cout << "Ingrese ID del usuario a eliminar: ";
        cin >> idUsuario;
    } while (!esEntero(idUsuario));

    int id = std::stoi(idUsuario);

    auto it = std::find_if(usuarios.begin(), usuarios.end(), [id](const usuario& u) { return u.id == id; });
    string perfil = it -> perfil;

    if (it == usuarios.end()) {
        cout << "(ERROR) No se encontró ningún usuario con el ID especificado." << endl;
    } else if (perfil == "ADMIN"){
        cout << "(ERROR) No se puede eliminar un usuario con perfil ADMIN." << endl;
    } else {
        eliminarUsuarioGuardado(usuarios, id, archivoUsuarios);
        usuarios.erase(it);
        cout << "Usuario eliminado correctamente." << endl;
    }

    esperarTecla();
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

void esperarTecla() {
    cout << endl;
    cout << "Presiona ENTER para volver al menú principal...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}
