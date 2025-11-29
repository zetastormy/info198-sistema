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
json lookupResult(string query, int topk, deque<string> &queryCache, unordered_map<string, json> &resultsCache, int cacheSize, int searchPort);
json searchEngineLookup(string query, int searchPort);
void cache(string query, json result, deque<string> &queryCache, unordered_map<string, json> &resultsCache, int cacheSize);

// DATOS DE PRUEBA GENERADOS POR GEMINI (BORRAR UNA VEZ REALIZADA CONEXIÓN CON MOTOR DE BÚSQUEDA)
void preloadCache(deque<string> &queryCache, unordered_map<string, json> &resultsCache, int cacheSize) {
    // 1. Query: "ciencia ficcion"
    // Simulamos una lista de resultados desordenada o parcialmente ordenada
    string q1 = "ciencia ficcion";
    json r1 = json::array({
        { {"title", "Dune"}, {"score", 98} },
        { {"title", "Neuromancer"}, {"score", 92} },
        { {"title", "Fahrenheit 451"}, {"score", 85} },
        { {"title", "The Three-Body Problem"}, {"score", 96} } // Un intruso con alto score
    });
    cache(q1, r1, queryCache, resultsCache, cacheSize);

    // 2. Query: "programacion c++"
    string q2 = "programacion c++";
    json r2 = json::array({
        { {"title", "Effective Modern C++"}, {"score", 99} },
        { {"title", "The C++ Programming Language"}, {"score", 95} },
        { {"title", "A Tour of C++"}, {"score", 90} }
    });
    cache(q2, r2, queryCache, resultsCache, cacheSize);

    // 3. Query: "novelas visuales"
    string q3 = "novelas visuales";
    json r3 = json::array({
        { {"title", "Steins;Gate"}, {"score", 100} },
        { {"title", "Clannad"}, {"score", 97} },
        { {"title", "Fate/Stay Night"}, {"score", 94} }
    });
    cache(q3, r3, queryCache, resultsCache, cacheSize);

    // 4. Query: "dbz" (Para probar relleno)
    string q4 = "dbz";
    json r4 = json::array({
        { {"title", "Dragon Ball Super: Broly Manga"}, {"score", 88} },
        { {"title", "Dragon Ball Z - Saiyan Arc"}, {"score", 91} }
    });
    cache(q4, r4, queryCache, resultsCache, cacheSize);
}

int main(int argc, char* argv[]) {
    dotenv env(".env");
    int topk = stoi(env.get("TOPK"));
    int cacheSize = stoi(env.get("CACHE_SIZE"));
    int cachePort = stoi(env.get("CACHE_PORT"));
    int searchPort = stoi(env.get("SEARCH_PORT"));

    deque<string> queryCache;
    unordered_map<string, json> resultsCache;

    preloadCache(queryCache, resultsCache, cacheSize);

    int serverSocket = startServerSocket(cachePort);

    cout << "---= SERVICIO CACHE DE BÚSQUEDAS (PID: " << getpid() << ") =---" << endl;

    if (serverSocket == -1) {
        cout << "Error al crear socket de servicio de cache, posiblemente porque el puerto " << cachePort << " está ocupado." << endl;
        return -1;
    }

    cout << "(INFO) Escuchando en puerto " << cachePort << "." << endl;

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

            json response = lookupResult(query, topk, queryCache, resultsCache, cacheSize, searchPort);
            string responseStr = response.dump();

            send(clientSocket, responseStr.c_str(), responseStr.size(), 0);

            close(clientSocket);
        } else if (bytesRead == 0) {
            cout << "(ERROR) El cliente terminó la conexión abruptamente." << endl;
        } else {
            cout << "(ERROR) Error al recibir datos." << endl;
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

    int status = bind(serverSocket, (struct sockaddr*) &serverAddress, sizeof(serverAddress));

    if (status == -1) {
        return -1;
    }

    listen(serverSocket, 5);

    return serverSocket;
}

json lookupResult(string query, int topk, deque<string> &queryCache, unordered_map<string, json> &resultsCache, int cacheSize, int searchPort) {
    auto lookupStart = high_resolution_clock::now();

    json response;
    response["query"] = query;
    response["topk"] = topk;

    if (resultsCache.count(query) != 0) {
        response["results"] = resultsCache[query];

        auto lookupEnd = high_resolution_clock::now();
        auto lookupTime = duration_cast<microseconds>(lookupEnd - lookupStart);

        response["lookupTime"] = lookupTime.count();

        return response;
    }

    json searchResults(searchEngineLookup(query, searchPort));
    cache(query, searchResults, queryCache, resultsCache, cacheSize);

    response["results"] = searchResults;

    auto lookupEnd = high_resolution_clock::now();
    auto lookupTime = duration_cast<microseconds>(lookupEnd - lookupStart);

    response["lookupTime"] = lookupTime.count();

    return response;
}

json searchEngineLookup(string query, int searchPort) {
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(searchPort);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    int status = connect(clientSocket, (struct sockaddr*) &serverAddress, sizeof(serverAddress));

    if (status == -1) {
        cout << "(ERROR) El motor de búsqueda está caído." << endl;
        return json::array();
    }

    send(clientSocket, query.c_str(), query.size(), 0);

    // Recibimos cadenas de hasta 1024 caracteres
    char buffer[1024] = {0};
    // Leemos el buffer, ignorando la basura que pudiese contener
    int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0); // -1 para dejar espacio al \0

    if (bytesRead > 0) {
        buffer[bytesRead] = '\0'; // Asegurar terminación nula
        string rawResults(buffer);

        return json::parse(rawResults);
    } else if (bytesRead == 0) {
        cout << "(ERROR) El cliente terminó la conexión abruptamente." << endl;
    } else {
        cout << "(ERROR) Error al recibir datos." << endl;
    }

    return json::array();
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
