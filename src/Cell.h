#ifndef CELL_H
#define CELL_H

#include <vector>

class Cell
{
public:
    Cell();

    std::vector<int> bucket; //list of ids boids in the cell
    unsigned long int timestamp = 0;
};

#endif // CELL_H
