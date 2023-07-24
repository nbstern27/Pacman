#include "ghost.h"

Ghost::Ghost(int r, int c, GameData* game, int gNum) {
    this->r = r;
    this->c = c;
    this->frameOfAnimation = 0;
    this->gNum = gNum;
    this->prevFrameDC = 0;
    this->prevFrameDR = 0;
    this->prevC = c;
    this->prevR = r;
    this->moveDirec = 0;
    this->pacC = 0;
    this->pacR = 0;
    this->pacDir = new int[2];
    this->pacDir[0] = 0;
    this->pacDir[1] = 1;
    this->changeDirec = false;
    this->leaveHouse = false;
    this->leaveHouseDirec = 4;
    this->vulnerable = false;
    this->flash = false;
    this->dotCount = 0;
    this->ghostHouseOffset = 16;
    int coords[2];
    switch (gNum) {
        case 0:
            this->prevFrameX = 503;
            this->prevFrameY = 83;
            coords[0] = 34;
            coords[1] = 27;
            this->state = 5;
            break;
        case 1:
            this->prevFrameX = 69;
            this->prevFrameY = 83; 
            coords[0] = -1;
            coords[1] = 25;
            this->state = 0;
            this->moveDirec = 4;
            break;
        case 2:
            this->prevFrameX = 235;
            this->prevFrameY = 83; 
            coords[0] = -1;
            coords[1] = 2;
            this->state = 5;
            break;
        case 3:
            this->prevFrameX = 703;
            this->prevFrameY = 83; 
            coords[0] = 34;
            coords[1] = 0;
            this->state = 5;
            break;
    }
    this->targetScatter[0] = coords[0];
    this->targetScatter[1] = coords[1];
    Texture ghostTextures;
    if (!ghostTextures.loadFromFile("AllAssets.png")) {
        std::cout << "Failed to load entity sprites!" << std::endl;
    }

    Image image = ghostTextures.copyToImage();
    for (int x = 0; x < 999; x++) {
        for (int y = 0; y < 749; y++) {
            Color color = image.getPixel(x, y);
            if (color == Color::Black) {
                color.a = 0; // set alpha channel to 0 to make pixel transparent
                image.setPixel(x, y, color);
            }
        }
    }
    ghostTextures.update(image);
    this->ghostTextures = ghostTextures;
}

void Ghost::reset(int r, int c, GameData* game) {
    this->r = r;
    this->c = c;
    this->frameOfAnimation = 0;
    this->prevFrameDC = 0;
    this->prevFrameDR = 0;
    this->prevC = c;
    this->prevR = r;
    this->moveDirec = 0;
    this->changeDirec = false;
    this->leaveHouse = false;
    this->leaveHouseDirec = 4;
    this->vulnerable = false;
    this->dotCount = 0;
    this->ghostHouseOffset = 16;
    int coords[2];
    switch (gNum) {
        case 0:
            this->prevFrameX = 503;
            this->prevFrameY = 83;
            coords[0] = 34;
            coords[1] = 27;
            this->state = 5;
            break;
        case 1:
            this->prevFrameX = 69;
            this->prevFrameY = 83; 
            coords[0] = -1;
            coords[1] = 25;
            this->state = 0;
            this->moveDirec = 4;
            break;
        case 2:
            this->prevFrameX = 235;
            this->prevFrameY = 83; 
            coords[0] = -1;
            coords[1] = 2;
            this->state = 5;
            break;
        case 3:
            this->prevFrameX = 703;
            this->prevFrameY = 83; 
            coords[0] = 34;
            coords[1] = 0;
            this->state = 5;
            break;
    }
    this->targetScatter[0] = coords[0];
    this->targetScatter[1] = coords[1];
}

int* Ghost::getSprite(int frame, int direc) {
    int x;
    int y;
    int ghostDx;
    if (this->state == 3) {
        y = 269;
        ghostDx = 0;
        switch(direc) {
            case 0:
                x = this->prevFrameX;
                y = this->prevFrameY;
                break;
            case 1:
                if (this->frameOfAnimation > -4) {
                    x = 303;
                } else {
                    x = 320;
                }
                break;
            case 2:
                if (this->frameOfAnimation < 4) {
                    x = 201;
                } else {
                    x = 218;
                }
                break;
            case 3:
                if (this->frameOfAnimation < 4) {
                    x = 235;
                } else {
                    x = 252;
                }
                break;
            case 4:
                if (this->frameOfAnimation > -4) {
                    x = 269;
                } else {
                    x = 286;
                }
                break;
         }
    } else {
        switch (this->gNum) {
            case 0:
                ghostDx = 0;
                break;
            case 1:
                ghostDx = -400;
                break;
            case 2:
                ghostDx = -200;
                break;
            case 3:
                ghostDx = 200;
                break;
        }

        if (this->state == 2 || (this->state == 5 && this->vulnerable)) {
            ghostDx = 0;
            y = 540;
            if (this->frameOfAnimation < 4) {
                x = 201;
            } else {
                x = 218;
            }
            if (this->flash) {
                y = 354;
                if (this->frameOfAnimation < 4) {
                    x = 1;
                } else {
                    x = 18;
                }
            }
        } else {
            y = 83;
            switch(direc) {
                    case 0:
                        x = this->prevFrameX;
                        y = this->prevFrameY;
                        break;
                    case 1:
                        if (this->frameOfAnimation > -4) {
                            x = 503;
                        } else {
                            x = 520;
                        }
                        break;
                    case 2:
                        if (this->frameOfAnimation < 4) {
                            x = 401;
                        } else {
                            x = 418;
                        }
                        break;
                    case 3:
                        if (this->frameOfAnimation < 4) {
                            x = 435;
                        } else {
                            x = 452;
                        }
                        break;
                    case 4:
                        if (this->frameOfAnimation > -4) {
                            x = 469;
                        } else {
                            x = 486;
                        }
                        break;
            }

        }
    }

    x = x+ghostDx;
    int * ret = new int[2];
    ret[0] = x;
    ret[1] = y;
    return ret;
}

void Ghost::drawGhost(RenderWindow* window, bool still) {
    Sprite sprite;
    sprite.setTexture(this->ghostTextures);

    // Set scaling factor
    sprite.setScale(3, 3);

    int x = this->prevFrameX; // X location of sprite
    int y = this->prevFrameY; // Y location of sprite
    int dc = this->prevFrameDC;
    int dr = this->prevFrameDR;
    int c = this->prevC;
    int r = this->prevR;
    
    IntRect spriteRect(x, y, 16, 16);
    sprite.setTextureRect(spriteRect);
    if (this->state == 5 || this->state == 3 || still) {
        dc+=this->ghostHouseOffset / 4;
    } else {
        if (this->ghostHouseOffset > 0) {
            dc+=--this->ghostHouseOffset / 4;
        }
    }

    sprite.setPosition(Vector2f(c * 24 + 3*dc - 12, r * 24 + 3*dr - 12));
    window->draw(sprite);
}

void Ghost::updateMovement(GameData* game) {
    if ((this->frameOfAnimation == 0)) {getMovement(game);}
    this->prevC = this->c;
    this->prevR = this->r;
    int newRow, newCol;
    int dx, dy; // Offset for animation cycle

    int x = prevFrameX; // X location of sprite
    int y = prevFrameY; // Y location of sprite
    int * coords;

    int* dir = new int[2];
    dir[0] = 0;
    dir[1] = 0;

    switch (this->moveDirec) {
        case 0:
            dx = 0;
            dy = 0;
            break;
        case 1:
            newRow = r-1;
            newCol = c;
            if (newRow == -1) {
                newRow = game->getRows()-1;
            }
            if (game->checkCollision(newRow, c, this->gNum)) {
                break;
            }
            dx = 0;
            dy = --this->frameOfAnimation;
            coords = getSprite(dy, 1);
            x = coords[0];
            y = coords[1];
            if (dy == -8) {
                dir[0] = 0;
                dir[1] = -1;
                this->frameOfAnimation = 0;
            }
            break;
        case 2:
            newRow = r;
            newCol = c+1;
            if (newCol == game->getCols()) {
                newCol = 0;
            }
            if (game->checkCollision(r, newCol, this->gNum)) {
                break;
            }
            dx = ++this->frameOfAnimation;
            dy = 0;
            coords = getSprite(dx, 2);
            x = coords[0];
            y = coords[1];
            if (dx == 8) {
                dir[0] = 1;
                dir[1] = 0;
                this->frameOfAnimation = 0;
            }
            break;
        case 3:
            newRow = r+1;
            newCol = c;
            if (newRow == game->getRows()) {
                newRow = 0;
            }
            if (game->checkCollision(newRow, c, this->gNum)) {
                break;
            }
            dx = 0;
            dy = ++this->frameOfAnimation;
            coords = getSprite(dy, 3);
            x = coords[0];
            y = coords[1];
            if (dy == 8) {
                dir[0] = 0;
                dir[1] = 1;
                this->frameOfAnimation = 0;
            }
            break;
        case 4:
            newRow = r;
            newCol = c-1;
            if (newCol == -1) {
                newCol = game->getCols()-1;
            }
            if (game->checkCollision(r, newCol, this->gNum)) {
                break;
            }
            dx = --this->frameOfAnimation;
            dy = 0;
            coords = getSprite(dx, 4);
            x = coords[0];
            y = coords[1];
            if (dx == -8) {
                dir[0] = -1;
                dir[1] = 0;
                this->frameOfAnimation = 0;
            }
            break;
    }

    this->prevFrameX = x;
    this->prevFrameY = y;
    this->prevFrameDC = dx;
    this->prevFrameDR = dy;
    
    switch (this->gNum) {
        case 0:
            if ((game->qMoveI[0] != 0) || (game->qMoveI[1] != 0)) {break;}
            game->qMoveI[0] = dir[0];
            game->qMoveI[1] = dir[1];
            break;
        case 1:
            if ((game->qMoveB[0] != 0) || (game->qMoveB[1] != 0)) {break;}
            game->qMoveB[0] = dir[0];
            game->qMoveB[1] = dir[1];
            break;
        case 2:
            if ((game->qMoveP[0] != 0) || (game->qMoveP[1] != 0)) {break;}
            game->qMoveP[0] = dir[0];
            game->qMoveP[1] = dir[1];
            break;
        case 3:
            if ((game->qMoveC[0] != 0) || (game->qMoveC[1] != 0)) {break;}
            game->qMoveC[0] = dir[0];
            game->qMoveC[1] = dir[1];
            break;        
    }
}

void Ghost::commenceMove(GameData* game, int row, int col) {
    if (row == this->r && col == this->c) {return;}
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
    this->r = row;
    this->c = col;
    return;
}

bool* Ghost::getValidDirecs(GameData* game) {
    bool up = false;
    bool down = false;
    bool left = false;
    bool right = false;

    if (!game->checkCollision(this->r-1, this->c, this->gNum) && 
    ((this->r != 26) || ((this->c != 12) && (this->c != 15))) &&
    ((this->r != 14) || ((this->c != 12) && (this->c != 15)))) {
        up = true;
        if ((this->r == 16) && (this->state == 5) && (this->c == 13) || (this->c == 14)) {
            up = false;
        }
    }

    if (!game->checkCollision(this->r+1, this->c, this->gNum)) {
        down = true;
        if ((this->r == 14) && (this->state != 5) && ((this->c == 13) || (this->c == 14))) {
            down = false;
        }
    }

    if (!game->checkCollision(this->r, this->c+1, this->gNum)) {
        right = true;
    }

    if (!game->checkCollision(this->r, this->c-1, this->gNum)) {
        left = true;
    }

    bool* direcs = new bool[4];
    direcs[0] = up;
    direcs[1] = left;
    direcs[2] = down;
    direcs[3] = right;
    return direcs;
}

int* Ghost::getMovementTile(GameData* game) {
    int* coords = new int[2];
    int x, y;
    if (this->gNum == 1 && game->isElroyI && this->state == 0) {
        y = pacR;
        x = pacC;
        coords[0] = y;
        coords[1] = x;
        return coords;
    }
    if (this->state == 0) {
        return this->targetScatter;
    }
    if (this->state == 1) {
        switch (this->gNum) {
            case 0:
                y = pacR + 2*pacDir[1];
                x = pacC + 2*pacDir[0];
                // Glitch in original game
                if (pacDir[1] == -1) {
                    x += -2;
                }
                x = 2*x - game->blinkX;
                y = 2*y - game->blinkY;
                break;
            case 1:
                y = pacR;
                x = pacC;
                break;
            case 2:
                y = pacR + 4*pacDir[1];
                x = pacC + 4*pacDir[0];
                // Glitch in original game
                if (pacDir[1] == -1) {
                    x += -4;
                }
                break;
            case 3:
                double distance = std::sqrt(std::pow(this->c - pacC, 2) + std::pow(this->r - pacR, 2));
                if (distance >= 8) {
                    y = pacR; x = pacC;
                } else {
                    return this->targetScatter;
                }
                break;
        }
    }

    if (this->state == 3) {
        x = 14;
        if (((this->c == 15) || this->c == 11) && (this->r == 14)) {
            this->ghostHouseOffset+=4;
        }
        if (((this->c == 14) || this->c == 12) && (this->r == 14)) {
            this->ghostHouseOffset+=4;
        }
        if ((this->c == 13) && ((this->r == 14) || (this->r == 15) || (this->r == 16))) {
            this->moveDirec = 3;
            y = -20;
            this->ghostHouseOffset+=4;
        } else if (this->c >= 11 && this->c <= 15 && this->r == 17) {
            this->ghostHouseOffset+=4;
            switch (this->gNum) {
                case 0:
                    if (this->c == 11) {
                        setState(5);
                        this->moveDirec = 1;
                    } else {
                        this->moveDirec = 4;
                    }
                    break;
                case 1:
                    this->moveDirec = 1;
                    setState(5);
                    break;
                case 2:
                    this->moveDirec = 1;
                    setState(5);
                    break;
                case 3:
                    if (this->c == 15) {
                        setState(5);
                        this->moveDirec = 1;
                    } else {
                        this->moveDirec = 2;
                    }
                    break;
            }
            y = -20;
        } else {
            y = 13;
        }
    }
    
    coords[0] = y;
    coords[1] = x;
    return coords;
}

void Ghost::getMovement(GameData* game) {
    int currDir = this->moveDirec;
    bool* direcs = getValidDirecs(game);
    if (this->state == 5) {
        if (this->changeDirec) {this->leaveHouseDirec = 2;}
        if (!this->leaveHouse) {
            if (leaveHouseCheck(game)) {this->leaveHouse = true; getMovement(game); return;}
            if (direcs[0] && direcs[2] && (this->moveDirec == 1 || this->moveDirec == 3)) {return;}
            switch (this->gNum) {
                case 0:
                    direcs[0]? this->moveDirec = 1 : this->moveDirec = 3;
                    return;
                case 1:
                    direcs[2]? this->moveDirec = 3 : this->moveDirec = 1;
                    return;
                case 2:
                    direcs[2]? this->moveDirec = 3 : this->moveDirec = 1;
                    return;
                case 3:
                    direcs[0]? this->moveDirec = 1 : this->moveDirec = 3;
                    return;
            }
        } else if (this->r > 14) {
            switch (this->gNum) {
                case 0:
                    this->moveDirec = 1;
                    if (this->c != 13) {this->moveDirec = 2;}
                    break;
                case 1:
                    this->moveDirec = 1;
                    break;
                case 2:
                    this->moveDirec = 1;
                    break;
                case 3:
                    this->moveDirec = 1;
                    if (this->c != 13) {this->moveDirec = 4;}
                    break;
            }
            return;
        } else {
            this->moveDirec = this->leaveHouseDirec; 
            this->changeDirec = false;
            this->leaveHouseDirec = 4; 
            this->vulnerable ? this->setState(2) : this->setState(game->ghostState);
            return;
        }
    }

    if (this->changeDirec && this->state != 5) {
        this->changeDirec = false;
        switch (this->moveDirec) {
            case 1:
                this->moveDirec = 3;
                return;
            case 2:
                this->moveDirec = 4;
                return;
            case 3:
                this->moveDirec = 1;
                return;
            case 4:
                this->moveDirec = 2;
                return;
        }
    } 

    // Can't usually turn around 180 degrees
    if (this->moveDirec == 3) {direcs[0] = false;}
    if (this->moveDirec == 2) {direcs[1] = false;}
    if (this->moveDirec == 1) {direcs[2] = false;}
    if (this->moveDirec == 4) {direcs[3] = false;}


    int* coords = getMovementTile(game);
    if (coords[0] == -20) {return;}

    double distX, distY;
    double* distances = new double[4];
    int validDirecs[4];
    int j = 0;
    for (int i = 0; i < 4; i++) {
        if (!direcs[i]) {
            distances[i] = std::numeric_limits<double>::infinity();
        } else {
            validDirecs[j++] = i;
            int x = this->c;
            int y = this->r;
            switch (i) {
                case 0:
                    y--;
                    break;
                case 1:
                    x--;
                    break;
                case 2:
                    y++;
                    break;
                case 3:
                    x++;
                    break;
            }

            distX = abs(x - coords[1]);
            distY = abs(y - coords[0]);
            distances[i] = (distX*distX) + (distY*distY);
        }
    } 

    double minDist = std::numeric_limits<double>::infinity();
    int optimalDirec = 0;

    for (int i = 0; i < 4; i++) {
        if (distances[i] < minDist) {
            minDist = distances[i];
            optimalDirec = i+1;
        }
    }

    if (minDist == std::numeric_limits<double>::infinity()) {
        return;
    }

    // Ghost is vulnerable so we pick a random direction instead of minimum distance direction
    if (this->state == 2) {
        std::srand(std::time(nullptr));
        int randomNum = std::rand() % j;
        optimalDirec = validDirecs[randomNum] + 1;
    }

    switch (optimalDirec) {
        case 1:
            this->moveDirec = 1;
            break;
        case 2:
            this->moveDirec = 4;
            break;
        case 3:
            this->moveDirec = 3;
            break;
        case 4:
            this->moveDirec = 2;
            break;
        default:
            this->moveDirec = 0;
    }
}

bool Ghost::leaveHouseCheck(GameData* game) {
    if (gNum == 1) {return true;}
    if (!game->dotsSwitch) {
        if (gNum == 2) {
            game->tempDots = 0;
            return true;
        } else if (gNum == 0) {
            if (game->levelNum == 1) {
                if (this->dotCount >= 30) {
                    game->tempDots = 0;
                    return true;
                } else {
                    return false;
                }
            } else {
                game->tempDots = 0;
                return true;
            }
        } else {
            if (game->levelNum == 1) {
                if (this->dotCount >= 60) {
                    game->tempDots = 0;
                    return true;
                } else {
                    return false;
                }
            } else if (game->levelNum == 2) {
                if (this->dotCount >= 50) {
                    game->tempDots = 0;
                    return true;
                } else {
                    return false;
                }
            } else {
                game->tempDots = 0;
                return true;
            }
        }
    } else {
        if (gNum == 2) {
            if (game->tempDots == 7) {
                return true;
            } else {
                return false;
            }
        } else if (gNum == 0) {
            if (game->tempDots == 17) {
                return true;
            } else {
                return false;
            }
        } else {
            if (game->tempDots == 32) {
                game->tempDots = 0;
                game->dotsSwitch = false;
                return true;
            } else {
                return false;
            }
        }
    }
}

void Ghost::setMoveDirec(int direc) {
    this->moveDirec = direc;
}

void Ghost::setState(int state) {
    if (state == this->state) {
        return;
    }
    if (state == 5) {
        this->state = 5;
        this->leaveHouse = false;
        this->ghostHouseOffset = 16;
    }
    if (state != 3 && this->state != 5 && state != 5) {
        this->changeDirec = true;
    }
    this->state = state;
    if (state == 2) {this->vulnerable = true;}
    if (state == 3) {this->vulnerable = false;}
    int direc;
    if (this->moveDirec == 0) {
        direc = 1;
    } else {
        direc = this->moveDirec;
    }
    int * coords = getSprite(this->frameOfAnimation, direc);
    this->prevFrameX = coords[0];
    this->prevFrameY = coords[1];
}

int Ghost::getState() {
    return this->state;
}

int Ghost::getC() {
    return this->c;
}

int Ghost::getR() {
    return this->r;
}

int Ghost::getGNum() {
    return this->gNum;
}