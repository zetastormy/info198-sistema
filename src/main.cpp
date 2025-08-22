#include <iostream>
#include <string>
#include <cctype>
using namespace std;

struct usuario crearUsuario();
bool esIdValido(string id);

struct usuario {
    int id;
    string nombre;
    string perfil;
};

int main(int argn, char* argv[]) {
    cout << "Sistema de usuarios" << endl;
    cout << "1. Crear usuario" << endl;

    int opcion = 0;

    cout << "Ingrese su opción (debe ser número): ";

    //crearUsuario();

    return 0;
}

struct usuario crearUsuario() {
    struct usuario nuevoUsuario;
    string idUsuario = "";

    do {
        cout << "Ingrese ID (debe ser número): ";
        cin >> idUsuario;
    } while (!esIdValido(idUsuario));

    nuevoUsuario.id = std::stoi(idUsuario);

    cout << "Ingrese nombre: ";
    cin >> nuevoUsuario.nombre;

    cout << "Ingrese perfil: ";
    cin >> nuevoUsuario.perfil;

    return nuevoUsuario;
}

bool esIdValido(string id) {
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
