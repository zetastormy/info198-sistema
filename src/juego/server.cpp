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
    // Usamos el puerto 8080
    if (m_tcpServer->listen(QHostAddress::Any, 8080)) {

        // qInfo() es más estándar para mensajes informativos
        qInfo() << ">>> SERVIDOR INICIADO: Escuchando en el puerto 8080. Esperando conexiones... <<<";

    } else {

        // qCritical() imprime un error grave
        qCritical() << "--- ¡ERROR GRAVE! NO SE PUDO INICIAR EL SERVIDOR: ---";
        qCritical() << m_tcpServer->errorString();
        qCritical() << "--- Revisa si otro programa (Spotify, Skype, otro juego) usa el puerto 8080 ---";
    }
}

void Server::onNewConnection() {
    // Obtenemos el socket del nuevo cliente
    QTcpSocket *clientSocket = m_tcpServer->nextPendingConnection();

    // Conectamos las señales de este cliente a nuestros slots
    connect(clientSocket, &QTcpSocket::readyRead, this, &Server::onReadyRead);
    connect(clientSocket, &QTcpSocket::disconnected, this, &Server::onClientDisconnected);

    // Damos rol de Admin si es el primero en conectarse
    if (m_admin == nullptr) {
        m_admin = clientSocket;
    }

    qDebug() << "Nuevo cliente conectado:" << clientSocket->peerAddress().toString();
}

void Server::onReadyRead() {
    // Identificamos qué cliente envió el mensaje
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket*>(sender());
    if (!clientSocket) return;

    // Leemos el mensaje
    QByteArray data = clientSocket->readAll();
    QString message = QString(data).trimmed(); // .trimmed() quita el \n

    // Un cliente puede enviar varios mensajes juntos, los separamos por \n
    QStringList messages = message.split('\n', Qt::SkipEmptyParts);
    for (const QString& msg : messages) {
        processMessage(clientSocket, msg);
    }
}

void Server::processMessage(QTcpSocket* client, const QString& message)
{
    qDebug() << "Mensaje recibido de" << m_clientConnections.value(client, "??") << ":" << message;

    if (message.startsWith("CONNECT:")) {
        QString playerName = message.section(':', 1);

        // Guardamos el nombre del jugador asociado a su socket
        m_clientConnections[client] = playerName;

        // Respondemos si es Admin o Player
        if (client == m_admin) {
            client->write("CONNECT_OK:ADMIN\n");
        } else {
            client->write("CONNECT_OK:PLAYER\n");
        }
        qDebug() << playerName << "se ha unido.";
        broadcastLobbyUpdate(); // Enviamos el estado del lobby (ahora con el jugador)

    }else if (message.startsWith("JOIN_TEAM:")) {
        QString teamName = message.section(':', 1);
        QString playerName = m_clientConnections.value(client, "");

        if (playerName.isEmpty()) return; // Ignorar si no se ha conectado bien

        std::string s_teamName = teamName.toStdString();
        std::string s_playerName = playerName.toStdString();

        // --- ¡NUEVA LÓGICA DE LIMPIEZA! ---
        // 1. Recorrer todos los equipos para borrar a este jugador
        for (auto it_team = m_lobbyTeams.begin(); it_team != m_lobbyTeams.end(); /* no hay incremento aquí */) {

            auto& members = it_team->second.members;
            auto it_player = std::remove_if(members.begin(), members.end(),
                                            [&s_playerName](const Player& player) {
                                                return player.name == s_playerName;
                                            });

            if (it_player != members.end()) {
                members.erase(it_player, members.end());
                qDebug() << "  > (Limpiando) " << playerName << " eliminado del equipo " << QString::fromStdString(it_team->first);

                // ¡AÑADIDO! Si el equipo queda vacío, bórralo
                if (it_team->second.members.empty()) {
                    qDebug() << "  > (Limpiando) Equipo " << QString::fromStdString(it_team->first) << " está vacío. Borrando.";
                    it_team = m_lobbyTeams.erase(it_team); // Borra y obtiene el siguiente iterador
                } else {
                    ++it_team; // Incrementa solo si no borramos el equipo
                }
            } else {
                ++it_team; // Incrementa si el jugador no estaba en este equipo
            }
        }
        // --- FIN DE LA LÓGICA DE LIMPIEZA ---

        // 4. Si el equipo nuevo no existe, lo crea
        if (m_lobbyTeams.find(s_teamName) == m_lobbyTeams.end()) {
            m_lobbyTeams[s_teamName] = Team{s_teamName};
        }

        // 5. Añade el jugador al equipo nuevo
        m_lobbyTeams[s_teamName].members.push_back(Player{s_playerName});

        qDebug() << playerName << "se unió al equipo" << teamName;

        // Informamos a todos del cambio
        broadcastLobbyUpdate();

    }

    else if (message == "START_GAME") {
        if (client != m_admin) {
            client->write("ERROR:Solo el Admin puede iniciar la partida.\n");
            return;
        }

        qDebug() << "El Admin inició la partida. Configurando el juego...";
        m_game = Game();

        for (auto const& [name, team] : m_lobbyTeams) {
            m_game.addTeam(team);
        }

        std::string startResult = m_game.start();

        if (m_game.isGameActive()) {
            qDebug() << "¡Juego iniciado con éxito! Informando a los clientes.";


            // 1. Preparamos el mensaje de inicio CON la lista de equipos
            QString teamList = "JUEGO_INICIADO:";
            for (const auto& team : m_game.getTeams()) {
                teamList += QString::fromStdString(team.name) + ";";
            }
            teamList.chop(1); // Quita el último ';'
            teamList += "\n";

            // 2. Enviamos el mensaje de inicio
            sendToAll(teamList);

            // 3. Obtenemos el nombre del PRIMER jugador
            // 3. Obtenemos el nombre del PRIMER y SEGUNDO jugador
            std::string s_currentPlayer = m_game.getCurrentPlayerName();
            std::string s_nextPlayer = m_game.getNextPlayerName(); // <-- NUEVO

            // 4. Enviamos un log anunciando el primer turno
            sendToAll("LOG:¡Juego iniciado! Es el turno de " + QString::fromStdString(s_currentPlayer) + ".\n");

            // 5. ¡MODIFICADO! Enviamos el mensaje de TURNO con ambos nombres
            sendToAll("TURN:" + QString::fromStdString(s_currentPlayer) + ":" + QString::fromStdString(s_nextPlayer) + "\n");
        } else {
            qDebug() << "Falló el inicio del juego:" << QString::fromStdString(startResult);
            QString errorMsg = "ERROR:" + QString::fromStdString(startResult);
            client->write(errorMsg.toUtf8() + '\n');
        }
    }else if (message == "NEXT_TURN") {

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

        // 1. Procesar el turno
        std::string turnResult = m_game.nextTurn();
        qDebug() << "Turno procesado:" << QString::fromStdString(turnResult);

        // 2. Enviar el log del turno (como antes)
        sendToAll("LOG:" + QString::fromStdString(turnResult) + "\n");

        // 3. --- ¡NUEVO! Enviar el estado del juego ---
        QString gameStateMsg = "GAME_UPDATE:";
        const auto& teams = m_game.getTeams();
        int winGoal = m_game.getWinThreshold();

        for (const auto& team : teams) {
            gameStateMsg += QString::fromStdString(team.name) + ":" +
                            QString::number(team.totalPosition) + ";";
        }
        gameStateMsg += "Goal:" + QString::number(winGoal) + "\n";
        sendToAll(gameStateMsg);
        // --- FIN DE LO NUEVO ---

        // 4. Comprobar si el juego terminó
        if (!m_game.isGameActive()) {
            qDebug() << "El juego ha terminado.";
            sendToAll("GAME_OVER:" + QString::fromStdString(m_game.getWinnerName()) + "\n");
        } else {
            // 5. ¡MODIFICADO! Si el juego no ha terminado, anunciar el SIGUIENTE turno
            std::string s_currentPlayer = m_game.getCurrentPlayerName(); // El nuevo jugador actual
            std::string s_nextPlayer = m_game.getNextPlayerName();    // El jugador que le sigue

            sendToAll("TURN:" + QString::fromStdString(s_currentPlayer) + ":" + QString::fromStdString(s_nextPlayer) + "\n");
        }
    }
}
void Server::onClientDisconnected()
{
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket*>(sender());
    if (!clientSocket) return;

    QString playerName = m_clientConnections.value(clientSocket, "Desconocido");
    qDebug() << playerName << "se ha desconectado.";

    // --- ¡NUEVA LÓGICA DE LIMPIEZA! ---
    // 1. Borrar al jugador de CUALQUIER equipo en el que estuviera
    std::string s_playerName = playerName.toStdString();
    for (auto it_team = m_lobbyTeams.begin(); it_team != m_lobbyTeams.end(); /* no hay incremento aquí */) {

        auto& members = it_team->second.members;
        auto it_player = std::remove_if(members.begin(), members.end(),
                                        [&s_playerName](const Player& player) {
                                            return player.name == s_playerName;
                                        });

        if (it_player != members.end()) {
            members.erase(it_player, members.end());
            qDebug() << "  > (Limpiando) " << playerName << " eliminado del equipo " << QString::fromStdString(it_team->first);

            // ¡AÑADIDO! Si el equipo queda vacío, bórralo
            if (it_team->second.members.empty()) {
                qDebug() << "  > (Limpiando) Equipo " << QString::fromStdString(it_team->first) << " está vacío. Borrando.";
                it_team = m_lobbyTeams.erase(it_team); // Borra y obtiene el siguiente iterador
            } else {
                ++it_team; // Incrementa solo si no borramos el equipo
            }
        } else {
            ++it_team; // Incrementa si el jugador no estaba en este equipo
        }
    }

    // --- FIN LÓGICA DE LIMPIEZA ---


    m_clientConnections.remove(clientSocket);

    // (Lógica opcional: si el admin se va, nombrar a otro)
    if (m_admin == clientSocket) {
        qDebug() << "¡El Admin se ha desconectado!";
        m_admin = nullptr;
        if (!m_clientConnections.isEmpty()) {
            m_admin = m_clientConnections.firstKey(); // Nombra al siguiente en la lista
            qDebug() << "Nuevo admin es:" << m_clientConnections.value(m_admin);
            // Avisarle que es el nuevo admin
            m_admin->write("PROMOTION:ADMIN\n");
        }
    }

    broadcastLobbyUpdate(); // Informar a todos que el jugador se fue
}
void Server::broadcastLobbyUpdate()
{
    // Construye el mensaje LOBBY_UPDATE:Azul:Ana,Carlos;Rojo:Bea
    QString lobbyState = "LOBBY_UPDATE:";
    for (auto const& [name, team] : m_lobbyTeams) {
        lobbyState += QString::fromStdString(name) + ":";
        for (size_t i = 0; i < team.members.size(); ++i) {
            lobbyState += QString::fromStdString(team.members[i].name);
            if (i < team.members.size() - 1) lobbyState += ",";
        }
        lobbyState += ";";
    }
    lobbyState.chop(1); // Quita el último ';'
    lobbyState += "\n";

    sendToAll(lobbyState);
}

void Server::sendToAll(const QString& message)
{
    for (QTcpSocket* client : m_clientConnections.keys()) {
        client->write(message.toUtf8());
    }
}
