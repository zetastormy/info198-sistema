#include "../include/game.h"
#include <iostream>
#include <random>
#include <sstream>
#include <QDebug>

Game::Game() : dice(6), started(false), currentTeam(0),winnerName("") {
    const char* envWin = std::getenv("WIN_THRESHOLD");

    winThreshold = envWin ? std::atoi(envWin) : 20;
}

void Game::addTeam(const Team& team) {
    if (!started) {
        teams.push_back(team);
    }
}

std::string Game::getWinnerName() const {
    return winnerName;
}

bool Game::canStart() const {
    if (teams.size() < 2) return false;
    for (const auto& t : teams)
        if (t.members.size() < 2)
            return false;
    return true;
}

bool Game::isGameActive() const {
    return started;
}

std::string Game::start() {
    if (!canStart()) {
        started = false;
        return "ERROR: No se cumplen las condiciones para iniciar (min 2 equipos, min 2 jugadores/equipo).\n";
    }
    started = true;

    if (!teams.empty()) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, teams.size() - 1);
        currentTeam = dis(gen);
        qDebug() << "[DEBUG] Equipo inicial aleatorio elegido:" << currentTeam;
    }

    std::string msg = "¡Juego iniciado con " + std::to_string(teams.size()) + " equipos!\n";
    return msg;
}

std::string Game::nextTurn() {
    if (!started) return "El juego ha terminado.";

    std::stringstream ss;

    Team& team = teams[currentTeam];
    Player& player = team.members[team.nextPlayerIndex];

    ss << "Turno de: " << player.name << " (Equipo " << team.name << ")|";

    int roll = dice.roll();
    player.position += roll;
    ss << "  > " << player.name << " lanzó un " << roll
       << ". Posición total: " << player.position << "|";

    team.updateTotal();
    ss << "  Total del equipo " << team.name << ": " << team.totalPosition << " / " << winThreshold;

    if (checkWin()) {
        ss << "|¡¡¡El equipo " << team.name << " ha ganado!!!";
        started = false;
        winnerName = team.name;
    } else {
        team.nextPlayerIndex = (team.nextPlayerIndex + 1) % team.members.size();
        currentTeam = (currentTeam + 1) % teams.size();
    }

    return ss.str();
}

std::string Game::getNextPlayerName() const {
    if (!started || teams.empty() || teams.size() < 2) {
        return "";
    }

    int nextTeamIndex = (currentTeam + 1) % teams.size();
    const Team& nextTeam = teams[nextTeamIndex];

    int nextPlayerIndex = nextTeam.nextPlayerIndex;

    if (nextTeam.members.empty()) {
        return "";
    }

    return nextTeam.members[nextPlayerIndex].name;
}

std::string Game::getCurrentPlayerName() const {
    if (!started || teams.empty()) {
        return "";
    }

    const Team& team = teams[currentTeam];

    if (team.members.empty()) {
        return "";
    }

    return team.members[team.nextPlayerIndex].name;
}

bool Game::checkWin() {
    return teams[currentTeam].totalPosition >= winThreshold;
}
