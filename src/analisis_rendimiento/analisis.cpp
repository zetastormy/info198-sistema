#include "../include/analisis_rendimiento.h"
#include "../include/dotenv.h"
#include <string>

void analizar_rendimiento();

int main(int argc, char* argv[]) {
    dotenv env(".env");
    int n_lote = stoi(env.get("N_LOTE"));
    string binIndiceInvertidoParalelo = env.get("INDICE_INVET_PARALELO");
}

void analizar_rendimiento(){
    int CANT_THREADS [7] = {1, 2, 3, 4, 5, 6, 7};
    for (int i = 0 ; i < sizeof (CANT_THREADS) ; i++){

    }
}
