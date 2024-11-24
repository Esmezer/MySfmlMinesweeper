#pragma once
#ifndef HIGHSCOREENTRY_H
#define HIGHSCOREENTRY_H

#include <SFML/Graphics.hpp>
#include "Enums.h"

struct HighScoreEntry {
    sf::String playerName;
    sf::Time time;
    Difficulty difficulty;
};

#endif // HIGHSCOREENTRY_H