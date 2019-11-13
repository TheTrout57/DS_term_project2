#include <iostream>
#include <fstream>
#include "Robot.h"

using namespace std;

int main()
{
    int row, col, battery;
    fstream file;
    file.open("floor.data", ios::in);
    file >> row >> col >> battery;

    char** matrix = new char* [row];
    for (int i = 0; i < row; i++)
        matrix[i] = new char [col];

    for (int i = 0; i < row; i++){
        for (int j = 0; j < col; j++)
            file >> matrix[i][j];
        file.get();
    }
    
    Robot s(row, col, battery, matrix);
    s.clean();
    return 0;
}