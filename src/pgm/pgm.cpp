#include "../include/dotenv.h"
#include <vector>
#include <chrono>
#include <cstring>
#include <filesystem>
using namespace std;
using namespace std::chrono;

struct usuario {
    int id;
    char nombre[21];
    char username[21];
    char password[21];
    char perfil[21];
};

bool verificarArgumentos(int argc, char* argv[]);
void cargarUsuarios(vector<usuario>& usuarios, string archivoUsuarios);
int solicitarOpcion();
bool esEntero(string id);
void esperarTecla();

int main(int argc, char* argv[]) {
    if (argc < 6) {
        cout << "(ERROR) Argumentos insuficientes." << endl;
        cout << "Ejecute como ./bin/pgm -u <usuario> -p <contraseña> -f <ruta a texto a evaluar>" << endl;
        return 0;
    }

    if (!verificarArgumentos(argc, argv)) {
        return 0;
    }

    dotenv env(".env");
    vector<usuario> usuarios;
    string archivoUsuarios = env.get("USER_FILE");

    cargarUsuarios(usuarios, archivoUsuarios);


    while (true) {
        cout << endl;
        cout << "---= SISTEMA DE USUARIOS =---" << endl;
        cout << "0) Salir" << endl;
        cout << "1) Administrador de usuarios" << endl;
        cout << "2) Multiplicación de matrices cuadradas" << endl;
        cout << "3) Juego" << endl;
        cout << "4) ¿Es palíndromo?" << endl;
        cout << "5) Calcula f(x) = x² + 2x + 8" << endl;
        cout << "6) Conteo sobre texto" << endl;
        cout << endl;

        int opcionInt = solicitarOpcion();

        switch (opcionInt) {
            case 1:
                cout << "(ERROR) Opción en construcción." << endl;
                esperarTecla();
                break;
            case 2:
                cout << "(ERROR) Opción en construcción." << endl;
                esperarTecla();
                break;
            case 3:
                cout << "(ERROR) Opción en construcción." << endl;
                esperarTecla();
                break;
            case 4:
                // TODO: Llamar a método de palíndromo.
                cout << "LLAMADA A MÉTODO PRINCIPAL DE PALINDROMO" << endl;
                esperarTecla();
                break;
            case 5:
                // TODO: Llamar a método de f(x)....
                cout << "LLAMADA A MÉTODO PRINCIPAL DE CALCULA FUNCION" << endl;
                esperarTecla();
                break;
            case 6:
                // TODO: Llamar a método de conteo.
                cout << "LLAMADA A MÉTODO PRINCIPAL DE CONTEO SOBRE TEXTO" << endl;
                esperarTecla();
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
