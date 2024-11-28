#include "Game.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <ctime>



const int Game::directions[8][2] = {
    {-1, -1}, {-1, 0}, {-1, 1},
    {0, -1},  {0, 1},
    {1, -1},  {1, 0},  {1, 1}
};

Game::Game() : GameWindow(sf::VideoMode(windowSizeX, windowSizeY), "Minesweeper!", sf::Style::Titlebar | sf::Style::Close),
firstClick(true), gameLost(false), gameWon(false), isInitializing(true),
hoveredX(-1), hoveredY(-1), currentDifficulty(Difficulty::Hard),
openCellButton(sf::Mouse::Left), flagCellButton(sf::Mouse::Right),
saveFileName("savegame.dat"), highScoresFileName("highscores.dat"), flagsPlaced(0),
fieldOffsetX(0), fieldOffsetY(0), timeOffset(sf::Time::Zero),
allowMistakes(false), mistakesAllowed(0), mistakesMade(0), unknownMineCount(false), modalWindowOpen(false)
{
    // Загрузка ресурсов
    if (!FlagTexture.loadFromFile("flag.png")) {
        exit(-1);
    }
    if (!MineTexture.loadFromFile("mine.png")) {
        exit(-1);
    }
    if (!QuestionTexture.loadFromFile("question.png"))
    {
        exit(-1);
    }
    if (!Font.loadFromFile("arial.ttf")) {
        exit(-1);
    }

    RestartButton.setSize(sf::Vector2f(200, 50));
    RestartButton.setFillColor(sf::Color::White);
    RestartButton.setOutlineColor(sf::Color::Black);
    RestartButton.setOutlineThickness(2);
    RestartButton.setPosition(windowSizeX / 2 - RestartButton.getSize().x / 2, 10);

    RestartButtonText.setFont(Font);
    RestartButtonText.setString("Restart");
    RestartButtonText.setCharacterSize(24);
    RestartButtonText.setFillColor(sf::Color::Black);
    RestartButtonText.setPosition(
        RestartButton.getPosition().x + (RestartButton.getSize().x - RestartButtonText.getGlobalBounds().width) / 2,
        RestartButton.getPosition().y + (RestartButton.getSize().y - RestartButtonText.getGlobalBounds().height) / 2 - 5
    );

    DifficultyButton.setSize(sf::Vector2f(200, 50));
    DifficultyButton.setFillColor(sf::Color::White);
    DifficultyButton.setOutlineColor(sf::Color::Black);
    DifficultyButton.setOutlineThickness(2);
    DifficultyButton.setPosition(10, 10);

    DifficultyButtonText.setFont(Font);
    DifficultyButtonText.setString("Difficulty");
    DifficultyButtonText.setCharacterSize(18);
    DifficultyButtonText.setFillColor(sf::Color::Black);
    DifficultyButtonText.setPosition(
        DifficultyButton.getPosition().x + (DifficultyButton.getSize().x - DifficultyButtonText.getGlobalBounds().width) / 2,
        DifficultyButton.getPosition().y + (DifficultyButton.getSize().y - DifficultyButtonText.getGlobalBounds().height) / 2 - 5
    );

    SettingsButton.setSize(sf::Vector2f(200, 50));
    SettingsButton.setFillColor(sf::Color::White);
    SettingsButton.setOutlineColor(sf::Color::Black);
    SettingsButton.setOutlineThickness(2);
    SettingsButton.setPosition(10, 70);

    SettingsButtonText.setFont(Font);
    SettingsButtonText.setString("Settings");
    SettingsButtonText.setCharacterSize(18);
    SettingsButtonText.setFillColor(sf::Color::Black);
    SettingsButtonText.setPosition(
        SettingsButton.getPosition().x + (SettingsButton.getSize().x - SettingsButtonText.getGlobalBounds().width) / 2,
        SettingsButton.getPosition().y + (SettingsButton.getSize().y - SettingsButtonText.getGlobalBounds().height) / 2 - 5
    );

    SaveButton.setSize(sf::Vector2f(200, 50));
    SaveButton.setFillColor(sf::Color::White);
    SaveButton.setOutlineColor(sf::Color::Black);
    SaveButton.setOutlineThickness(2);
    SaveButton.setPosition(windowSizeX - 210, 10);

    SaveButtonText.setFont(Font);
    SaveButtonText.setString("Save Game");
    SaveButtonText.setCharacterSize(18);
    SaveButtonText.setFillColor(sf::Color::Black);
    SaveButtonText.setPosition(
        SaveButton.getPosition().x + (SaveButton.getSize().x - SaveButtonText.getGlobalBounds().width) / 2,
        SaveButton.getPosition().y + (SaveButton.getSize().y - SaveButtonText.getGlobalBounds().height) / 2 - 5
    );

    LoadButton.setSize(sf::Vector2f(200, 50));
    LoadButton.setFillColor(sf::Color::White);
    LoadButton.setOutlineColor(sf::Color::Black);
    LoadButton.setOutlineThickness(2);
    LoadButton.setPosition(windowSizeX - 210, 70);

    LoadButtonText.setFont(Font);
    LoadButtonText.setString("Load Game");
    LoadButtonText.setCharacterSize(18);
    LoadButtonText.setFillColor(sf::Color::Black);
    LoadButtonText.setPosition(
        LoadButton.getPosition().x + (LoadButton.getSize().x - LoadButtonText.getGlobalBounds().width) / 2,
        LoadButton.getPosition().y + (LoadButton.getSize().y - LoadButtonText.getGlobalBounds().height) / 2 - 5
    );

    HighScoresButton.setSize(sf::Vector2f(200, 50));
    HighScoresButton.setFillColor(sf::Color::White);
    HighScoresButton.setOutlineColor(sf::Color::Black);
    HighScoresButton.setOutlineThickness(2);
    HighScoresButton.setPosition(windowSizeX / 2 - 100, 70);

    HighScoresButtonText.setFont(Font);
    HighScoresButtonText.setString("High Scores");
    HighScoresButtonText.setCharacterSize(18);
    HighScoresButtonText.setFillColor(sf::Color::Black);
    HighScoresButtonText.setPosition(
        HighScoresButton.getPosition().x + (HighScoresButton.getSize().x - HighScoresButtonText.getGlobalBounds().width) / 2,
        HighScoresButton.getPosition().y + (HighScoresButton.getSize().y - HighScoresButtonText.getGlobalBounds().height) / 2 - 5
    );

    loadHighScores();
    RestartGame();

    // Устанавливаем ограничение по частоте кадров
    GameWindow.setFramerateLimit(60);
}

void Game::RunDifficultyWindow() {
    modalWindowOpen = true;
    sf::RenderWindow difficultyWindow(sf::VideoMode(500, 500), "Difficulty Selection", sf::Style::Titlebar | sf::Style::Close);
    difficultyWindow.clear(sf::Color(192, 192, 192));

    // Ограничение FPS для окна выбора сложности
    difficultyWindow.setFramerateLimit(60);

    // Кнопки выбора сложности
    sf::RectangleShape easyButton(sf::Vector2f(200, 50));
    easyButton.setFillColor(sf::Color::White);
    easyButton.setOutlineColor(sf::Color::Black);
    easyButton.setOutlineThickness(2);
    easyButton.setPosition(150, 50);
    sf::Text easyText("Easy", Font, 18);
    easyText.setFillColor(sf::Color::Black);
    easyText.setPosition(225, 60);

    sf::RectangleShape mediumButton(sf::Vector2f(200, 50));
    mediumButton.setFillColor(sf::Color::White);
    mediumButton.setOutlineColor(sf::Color::Black);
    mediumButton.setOutlineThickness(2);
    mediumButton.setPosition(150, 110);
    sf::Text mediumText("Medium", Font, 18);
    mediumText.setFillColor(sf::Color::Black);
    mediumText.setPosition(215, 120);

    sf::RectangleShape hardButton(sf::Vector2f(200, 50));
    hardButton.setFillColor(sf::Color::White);
    hardButton.setOutlineColor(sf::Color::Black);
    hardButton.setOutlineThickness(2);
    hardButton.setPosition(150, 170);
    sf::Text hardText("Hard", Font, 18);
    hardText.setFillColor(sf::Color::Black);
    hardText.setPosition(225, 180);

    sf::RectangleShape customButton(sf::Vector2f(200, 50));
    customButton.setFillColor(sf::Color::White);
    customButton.setOutlineColor(sf::Color::Black);
    customButton.setOutlineThickness(2);
    customButton.setPosition(150, 230);
    sf::Text customText("Custom", Font, 18);
    customText.setFillColor(sf::Color::Black);
    customText.setPosition(220, 240);

    sf::RectangleShape randomButton(sf::Vector2f(200, 50));
    randomButton.setFillColor(sf::Color::White);
    randomButton.setOutlineColor(sf::Color::Black);
    randomButton.setOutlineThickness(2);
    randomButton.setPosition(150, 290);
    sf::Text randomText("Random", Font, 18);
    randomText.setFillColor(sf::Color::Black);
    randomText.setPosition(215, 300);

    // Чекбоксы для опций
    sf::RectangleShape mistakeOption(sf::Vector2f(20, 20));
    mistakeOption.setFillColor(allowMistakes ? sf::Color::Green : sf::Color::White);
    mistakeOption.setOutlineColor(sf::Color::Black);
    mistakeOption.setOutlineThickness(2);
    mistakeOption.setPosition(50, 370);
    sf::Text mistakeText("Allow mistakes", Font, 18);
    mistakeText.setFillColor(sf::Color::Black);
    mistakeText.setPosition(80, 365);

    sf::RectangleShape unknownMinesOption(sf::Vector2f(20, 20));
    unknownMinesOption.setFillColor(unknownMineCount ? sf::Color::Green : sf::Color::White);
    unknownMinesOption.setOutlineColor(sf::Color::Black);
    unknownMinesOption.setOutlineThickness(2);
    unknownMinesOption.setPosition(50, 400);
    sf::Text unknownMinesText("Unknown mine count", Font, 18);
    unknownMinesText.setFillColor(sf::Color::Black);
    unknownMinesText.setPosition(80, 395);

    difficultyWindow.clear(sf::Color(192, 192, 192));
    difficultyWindow.draw(easyButton);
    difficultyWindow.draw(easyText);
    difficultyWindow.draw(mediumButton);
    difficultyWindow.draw(mediumText);
    difficultyWindow.draw(hardButton);
    difficultyWindow.draw(hardText);
    difficultyWindow.draw(customButton);
    difficultyWindow.draw(customText);
    difficultyWindow.draw(randomButton);
    difficultyWindow.draw(randomText);
    difficultyWindow.draw(mistakeOption);
    difficultyWindow.draw(mistakeText);
    difficultyWindow.draw(unknownMinesOption);
    difficultyWindow.draw(unknownMinesText);
    difficultyWindow.display();

    // Обработка событий
    sf::Event ev;
    while (difficultyWindow.isOpen()) {
        while (difficultyWindow.pollEvent(ev)) {
            if (ev.type == sf::Event::Closed) {
                difficultyWindow.close();
            }
            if (ev.type == sf::Event::MouseButtonPressed) {
                // Проверяем нажатия на кнопки
                if (easyButton.getGlobalBounds().contains(ev.mouseButton.x, ev.mouseButton.y)) {
                    currentDifficulty = Difficulty::Easy;
                    mapSizeX = 9;
                    mapSizeY = 9;
                    MinesCount = 10;
                    difficultyWindow.close();
                    RestartGame();
                }
                else if (mediumButton.getGlobalBounds().contains(ev.mouseButton.x, ev.mouseButton.y)) {
                    currentDifficulty = Difficulty::Medium;
                    mapSizeX = 16;
                    mapSizeY = 16;
                    MinesCount = 40;
                    difficultyWindow.close();
                    RestartGame();
                }
                else if (hardButton.getGlobalBounds().contains(ev.mouseButton.x, ev.mouseButton.y)) {
                    currentDifficulty = Difficulty::Hard;
                    mapSizeX = 30;
                    mapSizeY = 16;
                    MinesCount = 99;
                    difficultyWindow.close();
                    RestartGame();
                }
                else if (customButton.getGlobalBounds().contains(ev.mouseButton.x, ev.mouseButton.y)) {
                    // Открываем окно для ввода пользовательских параметров
                    sf::RenderWindow customWindow(sf::VideoMode(400, 300), "Custom Settings", sf::Style::Titlebar | sf::Style::Close);
                    sf::Text widthText("Width (max 30):", Font, 18);
                    widthText.setPosition(50, 50);
                    sf::Text heightText("Height (max 16):", Font, 18); 
                    heightText.setPosition(50, 100);
                    sf::Text minesText("Mines:", Font, 18);
                    minesText.setPosition(50, 150);

                    sf::String widthInput, heightInput, minesInput;
                    sf::Text widthInputText("", Font, 18);
                    widthInputText.setPosition(200, 50);
                    sf::Text heightInputText("", Font, 18);
                    heightInputText.setPosition(200, 100);
                    sf::Text minesInputText("", Font, 18);
                    minesInputText.setPosition(200, 150);

                    bool enteringWidth = true;
                    bool enteringHeight = false;
                    bool enteringMines = false;

                    customWindow.clear(sf::Color(192, 192, 192));
                    customWindow.draw(widthText);
                    customWindow.draw(heightText);
                    customWindow.draw(minesText);
                    customWindow.display();

                    bool done = false;
                    sf::Event customEvent;
                    while (customWindow.isOpen() && !done) {
                        while (customWindow.pollEvent(customEvent)) {
                            if (customEvent.type == sf::Event::Closed) {
                                customWindow.close();
                            }
                            if (customEvent.type == sf::Event::TextEntered) {
                                if (customEvent.text.unicode == '\b') {
                                    // Обработка Backspace
                                    if (enteringWidth && widthInput.getSize() > 0) {
                                        widthInput = widthInput.substring(0, widthInput.getSize() - 1);
                                        widthInputText.setString(widthInput);
                                    }
                                    else if (enteringHeight && heightInput.getSize() > 0) {
                                        heightInput = heightInput.substring(0, heightInput.getSize() - 1);
                                        heightInputText.setString(heightInput);
                                    }
                                    else if (enteringMines && minesInput.getSize() > 0) {
                                        minesInput = minesInput.substring(0, minesInput.getSize() - 1);
                                        minesInputText.setString(minesInput);
                                    }
                                }
                                else if (customEvent.text.unicode == '\r' || customEvent.text.unicode == '\n') {
                                    if (enteringWidth) {
                                        enteringWidth = false;
                                        enteringHeight = true;
                                    }
                                    else if (enteringHeight) {
                                        enteringHeight = false;
                                        enteringMines = true;
                                    }
                                    else if (enteringMines) {
                                        if (widthInput.isEmpty() || heightInput.isEmpty() || minesInput.isEmpty())
                                            continue;

                                        // Преобразуем ввод в числа
                                        int inputWidth = std::stoi(widthInput.toAnsiString());
                                        int inputHeight = std::stoi(heightInput.toAnsiString());
                                        int inputMines = std::stoi(minesInput.toAnsiString());

                                        // Проверяем на максимальные размеры
                                        const int maxWidth = 30;
                                        const int maxHeight = 16;
                                        const int maxMines = inputWidth * inputHeight - 8; // -1 было изначально; Максимум мин - все клетки кроме 8-ми
                                        // Костыльненько : - /

                                        if ((inputWidth > maxWidth) || (inputHeight > maxHeight)) {
                                            // Выводим сообщение об ошибке
                                            //sf::Text errorText("Field size too large!", Font, 18);
                                            //errorText.setFillColor(sf::Color::Red);
                                            //errorText.setPosition(50, 200); 
                                            //customWindow.clear(sf::Color(192, 192, 192));
                                            //customWindow.draw(widthText);
                                            //customWindow.draw(heightText);
                                            //customWindow.draw(minesText);
                                            //customWindow.draw(widthInputText);
                                            //customWindow.draw(heightInputText);
                                            //customWindow.draw(minesInputText);
                                            //customWindow.draw(errorText);
                                            //customWindow.display();
                                            continue;
                                        }

                                        if (inputMines <= 0 || inputMines >= maxMines) {
                                            // Выводим сообщение об ошибке
                                            //sf::Text errorText("Invalid number of mines!", Font, 18);
                                            //errorText.setFillColor(sf::Color::Red);
                                            //errorText.setPosition(50, 200);
                                            //customWindow.clear(sf::Color(192, 192, 192));
                                            //customWindow.draw(widthText);
                                            //customWindow.draw(heightText);
                                            //customWindow.draw(minesText);
                                            //customWindow.draw(widthInputText);
                                            //customWindow.draw(heightInputText);
                                            //customWindow.draw(minesInputText);
                                            //customWindow.draw(errorText);
                                            //customWindow.display();
                                            continue;
                                        }

                                        mapSizeX = inputWidth;
                                        mapSizeY = inputHeight;
                                        MinesCount = inputMines;
                                        currentDifficulty = Difficulty::Custom;
                                        customWindow.close();
                                        difficultyWindow.close();
                                        RestartGame();
                                        done = true;
                                    }
                                }
                                else if (customEvent.text.unicode >= '0' && customEvent.text.unicode <= '9') {
                                    if (enteringWidth) {
                                        widthInput += customEvent.text.unicode;
                                        widthInputText.setString(widthInput);
                                    }
                                    else if (enteringHeight) {
                                        heightInput += customEvent.text.unicode;
                                        heightInputText.setString(heightInput);
                                    }
                                    else if (enteringMines) {
                                        minesInput += customEvent.text.unicode;
                                        minesInputText.setString(minesInput);
                                    }
                                }
                            }
                        }
                        customWindow.clear(sf::Color(192, 192, 192));
                        customWindow.draw(widthText);
                        customWindow.draw(heightText);
                        customWindow.draw(minesText);
                        customWindow.draw(widthInputText);
                        customWindow.draw(heightInputText);
                        customWindow.draw(minesInputText);
                        customWindow.display();
                    }
                }
                else if (randomButton.getGlobalBounds().contains(ev.mouseButton.x, ev.mouseButton.y)) {
                    currentDifficulty = Difficulty::Random;
                    mapSizeX = random(5, 30);
                    mapSizeY = random(5, 24);
                    MinesCount = random(1, mapSizeX * mapSizeY / 2);
                    difficultyWindow.close();
                    RestartGame();
                }
                // Обработка опций
                else if (mistakeOption.getGlobalBounds().contains(ev.mouseButton.x, ev.mouseButton.y)) {
                    allowMistakes = !allowMistakes;
                    mistakeOption.setFillColor(allowMistakes ? sf::Color::Green : sf::Color::White);
                    // Если включено, запрашиваем количество ошибок
                    if (allowMistakes) {
                        sf::RenderWindow mistakesWindow(sf::VideoMode(300, 150), "Set Mistakes Allowed", sf::Style::Titlebar | sf::Style::Close);
                        sf::Text mistakesPrompt("Enter mistakes allowed:", Font, 18);
                        mistakesPrompt.setPosition(20, 20);
                        sf::String mistakesInput;
                        sf::Text mistakesInputText("", Font, 18);
                        mistakesInputText.setPosition(20, 60);
                        mistakesWindow.clear(sf::Color(192, 192, 192));
                        mistakesWindow.draw(mistakesPrompt);
                        mistakesWindow.display();
                        bool done = false;
                        sf::Event mistakesEvent;
                        while (mistakesWindow.isOpen() && !done) {
                            while (mistakesWindow.pollEvent(mistakesEvent)) {
                                if (mistakesEvent.type == sf::Event::Closed) {
                                    mistakesWindow.close();
                                    allowMistakes = false; // Отменяем опцию
                                    mistakeOption.setFillColor(sf::Color::White);
                                }
                                if (mistakesEvent.type == sf::Event::TextEntered) {
                                    if (mistakesEvent.text.unicode == '\b') {
                                        if (mistakesInput.getSize() > 0) {
                                            mistakesInput = mistakesInput.substring(0, mistakesInput.getSize() - 1);
                                            mistakesInputText.setString(mistakesInput);
                                        }
                                    }
                                    else if (mistakesEvent.text.unicode == '\r' || mistakesEvent.text.unicode == '\n') {
                                        if (mistakesInput.isEmpty())
                                            continue;
                                        mistakesAllowed = std::stoi(mistakesInput.toAnsiString());
                                        mistakesMade = 0;
                                        mistakesWindow.close();
                                        done = true;
                                    }
                                    else if (mistakesEvent.text.unicode >= '0' && mistakesEvent.text.unicode <= '9') {
                                        mistakesInput += mistakesEvent.text.unicode;
                                        mistakesInputText.setString(mistakesInput);
                                    }
                                }
                            }
                            mistakesWindow.clear(sf::Color(192, 192, 192));
                            mistakesWindow.draw(mistakesPrompt);
                            mistakesWindow.draw(mistakesInputText);
                            mistakesWindow.display();
                        }
                    }
                }
                else if (unknownMinesOption.getGlobalBounds().contains(ev.mouseButton.x, ev.mouseButton.y)) {
                    unknownMineCount = !unknownMineCount;
                    unknownMinesOption.setFillColor(unknownMineCount ? sf::Color::Green : sf::Color::White);
                }
            }
        }
        // Обновление отображения опций
        mistakeOption.setFillColor(allowMistakes ? sf::Color::Green : sf::Color::White);
        unknownMinesOption.setFillColor(unknownMineCount ? sf::Color::Green : sf::Color::White);

        difficultyWindow.clear(sf::Color(192, 192, 192));
        difficultyWindow.draw(easyButton);
        difficultyWindow.draw(easyText);
        difficultyWindow.draw(mediumButton);
        difficultyWindow.draw(mediumText);
        difficultyWindow.draw(hardButton);
        difficultyWindow.draw(hardText);
        difficultyWindow.draw(customButton);
        difficultyWindow.draw(customText);
        difficultyWindow.draw(randomButton);
        difficultyWindow.draw(randomText);
        difficultyWindow.draw(mistakeOption);
        difficultyWindow.draw(mistakeText);
        difficultyWindow.draw(unknownMinesOption);
        difficultyWindow.draw(unknownMinesText);
        difficultyWindow.display();
    }
    modalWindowOpen = false;
}

void Game::openSettingsWindow() {
    modalWindowOpen = true;
    sf::RenderWindow settingsWindow(sf::VideoMode(400, 300), "Mouse Button Settings", sf::Style::Titlebar | sf::Style::Close);
    settingsWindow.clear(sf::Color(192, 192, 192));

    // Ограничение FPS для окна настроек
    settingsWindow.setFramerateLimit(60);

    sf::Text instructionText("Click to set the button for opening cells", Font, 18);
    instructionText.setFillColor(sf::Color::Black);
    instructionText.setPosition(20, 50);

    settingsWindow.draw(instructionText);
    settingsWindow.display();

    bool settingOpenButton = true;
    sf::Event ev;
    while (settingsWindow.isOpen()) {
        while (settingsWindow.pollEvent(ev)) {
            if (ev.type == sf::Event::Closed) {
                settingsWindow.close();
            }
            if (ev.type == sf::Event::MouseButtonPressed) {
                if (settingOpenButton) {
                    openCellButton = ev.mouseButton.button;
                    instructionText.setString("Click to set the button for flagging cells");
                    settingsWindow.clear(sf::Color(192, 192, 192));
                    settingsWindow.draw(instructionText);
                    settingsWindow.display();
                    settingOpenButton = false;
                }
                else {
                    if ((openCellButton == flagCellButton))
                    {
                        if (openCellButton == sf::Mouse::Button::Left)
                        {
                            flagCellButton = sf::Mouse::Button::Right;
                        }
                        else
                        {
                            flagCellButton = sf::Mouse::Button::Left;
                        }
                    }
                    //flagCellButton = openCellButton == flagCellButton ? sf::Mouse::Button::Left : sf::Mouse::Button::Right;
                    settingsWindow.close();
                }

            }
        }
    }
    modalWindowOpen = false;
}

void Game::saveGame() {
    // Оставляем как есть
    std::ofstream saveFile(saveFileName, std::ios::binary);
    if (saveFile.is_open()) {
        // Сохраняем текущую сложность
        int difficulty = static_cast<int>(currentDifficulty);
        saveFile.write(reinterpret_cast<char*>(&difficulty), sizeof(difficulty));

        saveFile.write(reinterpret_cast<char*>(&mapSizeX), sizeof(mapSizeX));
        saveFile.write(reinterpret_cast<char*>(&mapSizeY), sizeof(mapSizeY));
        saveFile.write(reinterpret_cast<char*>(&MinesCount), sizeof(MinesCount));
        saveFile.write(reinterpret_cast<char*>(&firstClick), sizeof(firstClick));
        saveFile.write(reinterpret_cast<char*>(&gameLost), sizeof(gameLost));
        saveFile.write(reinterpret_cast<char*>(&gameWon), sizeof(gameWon));

        // Сохранение карты
        for (const auto& row : map) {
            saveFile.write(reinterpret_cast<const char*>(row.data()), row.size() * sizeof(int));
        }
        // Сохранение отображения карты
        for (const auto& row : mapView) {
            saveFile.write(reinterpret_cast<const char*>(row.data()), row.size() * sizeof(CellState));
        }

        // Сохранение времени
        sf::Int32 elapsed = elapsedTime.asMilliseconds();
        saveFile.write(reinterpret_cast<char*>(&elapsed), sizeof(elapsed));

        // Сохранение новых опций
        saveFile.write(reinterpret_cast<char*>(&allowMistakes), sizeof(allowMistakes));
        saveFile.write(reinterpret_cast<char*>(&mistakesAllowed), sizeof(mistakesAllowed));
        saveFile.write(reinterpret_cast<char*>(&mistakesMade), sizeof(mistakesMade));
        saveFile.write(reinterpret_cast<char*>(&unknownMineCount), sizeof(unknownMineCount));
        

        saveFile.close();

    }
}

void Game::loadGame() {
    // Оставляем как есть
    std::ifstream loadFile(saveFileName, std::ios::binary);
    if (loadFile.is_open()) {
        // Загружаем сложность
        int difficulty;
        loadFile.read(reinterpret_cast<char*>(&difficulty), sizeof(difficulty));
        currentDifficulty = static_cast<Difficulty>(difficulty);

        loadFile.read(reinterpret_cast<char*>(&mapSizeX), sizeof(mapSizeX));
        loadFile.read(reinterpret_cast<char*>(&mapSizeY), sizeof(mapSizeY));
        loadFile.read(reinterpret_cast<char*>(&MinesCount), sizeof(MinesCount));
        loadFile.read(reinterpret_cast<char*>(&firstClick), sizeof(firstClick));
        loadFile.read(reinterpret_cast<char*>(&gameLost), sizeof(gameLost));
        loadFile.read(reinterpret_cast<char*>(&gameWon), sizeof(gameWon));

        map.assign(mapSizeY, std::vector<int>(mapSizeX));
        mapView.assign(mapSizeY, std::vector<CellState>(mapSizeX));

        for (auto& row : map) {
            loadFile.read(reinterpret_cast<char*>(row.data()), row.size() * sizeof(int));
        }

        for (auto& row : mapView) {
            loadFile.read(reinterpret_cast<char*>(row.data()), row.size() * sizeof(CellState));
        }

        sf::Int32 elapsed;
        loadFile.read(reinterpret_cast<char*>(&elapsed), sizeof(elapsed));
        elapsedTime = sf::milliseconds(elapsed);
        timeOffset = elapsedTime; // Устанавливаем смещение времени
        gameClock.restart();      // Перезапускаем часы

        // Загружаем новые опции
        loadFile.read(reinterpret_cast<char*>(&allowMistakes), sizeof(allowMistakes));
        loadFile.read(reinterpret_cast<char*>(&mistakesAllowed), sizeof(mistakesAllowed));
        loadFile.read(reinterpret_cast<char*>(&mistakesMade), sizeof(mistakesMade));
        loadFile.read(reinterpret_cast<char*>(&unknownMineCount), sizeof(unknownMineCount));

        loadFile.close();


        isInitializing = false;

        // Вычисляем смещение поля для центрирования
        fieldOffsetX = std::max(0, (windowSizeX - mapSizeX * Cell::Size) / 2);
        fieldOffsetY = std::max(topAreaHeight, (windowSizeY - topAreaHeight - mapSizeY * Cell::Size) / 2 + topAreaHeight);

        if (gameLost) {
            RestartButton.setFillColor(sf::Color::Red);
        }
        else if (gameWon) {
            RestartButton.setFillColor(sf::Color::Green);
        }
        else {
            RestartButton.setFillColor(sf::Color::White);
        }
    }
}

void Game::saveHighScore(const sf::String& playerName) {
    // Оставляем как есть
    // Проверяем, существует ли игрок в списке
    auto it = std::find_if(highScores.begin(), highScores.end(),
        [&playerName](const HighScoreEntry& entry) {
            return entry.playerName == playerName;
        });

    if (it != highScores.end()) {
        // Если время лучше, обновляем запись
        if (elapsedTime < it->time) {
            it->time = elapsedTime;
            it->difficulty = currentDifficulty;
        }
    }
    else {
        // Добавляем новую запись
        HighScoreEntry newEntry = { playerName, elapsedTime, currentDifficulty };
        highScores.push_back(newEntry);
    }

    // Сортируем по времени
    std::sort(highScores.begin(), highScores.end(), [](const HighScoreEntry& a, const HighScoreEntry& b) {
        return a.time < b.time;
        });

    // Оставляем только топ 10
    if (highScores.size() > 10) {
        highScores.resize(10);
    }

    // Сохраняем в файл
    std::ofstream hsFile(highScoresFileName, std::ios::binary);
    if (hsFile.is_open()) {
        int count = highScores.size();
        hsFile.write(reinterpret_cast<char*>(&count), sizeof(count));
        for (const auto& entry : highScores) {
            // Кодируем имя в UTF-8
            std::basic_string<sf::Uint8> utf8Name = entry.playerName.toUtf8();
            size_t nameSize = utf8Name.size();
            hsFile.write(reinterpret_cast<const char*>(&nameSize), sizeof(nameSize));
            hsFile.write(reinterpret_cast<const char*>(utf8Name.data()), nameSize);
            sf::Int32 time = entry.time.asMilliseconds();
            hsFile.write(reinterpret_cast<char*>(&time), sizeof(time));
            int difficulty = static_cast<int>(entry.difficulty);
            hsFile.write(reinterpret_cast<char*>(&difficulty), sizeof(difficulty));
        }
        hsFile.close();
    }
}

void Game::loadHighScores() {
    // Оставляем как есть
    std::ifstream hsFile(highScoresFileName, std::ios::binary);
    if (hsFile.is_open()) {
        int count;
        hsFile.read(reinterpret_cast<char*>(&count), sizeof(count));
        highScores.clear();
        for (int i = 0; i < count; ++i) {
            size_t nameSize;
            hsFile.read(reinterpret_cast<char*>(&nameSize), sizeof(nameSize));
            std::basic_string<sf::Uint8> utf8Name(nameSize, '\0');
            hsFile.read(reinterpret_cast<char*>(&utf8Name[0]), nameSize);
            sf::String name = sf::String::fromUtf8(utf8Name.begin(), utf8Name.end());
            sf::Int32 time;
            hsFile.read(reinterpret_cast<char*>(&time), sizeof(time));
            int difficulty;
            hsFile.read(reinterpret_cast<char*>(&difficulty), sizeof(difficulty));
            HighScoreEntry entry = { name, sf::milliseconds(time), static_cast<Difficulty>(difficulty) };
            highScores.push_back(entry);
        }
        hsFile.close();
    }
}

void Game::displayHighScores() {
    // Оставляем как есть
    modalWindowOpen = true;
    sf::RenderWindow hsWindow(sf::VideoMode(400, 500), "High Scores", sf::Style::Titlebar | sf::Style::Close);
    hsWindow.clear(sf::Color(192, 192, 192));

    // Ограничение FPS для окна рейтингов
    hsWindow.setFramerateLimit(60);

    sf::Text title("High Scores", Font, 24);
    title.setFillColor(sf::Color::Black);
    title.setPosition(120, 20);
    hsWindow.draw(title);

    int yOffset = 60;
    for (const auto& entry : highScores) {
        std::string difficultyStr;
        switch (entry.difficulty) {
        case Difficulty::Easy:
            difficultyStr = "Easy";
            break;
        case Difficulty::Medium:
            difficultyStr = "Medium";
            break;
        case Difficulty::Hard:
            difficultyStr = "Hard";
            break;
        case Difficulty::Custom:
            difficultyStr = "Custom";
            break;
        case Difficulty::Random:
            difficultyStr = "Random";
            break;
        }

        sf::Text entryText(entry.playerName + " - " + std::to_string(entry.time.asSeconds()) + "s - " + difficultyStr, Font, 18);
        entryText.setFillColor(sf::Color::Black);
        entryText.setPosition(20, yOffset);
        hsWindow.draw(entryText);
        yOffset += 30;
    }

    hsWindow.display();

    sf::Event ev;
    while (hsWindow.isOpen()) {
        while (hsWindow.pollEvent(ev)) {
            if (ev.type == sf::Event::Closed) {
                hsWindow.close();
            }
        }
    }
    modalWindowOpen = false;
}

int Game::random(int min, int max)
{
    static bool first = true;
    if (first)
    {
        srand(static_cast<unsigned>(time(nullptr)));
        first = false;
    }
    return min + rand() % ((max + 1) - min);
}

void Game::setNumbers()
{
    // Оставляем как есть
    for (int y = 0; y < mapSizeY; y++)
    {
        for (int x = 0; x < mapSizeX; x++)
        {
            if (map[y][x] == static_cast<int>(CellState::Mine))
                continue;

            int mineCount = 0;
            for (int dy = -1; dy <= 1; dy++)
            {
                for (int dx = -1; dx <= 1; dx++)
                {
                    int newX = x + dx;
                    int newY = y + dy;

                    if (newX >= 0 && newX < mapSizeX && newY >= 0 && newY < mapSizeY &&
                        map[newY][newX] == static_cast<int>(CellState::Mine))
                    {
                        mineCount++;
                    }
                }
            }

            map[y][x] = mineCount; // Записываем количество мин вокруг клетки
        }
    }
}

void Game::GenerateMap(int startX, int startY) {
    // Оставляем как есть
    map.assign(mapSizeY, std::vector<int>(mapSizeX, 0));

    int minesPlaced = 0;
    while (minesPlaced < MinesCount) {
        int x = random(0, mapSizeX - 1);
        int y = random(0, mapSizeY - 1);

        if (map[y][x] == static_cast<int>(CellState::Mine) ||
            (std::abs(x - startX) <= 1 && std::abs(y - startY) <= 1)) {
            continue;
        }

        map[y][x] = static_cast<int>(CellState::Mine);
        minesPlaced++;
    }

    setNumbers();
}

void Game::RestartGame()
{
    gameLost = false;
    gameWon = false;
    firstClick = true;
    isInitializing = true;
    flagsPlaced = 0;
    elapsedTime = sf::Time::Zero;
    timeOffset = sf::Time::Zero; // Сбрасываем смещение времени
    gameClock.restart();
    mistakesMade = 0; // Сбрасываем количество сделанных ошибок

    RestartButton.setFillColor(sf::Color::White);

    map.clear();
    mapView.clear();

    // Инициализируем только отображение карты, но не генерируем мин
    map.resize(mapSizeY, std::vector<int>(mapSizeX, 0));
    mapView.resize(mapSizeY, std::vector<CellState>(mapSizeX, CellState::Unopened));

    // Обновление кнопок
    RestartButton.setPosition(windowSizeX / 2 - RestartButton.getSize().x / 2, 10);
    SaveButton.setPosition(windowSizeX - 210, 10);
    LoadButton.setPosition(windowSizeX - 210, 70);
    HighScoresButton.setPosition(windowSizeX / 2 - 100, 70);

    // Вычисляем смещение поля для центрирования
    fieldOffsetX = std::max(0, (windowSizeX - mapSizeX * Cell::Size) / 2);
    fieldOffsetY = std::max(topAreaHeight, (windowSizeY - topAreaHeight - mapSizeY * Cell::Size) / 2 + topAreaHeight);
}

void Game::safeOpen(int x, int y)
{
    // Оставляем как есть
    int flaggedMines = 0;

    // Подсчитываем количество флагов вокруг
    for (auto& direction : directions)
    {
        int newX = x + direction[0];
        int newY = y + direction[1];
        if (newX >= 0 && newX < mapSizeX && newY >= 0 && newY < mapSizeY)
        {
            if (mapView[newY][newX] == CellState::Flagged)
            {
                flaggedMines++;
            }
        }
    }

    // Если количество флагов совпадает с числом в ячейке, открываем соседние клетки
    if (flaggedMines == map[y][x])
    {
        for (auto& direction : directions)
        {
            int newX = x + direction[0];
            int newY = y + direction[1];
            if (newX >= 0 && newX < mapSizeX && newY >= 0 && newY < mapSizeY)
            {
                if (mapView[newY][newX] == CellState::Unopened)
                {
                    if (map[newY][newX] == static_cast<int>(CellState::Mine) && mapView[y][x] != CellState::Mine)
                    {
                        gameOver();
                        return;
                    }
                    mapView[newY][newX] = static_cast<CellState>(map[newY][newX]);
                    if (map[newY][newX] == 0)
                    {
                        openAllZeroAround(newX, newY);
                    }
                }
            }
        }
    }
}

void Game::revealAllMines()
{
    // Оставляем как есть
    for (int y = 0; y < mapSizeY; y++)
    {
        for (int x = 0; x < mapSizeX; x++)
        {
            if (mapView[y][x] == CellState::Flagged)
            {
                if (map[y][x] == static_cast<int>(CellState::Mine))
                {
                    continue; // Правильный флаг
                }
                else
                {
                    mapView[y][x] = CellState::WrongFlag; // Ошибочный флаг
                }
            }
            else if (map[y][x] == static_cast<int>(CellState::Mine))
            {
                mapView[y][x] = CellState::Mine; // Показываем мину
            }
        }
    }
}

void Game::gameOver()
{
    if (allowMistakes && mistakesMade < mistakesAllowed) {
        mistakesMade++;
        // Помечаем мину как открытую, но продолжаем игру
        mapView[hoveredY][hoveredX] = CellState::Mine;
        if (mistakesMade == mistakesAllowed + 1) {
            // Игра заканчивается после превышения ошибок
            revealAllMines();
            gameLost = true;
            RestartButton.setFillColor(sf::Color::Red);
        }
    }
    else {
        revealAllMines();
        gameLost = true;
        RestartButton.setFillColor(sf::Color::Red);
    }
}

void Game::checkWin()
{
    // Оставляем как есть
    bool won = true;
    for (int y = 0; y < mapSizeY; y++)
    {
        for (int x = 0; x < mapSizeX; x++)
        {
            if (map[y][x] != static_cast<int>(CellState::Mine) && mapView[y][x] == CellState::Unopened)
            {
                won = false;
                break;
            }
        }
        if (!won) break;
    }

    if (won)
    {
        gameWon = true;
        RestartButton.setFillColor(sf::Color::Green);

        // Устанавливаем флаг модального окна
        modalWindowOpen = true;

        // Запрос имени игрока и сохранение результата
        sf::String playerName;
        sf::Text inputText("Enter your name: ", Font, 18);
        inputText.setFillColor(sf::Color::Black);
        inputText.setPosition(50, windowSizeY / 2);

        GameWindow.clear(sf::Color(192, 192, 192));
        GameWindow.draw(inputText);
        GameWindow.display();

        sf::Event event;
        while (GameWindow.waitEvent(event)) {
            if (event.type == sf::Event::TextEntered) {
                if (event.text.unicode == '\b') { // Backspace
                    if (!playerName.isEmpty())
                        playerName.erase(playerName.getSize() - 1, 1);
                }
                else if (event.text.unicode == '\r' || event.text.unicode == '\n') { // Enter
                    break;
                }
                else {
                    playerName += event.text.unicode;
                }
                inputText.setString("Enter your name: " + playerName);
                GameWindow.clear(sf::Color(192, 192, 192));
                GameWindow.draw(inputText);
                GameWindow.display();
            }
        }

        // Сбрасываем флаг после ввода имени
        modalWindowOpen = false;

        saveHighScore(playerName);
    }
}

void Game::openAllZeroAround(int x, int y)
{

    for (auto& direction : directions)
    {
        int newX = x + direction[0];
        int newY = y + direction[1];
        if (newX >= 0 && newX < mapSizeX && newY >= 0 && newY < mapSizeY && mapView[newY][newX] == CellState::Unopened)
        {
            mapView[newY][newX] = static_cast<CellState>(map[newY][newX]);
            if (map[newY][newX] == 0)
            {
                openAllZeroAround(newX, newY);
            }
        }
    }
}

void Game::HandleEvents()
{
    sf::Event e;
    while (GameWindow.pollEvent(e))
    {
        if (e.type == sf::Event::Closed)
        {
            GameWindow.close();
        }
        if (modalWindowOpen)
        {
            // Пропускаем обработку событий, когда модальное окно открыто
            continue;
        }
        if (e.type == sf::Event::MouseButtonPressed)
        {
            int x = (e.mouseButton.x - fieldOffsetX) / Cell::Size;
            int y = (e.mouseButton.y - fieldOffsetY) / Cell::Size;

            if (DifficultyButton.getGlobalBounds().contains(e.mouseButton.x, e.mouseButton.y))
            {
                RunDifficultyWindow();
            }
            else if (SettingsButton.getGlobalBounds().contains(e.mouseButton.x, e.mouseButton.y))
            {
                openSettingsWindow();
            }
            else if (SaveButton.getGlobalBounds().contains(e.mouseButton.x, e.mouseButton.y))
            {
                if (!firstClick) // Кнопка save доступна только после первого клика
                {
                    saveGame();
                }
            }
            else if (LoadButton.getGlobalBounds().contains(e.mouseButton.x, e.mouseButton.y))
            {
                loadGame();
            }
            else if (HighScoresButton.getGlobalBounds().contains(e.mouseButton.x, e.mouseButton.y))
            {
                displayHighScores();
            }
            else if (e.mouseButton.y < topAreaHeight)
            {
                if (RestartButton.getGlobalBounds().contains(e.mouseButton.x, e.mouseButton.y))
                {
                    if (!firstClick)
                    {
                        RestartGame();
                    }
                }
            }
            else if (!gameLost && !gameWon)
            {
                if (x < mapSizeX && x >= 0 && y < mapSizeY && y >= 0)
                {
                    if (!firstClick)
                    {
                        if (e.mouseButton.button == flagCellButton)
                        {
                            if (mapView[y][x] == CellState::Unopened)
                            {
                                mapView[y][x] = CellState::Flagged;
                                flagsPlaced++;
                            }
                            else if (mapView[y][x] == CellState::Flagged)
                            {
                                mapView[y][x] = CellState::Question;
                                flagsPlaced--;
                            }
                            else if (mapView[y][x] == CellState::Question)
                            {
                                mapView[y][x] = CellState::Unopened;
                            }
                        }
                    }

                    if (e.mouseButton.button == openCellButton)
                    {
                        if (firstClick)
                        {
                            GenerateMap(x, y);
                            firstClick = false;
                            isInitializing = false;
                            gameClock.restart();
                        }

                        if (mapView[y][x] == CellState::Flagged)
                        {
                            continue; // Пропускаем обработку этой клетки
                        }

                        if (map[y][x] == static_cast<int>(CellState::Mine) && mapView[y][x] != CellState::Mine) // Если кликнули на мину
                        {
                            hoveredX = x;
                            hoveredY = y;
                            gameOver(); // Заканчиваем игру (проигрыш)
                        }
                        else
                        {
                            mapView[y][x] = static_cast<CellState>(map[y][x]); // Открываем клетку, показывая число мин вокруг
                            if (map[y][x] == static_cast<int>(CellState::Zero)) // Если вокруг клетки нет мин
                            {
                                openAllZeroAround(x, y); // Открываем все соседние клетки без мин
                            }
                            safeOpen(x, y); // Безопасное открытие клеток вокруг
                            checkWin(); // Проверяем, выиграна ли игра
                        }
                    }
                }
            }
        }

        if (e.type == sf::Event::MouseMoved)
        {
            hoveredX = (e.mouseMove.x - fieldOffsetX) / Cell::Size;
            hoveredY = (e.mouseMove.y - fieldOffsetY) / Cell::Size;
        }
    }
}

void Game::RenderEvents()
{
    GameWindow.clear(sf::Color(192, 192, 192));

    sf::RectangleShape topArea(sf::Vector2f(windowSizeX, topAreaHeight));
    topArea.setFillColor(sf::Color(128, 128, 128));
    GameWindow.draw(topArea);

    GameWindow.draw(RestartButton);
    GameWindow.draw(RestartButtonText);

    GameWindow.draw(DifficultyButton);
    GameWindow.draw(DifficultyButtonText);

    GameWindow.draw(SettingsButton);
    GameWindow.draw(SettingsButtonText);

    GameWindow.draw(SaveButton);
    GameWindow.draw(SaveButtonText);

    GameWindow.draw(LoadButton);
    GameWindow.draw(LoadButtonText);

    GameWindow.draw(HighScoresButton);
    GameWindow.draw(HighScoresButtonText);

    // Отображение текущего времени игры
    if (!firstClick && !gameWon && !gameLost)
        elapsedTime = timeOffset + gameClock.getElapsedTime(); // Учитываем смещение времени

    sf::Text timeText("Time: " + std::to_string(static_cast<int>(elapsedTime.asSeconds())) + "s", Font, 18);
    timeText.setFillColor(sf::Color::Black);
    timeText.setPosition(windowSizeX - 150, 100);
    GameWindow.draw(timeText);

    // Отображение оставшихся мин
    int minesLeft = MinesCount - flagsPlaced;
    sf::Text minesText;
    if (unknownMineCount) {
        minesText.setString("Mines left: ???");
    }
    else {
        minesText.setString("Mines left: " + std::to_string(minesLeft));
    }
    minesText.setFont(Font);
    minesText.setCharacterSize(18);
    minesText.setFillColor(sf::Color::Black);
    minesText.setPosition(10, 100);
    GameWindow.draw(minesText);

    // Отображение количества ошибок, если включено "право на ошибку"
    if (allowMistakes) {
        sf::Text mistakesText("Mistakes: " + std::to_string(mistakesMade) + "/" + std::to_string(mistakesAllowed), Font, 18);
        mistakesText.setFillColor(sf::Color::Black);
        mistakesText.setPosition(250, 100);
        GameWindow.draw(mistakesText);
    }

    bool shiftPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift);

    for (int y = 0; y < mapSizeY; y++)
    {
        for (int x = 0; x < mapSizeX; x++)
        {
            sf::RectangleShape rectangle(sf::Vector2f(Cell::Size, Cell::Size));
            rectangle.setPosition(x * Cell::Size + fieldOffsetX, y * Cell::Size + fieldOffsetY);

            if (hoveredX == x && hoveredY == y && shiftPressed && !isInitializing && map[y][x] == static_cast<int>(CellState::Mine))
            {
                rectangle.setFillColor(sf::Color::Red); // Подсветка мины при зажатом Shift
            }
            else if (mapView[y][x] == CellState::Unopened || mapView[y][x] == CellState::Flagged)
            {
                rectangle.setFillColor(sf::Color(128, 128, 128)); // Неоткрытая клетка
            }
            else if (mapView[y][x] == CellState::WrongFlag)
            {
                rectangle.setFillColor(sf::Color::Red); // Ошибочный флаг
            }
            else
            {
                rectangle.setFillColor(sf::Color(192, 192, 192)); // Открытая клетка
            }

            rectangle.setOutlineColor(sf::Color::Black);
            rectangle.setOutlineThickness(1);
            GameWindow.draw(rectangle);

            if (mapView[y][x] == CellState::Mine && !isInitializing)
            {
                sf::Sprite sprite;
                sprite.setTexture(MineTexture);
                sprite.setScale(static_cast<float>(Cell::Size) / MineTexture.getSize().x, static_cast<float>(Cell::Size) / MineTexture.getSize().y);
                sprite.setPosition(x * Cell::Size + fieldOffsetX, y * Cell::Size + fieldOffsetY);
                GameWindow.draw(sprite);
            }
            else if (mapView[y][x] == CellState::Flagged)
            {
                sf::Sprite sprite;
                sprite.setTexture(FlagTexture);
                sprite.setScale(static_cast<float>(Cell::Size) / FlagTexture.getSize().x, static_cast<float>(Cell::Size) / FlagTexture.getSize().y);
                sprite.setPosition(x * Cell::Size + fieldOffsetX, y * Cell::Size + fieldOffsetY);
                GameWindow.draw(sprite);
            }
            else if (mapView[y][x] == CellState::Question)
            {
                sf::Sprite sprite;
                sprite.setTexture(QuestionTexture);
                sprite.setScale(static_cast<float>(Cell::Size) / QuestionTexture.getSize().x, static_cast<float>(Cell::Size) / QuestionTexture.getSize().y);
                sprite.setPosition(x * Cell::Size + fieldOffsetX, y * Cell::Size + fieldOffsetY);
                GameWindow.draw(sprite);

            }
            else if (mapView[y][x] == CellState::WrongFlag)
            {
                sf::Text text;
                text.setFillColor(sf::Color::Red);
                text.setPosition(x * Cell::Size + Cell::Size / 4, y * Cell::Size + topAreaHeight - Cell::Size / 6);
                GameWindow.draw(text);
            }
            else if (mapView[y][x] != CellState::Unopened && mapView[y][x] != CellState::Flagged)
            {
                if (map[y][x] > 0)
                {
                    sf::Color colors[] = {
                        sf::Color(100, 149, 237), // Темно-голубой
                        sf::Color(60, 179, 113),  // Средне-морской зеленый
                        sf::Color(205, 92, 92),   // Индийский красный
                        sf::Color(186, 85, 211),  // Средне-орхидейный
                        sf::Color(238, 232, 170), // Светло-золотистый
                        sf::Color(199, 21, 133),  // Средний пурпурно-красный
                        sf::Color(169, 169, 169), // Темно-серый
                        sf::Color(72, 209, 204)   // Средне-бирюзовый
                    };

                    sf::Text text;
                    text.setFont(Font);
                    text.setString(std::to_string(map[y][x]));
                    text.setCharacterSize(Cell::Size);
                    text.setFillColor(colors[map[y][x] - 1]);
                    text.setPosition(x * Cell::Size + fieldOffsetX + Cell::Size / 4, y * Cell::Size + fieldOffsetY - Cell::Size / 6);
                    GameWindow.draw(text);
                }
            }
        }
    }

    GameWindow.display();
}

void Game::run()
{
    while (GameWindow.isOpen())
    {
        HandleEvents();
        RenderEvents();
    }
}
