#include <iostream>
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
bool esEntero(string id);

int main(int argn, char* argv[]) {
    vector<usuario> usuarios;

    cout << "Sistema de usuarios" << endl;
    cout << "1. Crear usuario" << endl;
    cout << "2. Listar usuarios" << endl;
    cout << "3. Eliminar usuario" << endl;
    cout << "4. Modificar usuario" << endl;
    cout << "5. Salir" << endl;

    bool opcionValida;

    do {
        int opcionInt = solicitarOpcion();
        opcionValida = true;

        switch (opcionInt) {
            case 1:
                crearUsuario(usuarios);
                break;
            case 2:
                listarUsuarios(usuarios);
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
    for (const usuario& u : usuarios) {
        cout << "ID: " << u.id << ", Nombre: " << u.nombre << ", Perfil: " << u.perfil << endl;
    }
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
