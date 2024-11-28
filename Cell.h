#pragma once
#ifndef CELL_H
#define CELL_H

class Game; // Предварительное объявление класса Game

class Cell
{
public:
    Cell();
    static int Size;

private:
    friend class Game;
};

#endif // CELL_H