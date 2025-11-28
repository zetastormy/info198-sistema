#include "../include/dotenv.h"
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
using namespace std;

int main (int argc, char**argv) {
    dotenv env(".env");
    int searchPort = stoi(env.get("SEARCH_PORT"));

    cout << "---= MOTOR DE BUSQUEDA (PID: )" << getpid() << ") =---" << endl;

    return 0;
}

int startServerSocket(int port) {
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) return -1;

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
