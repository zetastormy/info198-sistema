#include "../include/util.h"
#include "../include/dotenv.h"
#include <algorithm>
#include <cstring>
#include <iostream>
#include <ostream>
#include <vector>
#include <string>
#include <chrono>
#include <sys/types.h>
#include <unistd.h>
using namespace std;
using namespace std::chrono;

void guardarUsuario(vector<usuario>& usuarios, string archivoUsuarios);
void eliminarUsuarioGuardado(vector<usuario>& usuarios, string archivoUsuarios);
int solicitarOpcion();
void crearUsuario(vector<usuario>& usuarios, string archivoUsuarios);
void listarUsuarios(const vector<usuario>& usuarios);
void eliminarUsuario(vector<usuario>& usuarios, string archivoUsuarios);

int main(int argc, char* argv[]) {
    dotenv env(".env");
    vector<usuario> usuarios;
    string archivoUsuarios = env.get("USER_FILE");

    cargarUsuarios(usuarios, archivoUsuarios);

    while (true) {
        cout << endl;
        cout << "---= SISTEMA DE USUARIOS (PID: " << getpid() << ") =---" << endl;
        cout << "0) Salir" << endl;
        cout << "1) Crear usuario" << endl;
        cout << "2) Listar usuarios" << endl;
        cout << "3) Eliminar usuario" << endl;
        cout << endl;

        int opcionInt = solicitarOpcion();
        cout << endl;

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

void guardarUsuario(usuario nuevoUsuario, string archivoUsuarios) {
    auto start = high_resolution_clock::now();
    ofstream archivo(archivoUsuarios, ios::binary | ios::app);

    if (!archivo.is_open()) {
        cout << "(ERROR) No se pudo abrir el archivo de usuarios." << endl;
        esperarTecla();
        return;
    }

    archivo.write((char*) &nuevoUsuario, sizeof(usuario));
    archivo.close();

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);

    cout << "(INFO) El usuario se guardó en: " << duration.count() << " microsegundos" << endl;
}

void eliminarUsuarioGuardado(vector<usuario>& usuarios, string archivoUsuarios) {
    auto start = high_resolution_clock::now();

    ofstream archivo(archivoUsuarios, ios::binary);

    if (!archivo.is_open()) {
        cout << "(ERROR) No se pudo abrir el archivo de usuarios." << endl;
        esperarTecla();
        return;
    }

    for (const usuario& u : usuarios) {
        archivo.write((char*) &u, sizeof(usuario));
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
    cout << "---= CREACIÓN DE USUARIO =---" << endl;

    usuario nuevoUsuario;
    int idUsuario = usuarios.empty() ? 1 : usuarios.back().id + 1;

    nuevoUsuario.id = idUsuario;

    while (true) {
        char nombreUsuario[21];
        cout << "Ingrese nombre: ";
        cin >> nombreUsuario;

        if (strlen(nombreUsuario) < 20) {
           strcpy(nuevoUsuario.nombre, nombreUsuario);
           break;
       }

       cout << "(ERROR) Nombre sobre los 20 caracteres. Intente nuevamente." << endl;
    }

    while (true) {
        char usernameUsuario[21];
        cout << "Ingrese nombre de usuario: ";
        cin >> usernameUsuario;

        if (strlen(usernameUsuario) < 20) {
            strcpy(nuevoUsuario.username, usernameUsuario);
            break;
        }

        cout << "(ERROR) Nombre de usuario sobre los 20 caracteres. Intente nuevamente." << endl;
    }

    while (true) {
        char passwordUsuario[21];
        cout << "Ingrese contraseña: ";
        cin >> passwordUsuario;

        if (strlen(passwordUsuario) < 20) {
            strcpy(nuevoUsuario.password, passwordUsuario);
            break;
        }

        cout << "(ERROR) Contraseña sobre los 20 caracteres. Intente nuevamente." << endl;
    }

    while (true) {
        char perfilUsuario[21];

        while (true) {
            cout << "Ingrese perfil (GENERAL o ADMIN): ";
            cin >> perfilUsuario;

            if (strlen(perfilUsuario) < 20) {
                break;
            }

            cout << "(ERROR) Perfil sobre los 20 caracteres. Intente nuevamente." << endl;
        }

        // Convertimos a mayúsculas
        for (int i = 0; perfilUsuario[i]; i++) {
            perfilUsuario[i] = toupper(perfilUsuario[i]);
        }

        if (strcmp(perfilUsuario, "GENERAL") == 0 || strcmp(perfilUsuario, "ADMIN") == 0) {
            strcpy(nuevoUsuario.perfil, perfilUsuario);
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

    cout << "---= ELIMINACIÓN DE USUARIO =---" << endl;

    string idUsuario = "";

    do {
        cout << "Ingrese ID del usuario a eliminar: ";
        cin >> idUsuario;
    } while (!esEntero(idUsuario));

    int id = std::stoi(idUsuario);

    auto it = std::find_if(usuarios.begin(), usuarios.end(), [id](const usuario& u) { return u.id == id; });

    if (it == usuarios.end()) {
        cout << "(ERROR) No se encontró ningún usuario con el ID especificado." << endl;
    } else if (strcmp(it -> perfil, "ADMIN") == 0) {
        cout << "(ERROR) No se puede eliminar un usuario con perfil ADMIN." << endl;
    } else {
        usuarios.erase(it);
        eliminarUsuarioGuardado(usuarios, archivoUsuarios);
        cout << "Usuario eliminado correctamente." << endl;
    }

    esperarTecla();
}
