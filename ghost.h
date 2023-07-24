#ifndef GHOST_H
#define GHOST_H

#include "gameData.h"
#include <limits>
#include <cstdlib>

class Ghost {
    private:
        int r; // Row location
        int c; // Column location

        int state; // 0 is scatter, 1 is chase, 2 is scared, 3 is eyeballs, 4 is ghost house
        int gNum; // 0 is inky, 1 is blinky, 2 is pinky, 3 is clyde
        int moveDirec; // 0 for stationary, 1 for north, 2 for east, 3 for south, 4 for west
        int frameOfAnimation; // 0-7

        int prevFrameX;
        int prevFrameY;
        int prevFrameDC;
        int prevFrameDR;
        int prevC;
        int prevR;
        int ghostHouseOffset;

        Texture ghostTextures;
        int targetScatter[2];

    public:
        Ghost(int r, int c, GameData* game, int gNum);
        void reset(int r, int c, GameData* game);
        void drawGhost(RenderWindow* window, bool still);
        void updateMovement(GameData* game);
        void commenceMove(GameData* game, int r, int c);
        int* getSprite(int frame, int direc);

        bool* getValidDirecs(GameData* game);
        int* getMovementTile(GameData* game);
        void getMovement(GameData* game);
        bool leaveHouseCheck(GameData* game);

        void setMoveDirec(int direc);
        void setState(int state);
        int getState();
        int getR();
        int getC();
        int getGNum();

        int pacC;
        int pacR;
        int* pacDir;
        bool changeDirec;
        bool vulnerable;
        bool flash;
        int dotCount;
        bool leaveHouse;
        int leaveHouseDirec;

};

#endif