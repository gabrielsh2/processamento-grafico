#ifndef DiamondView_h
#define DiamondView_h

#include "TilemapView.h"
#include <iostream>

using namespace std;
class DiamondView : public TilemapView {
public:
    void computeDrawPosition(const int col, const int row, const float tw, const float th, float& targetx, float& targety) const {
        targetx = ((col * tw) + (row * tw))/2;
        targety = ((col * th) - (row * th))/2;
    }
    void computeMouseMap(int& col, int& row, const float tw, const float th, float mx, float my) const {
        //cout << mx << " + " << my << " | ";
        my = (my + 1)/2 ; //variar y de 0 até 1
        mx = (mx + 1) ; //variar x de 0 até 2
        col = 29.25 - ((my / th) + ((-mx / tw)));
        row = ((my / th) + (mx / tw)) - 58.25;


        //cout << row << " + " << col;


        //row = my / th2;
        //col = (mx - row * tw2) / tw;
        //row = (my - (col * th2)) / th;
        
        //cout << "dest: " << col << "," << row << endl;
        //cout << "tw/th " << tw << "," << th << endl;
        //cout << "mx/my " << mx << "," << my << endl;
    }
    void computeTileWalking(int& col, int& row, const int direction) const {
        switch (direction) {
        case DIRECTION_NORTH:
            row--;
            break;
        case DIRECTION_EAST:
            col++;
            break;
        case DIRECTION_SOUTH:
            row++;
            break;
        case DIRECTION_WEST:
            col--;
            break;
        }
    }

};
#endif
