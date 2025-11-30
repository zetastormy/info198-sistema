#pragma once

#include <QMainWindow>
#include <QTcpSocket>
#include <QHostAddress>
#include <QMessageBox>
#include <QProgressBar>
#include <QLabel>
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

private slots:
    void on_addPlayerButton_clicked();
    void on_startGameButton_clicked();
    void on_nextTurnButton_clicked();
    void on_lobbyButton_clicked();
    void on_exitButton_ConnectionPage_clicked();

    void on_hostButton_clicked();
    void on_joinButton_clicked();

    // Slots para la red
    void onSocketConnected();
    void onSocketReadyRead();
    void onSocketDisconnected();
    void onSocketError(QAbstractSocket::SocketError socketError);
    void onConnectionTimeout();

private:
    Ui::MainWindow *ui;
    QTcpSocket* m_socket;
    QString m_playerName;
    QTimer *m_connectionTimer;
    bool m_isAdmin = false;

    QList<QLabel*> m_teamLabels;
    QList<QProgressBar*> m_teamProgressBars;
    Server* m_server;
    void clearGameWidgets();
};
