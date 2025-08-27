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
        cout << "1. Crear usuario" << endl;
        cout << "2. Listar usuarios" << endl;
        cout << "3. Eliminar usuario" << endl;
        cout << "4. Modificar usuario" << endl;
        cout << "5. Salir" << endl;
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
            case 5:
                cout << endl;
                cout << "¡Hasta pronto!" << endl;
                return 0;
            default:
                cout << "¡Opción inválida!" << endl;
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

    do {
        cout << "Ingrese ID (debe ser número): ";
        cin >> idUsuario;
    } while (!esEntero(idUsuario));

    nuevoUsuario.id = std::stoi(idUsuario);

    cout << "Ingrese nombre: ";
    cin >> nuevoUsuario.nombre;

    cout << "Ingrese perfil: ";
    cin >> nuevoUsuario.perfil;

    usuarios.push_back(nuevoUsuario);
}

void listarUsuarios(const vector<usuario>& usuarios) {
    cout << endl;
    cout << "---= LISTADO DE USUARIOS =---" << endl;

    if (usuarios.empty()) {
        cout << "No hay usuarios registrados." << endl;
        esperarTecla();
        return;
    }

    for (const usuario& u : usuarios) {
        cout << "[*] ID: " << u.id << ", Nombre: " << u.nombre << ", Perfil: " << u.perfil << endl;
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

    if (it != usuarios.end()) {
        usuarios.erase(it);
        cout << "Usuario eliminado correctamente." << endl;
    } else {
        cout << "No se encontró ningún usuario con el ID especificado." << endl;
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
    cout << "Presiona ENTER para continuar...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}
