#include <iostream>
#include <string>
#include <cctype>
using namespace std;

int solicitarOpcion();
struct usuario crearUsuario();
bool esEntero(string id);

struct usuario {
    int id;
    string nombre;
    string perfil;
};

int main(int argn, char* argv[]) {
    cout << "Sistema de usuarios" << endl;
    cout << "1. Crear usuario" << endl;

    bool opcionValida;

    do {
        int opcionInt = solicitarOpcion();
        opcionValida = true;

        switch (opcionInt) {
            case 1:
                crearUsuario();
                break;
            default:
                cout << "Opción inválida" << endl;
                opcionValida = false;
                break;
        }
    } while (!opcionValida);

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

struct usuario crearUsuario() {
    struct usuario nuevoUsuario;
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

    return nuevoUsuario;
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
