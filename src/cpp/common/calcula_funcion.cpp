#include "../include/calcula_funcion.h"
#include "../include/util.h"
#include <iostream>
#include <limits>
using namespace std;

void calcularFuncion() {
    double x;
    bool continuar = true;

    while (continuar) {
        cout << "---= Calculo de f(x) = x^2 + 2x + 8 =---";
        cout << endl;
        cout << "Ingrese un numero real para X: ";

        if (!(cin >> x)) {
            cout << "(ERROR) Entrada no valida, intente de nuevo." << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;         }

        double resultado = x * x + 2 * x + 8;
        cout << "f(" << x << ") = (" << x << ")^2 + 2*(" << x << ") + 8 = "
             << resultado << endl;

        continuar = false;
    }

    esperarTecla();
}
