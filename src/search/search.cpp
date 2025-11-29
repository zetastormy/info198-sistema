#include "../include/util.h"
#include "../include/dotenv.h"
#include "../include/json.hpp"
#include <netinet/in.h>
#include <string>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <algorithm>
using namespace std;
using json = nlohmann::json;

void search(string query, int cachePort);
void showResults(json searchResults);

int main() {
    dotenv env(".env");
    int cachePort = stoi(env.get("CACHE_PORT"));

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

    switch(opcion) {
        case 1:
            if (busqueda.empty()) {
                cout << "(ERROR) La busqueda no puede estar vacía." << endl;
                esperarTecla();
                break;
            }

            search(busqueda, cachePort);

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

void search(string query, int cachePort) {
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(cachePort);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    int status = connect(clientSocket, (struct sockaddr*) &serverAddress, sizeof(serverAddress));

    if (status == -1) {
        cout << "El servicio de búsquedas está caído." << endl;
        return;
    }

    send(clientSocket, query.c_str(), query.size(), 0);

    // Recibimos cadenas de hasta 1024 caracteres
    char buffer[1024] = {0};
    // Leemos el buffer, ignorando la basura que pudiese contener
    int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0); // -1 para dejar espacio al \0

    if (bytesRead > 0) {
        buffer[bytesRead] = '\0'; // Asegurar terminación nula
        string rawResults(buffer);

        json searchResults = json::parse(rawResults);

        showResults(searchResults);
    } else if (bytesRead == 0) {
        cout << "El cliente terminó la conexión abruptamente." << endl;
    } else {
        cout << "Error al recibir datos." << endl;
    }

    close(clientSocket);
}

void showResults(json searchResults) {
    string query = searchResults["query"];
    int topk = searchResults["topk"];
    long lookupTime = searchResults["lookupTime"];
    vector<json> sortedResults = searchResults["results"].get<vector<json>>();

    sort(sortedResults.begin(), sortedResults.end(), [](const json &a, const json &b) {
        return a["score"] > b["score"];
    });

    cout << "---= " << sortedResults.size() << " RESULTADOS PARA '" << query << "' (" << lookupTime  << "μs) =---" << endl;

    int count = 0;

    for (const auto& result : sortedResults) {
        if (count >= topk) break;

        cout << count + 1 <<". » " << result["title"] << " (Puntuación: " << result["score"] << ")"<< endl;

        count++;
    }
}
