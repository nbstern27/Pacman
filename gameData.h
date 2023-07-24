#ifndef GAMEDATA_H
#define GAMEDATA_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <array>
#include <tuple>
#include <iostream>
#include <cmath>
#include <fstream>
#include <string>
#include <iomanip>

using namespace sf;

class Block {
    private:
        int x;          // Upper-left corner x position of block
        int y;          // Upper-left corner y position of block
        int l;          // Length of block in the x direction
        int h;          // Height of block in the y direction

        int type;       // 0 is blank space, 1 is wall, 2 is player, 3 is pellet, 4 is powerup, 5 is ghost, 6 is ghost + pellet, 7 is ghost + powerup
        int texture;    // 0-30, 61-62 is walls, 31 is pellet, 32 is powerup, 63 is blank space

    public:
        int getX();
        int getY();
        int getL();
        int getH();
        int getType();
        int getTexture();
        void setX(int x);
        void setY(int y);
        void setL(int l);
        void setH(int h);
        void setType(int type);
        void setTexture(int texture);
};

class GameData {
    private:
        Block** blocks;
        int cols;
        int rows;
        int scalingFactor;
        std::array<std::tuple<int, int>, 64> spriteArray;
        Texture spriteSheet;
        Texture flashBackground;
        Texture pinkText;
        int levelIcons[20][7] = {
        {0, 0, 0, 0, 0, 0, 1},
        {0, 0, 0, 0, 0, 2, 1},
        {0, 0, 0, 0, 3, 2, 1},
        {0, 0, 0, 3, 3, 2, 1},
        {0, 0, 4, 3, 3, 2, 1},
        {0, 4, 4, 3, 3, 2, 1},
        {5, 4, 4, 3, 3, 2, 1},
        {5, 5, 4, 4, 3, 3, 2},
        {6, 5, 5, 4, 4, 3, 3},
        {6, 6, 5, 5, 4, 4, 3},
        {7, 6, 6, 5, 5, 4, 4},
        {7, 7, 6, 6, 5, 5, 4},
        {8, 7, 7, 6, 6, 5, 5},
        {8, 8, 7, 7, 6, 6, 5},
        {8, 8, 8, 7, 7, 6, 6},
        {8, 8, 8, 8, 7, 7, 6},
        {8, 8, 8, 8, 8, 7, 7},
        {8, 8, 8, 8, 8, 8, 7},
        {8, 8, 8, 8, 8, 8, 8},
        {8, 8, 8, 8, 8, 8, 8}
    };

    public:
        bool setPowerUpTimer;
        bool isPoweredUp;
        int ghostState;
        int* qMovep;
        int* qMoveI;
        int* qMoveB;
        int* qMoveP;
        int* qMoveC;
        int score;
        int highScore;
        int numDots;
        int lives;
        int levelNum;

        int overlayCount;
        
        int blinkX;
        int blinkY;

        bool isElroyI;
        bool isElroyII;
        bool hasDied;
        int clydeState;

        bool isFruit;
        bool eatenFruit;
        bool firstFrameEat;

        // Used for ghosts leaving house
        int tempDots; 
        bool dotsSwitch;
        int lastDotTimer;

        GameData(int cols, int rows, int sizeX, int sizeY);
        void resetBoard(bool newLevel);
        Block** getBlocks();
        void setScalingFactor(int scalingFactor);
        int getCols();
        int getRows();
        int getScalingFactor();
        
        void changeBlock(int r, int c, int type);
        void loadSprites();
        void updateDisplay(RenderWindow* window, bool flashOrb);
        void drawFlash(RenderWindow* window);
        void drawPlayerOne(RenderWindow* window, Texture assests);
        void drawReady(RenderWindow* window, Texture assests);
        void drawGameOver(RenderWindow* window, Texture assets);
        void drawOverlay(RenderWindow* window, Texture assets, bool drawBottom);
        void drawOpeningScreen(RenderWindow* window, Texture assests, int fr);
        void drawOpeningOverlay(RenderWindow* window, Texture assests, int fr);
        bool checkCollision(int r, int c, int type);
        void debug(RenderWindow* window);

        void addWall(int r, int c, int texture);
        void addWalls();
        void setElroy();

        void readHighScore();
        void writeHighScore();

        ~GameData();
        
};

#endif