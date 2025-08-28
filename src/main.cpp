#include <algorithm>
#include <cstdio>
#include <iostream>
#include <limits>
#include <string>
#include <cctype>
#include <vector>
using namespace std;

struct usuario {
    int id;
    string nombre;
    string username;
    string password;
    string perfil;
};

int solicitarOpcion();
string serializarUsuarios(const vector<usuario>& usuarios);
void crearUsuario(vector<usuario>& usuarios);
void listarUsuarios(const vector<usuario>& usuarios);
void eliminarUsuario(vector<usuario>& usuarios);
bool esEntero(string id);
void esperarTecla();

int main(int argn, char* argv[]) {
    vector<usuario> usuarios;
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
                crearUsuario(usuarios);
                break;
            case 2:
                listarUsuarios(usuarios);
                break;
            case 3:
                eliminarUsuario(usuarios);
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

int solicitarOpcion() {
    string opcion = "0";

    do {
        cout << "Ingrese opción (debe ser número): ";
        cin >> opcion;
    } while (!esEntero(opcion));

    return std::stoi(opcion);
}

string serializarUsuarios(const vector<usuario>& usuarios) {
    string resultado = "[\n";
    for (size_t i = 0; i < usuarios.size(); i++) {
        const usuario& u = usuarios[i];
        resultado += "  {\n";
        resultado += "    \"id\": " + to_string(u.id) + ",\n";
        resultado += "    \"nombre\": \"" + u.nombre + "\",\n";
        resultado += "    \"username\": \"" + u.username + "\",\n";
        resultado += "    \"password\": \"" + u.password + "\",\n";
        resultado += "    \"perfil\": \"" + u.perfil + "\"\n";
        resultado += "  }";
        if (i < usuarios.size() - 1) resultado += ",";
        resultado += "\n";
    }

    resultado += "]";

    return resultado;
}

void crearUsuario(vector<usuario>& usuarios) {
    cout << endl;
    cout << "---= CREACIÓN DE USUARIO =---" << endl;

    usuario nuevoUsuario;
    string idUsuario = "";

    while (true) {
        cout << "Ingrese ID (debe ser número): ";
        cin >> idUsuario;

        if (esEntero(idUsuario)) break;

        cout << "(ERROR) ID inválido. Intente nuevamente." << endl;
    }

    nuevoUsuario.id = std::stoi(idUsuario);

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

void eliminarUsuario(vector<usuario>& usuarios) {
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
