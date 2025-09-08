#include "../include/calcula_funcion.h"
#include <iostream>
#include <string>
#include <limits>
using namespace std;

void calcularFuncion() {
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Limpiar el buffer de entrada, para que no aparezca error al elegir la opcion 5
    string entrada;
    double x;
    bool continuar = true;

    while (continuar) {
        cout << "\n=== Calculo de f(x) = x^2 + 2x + 8 ===\n";
        cout << "Ingrese un numero real para X: ";
        getline(cin, entrada);

        try {
            x = stod(entrada); // convertir string a double
            double resultado =  x * x + 2 * x + 8;

            cout << "f(" << x << ") = (" << x << ")^2 + 2*(" << x << ") + 8 = "
                 << resultado << "\n";
        } catch (invalid_argument&) {
            cout << "Error: entrada no valida, intente de nuevo.\n";
            continue;
        }

        cout << "\n¿Desea VOLVER al menú principal? (s/n): ";
        string opcion;
        getline(cin, opcion);

        if (opcion == "s" || opcion == "S") {
            continuar = false;
            cout << "Saliendo del programa...\n";
        }
    }
}
