#include "../include/args.h"
#include "../include/dotenv.h"
#include "../include/util.h"
#include <cstring>
#include <string>
#include <cstring>
using namespace std;

int autenticarUsuario(vector<usuario>& usuarios, string usuarioIngresado, string passwordIngresada);
int solicitarOpcion(char* perfil);

int main(int argc, char* argv[]) {
    args::ArgumentParser parser("Programa principal del sistema creado para INFO198.");
    args::HelpFlag help(parser, "help", "Muestra este menú de ayuda", {'h', "help"});
    args::Group obligatorio(parser, "ARGUMENTOS OBLIGATORIOS:", args::Group::Validators::All);
    args::ValueFlag<string> usuarioIngresado(obligatorio, "user", "El nombre de usuario", {'u', "user"});
    args::ValueFlag<string> passwordIngresada(obligatorio, "user", "La contraseña del usuario", {'p', "password"});
    args::ValueFlag<string> rutaArchivoIngresada(obligatorio, "file", "El archivo a analizar", {'f', "file"});

    try {
        parser.ParseCLI(argc, argv);
    } catch (args::Help) {
        std::cout << parser;
        return 0;
    } catch (args::ParseError e) {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        return 1;
    } catch (args::ValidationError e) {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        return 1;
    }

    dotenv env(".env");
    vector<usuario> usuarios;
    string archivoUsuarios = env.get("USER_FILE");

    cargarUsuarios(usuarios, archivoUsuarios);
    int indiceUsuario = autenticarUsuario(usuarios, args::get(usuarioIngresado), args::get(passwordIngresada));

    if (indiceUsuario == -1) {
        cout << "(ERROR) Usuario o contraseña incorrectos." << endl;
        return 1;
    }

    usuario u = usuarios[indiceUsuario];

    while (true) {
        cout << endl;
        cout << "---= SISTEMA DE USUARIOS =---" << endl;
        cout << "Usuario: " << args::get(usuarioIngresado) << endl;
        cout << endl;
        cout << "0) Salir" << endl;
        cout << "1) Administrador de usuarios" << endl;
        cout << "2) Multiplicación de matrices cuadradas" << endl;
        cout << "3) Juego" << endl;
        cout << "4) ¿Es palíndromo?" << endl;
        cout << "5) Calcula f(x) = x² + 2x + 8" << endl;
        cout << "6) Conteo sobre texto" << endl;
        cout << endl;

        int opcionInt = solicitarOpcion(u.perfil);
        cout << endl;

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

int autenticarUsuario(vector<usuario>& usuarios, string usuarioIngresado, string passwordIngresada) {
    for (int i = 0; i < usuarios.size(); i++) {
        usuario u = usuarios[i];

        if (strcmp(u.username, usuarioIngresado.c_str()) == 0 && strcmp(u.password, passwordIngresada.c_str()) == 0) {
            return i;
        }
    }

    return -1;
}

int solicitarOpcion(char* perfil) {
    string opcion = "0";

    while (true) {
        cout << "Ingrese opción (debe ser número): ";
        cin >> opcion;

        if (esEntero(opcion)) {
            if (std::stoi(opcion) == 1 && strcmp(perfil, "GENERAL") == 0) {
                cout << "(ERROR) Usted no tiene permisos para usar esta opción. Intente con otra." << endl;
                continue;
            }

            break;
        }

        cout << "(ERROR) Opción inválida. Intente nuevamente." << endl;
    }

    return std::stoi(opcion);
}
