#pragma once
#include <string>
#include <vector>
#include <random>

struct Player {
    std::string name;
    int position = 0;
};

struct Team {
    std::string name;
    std::vector<Player> members;
    int totalPosition = 0;
    int nextPlayerIndex = 0;

    void updateTotal() {
        totalPosition = 0;
        for (auto& p : members) totalPosition += p.position;
    }
};

struct Dice {
    int faces;
    std::mt19937 rng;
    std::uniform_int_distribution<int> dist;

    Dice(int f = 6) : faces(f), rng(std::random_device{}()), dist(1, f) {}

    int roll() { return dist(rng); }
};
