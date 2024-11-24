#pragma once
#ifndef ENUMS_H
#define ENUMS_H

enum class CellState
{
    Unopened = 10, // Неоткрытая клетка
    Flagged = 9,   // Флаг
    Question = 11, // Вопросительная клетка
    Mine = -1,     // Мина
    WrongFlag = -2,// Ошибочный флаг
    Zero = 0,
    One,
    Two,
    Three,
    Four,
    Five,
    Six,
    Seven,
    Eight
};

enum class Difficulty {
    Easy,
    Medium,
    Hard,
    Custom,    // Пользовательская сложность
    Random     // Случайная игра
};

#endif // ENUMS_H