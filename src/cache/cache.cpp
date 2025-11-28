#include "../include/dotenv.h"
#include <string>
#include <unistd.h>
#include <vector>
#include <netinet/in.h>
#include <sys/socket.h>
using namespace std;

int startServerSocket(int port);

int main(int argc, char* argv[]) {
    dotenv env(".env");
    int cacheSize = stoi(env.get("CACHE_SIZE"));
    int port = stoi(env.get("CACHE_PORT"));

    vector<int> cacheStorage(cacheSize);
    int serverSocket = startServerSocket(port);

    cout << "---= SERVICIO CACHE DE BÚSQUEDAS (PID: " << getpid() << ") =---" << endl;

    if (serverSocket == -1) {
        cout << "Creación de socket para cache falló, posiblemente porque el puerto " << port << " está ocupado." << endl;
        return -1;
    }

    cout << "Escuchando en puerto " << port << "." << endl;

    while (true) {
        int clientSocket = accept(serverSocket, nullptr, nullptr);

        char buffer[1024] = {0};
        recv(clientSocket, buffer, sizeof(buffer), 0);
        cout << "Message from client: " << buffer << endl;
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
