#include "../include/dotenv.h"
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <unordered_map>
#include <vector>
using namespace std;

unordered_map<string, vector<pair<int, int>>> memoriaIndice;
int startServerSocket(int searchPort);

int main (int argc, char**argv) {
    dotenv env(".env");
    int searchPort = stoi(env.get("SEARCH_PORT"));

    int serverSocket = startServerSocket(searchPort);

    cout << "---= MOTOR DE BUSQUEDA (PID: )" << getpid() << ") =---" << endl;

    if (serverSocket == -1) {
        cout << "Error al iniciar socket en puerto, posiblemente porque el puerto " << searchPort << "está ocupado." << endl;
    }

    cout << "Escuchando en puerto " << searchPort << "." << endl;

    while(true) {
        int clientSocket = accept(serverSocket, nullptr, nullptr);

        char buffer[1024] = {0};
        recv(clientSocket, buffer, sizeof(buffer), 0);
        cout << "Message from client: " << buffer << endl;
    }

    return 0;
}

int startServerSocket(int searchPort) {
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) return -1;

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
