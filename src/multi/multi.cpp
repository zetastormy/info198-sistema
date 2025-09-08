#include <iostream>
#include <string>
#include <fstream>

using namespace std;

void printMatriz(float* C, int m, int n);
void leeMatriz(const string& filepath,const string& second_filepath, char separador);
void multiplicacion(float* A, float* B, int m, int n, int k);

int main(int argc, char **argv) {
	if (argc != 4) {
		cout << "(Error) Debe ejecutarse como ./bin/multi \"<ruta archivo matriz A>\" \"<ruta archivo matriz B>\" \"<separador>\"" << endl;
		exit(EXIT_FAILURE);
	}

	string rutaA = argv[1];
	string rutaB = argv[2];
    char separador = argv[3][0];
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
    ifstream file(filepath);

    if (!file) {
        cerr << "Could not open "<< filepath << " file" <<  endl;
        return;
    }

    ifstream sec_file(second_filepath);

    if (!sec_file) {
        cerr << "Could not open "<< second_filepath << " file" <<  endl;
        return;
    }

    string linea;
    int numLineas = 0;
    int maxCaracteres = 0;

    while (std::getline(file, linea)) {
        numLineas++;
        int separadores = 0;

        for (char c : linea) {
            if (c == separador) separadores++;
        }

        int elementos = separadores + 1;

        if (maxCaracteres == 0) {
            maxCaracteres = elementos;
        }

        if (elementos != maxCaracteres) {
            cerr << "Matriz A de dimensiones incorrectas" << endl;
            return;
        }
    }

    if (maxCaracteres != numLineas) {
        cerr << "La matriz A no es NxN" << endl;
        return;
    }

    int sec_numLineas = 0;
    int sec_maxCaracteres = 0;
    string sec_linea;

    while (std::getline(sec_file, sec_linea)) {
        sec_numLineas++;
        int separadoresB = 0;
        for (char c : sec_linea) {
            if (c == separador) separadoresB++;
        }

        int elementosB = separadoresB + 1;

        if (sec_maxCaracteres == 0){
            sec_maxCaracteres = elementosB;
        }

        if (elementosB != sec_maxCaracteres) {
            cerr << "Matriz B de dimensiones incorrectas" << endl;
            return;
        }
    }

    if (sec_maxCaracteres != sec_numLineas) {
        cerr << "La matriz B no es NxN" << endl;
        return;
    }

    if (numLineas != sec_numLineas) {
        cerr << "Matrices de dimensiones distintas" << endl;
    }

    //Cargar datos
    float *A = new float[numLineas*maxCaracteres];
    float *B = new float[sec_numLineas*sec_maxCaracteres];

    file.clear();
    file.seekg(0);
    int idx = 0;

    while (getline(file, linea)) {
        string numero;

        for (size_t i = 0; i <= linea.size(); i++) {
            if (i == linea.size() || linea[i] == separador) {
                if (!numero.empty()) {
                    A[idx++] = stof(numero);
                    numero.clear();
                }
            } else {
                numero.push_back(linea[i]);
            }
        }
    }

    // ---- Volver a leer archivo B y cargar datos ----
    sec_file.clear();
    sec_file.seekg(0);
    idx = 0;

    while (getline(sec_file, sec_linea)) {
        string numero;

        for (size_t i = 0; i <= sec_linea.size(); i++) {
            if (i == sec_linea.size() || sec_linea[i] == separador) {
                if (!numero.empty()) {
                    B[idx++] = stof(numero);
                    numero.clear();
                }
            } else {
                numero.push_back(sec_linea[i]);
            }
        }
    }

    cout << "Matrices creadas correctamente" << endl;

    multiplicacion(A, B, numLineas, maxCaracteres, numLineas);
    delete[] A;
    delete[] B;
}
