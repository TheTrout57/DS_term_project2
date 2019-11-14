#include <iostream>
#include "Queue.h"
#include <stdlib.h>  
#include <iomanip>
#include <fstream>
#define N 1000000

using namespace std;

struct Position{
    int r, c, distance;
};

class Robot{
    public:
    Robot(int r, int c, int b, char** matrix)
    :row(r), col(c), battery(b), floor(matrix), steps(0){
        for (int i = 0; i < row; i++)
            for (int j = 0; j < col; j++)
                if (floor[i][j] == 'R'){
                    R_row = i;
                    R_col = j;
                }
        
        s = new bool* [row];
        for (int i = 0; i < row; i++)
            s[i] = new bool [col];

        dist = new int* [row];
        for (int i = 0; i < row; i++)
            dist[i] = new int [col];
        
        predecessor = new Position* [row];
        for (int i = 0; i < row; i++)
            predecessor[i] = new Position [col];
    }
    bool isEmpty(int i, int j){
        return (isValid(i, j) && floor[i][j] == '0');
    }
    bool isValid(int i, int j){
        return ((i >= 0 && j >= 0) && (i < row && j < col));
    }
    int getDegree(int i, int j){
        int count = 0;
        for (int k = 0; k < 4; k++)
            if (isEmpty(i + x[k], j + y[k]))
                count++;
        return count;
    }
    bool nextMove(){
        int minIndex = -1, minDegree = 5;

        int start = 0;
        for (int i = 0; i < 4; i++){
            int s = (i + start) % 4;
            if (isEmpty(currentRow + x[s], currentCol + y[s]))
                if (getDegree(currentRow + x[s], currentCol + y[s]) < minDegree){
                    minIndex = s;
                    minDegree = getDegree(currentRow + x[s], currentCol + y[s]);
                }
        }

        if (minIndex == -1){
            return false;
        }else {
            currentRow += x[minIndex];
            currentCol += y[minIndex];
            floor[currentRow][currentCol] = '.';
            Position s;
            s.r = currentRow;
            s.c = currentCol;
            deletion(currentRow, currentCol);
            q.Push(s);
            return true;
        }
    }
    int cleanable(){
        int count = 0;
        for (int i = 0; i < row; i++)
            for (int j = 0; j < col; j++)
                if (floor[i][j] == '0')
                    count ++;
        return count;
    }
    Position choose(){
        Position p;
        int min = N;
        for (int i = 0; i < row; i++){
            for (int j = 0; j < col; j++){
                if (isEmpty(i, j) && !s[i][j] && dist[i][j] < min){
                    p.r = i;
                    p.c = j;
                    min = dist[i][j];
                }
            }  
        }
        return p;
    }
    void shortestPath(){
        for (int i = 0; i < row; i++){
            for (int j = 0; j < col; j++){
                s[i][j] = false;
                dist[i][j] = N;
            }  
        }
        
        for (int w = 0; w < 4; w++){
            Position p;
            p.r = R_row;
            p.c = R_col;
            int r = R_row + x[w], c = R_col + y[w];
            if (isEmpty(r, c)){
                dist[r][c] = 1;
                predecessor[r][c] = p;
            } 
        }
        
        s[R_row][R_col] = true;
        dist[R_row][R_col] = 0;
        
        int v = cleanable();
        for (int i = 0; i < v - 2; i++){
            Position u = choose();
            s[u.r][u.c] = true;
            
            for (int w = 0; w < 4; w++){
                int r = u.r + x[w];
                int c = u.c + y[w];
                if (isEmpty(r, c) && !s[r][c] && dist[u.r][u.c] + 1 < dist[r][c]){
                    dist[r][c] = dist[u.r][u.c] + 1;
                    predecessor[r][c] = u;
                } 
            }
        }
    }
    void path(int i, int j){
        if (!(i == R_row && j == R_col)){
            path(predecessor[i][j].r, predecessor[i][j].c);
            Position p;
            p.r = currentRow = i;
            p.c = currentCol = j;
            floor[currentRow][currentCol] = '.';
            deletion(currentRow, currentCol);
            q.Push(p);
        }
    }
    void buildHeap(){
        heapSize = cleanable();
        heap = new Position [heapSize + 1];
        int index = 1;
        for (int i = 0; i < row; i++)
            for (int j = 0; j < col; j++)
                if (dist[i][j] != N && dist[i][j] != 0){
                    heap[index].r = i;
                    heap[index].c = j;
                    heap[index++].distance = dist[i][j];
                }
        for (int i = heapSize / 2; i > 0; i--)
            heaptify(i);
    }
    void heaptify(int i){
        int smallest;
        if (2 * i <= heapSize && heap[2 * i].distance < heap[i].distance)
            smallest = 2 * i;
        else
            smallest = i;
        if (2 * i + 1 <= heapSize && heap[2 * i + 1].distance < heap[smallest].distance)
            smallest = 2 * i + 1;
        if (smallest != i){
            Position temp;
            temp = heap[smallest];
            heap[smallest] = heap[i];
            heap[i] = temp;
            heaptify(smallest);
        }
    }
    Position extractMin(){
        Position temp;
        temp = heap[heapSize];
        heap[heapSize] = heap[1];
        heap[1] = temp;
        heapSize--;
        heaptify(1);
        return heap[heapSize + 1];
    }
    void deletion(int i, int j){
        int k = 1;
        for (; k <= heapSize; k++)
            if (heap[k].r == i && heap[k].c == j)
                break;
        if (k > heapSize)
            return;
        while (k > 1 && heap[k / 2].distance > -1){
            heap[k] = heap[k / 2];
            k /= 2;
        }
        heap[1] = heap[heapSize--];   
        heaptify(1);
    }
    void clean(ofstream &ofile){
        shortestPath();
        buildHeap();

        currentRow = R_row;
        currentCol = R_col;
        Position s;
        s.r = currentRow;
        s.c = currentCol;
        q.Push(s);

        while (heapSize){
            int usage = 0;
            Position z = extractMin();
            path(z.r, z.c);
            currentRow = z.r;
            currentCol = z.c;
            usage = dist[z.r][z.c];
            steps += dist[z.r][z.c];

            while (battery - usage > dist[currentRow][currentCol])
                if (nextMove()){
                    usage++;
                    steps++;
                }else
                    break;
        
            while (!(currentRow == R_row && currentCol == R_col)){
                Position p = predecessor[currentRow][currentCol];
                currentRow = p.r;
                currentCol = p.c;
                floor[currentRow][currentCol] = '.';
                deletion(currentRow, currentCol);
                q.Push(p);
                steps++;
            }
        }
        
        ofile << steps << endl;
        Position p;
        while (!q.isEmpity()){
            p = q.Front();
            q.Pop();
            ofile << p.r << " " << p.c << endl;
        }
    }
    private:
    char** floor;
    int** dist;
    Position** predecessor;
    bool** s;
    Position* heap;
    int row, col, battery, steps;
    int R_row, R_col;
    int currentRow, currentCol;
    int heapSize;
    int x[4] = {0, 1, 0, -1};
    int y[4] = {1, 0, -1, 0};
    Queue<Position> q;
};