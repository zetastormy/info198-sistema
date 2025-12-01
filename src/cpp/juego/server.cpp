#include "../include/server.h"
#include <QDebug>
#include <algorithm>

Server::Server(QObject *parent) : QObject(parent) {
    qInfo().noquote() << "  [Constructor Server] Creando m_tcpServer...";

    m_tcpServer = new QTcpServer(this);

    qInfo().noquote() << "  [Constructor Server] Conectando señal 'newConnection'...";

    connect(m_tcpServer, &QTcpServer::newConnection, this, &Server::onNewConnection);

    qInfo().noquote() << "  [Constructor Server] Finalizado.";
}

void Server::startServer() {
    if (m_tcpServer->listen(QHostAddress::Any, 8080)) {
        qInfo() << ">>> SERVIDOR INICIADO: Escuchando en el puerto 8080. Esperando conexiones... <<<";
    } else {
        qCritical() << "--- ¡ERROR GRAVE! NO SE PUDO INICIAR EL SERVIDOR: ---";
        qCritical() << m_tcpServer->errorString();
        qCritical() << "--- Revisa si otro programa (Spotify, Skype, otro juego) usa el puerto 8080 ---";
    }
}

void Server::onNewConnection() {
    QTcpSocket *clientSocket = m_tcpServer->nextPendingConnection();

    connect(clientSocket, &QTcpSocket::readyRead, this, &Server::onReadyRead);
    connect(clientSocket, &QTcpSocket::disconnected, this, &Server::onClientDisconnected);

    if (m_admin == nullptr) {
        m_admin = clientSocket;
    }

    qDebug() << "Nuevo cliente conectado:" << clientSocket->peerAddress().toString();
}

void Server::onReadyRead() {
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket*>(sender());
    if (!clientSocket) return;

    QByteArray data = clientSocket->readAll();
    QString message = QString(data).trimmed();

    QStringList messages = message.split('\n', Qt::SkipEmptyParts);
    for (const QString& msg : messages) {
        processMessage(clientSocket, msg);
    }
}

void Server::processMessage(QTcpSocket* client, const QString& message) {
    qDebug() << "Mensaje recibido de" << m_clientConnections.value(client, "??") << ":" << message;

    if (message.startsWith("CONNECT:")) {
        QString playerName = message.section(':', 1);

        m_clientConnections[client] = playerName;

        if (client == m_admin) {
            client->write("CONNECT_OK:ADMIN\n");
        } else {
            client->write("CONNECT_OK:PLAYER\n");
        }
        qDebug() << playerName << "se ha unido.";
        broadcastLobbyUpdate();

    } else if (message.startsWith("JOIN_TEAM:")) {
        QString teamName = message.section(':', 1);
        QString playerName = m_clientConnections.value(client, "");

        if (playerName.isEmpty()) return;

        std::string s_teamName = teamName.toStdString();
        std::string s_playerName = playerName.toStdString();

        for (auto it_team = m_lobbyTeams.begin(); it_team != m_lobbyTeams.end(); ) {
            auto& members = it_team->second.members;
            auto it_player = std::remove_if(members.begin(), members.end(),
                                            [&s_playerName](const Player& player) {
                                                return player.name == s_playerName;
                                            });

            if (it_player != members.end()) {
                members.erase(it_player, members.end());
                qDebug() << "  > (Limpiando) " << playerName << " eliminado del equipo " << QString::fromStdString(it_team->first);

                if (it_team->second.members.empty()) {
                    qDebug() << "  > (Limpiando) Equipo " << QString::fromStdString(it_team->first) << " está vacío. Borrando.";
                    it_team = m_lobbyTeams.erase(it_team);
                } else {
                    ++it_team;
                }
            } else {
                ++it_team;
            }
        }

        if (m_lobbyTeams.find(s_teamName) == m_lobbyTeams.end()) {
            m_lobbyTeams[s_teamName] = Team{s_teamName};
        }

        m_lobbyTeams[s_teamName].members.push_back(Player{s_playerName});

        qDebug() << playerName << "se unió al equipo" << teamName;

        broadcastLobbyUpdate();

    } else if (message == "START_GAME") {
        if (client != m_admin) {
            client->write("ERROR: Solo el anfitrión puede iniciar la partida.\n");
            return;
        }

        qDebug() << "El anfitrión inició la partida. Configurando el juego...";
        m_game = Game();

        for (auto const& [name, team] : m_lobbyTeams) {
            m_game.addTeam(team);
        }

        std::string startResult = m_game.start();

        if (m_game.isGameActive()) {
            qDebug() << "¡Juego iniciado con éxito! Informando a los clientes.";

            QString teamList = "JUEGO_INICIADO:";
            for (const auto& team : m_game.getTeams()) {
                teamList += QString::fromStdString(team.name) + ";";
            }
            teamList.chop(1);
            teamList += "\n";

            sendToAll(teamList);

            std::string s_currentPlayer = m_game.getCurrentPlayerName();
            std::string s_nextPlayer = m_game.getNextPlayerName();

            sendToAll("LOG:¡Juego iniciado! Es el turno de " + QString::fromStdString(s_currentPlayer) + ".\n");
            sendToAll("TURN:" + QString::fromStdString(s_currentPlayer) + ":" + QString::fromStdString(s_nextPlayer) + "\n");
        } else {
            qDebug() << "Falló el inicio del juego:" << QString::fromStdString(startResult);
            QString errorMsg = "ERROR:" + QString::fromStdString(startResult);
            client->write(errorMsg.toUtf8() + '\n');
        }
    } else if (message == "NEXT_TURN") {

        if (!m_game.isGameActive()) {
            client->write("ERROR:El juego no ha comenzado.\n");
            return;
        }

        QString senderName = m_clientConnections.value(client, "??");
        std::string s_currentPlayer = m_game.getCurrentPlayerName();

        if (senderName.toStdString() != s_currentPlayer) {
            client->write(("LOG:No es tu turno. (Turno de " + QString::fromStdString(s_currentPlayer) + ")\n").toUtf8());
            return;
        }

        std::string turnResult = m_game.nextTurn();
        qDebug() << "Turno procesado:" << QString::fromStdString(turnResult);

        sendToAll("LOG:" + QString::fromStdString(turnResult) + "\n");

        QString gameStateMsg = "GAME_UPDATE:";
        const auto& teams = m_game.getTeams();
        int winGoal = m_game.getWinThreshold();

        for (const auto& team : teams) {
            gameStateMsg += QString::fromStdString(team.name) + ":" +
                            QString::number(team.totalPosition) + ";";
        }
        gameStateMsg += "Objetivo:" + QString::number(winGoal) + "\n";
        sendToAll(gameStateMsg);

        if (!m_game.isGameActive()) {
            qDebug() << "El juego ha terminado.";
            
            // Declaramos el nombre del ganador antes de usarlo
            std::string winnerName = m_game.getWinnerName();
            
            sendToAll("GAME_OVER:" + QString::fromStdString(winnerName) + "\n");
            
            const auto& allTeams = m_game.getTeams();
            for (const auto& team : allTeams) {
                if (team.name == winnerName) {
                    // Encontramos al equipo ganador, ahora listamos a sus jugadores
                    for (const auto& player : team.members) {
                        sendToAll("LOG:VICTORIA_PLAYER:" + QString::fromStdString(player.name) + "\n");
                    }
                    break; 
                }
            }
            broadcastLobbyUpdate();
        } else {
            std::string s_currentPlayer = m_game.getCurrentPlayerName();
            std::string s_nextPlayer = m_game.getNextPlayerName();

            sendToAll("TURN:" + QString::fromStdString(s_currentPlayer) + ":" + QString::fromStdString(s_nextPlayer) + "\n");
        }
    }
}

void Server::onClientDisconnected() {
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket*>(sender());
    if (!clientSocket) return;

    QString playerName = m_clientConnections.value(clientSocket, "Desconocido");

    // Lógica de cierre total si es el ADMIN
    if (clientSocket == m_admin) {
        qDebug() << "¡El ANFITRIÓN (" << playerName << ") se desconectó! Cerrando todo.";

        sendToAll("LOG: El anfitrión cerró la sala. La partida ha terminado.\n");
        sendToAll("ERROR:El anfitrión se ha desconectado. \n");

        auto todosLosClientes = m_clientConnections.keys();
        for (QTcpSocket* socket : todosLosClientes) {
            if (socket != clientSocket) {
                socket->disconnectFromHost(); 
            }
        }

        m_clientConnections.clear();
        m_lobbyTeams.clear();
        m_game = Game();
        m_admin = nullptr;

        clientSocket->deleteLater();
        return;
    }

    std::string s_name = playerName.toStdString();

    // Manejo de juego activo (Jugador normal se va)
    if (m_game.isGameActive()) {
        int result = m_game.removePlayer(s_name);
        sendToAll("LOG: El jugador " + playerName + " se ha desconectado.\n");

        if (result == 3) { 
            std::string winner = m_game.getTeams()[0].name;
            sendToAll("LOG:¡El equipo rival se ha rendido/desconectado!\n");
            sendToAll("GAME_OVER:" + QString::fromStdString(winner) + "\n");
            m_game = Game();
            broadcastLobbyUpdate();
        } 
        else {
            if (result == 1) sendToAll("LOG: Un equipo ha sido eliminado.\n");
            else if (result == 2) sendToAll("LOG: Era su turno -> Pasa al siguiente.\n");
            
            std::string s_curr = m_game.getCurrentPlayerName();
            std::string s_next = m_game.getNextPlayerName();
            sendToAll("TURN:" + QString::fromStdString(s_curr) + ":" + QString::fromStdString(s_next) + "\n");
        }
    }

    // Limpieza de Lobby y memoria
    m_clientConnections.remove(clientSocket);
    qDebug() << playerName << "se ha desconectado (Jugador común).";

    std::string s_playerName = playerName.toStdString();
    for (auto it_team = m_lobbyTeams.begin(); it_team != m_lobbyTeams.end(); ) {
        auto& members = it_team->second.members;
        auto it_player = std::remove_if(members.begin(), members.end(),
                                        [&s_playerName](const Player& player) { return player.name == s_playerName; });
        if (it_player != members.end()) {
            members.erase(it_player, members.end());
            if (it_team->second.members.empty()) it_team = m_lobbyTeams.erase(it_team);
            else ++it_team;
        } else {
            ++it_team;
        }
    }

    if (!m_clientConnections.isEmpty()) {
        broadcastLobbyUpdate();
    }

    clientSocket->deleteLater();
}

void Server::broadcastLobbyUpdate() {
    QString lobbyState = "LOBBY_UPDATE:";

    for (auto const& [name, team] : m_lobbyTeams) {
        lobbyState += QString::fromStdString(name) + ":";

        for (size_t i = 0; i < team.members.size(); ++i) {
            lobbyState += QString::fromStdString(team.members[i].name);
            if (i < team.members.size() - 1) lobbyState += ",";
        }

        lobbyState += ";";
    }

    lobbyState.chop(1);
    lobbyState += "\n";

    sendToAll(lobbyState);
}

void Server::sendToAll(const QString& message) {
    for (QTcpSocket* client : m_clientConnections.keys()) {
        client->write(message.toUtf8());
    }
}
