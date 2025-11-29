#include "../include/dotenv.h"
#include <cstring>
#include <fstream>
#include <iostream>
#include <netinet/in.h>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <unordered_map>
#include <vector>
using namespace std;

unordered_map<string, vector<pair<int, int>>> memoriaIndice;
int startServerSocket(int searchPort);
bool cargarIndice(string rutaArchivo);

int main (int argc, char**argv) {
    dotenv env(".env");
    int searchPort = stoi(env.get("SEARCH_PORT"));
    int topK = stoi(env.get("TOPK"));
    string nombreArchivoindice = "example"; // recordatorio para utilizar env
    string rutaIndice = "data/" + nombreArchivoindice + ".idx";


    cout << "---= MOTOR DE BUSQUEDA (PID: )" << getpid() << ") =---" << endl;

    if (!cargarIndice(rutaIndice)) {
        cerr << "(FATAL) No se pudo cargar el índice. Asegúrate de haber ejecutado indice_invetido primero." << endl;
        return 1;
    }

    int serverSocket = startServerSocket(searchPort);
    if (serverSocket == -1) {
        cout << "Error al iniciar socket en puerto, posiblemente porque el puerto " << searchPort << "está ocupado." << endl;
    }

    cout << "Motor listo y escuchando en puerto " << searchPort << "." << endl;

    while(true) {
        int clientSocket = accept(serverSocket, nullptr, nullptr);

        char buffer[1024] = {0};
        recv(clientSocket, buffer, sizeof(buffer), 0);
        string query(buffer);

        query.erase(remove(query.begin(), query.end(), '\n'), query.end());
        query.erase(remove(query.begin(), query.end(), '\r'), query.end());

        cout << "Consulta recibida: " << query << endl;


        string respuesta = "Motor listo. Datos en memoria.";
        send(clientSocket, respuesta.c_str(), respuesta.size(), 0);

        close(clientSocket);
    }

    close(serverSocket);
    return 0;
}

bool cargarIndice(string rutaArchivo) {
    cout << "Cargando archivo desde: " << rutaArchivo << endl;
    ifstream archivoIndices(rutaArchivo);

    if (!archivoIndices.is_open()) return false;

    string linea;
    while (getline(archivoIndices, linea)) {
        stringstream ss(linea);
        string palabra;
        if (!getline(ss, palabra, ';')) continue;

        string segmento;
        while (getline(ss, segmento, ';')) {
            if (segmento.empty()) continue;

            stringstream ssSegmento(segmento);

            int docID, frecuencia;
            char ignora;
            ssSegmento >> ignora >> docID >> ignora >> frecuencia >> ignora;

            if (!ssSegmento.fail()) {
                memoriaIndice[palabra].push_back({docID, frecuencia});
                cout << "Guardado: " << palabra << "-> ID:" << docID << endl;
            }
        }
    }
    return true;
}

int startServerSocket(int searchPort) {
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) return -1;

    int opt = 1;
        setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(searchPort);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    int status = bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));

    if (status == -1) {
        return -1;
    }

    listen(serverSocket, 5);
    return serverSocket;
}
