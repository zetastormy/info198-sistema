#include "../include/dotenv.h"
#include "../include/json.hpp"
#include <string>
#include <unistd.h>
#include <unordered_map>
#include <deque>
#include <netinet/in.h>
#include <sys/socket.h>
using namespace std;
using namespace chrono;
using json = nlohmann::json;

int startServerSocket(int port);
json lookupResult(string query, int topk, deque<string> &queryCache, unordered_map<string, json> &resultCache, int cacheSize);
json searchEngineLookup(string query);
void cache(string query, json result, deque<string> &queryCache, unordered_map<string, json> &resultCache, int cacheSize);

int main(int argc, char* argv[]) {
    dotenv env(".env");
    int topk = stoi(env.get("TOPK"));
    int cacheSize = stoi(env.get("CACHE_SIZE"));
    int port = stoi(env.get("CACHE_PORT"));

    deque<string> queryCache;
    unordered_map<string, json> resultCache;
    int serverSocket = startServerSocket(port);

    cout << "---= SERVICIO CACHE DE BÚSQUEDAS (PID: " << getpid() << ") =---" << endl;

    if (serverSocket == -1) {
        cout << "Error al crear socket de servicio de cache, posiblemente porque el puerto " << port << " está ocupado." << endl;
        return -1;
    }

    cout << "Escuchando en puerto " << port << "." << endl;

    while (true) {
        int clientSocket = accept(serverSocket, nullptr, nullptr);

        // Recibimos cadenas de hasta 1024 caracteres
        char buffer[1024] = {0};
        // Leemos el buffer, ignorando la basura que pudiese contener
        int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0); // -1 para dejar espacio al \0

        if (bytesRead > 0) {
            buffer[bytesRead] = '\0'; // Asegurar terminación nula
            string query(buffer);

            cout << "Consulta recibida: " << query << endl;

            json response = lookupResult(query, topk, queryCache, resultCache, cacheSize);
            string responseStr = response.dump();

            send(clientSocket, responseStr.c_str(), responseStr.size(), 0);

            close(clientSocket);
        } else if (bytesRead == 0) {
            cout << "El cliente terminó la conexión abruptamente." << endl;
        } else {
            cout << "Error al recibir datos." << endl;
        }
    }

    close(serverSocket);
}

int startServerSocket(int port) {
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    int status = bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));

    if (status == -1) {
        return -1;
    }

    listen(serverSocket, 5);

    return serverSocket;
}

json lookupResult(string query, int topk, deque<string> &queryCache, unordered_map<string, json> &resultCache, int cacheSize) {
    auto lookupStart = high_resolution_clock::now();

    json response;
    response["query"] = query;
    response["topk"] = topk;

    if (resultCache.count(query) != 0) {
        response["result"] = resultCache[query];

        auto lookupEnd = high_resolution_clock::now();
        auto lookupTime = duration_cast<microseconds>(lookupEnd - lookupStart);

        response["lookupTime"] = lookupTime.count();

        return response;
    }

    json searchResult(searchEngineLookup(query));
    cache(query, searchResult, queryCache, resultCache, cacheSize);

    response["result"] = searchResult;

    auto lookupEnd = high_resolution_clock::now();
    auto lookupTime = duration_cast<microseconds>(lookupEnd - lookupStart);

    response["lookupTime"] = lookupTime.count();

    return response;
}

json searchEngineLookup(string query) {
    return nullptr;
}

void cache(string query, json result, deque<string> &queryCache, unordered_map<string, json> &resultCache, int cacheSize) {
    // Si el cache no está lleno todavía
    if (queryCache.size() < cacheSize) {
        queryCache.push_back(query);
        resultCache[query] = result;

        return;
    }

    // Si el cache llegó a su capacidad límite, aplicamos FIFO
    resultCache.erase(queryCache.front());
    queryCache.pop_front();

    queryCache.push_back(query);
    resultCache[query] = result;
}
