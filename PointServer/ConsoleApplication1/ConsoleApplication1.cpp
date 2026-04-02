
#include <iostream>
#include <vector>
#include "windows.h"
using namespace std;

// 0 - пустота; " "
// 1 - вертикаль; 179
// 2 - горизонталь; 196
// 3 - перекрестие; 197
// 4 - точка; 4
// 5 - точка другого игрока; 4


void MovingCursor(int x, int y)
{
    COORD coord = { x , y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

vector <vector<int>> CreateField(vector <vector<int>> Field, int x , int y)
{
    int a, b;
    for (size_t i = 0; i < y * 2 + 1; ++i)
    {
        if (i % 2 != 0)
        {
            a = 3;
            b = 2;
        }
        else
        {
            a = 1;
            b = 0;
        }
        std::vector<int> temp;
        for (size_t j = 0; j < x * 2 + 1; ++j)
        {
            if (j % 2 != 0) temp.push_back(a);
            else temp.push_back(b);
        }
        Field.push_back(temp);
    }
    return Field;
}

void SetColor(int color)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

void PrintField(vector <vector<int>> Field, bool Motion)
{
    system("cls");
    if (Motion) cout << "Motion: Server" << endl << endl;
    else cout << "Motion: Client" << endl << endl;
    cout << " " << char(179);
    char score = 'A';
    for (int i = 0; i < Field.size(); ++i)
        cout << (i % 2 != 0 ? score++ : ' ');
    cout << endl << char(196) << char(197);
    for (int i = 0; i < Field.size(); ++i)
        cout << char(196);
    int sim;
    score = 'A';
    for (int i = 0; i < Field.size(); ++i)
    {
        cout << endl << (i % 2 != 0 ? score++ : ' ') << char(179);
        for (int j = 0; j < Field[i].size(); ++j)
        {
            switch (Field[i][j])
            {
            case 0:
                cout << char(32);
                break;
            case 1:
                cout << char(179);
                break;
            case 2:
                cout << char(196);
                break;
            case 3:
                cout << char(197);
                break;
            case 4:
                SetColor(9);
                cout << char(4);
                SetColor(7);
                break;
            case 5:
                SetColor(12);
                cout << char(4);
                SetColor(7);
                break;
            default:
                cout << char(63);
                break;
            }
        }
    }
    cout << endl << endl;
}

void PrintMenu(int x, int y)
{
    MovingCursor((x * 2 + 1) + 10, 2);
    cout << "MENU:";
    MovingCursor((x * 2 + 1) + 10, 4);
    cout << "[1] Give up";
    MovingCursor((x * 2 + 1) + 10, 6);
    cout << "[2] Suggest a draw";
    MovingCursor((x * 2 + 1) + 10, 8);
    cout << "[3] End the game";
    MovingCursor(0, 4 + (y * 2 + 1));
}

int main()
{
    int x, y;
    cout << "Enter the field size: ";
    cin >> x;
    y = x;
    bool Motion = 0;
    vector <vector<int>> Field;
    Field = CreateField(Field, x, y);
    char XCoordPoint, YCoordPoint;
    while (1)
    {
        Motion = (Motion? 0 : 1);
        PrintField(Field, Motion);
        PrintMenu(x, y);
        cout << "Enter the coordinates of the point: ";
        cin >> XCoordPoint;
        cin >> YCoordPoint;
        XCoordPoint = toupper(XCoordPoint);
        YCoordPoint = toupper(YCoordPoint);
        while ((XCoordPoint < 65 || YCoordPoint < 65) || (XCoordPoint >= (x + 65) || YCoordPoint >= (y + 65)))
        {
            cout << "Enter valid coordinates: ";
            cin >> XCoordPoint >> YCoordPoint;
            XCoordPoint = toupper(XCoordPoint);
            YCoordPoint = toupper(YCoordPoint);
        }
        Field[(XCoordPoint - 64.5) * 2][(YCoordPoint - 64.5) * 2] = (Motion? 4 : 5);
    }
}

