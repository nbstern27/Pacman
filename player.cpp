#include "player.h"

Player::Player(int r, int c, GameData* game) {
    this->r = r;
    this->c = c;
    this->px = c*8;
    this->py = r*8+3;
    this->frameOfAnimation = 5;
    this->direc[0] = -1;
    this->direc[1] = 0;
    this->spriteX = 503;
    this->spriteY = 354; 
    this->rotate = false;

    Texture playerAssets;
    if (!playerAssets.loadFromFile("AllAssets.png")) {
        std::cout << "Failed to load entity sprites!" << std::endl;
    }

    Image image = playerAssets.copyToImage();
    for (int x = 0; x < 999; x++) {
        for (int y = 0; y < 749; y++) {
            Color color = image.getPixel(x, y);
            if (color == Color::Black) {
                color.a = 0; // set alpha channel to 0 to make pixel transparent
                image.setPixel(x, y, color);
            }
        }
    }
    playerAssets.update(image);
    this->playerAssets = playerAssets;
}

void Player::drawPacman(RenderWindow* window) {
    Sprite sprite;
    sprite.setTexture(this->playerAssets);

    // Set scaling factor
    sprite.setScale(3, 3);

    bool rotate = this->rotate; // Rotated sprite?
    int x = this->spriteX; // X location of sprite
    int y = this->spriteY; // Y location of sprite
    int dy = 0;
    int dx = 0;

    IntRect spriteRect(x, y, 16, 16);
    sprite.setTextureRect(spriteRect);
    if (rotate) {
        sprite.rotate(180.0f);
        dx+=15;
        dy+=15;
    }

    int drawX = this->px - 7;
    int drawY = this->py - 7;

    sprite.setPosition(Vector2f(3*(drawX + dx), 3*(drawY + dy)));
    window->draw(sprite);
}

void Player::drawGhostCombo(RenderWindow* window, int combo) {
    Sprite sprite;
    sprite.setTexture(this->playerAssets);
    int x = 435;
    int y = 354;
    switch (combo) {
        case 200:
            break;
        case 400:
            x+=17;
            break;
        case 800:
            x+=34;
            break;
        case 1600:
            x+=51;
            break;
    }

    // Set scaling factor
    sprite.setScale(3, 3);

    // Define the texture rectangle for the sprite
    IntRect spriteRect(x, y, 16, 16);
    sprite.setTextureRect(spriteRect);

    // Position the sprite on the screen
    int drawX = this->px - 7;
    int drawY = this->py - 7;

    sprite.setPosition(Vector2f(3*(drawX), 3*(drawY)));

    window->draw(sprite);
}

void Player::drawDeath(RenderWindow* window, int frame) {
    Sprite sprite;
    sprite.setTexture(this->playerAssets);
    int x = 401;
    int y = 320;
    int frameNum = frame / 20;
    x += 17 * (frameNum % 6);
    frameNum > 5 ? y += 17 : y += 0;

    // Set scaling factor
    sprite.setScale(3, 3);

    // Define the texture rectangle for the sprite
    IntRect spriteRect(x, y, 16, 16);
    sprite.setTextureRect(spriteRect);

    // Position the sprite on the screen
    int drawX = this->px - 7;
    int drawY = this->py - 7;

    sprite.setPosition(Vector2f(3*(drawX), 3*(drawY)));

    window->draw(sprite);
}

void Player::updateMovement(GameData* game) {
    int newRow, newCol;
    int dx = 0;
    int dy = 0;

    if (direc[0] == 0) {
        if (direc[1] == -1) {
            dy = -4;
        } else {
            dy = 5;
        }
    }
    if (direc[1] == 0) {
        if (direc[0] == -1) {
            dx = -4;
        } else {
            dx = 5;
        }
    }

    newCol = px+dx;
    newRow = py+dy;

    newCol = (game->getCols()+newCol / 8) % (game->getCols());
    newRow = (game->getRows()+newRow / 8) % (game->getRows());

    // Collision check
    if (!game->checkCollision(newRow, newCol, 4)) {
        this->px = (8*game->getCols()+px+direc[0]) % (8*game->getCols());
        this->py = (8*game->getRows()+py+direc[1]) % (8*game->getRows());
        this->frameOfAnimation = (this->frameOfAnimation + 1) % 8;
        getSprite();
    } else {
        // Wall collision keeps animation and advances movement 4 pixels
        this->frameOfAnimation = 5;
        getSprite();
        return;
    }

    // Move Pacman
    if (c != px / 8) {
        game->qMovep[0] = direc[0];
        game->qMovep[1] = direc[1];
    }

    if (r != py / 8) {
        game->qMovep[0] = direc[0];
        game->qMovep[1] = direc[1];
    }
}

void Player::commenceMove(GameData* game, int row, int col) {
    // Assumes no collision with ghost, as that is handled in the main collision function
    Block** blocks = game->getBlocks();
    if (row < 0) {
        row = game->getRows() - 1;
    }

    if (row == game->getRows()) {
        row = 0;
    }

    if (col < 0) {
        col = game->getCols() - 1;
    }

    if (col == game->getCols()) {
        col = 0;
    }

    switch (blocks[row][col].getType()) {
        case 0: // Blank space
            game->changeBlock(row, col, 2);
            game->changeBlock(this->r, this->c, 0);
            this->r = row;
            this->c = col;
            break;
        case 1: // Wall
            break;
        case 2: // Player
            break;
        case 3: // Pellet
            game->score+=10;
            game->numDots++;
            game->tempDots++;
            game->lastDotTimer = 0;
            game->changeBlock(row, col, 2);
            game->changeBlock(this->r, this->c, 0);
            this->r = row;
            this->c = col;
            break;
        case 4: // Powerup
            game->setPowerUpTimer = true;
            game->score+=50;
            game->changeBlock(row, col, 2);
            game->changeBlock(this->r, this->c, 0);
            this->r = row;
            this->c = col;
            break;
        case 5: // Ghosts
            game->changeBlock(row, col, 2);
            game->changeBlock(this->r, this->c, 0);
            this->r = row;
            this->c = col;
            break;
        case 6: // Ghost and pellet
            game->score+=10;
            game->numDots++;
            game->tempDots++;
            game->lastDotTimer = 0;
            game->changeBlock(row, col, 2);
            game->changeBlock(this->r, this->c, 0);
            this->r = row;
            this->c = col;
            break;
        case 7: // Ghost and powerup
            game->setPowerUpTimer = true; 
            game->score+=50;
            game->changeBlock(row, col, 2);
            game->changeBlock(this->r, this->c, 0);
            this->r = row;
            this->c = col;
            break;
    }
    game->setElroy();
}

void Player::getSprite() {
    bool wall = false;
    int x = this->spriteX;
    int y = this->spriteY;
    bool rotate;
    int moveDirec;

    if (direc[0] == 0) {
        if (direc[1] == -1) {
            moveDirec = 1;
        } else if (direc[1] == 1) {
            moveDirec = 3;
        } else {
            moveDirec = 0;
        }
    } else if (direc[1] == 0) {
        if (direc[0] == -1) {
            moveDirec = 4;
        } else if (direc[0] == 1) {
            moveDirec = 2;
        } else {
            moveDirec = 0;
        }
    } else {
        return;
    }

    switch (moveDirec) {
        case 0:
            return;
        case 1:
            rotate = true;
            // If next spot after move is wall we keep the mouth open
            if (wall) {
                x = 520;
                y = 337;
                break;
            }
            switch (this->frameOfAnimation) {
                case 0:
                    x = 520;
                    y = 337;
                    break;
                case 1:
                    x = 520;
                    y = 337;
                    break;
                case 2:
                    x = 520;
                    y = 320; 
                    break;
                case 3:
                    x = 520;
                    y = 320;
                    break;
                case 4:
                    x = 520;
                    y = 337;
                    break;
                case 5:
                    x = 520;
                    y = 337;
                    break;
                case 6:
                    x = 503;
                    y = 354; 
                    break;
                case 7:
                    x = 503;
                    y = 354; 
                    break;
            }
            break;
        case 2:
            rotate = false;
            // If next spot after move is wall we keep the mouth open
            if (wall) {
                x = 503;
                y = 337;
                break;
            }
            switch (this->frameOfAnimation) {
                case 0:
                    x = 503;
                    y = 337;
                    break;
                case 1:
                    x = 503;
                    y = 337; 
                    break;
                case 2:
                    x = 503;
                    y = 320;
                    break;
                case 3:
                    x = 503;
                    y = 320;
                    break;
                case 4:
                    x = 503;
                    y = 337;
                    break;
                case 5:
                    x = 503;
                    y = 337;
                    break;
                case 6:
                    x = 503;
                    y = 354; 
                    break;
                case 7:
                    x = 503;
                    y = 354; 
                    break;
            }
            break;
        case 3:
            rotate = false;
            // If next spot after move is wall we keep the mouth open
            if (wall) {
                x = 520;
                y = 337;
                break;
            }
            switch (this->frameOfAnimation) {
                case 0:
                    x = 520;
                    y = 337;
                    break;
                case 1:
                    x = 520;
                    y = 337;
                    break;
                case 2:
                    x = 520;
                    y = 320; 
                    break;
                case 3:
                    x = 520;
                    y = 320; 
                    break;
                case 4:
                    x = 520;
                    y = 337;
                    break;
                case 5:
                    x = 520;
                    y = 337;
                    break;
                case 6:
                    x = 503;
                    y = 354; 
                    break;
                case 7:
                    x = 503;
                    y = 354; 
                    break;
            }
            break;
        case 4:
            rotate = true;
            // If next spot after move is wall we keep the mouth open
            if (wall) {
                x = 503;
                y = 337;
                break;
            }
            switch (this->frameOfAnimation) {
                case 0:
                    x = 503;
                    y = 337;
                    break;
                case 1:
                    x = 503;
                    y = 337;
                    break;
                case 2:
                    x = 503;
                    y = 320;
                    break;
                case 3:
                    x = 503;
                    y = 320;
                    break;
                case 4:
                    x = 503;
                    y = 337;
                    break;
                case 5:
                    x = 503;
                    y = 337; 
                    break;
                case 6:
                    x = 503;
                    y = 354; 
                    break;
                case 7:
                    x = 503;
                    y = 354; 
                    break;
            }
            break;
    }

    this->spriteX = x;
    this->spriteY = y;
    this->rotate = rotate;
}

void Player::respawn(int r, int c) {
    this->r = r;
    this->c = c;
    this->px = c*8;
    this->py = r*8+3;
    this->frameOfAnimation = 5;
    this->direc[0] = 0;
    this->direc[1] = 0;
    this->spriteX = 503;
    this->spriteY = 354; 
    this->rotate = false;
}