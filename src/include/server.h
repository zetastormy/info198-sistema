#ifndef SERVER_H
#define SERVER_H

#pragma once

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QMap>
#include <QString>
#include "game.h"

class Server : public QObject
{
    Q_OBJECT

public:
    explicit Server(QObject *parent = nullptr);
    void startServer();

private slots:
    void onNewConnection();
    void onClientDisconnected();
    void onReadyRead();

private:
    void processMessage(QTcpSocket* client, const QString& message);
    void sendToAll(const QString& message);
    void broadcastLobbyUpdate();

    QTcpServer* m_tcpServer;
    Game m_game;

    QTcpSocket* m_admin = nullptr;

    QMap<QTcpSocket*, QString> m_clientConnections;

    std::map<std::string, Team> m_lobbyTeams;
};

#endif
