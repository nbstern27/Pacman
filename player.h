#ifndef PLAYER_H
#define PLAYER_H

#include "gameData.h"

class Player {
    public:
        int r; // Row location
        int c; // Column location
        int py; // Pixel Row location
        int px; // Pixel Column Location
        int direc[2]; // Movement direction

        bool killGhost;

        int frameOfAnimation; // 0-7
        int spriteX; // X location of sprite
        int spriteY; // Y location of sprite
        bool rotate; // Previous data on if sprite was rotated

        Texture playerAssets;

        Player(int r, int c, GameData* game);
        void drawPacman(RenderWindow* window);
        void drawDeath(RenderWindow* window, int frame);
        void drawGhostCombo(RenderWindow* window, int combo);
        void updateMovement(GameData* game);
        void commenceMove(GameData* game, int r, int c);
        void getSprite();
        void respawn(int r, int c);
};

#endif