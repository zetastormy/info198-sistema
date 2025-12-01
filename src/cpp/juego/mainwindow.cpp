#include "../include/mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include "../include/dotenv.h"
#include <QHostAddress>
#include <QTimer>
#include <QCoreApplication> // Necesario para applicationDirPath

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow), m_server(nullptr) {
    ui->setupUi(this);

    ui->stackedWidget->setCurrentIndex(0);

    m_socket = new QTcpSocket(this);

    m_connectionTimer = new QTimer(this);
    m_connectionTimer->setSingleShot(true);
    connect(m_connectionTimer, &QTimer::timeout, this, &MainWindow::onConnectionTimeout);

    connect(m_socket, &QTcpSocket::connected, this, &MainWindow::onSocketConnected);
    connect(m_socket, &QTcpSocket::readyRead, this, &MainWindow::onSocketReadyRead);
    connect(m_socket, &QTcpSocket::disconnected, this, &MainWindow::onSocketDisconnected);
    connect(m_socket, &QTcpSocket::errorOccurred, this, &MainWindow::onSocketError);

}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_hostButton_clicked() {
    m_playerName = ui->nameInput->text();

    if (m_playerName.isEmpty()) {
        QMessageBox::warning(this, "Error", "Debes ingresar un nombre.");
        return;
    }

    // 1. Lógica combinada: Configurar Log (Local)
    QString logPath = getLogFilePath(); 
    m_logFile.setFileName(logPath);
    
    // 2. Lógica combinada: Configurar .env y Puerto (Remoto)
    QString path = QCoreApplication::applicationDirPath() + "/../.env";
    dotenv env(path.toStdString());
    // Leemos el puerto o usamos 8020 por defecto si no existe
    int gamePort = std::stoi(env.get("GAME_PORT", "8020"));

    qDebug() << "Iniciando modo host...";
    if (m_logFile.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&m_logFile);
        out << "--------------------------------------------------\n"; 
        writeToLog("INICIO", "MODO:HOST, IP:127.0.0.1, CREADOR:" + m_playerName);
    }    
    
    m_server = new Server(this);
    m_server->startServer();

    ui->statusbar->showMessage("Creando partida (conectando a 127.0.0.1)...");
    
    m_connectionTimer->start(10000); 
    // Usamos el puerto del .env
    m_socket->connectToHost(QHostAddress("127.0.0.1"), gamePort);

    ui->hostButton->setEnabled(false);
    ui->joinButton->setEnabled(false);
}

void MainWindow::on_joinButton_clicked() {
    m_playerName = ui->nameInput->text();
    QString ip = ui->ipInput->text();

    if (ip.isEmpty() || m_playerName.isEmpty()) {
        QMessageBox::warning(this, "Error", "Debes ingresar una IP y un nombre.");
        return;
    }

    qDebug() << "Iniciando modo cliente, conectando a" << ip;
    
    // 1. Lógica combinada: Configurar Log (Local)
    QString logPath = getLogFilePath();
    m_logFile.setFileName(logPath);
    if (m_logFile.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&m_logFile);
        out << "--------------------------------------------------\n";
        writeToLog("INICIO", "MODO:CLIENTE, IP:" + ui->ipInput->text() + ", JUGADOR:" + m_playerName);
    }
    
    // 2. Lógica combinada: Configurar .env y Puerto (Remoto)
    QString path = QCoreApplication::applicationDirPath() + "/../.env";
    dotenv env(path.toStdString());
    int gamePort = std::stoi(env.get("GAME_PORT", "8020"));

    ui->statusbar->showMessage("Conectando a " + ip + "...");
    
    m_connectionTimer->start(10000); 
    // Usamos el puerto del .env
    m_socket->connectToHost(QHostAddress(ip), gamePort);

    ui->hostButton->setEnabled(false);
    ui->joinButton->setEnabled(false);
}

void MainWindow::on_addPlayerButton_clicked() {
    QString teamName = ui->teamNameInput->text();
    if (teamName.isEmpty()) return;

    QString message = "JOIN_TEAM:" + teamName + "\n";
    m_socket->write(message.toUtf8());

    ui->teamNameInput->clear();
}

void MainWindow::on_startGameButton_clicked() {
    m_socket->write("START_GAME\n");

    ui->startGameButton->setEnabled(false);
}

void MainWindow::writeToLog(const QString &eventType, const QString &details) {
        if (!m_logFile.isOpen()) return;

        QTextStream out(&m_logFile);
        out << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss") << ","
            << eventType << ","
            << details << "\n";
        m_logFile.flush();
}

void MainWindow::on_nextTurnButton_clicked() {
    m_socket->write("NEXT_TURN\n");
}

void MainWindow::on_lobbyButton_clicked() {
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::on_exitButton_ConnectionPage_clicked() {
    runGraphScript();
    qDebug() << "Botón Salir (Inicio) presionado. Cerrando la aplicación.";
    this->close();
}

void MainWindow::onSocketConnected() {
    qDebug() << "Cliente: Conectado al servidor.";

    m_connectionTimer->stop();
    ui->statusbar->clearMessage();

    QString message = "CONNECT:" + m_playerName + "\n";
    m_socket->write(message.toUtf8());
    this->setWindowTitle("Juego - " + m_playerName);
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::onSocketDisconnected() {
    qDebug() << "Cliente: Desconectado del servidor.";

    if (ui->stackedWidget->currentIndex() != 0) {
         QMessageBox::critical(this, "Desconectado", "Se perdió la conexión con el servidor.");
    }

    m_connectionTimer->stop();
    ui->statusbar->clearMessage();
    ui->hostButton->setEnabled(true);
    ui->joinButton->setEnabled(true);

    if (m_server) {
        qDebug() << "Cerrando el servidor local.";
        m_server->disconnect();
        m_server->deleteLater();
        m_server = nullptr;
    }

    m_isAdmin = false;
    ui->stackedWidget->setCurrentIndex(0);
}
void MainWindow::closeEvent(QCloseEvent *event) {
    runGraphScript(); 
    event->accept();  
}

QString MainWindow::getLogFilePath() {
    QDir dataDir("data");
    if (!dataDir.exists()) QDir().mkpath("data");
    
    if (!dataDir.exists()) {
        QDir().mkpath("data"); 
        qDebug() << "Carpeta 'data' creada.";
    }
    return "data/historial_global.txt"; 
}


void MainWindow::runGraphScript() {
    if (m_logFile.isOpen()) {
        m_logFile.flush();
        m_logFile.close();
    }

    QString scriptPath = "src/py/graph.py";
    QString csvPath = "data/historial_global.txt";

    qDebug() << "Ejecutando:" << scriptPath << "con datos en:" << csvPath;

    QString program = "python3";
    QStringList arguments;
    arguments << scriptPath << csvPath;
    bool exito = QProcess::startDetached(program, arguments, QDir::currentPath());
    
    if (exito) {
        qDebug() << "ÉXITO: El sistema aceptó el comando para lanzar Python.";
    } else {
        qCritical() << "ERROR: No se pudo lanzar Python. ¿Tienes 'python3' instalado? ¿La ruta es correcta?";
    }
}


void MainWindow::onConnectionTimeout() {
    qDebug() << "¡Timeout de conexión!";

    m_socket->abort();

    QMessageBox::warning(this, "Error de Conexión",
                         "No se pudo conectar al servidor (tiempo de espera de 10s agotado). "
                         "Verifica la IP o que el servidor esté activo.");

    ui->statusbar->clearMessage();
    ui->hostButton->setEnabled(true);
    ui->joinButton->setEnabled(true);
}

void MainWindow::onSocketError(QAbstractSocket::SocketError socketError) {
    if (m_connectionTimer->isActive()) {
        qDebug() << "Error de socket durante la conexión:" << m_socket->errorString();

        m_connectionTimer->stop();

        QMessageBox::critical(this, "Error de Conexión", m_socket->errorString());

        ui->statusbar->clearMessage();
        ui->hostButton->setEnabled(true);
        ui->joinButton->setEnabled(true);
    }
}

void MainWindow::clearGameWidgets() {
    for (QLabel* label : m_teamLabels) {
        delete label;
    }

    m_teamLabels.clear();

    for (QProgressBar* bar : m_teamProgressBars) {
        delete bar;
    }

    m_teamProgressBars.clear();
}

void MainWindow::onSocketReadyRead() {
    QByteArray data = m_socket->readAll();
    QStringList messages = QString(data).trimmed().split('\n', Qt::SkipEmptyParts);

    for (const QString& message : messages) {
        qDebug() << "Cliente: Mensaje recibido:" << message;

        if (message.startsWith("CONNECT_OK:")) {
            QString role = message.section(':', 1);

            m_isAdmin = (role == "ADMIN");

            if (m_isAdmin) {
                ui->startGameButton->setVisible(true);
            } else {
                ui->startGameButton->setVisible(false);
            }
        } else if (message.startsWith("LOBBY_UPDATE:")) {
            QString data = message.section(':', 1);
            ui->lobbyDisplay->clear();
            ui->lobbyDisplay->append("--- Estado del Lobby ---");

            QStringList teams = data.split(';', Qt::SkipEmptyParts);
            writeToLog("LOBBY_STATE", data);
            int teamCount = teams.size();
            bool canStart = true;

            if (teamCount < 2) {
                canStart = false;
            }

            for (const QString& team : teams) {
                QStringList parts = team.split(':');
                QString teamName = parts[0];
                QString players;
                QStringList playerList;

                if (parts.length() >= 2) {
                    players = parts[1];
                    playerList = players.split(',', Qt::SkipEmptyParts);
                }

                if (playerList.count() < 2) {
                    canStart = false;
                }

                ui->lobbyDisplay->append("Equipo [" + teamName + "]: " + players.replace(",", ", "));
            }

            if (m_isAdmin) {
                if (canStart) {
                    ui->startGameButton->setEnabled(true);
                    qDebug() << "Condiciones de inicio CUMPLIDAS (>=2 equipos, >=2 jugadores/equipo). Botón habilitado.";
                } else {
                    ui->startGameButton->setEnabled(false);
                    qDebug() << "Condiciones de inicio NO CUMPLIDAS. Botón deshabilitado.";
                }
            }
        } else if (message.startsWith("ERROR:")) {
            QString errorMsg = message.section(':', 1);
            QMessageBox::warning(this, "Error del Servidor", errorMsg);

            if (ui->startGameButton->isVisible()) {
                ui->startGameButton->setEnabled(true);
            }
        } else if (message.startsWith("JUEGO_INICIADO:")) {
            qDebug() << "[DEBUG] Entrando a JUEGO_INICIADO...";

            clearGameWidgets();
            qDebug() << "[DEBUG] 1. clearGameWidgets() OK.";

            ui->logDisplay->clear();
            qDebug() << "[DEBUG] 2. logDisplay->clear() OK. (¡Significa que ui->logDisplay NO es nulo!)";

            ui->logDisplay->append("¡El juego ha comenzado!");
            qDebug() << "[DEBUG] 3. logDisplay->append() OK.";

            QString teamData = message.section(':', 1);
            QStringList teams = teamData.split(';', Qt::SkipEmptyParts);
            qDebug() << "[DEBUG] 4. Lista de equipos procesada:" << teams;

            for (const QString& teamName : teams) {
                qDebug() << "[DEBUG] 5. Creando widgets para equipo:" << teamName;
                QLabel* nameLabel = new QLabel(teamName);
                nameLabel->setStyleSheet("font-weight: bold; font-size: 14px;");

                QProgressBar* bar = new QProgressBar();

                ui->gameLayout->addWidget(nameLabel);
                qDebug() << "[DEBUG] 6. nameLabel añadido a gameLayout OK.";

                ui->gameLayout->addWidget(bar);
                qDebug() << "[DEBUG] 7. bar añadido a gameLayout OK.";

                m_teamLabels.push_back(nameLabel);
                m_teamProgressBars.push_back(bar);
            }

            qDebug() << "[DEBUG] 8. Bucle de creación de widgets finalizado.";

            ui->stackedWidget->setCurrentIndex(2);
            qDebug() << "[DEBUG] 9. ¡¡CAMBIO DE PÁGINA REALIZADO!!";
        } else if (message.startsWith("GAME_UPDATE:")) {
            QStringList parts = message.section(':', 1).split(';', Qt::SkipEmptyParts);
            int winGoal = 20;

            if (!parts.isEmpty() && parts.last().startsWith("Objetivo:")) {
                winGoal = parts.last().section(':', 1).toInt();
                parts.removeLast();
            }

            for (const QString& teamData : parts) {
                QString teamName = teamData.section(':', 0, 0);
                int score = teamData.section(':', 1, 1).toInt();

                for (size_t i = 0; i < m_teamLabels.size(); ++i) {
                    if (m_teamLabels[i]->text() == teamName && i < m_teamProgressBars.size()) {
                        m_teamProgressBars[i]->setMaximum(winGoal);
                        m_teamProgressBars[i]->setValue(score);
                        QString text = (teamName + " (%1/%2)").arg(score).arg(winGoal);
                        m_teamProgressBars[i]->setFormat(text);
                        break;
                    }
                }
            }
        } else if (message.startsWith("LOG:")) {
            QString logMsg = message.section(':', 1);

            logMsg.replace("|", "\n");
            if (logMsg.contains("lanzó un")) {
                    writeToLog("TURNO", logMsg);
                } else {
                    writeToLog("INFO", logMsg); 
                }
            ui->logDisplay->append(logMsg);
        } else if (message.startsWith("GAME_OVER:")) {
            QString winner = message.section(':', 1);
            
            ui->winnerLabel->setText("¡El equipo '" + winner + "' ha ganado!");
            ui->nextTurnButton->setEnabled(false);
            ui->stackedWidget->setCurrentIndex(3);
            qDebug() << "Juego terminado. Generando gráficos...";
            runGraphScript();
        } else if (message.startsWith("TURN:")) {
            QString currentPlayer = message.section(':', 1, 1);
            QString nextPlayer = message.section(':', 2, 2);

            if (currentPlayer == m_playerName) {
                ui->nextTurnButton->setEnabled(true);
                ui->turnStatusLabel->setText("¡ES TU TURNO!");
                ui->turnStatusLabel->setStyleSheet("background-color: #4CAF50; color: white; padding: 5px; font-weight: bold; border-radius: 5px;");

            } else if (nextPlayer == m_playerName) {
                ui->nextTurnButton->setEnabled(false);
                ui->turnStatusLabel->setText("¡TE TOCA DESPUÉS! (Turno de: " + currentPlayer + ")");
                ui->turnStatusLabel->setStyleSheet("background-color: #FFC107; color: black; padding: 5px; font-weight: bold; border-radius: 5px;");

            } else {
                ui->nextTurnButton->setEnabled(false);
                ui->turnStatusLabel->setText("Turno de: " + currentPlayer);
                ui->turnStatusLabel->setStyleSheet("background-color: #F44336; color: white; padding: 5px; border-radius: 5px;");
            }
        } 
    }
}
