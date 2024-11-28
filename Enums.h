#pragma once
#ifndef ENUMS_H
#define ENUMS_H

enum class CellState
{
    Unopened = 10, // ���������� ������
    Flagged = 9,   // ����
    Question = 11, // �������������� ������
    Mine = -1,     // ����
    WrongFlag = -2,// ��������� ����
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
    Custom,    // ���������������� ���������
    Random     // ��������� ����
};

#endif // ENUMS_H