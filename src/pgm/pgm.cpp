#include "../include/args.h"
#include "../include/dotenv.h"
#include "../include/util.h"
#include "../include/conteo.h"
#include "../include/calcula_funcion.h"
#include "../include/palindrome.h"
#include <cstring>
#include <string>
#include <cstring>
#include <sys/types.h>
#include <unistd.h>
#include <cstdlib>
#include <filesystem>
using namespace std;

int autenticarUsuario(vector<usuario>& usuarios, string usuarioIngresado, string passwordIngresada);
int solicitarOpcion(char* perfil);
void conectarMultiplicaMatrices(string binarioMultMatrices);
void conectarIndiceInvertido(string binarioIndiceInvertido);
void conectarIndiceInvertidoParalelo(string binarioIndiceInvertido);


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
    string binarioAdminUsuarios = env.get("ADMIN_SYS");
    string binarioMultMatrices = env.get("MULTI_M");
    string binarioIndiceInvertido = env.get("CREATE_INDEX");
    string binIndiceInvertidoParalelo = env.get("INDICE_INVET_PARALELO");

    cargarUsuarios(usuarios, archivoUsuarios);
    int indiceUsuario = autenticarUsuario(usuarios, args::get(usuarioIngresado), args::get(passwordIngresada));

    if (indiceUsuario == -1) {
        cout << "(ERROR) Usuario o contraseña incorrectos." << endl;
        return 1;
    }

    usuario u = usuarios[indiceUsuario];

    while (true) {
        cout << endl;
        cout << "---= PROGRAMA PRINCIPAL (PID: " << getpid() << ") =---" << endl;
        cout << "Usuario: " << args::get(usuarioIngresado) << endl;
        cout << endl;
        cout << "0) Salir" << endl;
        cout << "1) Administrador de usuarios" << endl;
        cout << "2) Multiplicación de matrices cuadradas" << endl;
        cout << "3) Juego" << endl;
        cout << "4) ¿Es palíndromo?" << endl;
        cout << "5) Calcula f(x) = x² + 2x + 8" << endl;
        cout << "6) Conteo sobre texto" << endl;
        cout << "7) Crea índice invertido" << endl;
        cout << "8) Crea índice invertido en paralelo"<<endl;
        cout << endl;

        int opcionInt = solicitarOpcion(u.perfil);
        cout << endl;

        switch (opcionInt) {
            case 1:
                system(("./" + binarioAdminUsuarios).c_str());
                break;
            case 2:
                conectarMultiplicaMatrices(binarioMultMatrices);
                break;
            case 3:
                cout << "(ERROR) Opción en construcción." << endl;
                esperarTecla();
                break;
            case 4:
                menuPalindromo();
                break;
            case 5:
                calcularFuncion();
                break;
            case 6:
                imprimirConteo(args::get(rutaArchivoIngresada));
                esperarTecla();
                break;
            case 7:
                conectarIndiceInvertido(binarioIndiceInvertido);
                break;
            case 8:
                conectarIndiceInvertidoParalelo(binIndiceInvertidoParalelo);
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

void conectarMultiplicaMatrices(string binarioMultMatrices) {
    cout << "---= CONEXIÓN A PROGRAMA DE MULTIPLICACIÓN DE MATRICES =---" << endl;

    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    string rutaMatrizA;
    while (true) {
        cout << "Ingrese ruta a matriz A: ";
        getline(cin, rutaMatrizA);

        if (filesystem::exists(rutaMatrizA)) {
            break;
        }

        cout << "(ERROR) No se pudo abrir el archivo en esa ruta. Intente nuevamente." << endl;
    }

    string rutaMatrizB;
    while (true) {
        cout << "Ingrese ruta a matriz B: ";
        getline(cin, rutaMatrizB);

        if (filesystem::exists(rutaMatrizB)) {
            break;
        }

        cout << "(ERROR) No se pudo abrir el archivo en esa ruta. Intente nuevamente." << endl;
    }

    string separador;
    cout << "Ingrese separador: ";
    cin >> separador;

    int opcion = 0;
    cout << "\nOpciones:\n1) Ejecutar\n2) Cancelar\nSeleccione una opción: ";
    cin >> opcion;

    switch(opcion){
        case 1:
            cout << endl;
            system(("./" + binarioMultMatrices + " \"" + rutaMatrizA + "\" \"" + rutaMatrizB + "\" \"" + separador + "\"").c_str());
            esperarTecla();
            break;
        case 2:
            cout << "Operación cancelada." << endl;
            break;
        default:
            cout << "(ERROR) ¡Opción inválida!" << endl;
            esperarTecla();
    }
}

void conectarIndiceInvertido(string binarioIndiceInvertido) {
    string nombreArchivo, rutaCarpeta;
    int opcion;

    cout << "---= CONEXIÓN A PROGRAMA DE CREACIÓN ÍNDICE INVERTIDO =---" << endl;

    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    cout << "Ingrese nombre del archivo a crear: ";
    getline(cin, nombreArchivo);
    cout << "Ingrese la ruta de la carpeta donde tiene los libros: ";
    getline(cin, rutaCarpeta);
    cout << endl;

    cout << "Opciones: " << endl;
    cout << "1) Validar " << endl;
    cout << "2) Cancelar "<< endl;
    cout << "Seleccione una opción: ";
    cin >> opcion;
    cout << endl;

    switch(opcion){
        case 1: {
            if (nombreArchivo.empty()) {
                cout << "(ERROR) El nombre del archivo no puede estar vacío." << endl;
                esperarTecla();
                break;
            }

            if (rutaCarpeta.empty()) {
                cout << "(ERROR) La ruta de libros no puede estar vacía." << endl;
                esperarTecla();
                break;
            }

            if (!filesystem::exists(rutaCarpeta)) {
                cout << "(ERROR) La carpeta '" << rutaCarpeta << "' no existe." << endl;
                esperarTecla();
                break;
            }

            string rutaArchivo = "data/"+ nombreArchivo +".idx";

            system(("./" + binarioIndiceInvertido + " " + nombreArchivo + " \"" + rutaCarpeta + "\"").c_str());
            esperarTecla();
            break;
        }
        case 2:
            cout << "Operación cancelada." << endl;
            break;
        default:
            cout << "(ERROR) ¡Opción inválida!" << endl;
            esperarTecla();
    }
}

void conectarIndiceInvertidoParalelo(string binIndiceInvertidoParalelo) {
    string nombreArchivo, rutaCarpeta;
    int opcion;

    cout << "---= CONEXIÓN A PROGRAMA DE CREACIÓN ÍNDICE INVERTIDO =---" << endl;

    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    cout << "Ingrese nombre del archivo a crear: ";
    getline(cin, nombreArchivo);
    cout << "Ingrese la ruta de la carpeta donde tiene los libros: ";
    getline(cin, rutaCarpeta);
    cout << endl;

    cout << "Opciones: " << endl;
    cout << "1) Validar " << endl;
    cout << "2) Cancelar "<< endl;
    cout << "Seleccione una opción: ";
    cin >> opcion;
    cout << endl;

    switch(opcion){
        case 1: {
            if (nombreArchivo.empty()) {
                cout << "(ERROR) El nombre del archivo no puede estar vacío." << endl;
                esperarTecla();
                break;
            }

            if (rutaCarpeta.empty()) {
                cout << "(ERROR) La ruta de libros no puede estar vacía." << endl;
                esperarTecla();
                break;
            }

            if (!filesystem::exists(rutaCarpeta)) {
                cout << "(ERROR) La carpeta '" << rutaCarpeta << "' no existe." << endl;
                esperarTecla();
                break;
            }

            string rutaArchivo = "data/"+ nombreArchivo +".idx";

            system(("./" + binIndiceInvertidoParalelo + " " + nombreArchivo + " \"" + rutaCarpeta + "\"").c_str());
            esperarTecla();
            break;
        }
        case 2:
            cout << "Operación cancelada." << endl;
            break;
        default:
            cout << "(ERROR) ¡Opción inválida!" << endl;
            esperarTecla();
    }
}
