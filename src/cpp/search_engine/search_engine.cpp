#include "../include/dotenv.h"
#include "../include/json.hpp"
#include <codecvt>
#include <cstring>
#include <fstream>
#include <iostream>
#include <netinet/in.h>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <unordered_map>
#include <utility>
#include <vector>
using namespace std;
using json = nlohmann::json;

unordered_map<string, vector<pair<int, int>>> memoriaIndice;
unordered_map<int, string> baseDatosLibros;

int startServerSocket(int searchPort);
string buscarTopK(string query, int topK);
bool cargarIndice(string rutaArchivo);
bool cargarBaseDatosLibros(string rutaArchivo);

int main (int argc, char**argv) {
    dotenv env(".env");
    int searchPort = stoi(env.get("SEARCH_PORT"));
    int topK = stoi(env.get("TOPK"));
    string nombreArchivoindice = "indices_prueba"; // recordatorio para utilizar env
    string rutaIndice = "data/" + nombreArchivoindice + ".idx";
    string rutaLibros = "resources/mapaLibros.txt";


    cout << "---= MOTOR DE BUSQUEDA (PID: )" << getpid() << ") =---" << endl;

    if (!cargarIndice(rutaIndice)) {
        cerr << "(FATAL) No se pudo cargar el índice. Asegúrate de haber ejecutado indice_invetido primero." << endl;
        return 1;
    }

    if (!cargarBaseDatosLibros(rutaLibros)) {
        cout << "(FATAL) No se pudo cargar el mapa de libros. se usarán los IDs." << endl;
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

        string respuesta = buscarTopK(query, topK);
        send(clientSocket, respuesta.c_str(), respuesta.size(), 0);

        close(clientSocket);
    }

    close(serverSocket);
    return 0;
}

string buscarTopK(string query, int topK) {
    auto posicionQuery = memoriaIndice.find(query);
    if ( posicionQuery == memoriaIndice.end()) return json::array().dump();

    vector<pair<int, int>> listaResultados = posicionQuery -> second;

    sort(listaResultados.begin(), listaResultados.end(),
        [](const pair<int, int>& a, const pair<int,int>& b) {
            return a.second > b.second;
        });

    json respuestaJson = json::array();

    int contador = 0;
    for (const auto& par : listaResultados) {
        if (contador >= topK) break;

        int idLibro = par.first;

        string titulo = "Desconocido (ID:" + to_string(idLibro) + ").";
        if (baseDatosLibros.count(idLibro)) {
            titulo = baseDatosLibros[idLibro];
        }

        json documento;
        documento["title"] = titulo;
        documento["score"] = par.second;

        respuestaJson.push_back(documento);

        contador++;
    }

    return respuestaJson.dump();
}

bool cargarBaseDatosLibros(string rutaArchivo) {
    cout << "Cargando los nombres de los libros desde: " << rutaArchivo << endl;
    ifstream archivoTitulos(rutaArchivo);

    if (!archivoTitulos.is_open()) return false;

    string linea;
    while (getline(archivoTitulos, linea)) {
        stringstream ss(linea);
        string idLibro, nombreLibro;

        if (getline(ss, idLibro, ';')) {
            if (getline(ss, nombreLibro)) {
                try {
                    int id = stoi(idLibro);
                    baseDatosLibros[id] = nombreLibro;
                } catch (...) {
                    continue;
                }
            }
        }
    }

    cout << "Libros cargados: " << baseDatosLibros.size() << " libros" << endl;
    return true;
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
