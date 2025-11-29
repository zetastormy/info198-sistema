#include "../include/util.h"
#include <limits>
#include <string>
#include <iostream>
#include <unistd.h>
using namespace std;

int main() {
    string busqueda;
    int opcion;

    cout << "---= BUSQUEDA EN ÍNDICE INVERTIDO (PID: " << getpid() << ") =---" << endl;

    cout << "Buscar: ";
    getline(cin, busqueda);
    cout << endl;

    cout << "Opciones: " << endl;
    cout << "1) Validar " << endl;
    cout << "2) Cancelar "<< endl;
    cout << "Seleccione una opción: ";
    cin >> opcion;
    cout << endl;

    switch(opcion){
        case 1:
            if (busqueda.empty()) {
                cout << "(ERROR) La busqueda no puede estar vacía." << endl;
                esperarTecla();
                break;
            }

            cout << "LLAMADA A CACHE" << endl;
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
