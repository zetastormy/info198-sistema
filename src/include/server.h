#ifndef SERVER_H
#define SERVER_H
// Server.h
#pragma once

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QMap>       // Usaremos QMap en lugar de std::map para conveniencia de Qt
#include <QString>
#include "game.h"     // ¡El cerebro!

class Server : public QObject
{
    Q_OBJECT // Macro necesaria para Signals/Slots

public:
    explicit Server(QObject *parent = nullptr);
    void startServer(); // Inicia el servidor

private slots:
    // Slots que se disparan por eventos de red
    void onNewConnection();      // Alguien se conecta
    void onClientDisconnected(); // Alguien se desconecta
    void onReadyRead();          // Alguien envió un mensaje

private:
    void processMessage(QTcpSocket* client, const QString& message); // Procesa los mensajes
    void sendToAll(const QString& message); // Envía un mensaje a todos
    void broadcastLobbyUpdate(); // Envía el estado del lobby a todos

    QTcpServer* m_tcpServer;
    Game m_game; // La instancia de la lógica del juego

    // --- Estructuras del Lobby ---
    QTcpSocket* m_admin = nullptr; // Puntero al cliente que es Admin

    // Mapa de clientes: QTcpSocket* -> "Nombre del Jugador"
    QMap<QTcpSocket*, QString> m_clientConnections;

    // Mapa del Lobby: "Nombre del Equipo" -> Objeto Team (con lista de Players)
    // Este m_lobbyTeams se pasará al m_game cuando inicie la partida
    std::map<std::string, Team> m_lobbyTeams;
};
#endif // SERVER_H
