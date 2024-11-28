#pragma once
#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "Enums.h"
#include "Cell.h"
#include "HighScoreEntry.h"

class Game
{
public:
    Game();
    void run();
private:
    // ������� ��� � �.�.

    int windowSizeX = 960;
    int windowSizeY = 640;
    static constexpr int topAreaHeight = 128;
    int mapSizeX = 30;
    int mapSizeY = 16;    
    int MinesCount = 60;

    // ����� ����
    std::vector<std::vector<int>> map;
    std::vector<std::vector<CellState>> mapView;

    // ����������� ��������
    sf::RenderWindow GameWindow;
    sf::Texture MineTexture;
    sf::Texture FlagTexture;
    sf::Texture QuestionTexture;
    sf::Font Font;
    sf::RectangleShape RestartButton;
    sf::Text RestartButtonText;

    // ������ � ������
    sf::RectangleShape DifficultyButton;
    sf::Text DifficultyButtonText;
    Difficulty currentDifficulty;

    sf::RectangleShape SettingsButton;
    sf::Text SettingsButtonText;

    sf::RectangleShape SaveButton;
    sf::Text SaveButtonText;

    sf::RectangleShape LoadButton;
    sf::Text LoadButtonText;

    sf::RectangleShape HighScoresButton;
    sf::Text HighScoresButtonText;

    // ���������� ��������� ����
    int hoveredX;
    int hoveredY;

    bool firstClick;
    bool gameLost;
    bool gameWon;
    bool isInitializing;
    bool modalWindowOpen;

    // ��������� ������ ����
    sf::Mouse::Button openCellButton;
    sf::Mouse::Button flagCellButton;

    // ����� � ����
    sf::Clock gameClock;
    sf::Time elapsedTime;
    int flagsPlaced;
    sf::Time timeOffset;

    // ���������� � ��������
    std::string saveFileName;

    // ������� �������
    std::vector<HighScoreEntry> highScores;
    std::string highScoresFileName;

    // ����������� ��� ������ �������� ������
    static const int directions[8][2];

    // �������� ����
    int fieldOffsetX;
    int fieldOffsetY;

    // ����� ����
    bool allowMistakes;
    int mistakesAllowed;
    int mistakesMade;
    bool unknownMineCount;

    // directions
    static const int directions[8][2];



    // ��������� ������
    void HandleEvents();
    void RenderEvents();
    int random(int min, int max);
    void setNumbers();
    void GenerateMap(int startX, int startY);
    void RestartGame();
    void revealAllMines();
    void gameOver();
    void openAllZeroAround(int x, int y);
    void RunDifficultyWindow();
    void safeOpen(int x, int y);
    void checkWin();
    void openSettingsWindow();
    void saveGame();
    void loadGame();
    void saveHighScore(const sf::String& playerName);
    void loadHighScores();
    void displayHighScores();
};



#endif // GAME_H