#include "../include/game.h"
#include <iostream>
#include <random>
#include <sstream>
#include <QDebug>
#include <algorithm> // Necesario para find_if

Game::Game() : dice(6), started(false), currentTeam(0), winnerName("") {
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
    
    // Protección extra por si el índice se desincronizó
    if (team.nextPlayerIndex >= team.members.size()) {
        team.nextPlayerIndex = 0;
    }
    
    Player& player = team.members[team.nextPlayerIndex];

    ss << "Turno de: " << player.name << " (Equipo " << team.name << ")|";

    int roll = dice.roll();
    player.position += roll;
    ss << "  > " << player.name << " lanzó un " << roll
       << ". Posición total: " << player.position << "|";

    team.updateTotal(); // Suma las posiciones de todos los miembros actuales
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

    if (nextTeam.members.empty()) {
        return "";
    }
    
    // Asegurar índice válido
    int idx = nextTeam.nextPlayerIndex;
    if (idx >= nextTeam.members.size()) idx = 0;

    return nextTeam.members[idx].name;
}

int Game::removePlayer(const std::string& playerName) {
    if (!started) return 0;

    for (size_t i = 0; i < teams.size(); ++i) {
        auto& team = teams[i];
        
        // Usamos find_if para ubicar al jugador ANTES de borrarlo
        auto it = std::find_if(team.members.begin(), team.members.end(),
                               [&](const Player& p){ return p.name == playerName; });
        
        if (it != team.members.end()) {
            // 1. Guardar sus puntos para no perder el progreso del equipo
            int puntosGuardados = it->position;
            
            // 2. Calcular índices
            int indexBorrado = std::distance(team.members.begin(), it);
            bool eraSuTurno = (i == currentTeam && indexBorrado == team.nextPlayerIndex);
            
            // 3. Borrar al jugador
            team.members.erase(it);
            
            // 4. HERENCIA DE PUNTOS: Si queda alguien, le damos los puntos
            if (!team.members.empty()) {
                team.members[0].position += puntosGuardados;
                // Actualizamos el total del equipo inmediatamente para reflejar el cambio
                team.updateTotal(); 
            }
            
            // 5. Ajustar índice de turno
            if (indexBorrado < team.nextPlayerIndex) {
                team.nextPlayerIndex--;
            }
            if (team.nextPlayerIndex >= team.members.size()) {
                team.nextPlayerIndex = 0;
            }

            // 6. Verificar si el equipo murió
            if (team.members.empty()) {
                teams.erase(teams.begin() + i);
                
                if (i < currentTeam) {
                    currentTeam--;
                } 
                else if (i == currentTeam) {
                    if (currentTeam >= teams.size()) {
                        currentTeam = 0;
                    }
                }

                if (teams.size() < 2) return 3; // Victoria automática
                return 1; // Equipo eliminado
            }

            // 7. Si era su turno, pasamos al siguiente equipo
            if (eraSuTurno) {
                currentTeam = (currentTeam + 1) % teams.size();
                return 2; 
            }
            return 0; 
        }
    }
    return -1; 
}


std::string Game::getCurrentPlayerName() const {
    if (!started || teams.empty()) {
        return "";
    }

    const Team& team = teams[currentTeam];

    if (team.members.empty()) {
        return "";
    }
    
    int idx = team.nextPlayerIndex;
    if (idx >= team.members.size()) idx = 0;

    return team.members[idx].name;
}

bool Game::checkWin() {
    if (teams.empty()) return false;
    // Aseguramos que el total esté actualizado
    // (team es const en getTeams, pero aquí accedemos directo)
    return teams[currentTeam].totalPosition >= winThreshold;
}
