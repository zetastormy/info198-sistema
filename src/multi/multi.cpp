#include <iostream>
#include <stdexcept>
#include <string>
#include <fstream>
#include <sys/types.h>
#include <unistd.h>
using namespace std;

void printMatriz(float* C, int m, int n);
void leeMatriz(const string& filepath,const string& second_filepath, char separador);
void multiplicacion(float* A, float* B, int m, int n, int k);

int main(int argc, char **argv) {
	if (argc != 4) {
		cout << "(ERROR) Debe ejecutarse como ./bin/multi \"<ruta archivo matriz A>\" \"<ruta archivo matriz B>\" \"<separador>\"" << endl;
		exit(EXIT_FAILURE);
	}

	string rutaA = argv[1];
	string rutaB = argv[2];
    char separador = argv[3][0];

   	cout << "---= MULTIPLICADOR DE MATRICES AxB (PID: " << getpid() << ") =---" << endl;
    cout << "Separador: " << separador << endl;

    leeMatriz(rutaA, rutaB, separador);
}

void printMatriz(float* C, int m, int n) {
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            cout << C[i * n + j] << "  ";
        }

        cout << '\n';
    }
}

void multiplicacion(float* A, float* B, int m, int n, int k) {
    float *C = new float[m * n];

    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            float sum = 0;

            for (int l = 0; l < k; l++) {
                sum += A[i * k + l] * B[l * n + j];
            }

            C[i * n + j] = sum;
        }
    }

    printMatriz(C, m , n);
    delete[] C;
}

void leeMatriz(const string& filepath,const string& second_filepath, char separador) {
    ifstream matrizA(filepath);

    if (!matrizA) {
        cerr << "(ERROR) No se pudo abrir el archivo " << filepath << "." <<  endl;
        return;
    }

    ifstream matrizB(second_filepath);

    if (!matrizB) {
        cerr << "(ERROR) No se pudo abrir el archivo " << second_filepath << "." <<  endl;
        return;
    }

    string lineaA;
    int numLineasA = 0;
    int maxCaracteresA = 0;

    while (std::getline(matrizA, lineaA)) {
        numLineasA++;
        int separadoresA = 0;

        for (char c : lineaA) {
            if (c == separador) separadoresA++;
        }

        int elementosA = separadoresA + 1;

        if (maxCaracteresA == 0) {
            maxCaracteresA = elementosA;
        }

        if (separadoresA == 0) {
            cerr << "(ERROR) Matriz A, fila " << numLineasA << " no tiene el separador indicado." << endl;
            return;
        }

        if (separadoresA + 1 < elementosA || elementosA != maxCaracteresA) {
            cerr << "(ERROR) Matriz A, fila " << numLineasA << " tiene algún separador incorrecto o dimensiones incorrectas." << endl;
            return;
        }
    }

    if (maxCaracteresA != numLineasA) {
        cerr << "(ERROR) La matriz A no es NxN." << endl;
        return;
    }

    int numLineasB = 0;
    int maxCaracteresB = 0;
    string lineaB;

    while (std::getline(matrizB, lineaB)) {
        numLineasB++;
        int separadoresB = 0;
        for (char c : lineaB) {
            if (c == separador) separadoresB++;
        }

        int elementosB = separadoresB + 1;

        if (maxCaracteresB == 0){
            maxCaracteresB = elementosB;
        }


        if (separadoresB == 0) {
            cerr << "(ERROR) Matriz B, fila " << numLineasB << " no tiene el separador indicado." << endl;
            return;
        }

        if (separadoresB + 1 < elementosB || elementosB != maxCaracteresB) {
            cerr << "(ERROR) Matriz B, fila " << numLineasB << " tiene algún separador incorrecto o dimensiones incorrectas." << endl;
            return;
        }
    }

    if (maxCaracteresB != numLineasB) {
        cerr << "(ERROR) La matriz B no es NxN." << endl;
        return;
    }

    if (numLineasA != numLineasB) {
        cerr << "(ERROR) Matrices de dimensiones distintas." << endl;
    }

    float *A = new float[numLineasA*maxCaracteresA];
    float *B = new float[numLineasB*maxCaracteresB];

    matrizA.clear();
    matrizA.seekg(0);

    int numLinea = 1;
    int idx = 0;

    while (getline(matrizA, lineaA)) {
        string numero;

        for (size_t i = 0; i <= lineaA.size(); i++) {
            if (i == lineaA.size() || lineaA[i] == separador) {
                if (!numero.empty()) {
                    size_t posFinalConv = 0;

                    try {
                        float resultadoConv = stof(numero, &posFinalConv);

                        if (posFinalConv != numero.size()) {
                            throw invalid_argument("Conversión incompleta. El string contiene caracteres no numéricos.");
                        }

                        A[idx++] = resultadoConv;
                    } catch (invalid_argument&) {
                        cerr << "(ERROR) Matriz A, fila " << numLinea << " tiene un elemento que no es número." << endl;
                        return;
                    }

                    numero.clear();
                }
            } else {
                numero.push_back(lineaA[i]);
            }
        }

        numLinea++;
    }

    matrizB.clear();
    matrizB.seekg(0);

    numLinea = 1;
    idx = 0;

    while (getline(matrizB, lineaB)) {
        string numero;

        for (size_t i = 0; i <= lineaB.size(); i++) {
            if (i == lineaB.size() || lineaB[i] == separador) {
                if (!numero.empty()) {
                    size_t posFinalConv = 0;

                    try {
                        float resultadoConv = stof(numero, &posFinalConv);

                        if (posFinalConv != numero.size()) {
                            throw invalid_argument("Conversión incompleta. El string contiene caracteres no numéricos.");
                        }

                        B[idx++] = resultadoConv;
                    } catch (invalid_argument&) {
                        cerr << "(ERROR) Matriz B, fila " << numLinea << " tiene un elemento que no es número." << endl;
                        return;
                    }

                    numero.clear();
                }
            } else {
                numero.push_back(lineaB[i]);
            }
        }

        numLinea++;
    }

    cout << "Resultado:" << endl;

    multiplicacion(A, B, numLineasA, maxCaracteresA, numLineasA);
    delete[] A;
    delete[] B;
}
