// mainwindow.h
#pragma once

#include <QMainWindow>
#include <QTcpSocket>     // Para el socket
#include <QHostAddress>   // Para la IP
#include <QMessageBox>    // Para mostrar errores
#include <vector>            // <-- AÑADIR
#include <QProgressBar>      // <-- AÑADIR
#include <QLabel>            // <-- AÑADIR
#include "server.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // Slots para los botones (generados por Qt)
private slots:
    void on_addPlayerButton_clicked();
    void on_startGameButton_clicked();
    void on_nextTurnButton_clicked();
    void on_lobbyButton_clicked();

    void on_hostButton_clicked();
    void on_joinButton_clicked();

    // Slots para la red
    void onSocketConnected();
    void onSocketReadyRead();
    void onSocketDisconnected();

private:
    Ui::MainWindow *ui;
    QTcpSocket* m_socket;     // El "cable" al servidor
    QString m_playerName;     // El nombre de este jugador
    std::vector<QLabel*> m_teamLabels;
    std::vector<QProgressBar*> m_teamProgressBars;
    Server* m_server;
    void clearGameWidgets();
};
