#include "../include/palindrome.h"
#include "../include/util.h"
#include <iostream>
#include <algorithm>
#include <cctype>
#include <limits>

using namespace std;

bool esPalindromo(const string& textoOriginal) {
    string texto = textoOriginal;

    transform(texto.begin(), texto.end(), texto.begin(), ::tolower);

    texto.erase(remove_if(texto.begin(), texto.end(),
                          [](char c) { return !isalnum(c); }),
                texto.end());

    int inicio = 0;
    int fin = texto.size() - 1;

    while (inicio < fin) {
        if (texto[inicio] != texto[fin]) {
            return false;
        }
        inicio++;
        fin--;
    }
    return true;
}

void menuPalindromo(){
    string texto;
    int opcion;

    cout << "---= VERIFICADOR DE PALÍNDROMOS =---" << endl;

    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    cout << "Ingrese un texto: ";
    getline(cin, texto);

    cout << "\nOpciones:\n1) Validar\n2) Cancelar\nSeleccione una opción: ";
    cin >> opcion;

    switch(opcion){
        case 1:
            if(esPalindromo(texto)) cout << "El texto SÍ es un palíndromo." << endl;
            else cout << "El texto NO es un palíndromo." << endl;
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
