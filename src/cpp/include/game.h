#pragma once
#include "game_entities.h"
#include <vector>
#include <string>

class Game {
    int winThreshold;
    Dice dice;
    std::vector<Team> teams;
    int currentTeam = 0;
    bool started = false;
    std::string winnerName;

public:
    Game();
    std::string getCurrentPlayerName() const;
    void addTeam(const Team& team);
    bool canStart() const;

    std::string start();
    std::string nextTurn();
    bool isGameActive() const;
    std::string getWinnerName() const;
    std::string getNextPlayerName() const;
    bool checkWin();
    const std::vector<Team>& getTeams() const { return teams; }
    int getWinThreshold() const { return winThreshold; }
};
