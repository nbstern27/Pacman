#include "gameData.h"

int Block::getX() {
    return this->x;
}

int Block::getY() {
    return this->y;
}

int Block::getL() {
    return this->l;
}

int Block::getH() {
    return this->h;
}

int Block::getType() {
    return this->type;
}

int Block::getTexture() {
    return this->texture;
}

void Block::setX(int x) {
    this->x = x;
}

void Block::setY(int y) {
    this->y = y;
}

void Block::setL(int l) {
    this->l = l;
}

void Block::setH(int h) {
    this->h = h;
}

void Block::setType(int type) {
    this->type = type;
}

void Block::setTexture(int texture) {
    this->texture = texture;
}

GameData::GameData(int cols, int rows, int sizeX, int sizeY) {
    this->cols = cols;
    this->rows = rows;
    this->setPowerUpTimer = false;
    this->score = 0;
    this->numDots = 0;
    this->qMovep = new int[2]; this->qMovep[0] = 0; this->qMovep[1] = 0;
    this->qMoveI = new int[2]; this->qMoveI[0] = 0; this->qMoveI[1] = 0;
    this->qMoveB = new int[2]; this->qMoveB[0] = 0; this->qMoveB[1] = 0;
    this->qMoveP = new int[2]; this->qMoveP[0] = 0; this->qMoveP[1] = 0;
    this->qMoveC = new int[2]; this->qMoveC[0] = 0; this->qMoveC[1] = 0;
    this->ghostState = 0;
    this->levelNum = 1;
    this->isElroyI = false;
    this->isElroyII = false;
    this->hasDied = false;
    this->tempDots = 0;
    this->dotsSwitch = false;
    this->lastDotTimer = 0;
    this->lives = 5;
    this->overlayCount = 0;
    this->isFruit = false;
    this->eatenFruit = false;
    this->firstFrameEat = false;
    blocks = new Block* [rows];

    for (int r = 0; r < rows; r++) {
            blocks[r] = new Block[cols];
    }

    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            blocks[r][c].setX(c * sizeX);  // set the x-coordinate of the block
            blocks[r][c].setY(r * sizeY);  // set the y-coordinate of the block
            blocks[r][c].setL(sizeX);  // set the length of the block
            blocks[r][c].setH(sizeY);  // set the height of the block
            if (((((r > 3) && (r < 12)) || ((r > 22) && (r < 33))) && ((c > 0) && (c < 27) && (r != 6)))
            || ((r > 3) && (r <= 22) && ((c == 6) || (c == 21))) || (r == 6 && ((c == 12) || (c == 15)))) {
                blocks[r][c].setType(3); // Sets type to pellet
                blocks[r][c].setTexture(31); // Sets texure to pellet space texture
            } else {
                blocks[r][c].setType(0); // Sets type to empty space
                blocks[r][c].setTexture(63); // Sets texure to blank space texture
            }
        }
    }

    // Spawn powerups
    this->changeBlock(6, 1, 4);
    this->changeBlock(6, 26, 4);
    this->changeBlock(26, 1, 4);
    this->changeBlock(26, 26, 4);

    this->changeBlock(26, 13, 0);
    this->changeBlock(26, 14, 0);

    this->addWalls();
    this->loadSprites();
    this->readHighScore();
    this->blocks = blocks;
}

void GameData::resetBoard(bool newLevel) {
    this->setPowerUpTimer = false;
    this->qMovep[0] = 0; this->qMovep[1] = 0;
    this->qMoveI[0] = 0; this->qMoveI[1] = 0;
    this->qMoveB[0] = 0; this->qMoveB[1] = 0;
    this->qMoveP[0] = 0; this->qMoveP[1] = 0;
    this->qMoveC[0] = 0; this->qMoveC[1] = 0;
    this->ghostState = 0;
    this->isElroyI = false;
    this->isElroyII = false;
    this->hasDied = true;
    this->tempDots = 0;
    this->dotsSwitch = true;
    this->lastDotTimer = 0;

    if (!newLevel) {return;}

    this->hasDied = false;
    this->dotsSwitch = false;

    this->numDots = 0;
    this->levelNum++;
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            if (((((r > 3) && (r < 12)) || ((r > 22) && (r < 33))) && ((c > 0) && (c < 27) && (r != 6)))
            || ((r > 3) && (r <= 22) && ((c == 6) || (c == 21))) || (r == 6 && ((c == 12) || (c == 15))) 
            && (r != 6 || (c != 1 && c != 26)) && (r != 26 || (c != 1 && c != 26))) {
                blocks[r][c].setType(3); // Sets type to pellet
                blocks[r][c].setTexture(31); // Sets texure to pellet space texture
            } else if ((r != 6 || (c != 1 && c != 26)) && (r != 26 || (c != 1 && c != 26)) ){
                blocks[r][c].setType(0); // Sets type to empty space
                blocks[r][c].setTexture(63); // Sets texure to blank space texture
            }
        }
    }

    // Spawn powerups
    this->changeBlock(6, 1, 4);
    this->changeBlock(6, 26, 4);
    this->changeBlock(26, 1, 4);
    this->changeBlock(26, 26, 4);

    this->changeBlock(26, 13, 0);
    this->changeBlock(26, 14, 0);

    this->addWalls();
}

bool GameData::checkCollision(int r, int c, int id) {
    // id 4 is Pacman, 0-3 are ghosts
    // Ghost house entrance
    if (id == 4 && (r == 15) && ((c == 13) || (c == 14))) {
        return true;
    }
    switch (this->blocks[r][c].getType()) {
        case 0: // Blank space
            return false;
        case 1: // Wall
            return true;
        case 2: // Player
            return false;
        case 3: // Pellet
            return false;
        case 4: // Powerup
            return false;
        default: // Ghosts
            return false;
    }

    return false;
}

Block** GameData::getBlocks() {
    return this->blocks;
}

void GameData::changeBlock(int r, int c, int type) {
    this->blocks[r][c].setType(type);
    if (type == 0) {
        this->blocks[r][c].setTexture(63);
    } else if (type == 2) {
        this->blocks[r][c].setTexture(63);
    } else if ((type == 3) || (type == 6)) {
        this->blocks[r][c].setTexture(31);
    } else if ((type == 4) || (type == 7)) {
        this->blocks[r][c].setTexture(32);
    } else if (type == 5) {
        this->blocks[r][c].setTexture(63);
    } 
}

void GameData::setScalingFactor(int scalingFactor) {
    this->scalingFactor = scalingFactor;
}

int GameData::getCols() {
    return this->cols;
}

int GameData::getRows() {
    return this->rows;
}

int GameData::getScalingFactor() {
    return this->scalingFactor;
}

void GameData::loadSprites() {

    // Tuples of sprites specifying x and y coords of top left corner
    std::array<std::tuple<int, int>, 64> spriteTuples = {{
        std::make_tuple(0*8, 1*8),
        std::make_tuple(27*8, 2*8),
        std::make_tuple(1*8, 0*8),
        std::make_tuple(1*8, 30*8),
        std::make_tuple(0*8, 9*8),
        std::make_tuple(27*8, 30*8),
        std::make_tuple(8*28+9*10+1, 9*5),
        std::make_tuple(22*8, 9*8),
        std::make_tuple(0*8, 19*8),
        std::make_tuple(27*8, 0*8), //9
        std::make_tuple(8*28+9*8+1, 9*3),
        std::make_tuple(22*8, 13*8),
        std::make_tuple(8*28+9*7+1, 9*3),
        std::make_tuple(27*8, 24*8),
        std::make_tuple(0*8, 25*8),
        std::make_tuple(13*8, 0*8),
        std::make_tuple(3*8, 2*8),
        std::make_tuple(13*8, 2*8),
        std::make_tuple(8*8, 27*8),
        std::make_tuple(8*28+9*5+1, 9*5), //19
        std::make_tuple(17*8, 24*8),
        std::make_tuple(2*8, 27*8),
        std::make_tuple(14*8, 25*8),
        std::make_tuple(19*8, 10*8),
        std::make_tuple(11*8, 4*8),
        std::make_tuple(7*8, 4*8),
        std::make_tuple(17*8, 16*8),
        std::make_tuple(8*28+9*15+1, 9*4),
        std::make_tuple(17*8, 12*8),
        std::make_tuple(8*28+9*13+1, 9*4), //29
        std::make_tuple(13*8, 12*8),
        std::make_tuple(8*28+9*13+1, 9*2), // PELLET
        std::make_tuple(8*28+9*15+1, 9*2),   // POWERUP
        std::make_tuple(8*15, 8*3),
        std::make_tuple(0, 0),
        std::make_tuple(0, 0),
        std::make_tuple(0, 0),
        std::make_tuple(0, 0),
        std::make_tuple(0, 0),
        std::make_tuple(0, 0), //39
        std::make_tuple(0, 0), 
        std::make_tuple(0, 0),
        std::make_tuple(0, 0),
        std::make_tuple(0, 0),
        std::make_tuple(0, 0),
        std::make_tuple(0, 0),
        std::make_tuple(0, 0),
        std::make_tuple(0, 0),
        std::make_tuple(0, 0),
        std::make_tuple(0, 0), //49
        std::make_tuple(0, 0), 
        std::make_tuple(0, 0),
        std::make_tuple(0, 0),
        std::make_tuple(0, 0),
        std::make_tuple(0, 0),
        std::make_tuple(0, 0),
        std::make_tuple(0, 0),
        std::make_tuple(0, 0),
        std::make_tuple(0, 0),
        std::make_tuple(0, 0), //59
        std::make_tuple(0, 0), 
        std::make_tuple(14*8, 3*8),
        std::make_tuple(3*8, 4*8),
        std::make_tuple(27*8, 10*8)
    }};

    Texture sheet;
    if (!sheet.loadFromFile("MazeParts.png")) {
        std::cout << "Failed to load maze sprites!" << std::endl;
    }

    this->spriteSheet = sheet;
    this->spriteArray = spriteTuples;

    Texture sheet2;
    if (!sheet2.loadFromFile("FlashMaze.png")) {
        std::cout << "Failed to load maze sprites!" << std::endl;
    }

    Image image = sheet2.copyToImage();
    unsigned int width = image.getSize().x;
    unsigned int height = image.getSize().y;

    for (unsigned int x = 0; x < width; x++) {
        for (unsigned int y = 0; y < height; y++) {
            Color color = image.getPixel(x, y);

            if (color != Color::White && color != Color::Black) {
                image.setPixel(x, y, Color::White);
            }
        }
    }
    sheet2.update(image);
    this->flashBackground = sheet2;

    Texture sheet3;
    if (!sheet3.loadFromFile("FlashMaze.png")) {
        std::cout << "Failed to load maze sprites!" << std::endl;
    }

    Image image2 = sheet3.copyToImage();
    width = image2.getSize().x;
    height = image2.getSize().y;

    for (unsigned int x = 0; x < width; x++) {
        for (unsigned int y = 0; y < height; y++) {
            Color color = image2.getPixel(x, y);
            if (color == Color::Black) {
                color.a = 0;
                image2.setPixel(x, y, color);
            }
        }
    }
    sheet3.update(image2);
    this->pinkText = sheet3;

}

void GameData::updateDisplay(RenderWindow* window, bool flashOrb) {
    window->clear();
    // Restore ghost house roof
    blocks[15][13].setType(0);
    blocks[15][13].setTexture(30);
    blocks[15][14].setType(0);
    blocks[15][14].setTexture(30);
    // Update display
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            Sprite sprite;
            int texture = blocks[r][c].getTexture();
            if (texture == 32) {
                if (flashOrb) {
                    texture = blocks[0][0].getTexture();
                } 
            }
            if ((texture < 33) || (texture > 60)) {
                sprite.setTexture(this->spriteSheet);
                int leftX = std::get<0>(this->spriteArray[texture]);
                int leftY = std::get<1>(this->spriteArray[texture]);

                // Define the texture rectangle for the sprite
                IntRect spriteRect(leftX, leftY, blocks[r][c].getL() / this->scalingFactor, blocks[r][c].getH() / this->scalingFactor);
                sprite.setTextureRect(spriteRect);

                // Position the sprite on the screen
                sprite.setPosition(Vector2f(blocks[r][c].getX(), blocks[r][c].getY()));

                // Set scaling factor
                sprite.setScale(this->scalingFactor, this->scalingFactor);
                window->draw(sprite);
            }
        }
    }
}

void GameData::drawFlash(RenderWindow* window) {
    Sprite sprite;
    sprite.setTexture(this->flashBackground);

    // Set scaling factor
    sprite.setScale(3, 3);

    // Draw background
    IntRect spriteRect(228, 0, 451-228+1, 247+1);
    sprite.setTextureRect(spriteRect);
    sprite.setPosition(Vector2f(3*(0), 3*(8*3)));
    window->draw(sprite);
    return;
}

void GameData::drawPlayerOne(RenderWindow* window, Texture assets) {
    Sprite sprite;
    sprite.setTexture(assets);

    // Set scaling factor
    sprite.setScale(3, 3);

    int xVals[9] = {419, 500, 401, 500, 437, 437, 410, 401, 437};
    int yVals[9] = {37, 28, 28, 37, 28, 37, 37, 37, 28};

    // Draw "Player"
    for (int i = 0; i < 6; i++) {
        IntRect spriteRect(xVals[i], yVals[i], 8, 8);
        sprite.setTextureRect(spriteRect);
        sprite.setPosition(Vector2f(3*(8*(9+i)), 3*(8*14)));
        window->draw(sprite);
    }
    // Draw "One"
    for (int i = 6; i < 9; i++) {
        IntRect spriteRect(xVals[i], yVals[i], 8, 8);
        sprite.setTextureRect(spriteRect);
        sprite.setPosition(Vector2f(3*(8*(9+i+1)), 3*(8*14)));
        window->draw(sprite);
    }
    return;
}

void GameData::drawReady(RenderWindow* window, Texture assets) {
    Sprite sprite;
    sprite.setTexture(assets);

    // Set scaling factor
    sprite.setScale(3, 3);

    int xVals[6] = {437, 437, 401, 428, 500, 509};
    int yVals[6] = {223, 214, 214, 214, 223, 205};

    // Draw "Ready"
    for (int i = 0; i < 6; i++) {
        IntRect spriteRect(xVals[i], yVals[i], 8, 8);
        sprite.setTextureRect(spriteRect);
        sprite.setPosition(Vector2f(3*(8*(11+i)), 3*(8*20)));
        window->draw(sprite);
    }
    return;
}

void GameData::drawGameOver(RenderWindow* window, Texture assets) {
    Sprite sprite;
    sprite.setTexture(assets);

    // Set scaling factor
    sprite.setScale(3, 3);

    int xVals[10] = {55, 1, 109, 37, 19, 19, 10, 73, 37, 37};
    int yVals[10] = {400, 400, 400, 400, 436, 436, 409, 409, 400, 409};

    // Draw "Game  Over"
    for (int i = 0; i < 10; i++) {
        IntRect spriteRect(xVals[i], yVals[i], 8, 8);
        sprite.setTextureRect(spriteRect);
        sprite.setPosition(Vector2f(3*(8*(9+i)), 3*(8*20)));
        window->draw(sprite);
    }
    return;
}

void GameData::drawOverlay(RenderWindow* window, Texture assets, bool drawBottom) {
    if (this->overlayCount >= 160) {
        overlayCount = 0;
    } else {
        overlayCount++;
    }

    if (!drawBottom) {
        // Draw "Credit   0"
        for (int i = 0; i < 9; i++) {
            Sprite s;
            s.setTexture(assets);

            // Set scaling factor
            s.setScale(3, 3);
            int c = 0;
            int r = 0;

            int x = 486; int y = 337;
            switch (i) {
                case 0:
                    x = 419; y = 400;
                    break;
                case 1:
                    x = 437; y = 409;
                    break;
                case 2:
                    x = 437; y = 400;
                    break;
                case 3:
                    x = 428; y = 400;
                    break;
                case 4:
                    x = 473; y = 400;
                    break;
                case 5:
                    x = 455; y = 409;
                    break;
                case 6:
                    break;
                case 7:
                    break;
                case 8:
                    x = 401; y = 373;
                    break;
            }
            
            IntRect spriteRect(x, y, 8, 8);
            s.setTextureRect(spriteRect);

            c = 2 + i;
            r += 35;

            s.setPosition(Vector2f(3*(8*c), 3*(8*r)));
            window->draw(s);
        }
    }

    if (drawBottom) {
        // Draw Pacmans
        for (int i = 1; i < 6; i++) {
            Sprite sprite;
            sprite.setTexture(assets);

            // Set scaling factor
            sprite.setScale(3, 3);

            int x; int y;
            if (i > this->lives) {
                x = 486; y = 337;
            } else {
                x = 503;
                y = 337;
            }

            IntRect spriteRect(x, y, 16, 16);
            sprite.setTextureRect(spriteRect);
            sprite.rotate(180.0f);

            int c = i + i;
            int r = 34;

            sprite.setPosition(Vector2f(3*(8*c + 15), 3*(8*r + 15)));
            window->draw(sprite);
        }

        // Draw level icons
        for (int i = 0; i < 7; i++) {
            Sprite sprite;
            sprite.setTexture(assets);

            // Set scaling factor
            sprite.setScale(3, 3);

            int x; int y; int j;
            if (this->levelNum > 20) {
                j = 19;
            } else {
                j = this->levelNum - 1;
            }

            int spriteNum = this->levelIcons[j][i];
            switch (spriteNum) {
                case 0:
                    x = 486; y = 337;
                    break;
                case 1:
                    x = 401; y = 489;
                    break;
                case 2:
                    x = 618; y = 489;
                    break;
                case 3:
                    x = 835; y = 489;
                    break;
                case 4:
                    x = 452; y = 489;
                    break;
                case 5:
                    x = 69; y = 675;
                    break;
                case 6:
                    x = 286; y = 675;
                    break;
                case 7:
                    x = 503; y = 675;
                    break;
                case 8:
                    x = 520; y = 675;
                    break;
            }
            
            IntRect spriteRect(x, y, 16, 16);
            sprite.setTextureRect(spriteRect);

            int c = 12 + i + i;
            int r = 34;

            sprite.setPosition(Vector2f(3*(8*c), 3*(8*r)));
            window->draw(sprite);
        }
    }

    int scoreDigits[] = {-1, -1, -1, -1, -1, -1};

    if (score == 0) {
        scoreDigits[4] = 0;
        scoreDigits[5] = 0;
    } else if (score / 100000 > 0) {
        scoreDigits[0] = score / 100000;
        scoreDigits[1] = (score % 100000) / 10000;
        scoreDigits[2] = (score % 10000) / 1000;
        scoreDigits[3] = (score % 1000) / 100;
        scoreDigits[4] = (score % 100) / 10;
        scoreDigits[5] = (score % 10);
    } else if (score / 10000 > 0) {
        scoreDigits[1] = (score) / 10000;
        scoreDigits[2] = (score % 10000) / 1000;
        scoreDigits[3] = (score % 1000) / 100;
        scoreDigits[4] = (score % 100) / 10;
        scoreDigits[5] = (score % 10);
    } else if (score / 1000 > 0) {
        scoreDigits[2] = (score) / 1000;
        scoreDigits[3] = (score % 1000) / 100;
        scoreDigits[4] = (score % 100) / 10;
        scoreDigits[5] = (score % 10);
    } else if (score / 100 > 0) {
        scoreDigits[3] = (score) / 100;
        scoreDigits[4] = (score % 100) / 10;
        scoreDigits[5] = (score % 10);
    } else if (score / 10 > 0) {
        scoreDigits[4] = (score) / 10;
        scoreDigits[5] = (score % 10);
    } else {
        scoreDigits[5] = score;
    }

    // Draw score
    for (int i = 0; i < 6; i++) {
        Sprite sprite;
        sprite.setTexture(assets);

        // Set scaling factor
        sprite.setScale(3, 3);

        int x = 401; int y = 373;

        int scoreDigit = scoreDigits[i];
        switch (scoreDigit) {
            case -1:
                x = 486; y = 337;
                break;
            default:
                x += 9*(scoreDigit);
                break;
        }
        
        IntRect spriteRect(x, y, 8, 8);
        sprite.setTextureRect(spriteRect);

        int c = 1 + i;
        int r = 1;

        sprite.setPosition(Vector2f(3*(8*c), 3*(8*r)));
        window->draw(sprite);
    }

    int highScoreDigits[] = {-1, -1, -1, -1, -1, -1};
    int highScore;
    this->highScore > score ? highScore = this->highScore : highScore = score;
    if (highScore == 0) {
        highScoreDigits[4] = 0;
        highScoreDigits[5] = 0;
    } else if (highScore / 100000 > 0) {
        highScoreDigits[0] = highScore / 100000;
        highScoreDigits[1] = (highScore % 100000) / 10000;
        highScoreDigits[2] = (highScore % 10000) / 1000;
        highScoreDigits[3] = (highScore % 1000) / 100;
        highScoreDigits[4] = (highScore % 100) / 10;
        highScoreDigits[5] = (highScore % 10);
    } else if (highScore / 10000 > 0) {
        highScoreDigits[1] = (highScore) / 10000;
        highScoreDigits[2] = (highScore % 10000) / 1000;
        highScoreDigits[3] = (highScore % 1000) / 100;
        highScoreDigits[4] = (highScore % 100) / 10;
        highScoreDigits[5] = (highScore % 10);
    } else if (highScore / 1000 > 0) {
        highScoreDigits[2] = (highScore) / 1000;
        highScoreDigits[3] = (highScore % 1000) / 100;
        highScoreDigits[4] = (highScore % 100) / 10;
        highScoreDigits[5] = (highScore % 10);
    } else if (highScore / 100 > 0) {
        highScoreDigits[3] = (highScore) / 100;
        highScoreDigits[4] = (highScore % 100) / 10;
        highScoreDigits[5] = (highScore % 10);
    } else if (highScore / 10 > 0) {
        highScoreDigits[4] = (highScore) / 10;
        highScoreDigits[5] = (highScore % 10);
    } else {
        highScoreDigits[5] = highScore;
    }

    // Draw high score
    for (int i = 0; i < 6; i++) {
        Sprite sprite;
        sprite.setTexture(assets);

        // Set scaling factor
        sprite.setScale(3, 3);

        int x = 401; int y = 373;

        int highScoreDigit = highScoreDigits[i];
        switch (highScoreDigit) {
            case -1:
                x = 486; y = 337;
                break;
            default:
                x += 9*(highScoreDigit);
                break;
        }
        
        IntRect spriteRect(x, y, 8, 8);
        sprite.setTextureRect(spriteRect);

        int c = 11 + i;
        int r = 1;

        sprite.setPosition(Vector2f(3*(8*c), 3*(8*r)));
        window->draw(sprite);
    }

    // Draw "HIGH SCORE"
    for (int i = 0; i < 10; i++) {
        Sprite sprite;
        sprite.setTexture(assets);

        // Set scaling factor
        sprite.setScale(3, 3);

        int x = 486; int y = 337;
        switch (i) {
            case 0:
                x = 464; y = 400;
                break;
            case 1:
                x = 473; y = 400;
                break;
            case 2:
                x = 455; y = 400;
                break;
            case 3:
                x = 464; y = 400;
                break;
            case 4:
                break;
            case 5:
                x = 446; y = 409;
                break;
            case 6:
                x = 419; y = 400;
                break;
            case 7:
                x = 410; y = 409;
                break;
            case 8:
                x = 437; y = 409;
                break;
            case 9:
                x = 437; y = 400;
                break;
        }
        
        IntRect spriteRect(x, y, 8, 8);
        sprite.setTextureRect(spriteRect);

        int c = 9 + i;
        int r = 0;

        sprite.setPosition(Vector2f(3*(8*c), 3*(8*r)));
        window->draw(sprite);
    }

    // Draw "1UP"
    for (int i = 0; i < 3; i++) {
        Sprite sprite;
        sprite.setTexture(assets);

        // Set scaling factor
        sprite.setScale(3, 3);

        int x; int y;
        switch (i) {
            case 0:
                x = 410; y = 391;
                break;
            case 1:
                x = 464; y = 409;
                break;
            case 2:
                x = 419; y = 409;
                break;
        }

        if (this->overlayCount <= 80 && drawBottom) {
            x = 486; y = 337;
        }
        
        IntRect spriteRect(x, y, 8, 8);
        sprite.setTextureRect(spriteRect);

        int c = 3 + i;
        int r = 0;

        sprite.setPosition(Vector2f(3*(8*c), 3*(8*r)));
        window->draw(sprite);
    }

    // Draw fruit
    if (this->isFruit) {
        Sprite sprite;
        sprite.setTexture(assets);

        // Set scaling factor
        sprite.setScale(3, 3);

        int x; int y;

        int spriteNum;
        switch (this->levelNum) {
            case 1:
                spriteNum = 1;
                break;
            case 2:
                spriteNum = 2;
                break;
            case 3:
                spriteNum = 3;
                break;
            case 4:
                spriteNum = 3;
                break;
            case 5:
                spriteNum = 4;
                break;
            case 6:
                spriteNum = 4;
                break;
            case 7:
                spriteNum = 5;
                break;
            case 8:
                spriteNum = 5;
                break;
            case 9:
                spriteNum = 6;
                break;
            case 10:
                spriteNum = 6;
                break;
            case 11:
                spriteNum = 7;
                break;
            case 12:
                spriteNum = 7;
                break;
            default:
                spriteNum = 8;
                break;
        }
        switch (spriteNum) {
            case 0:
                x = 486; y = 337;
                break;
            case 1:
                x = 401; y = 489;
                break;
            case 2:
                x = 618; y = 489;
                break;
            case 3:
                x = 835; y = 489;
                break;
            case 4:
                x = 452; y = 489;
                break;
            case 5:
                x = 69; y = 675;
                break;
            case 6:
                x = 286; y = 675;
                break;
            case 7:
                x = 503; y = 675;
                break;
            case 8:
                x = 520; y = 675;
                break;
        }
        
        IntRect spriteRect(x, y, 16, 16);
        sprite.setTextureRect(spriteRect);

        sprite.setPosition(Vector2f(3*(8*13), 3*(8*20 - 4)));
        window->draw(sprite);
    }

    if (!this->eatenFruit) {
        return;
    }
    this->isFruit = false;
    int x;
    int y = 144;
    int r = 20;
    int c = 13;
    int size = 16;
    int dc = 0;
    int score = 0;
    switch (this->levelNum) {
        case 1:
            x = 456;
            score+= 100;
            break;
        case 2:
            x = 472;
            score+= 300;
            break;
        case 3:
            x = 488;
            score+= 500;
            break;
        case 4:
            x = 488;
            score+= 500;
            break;
        case 5:
            x = 488 + 16;
            score+= 700;
            break;
        case 6:
            x = 488 + 16;
            score+= 700;
            break;
        case 7:
            x = 488 + 32;
            score+= 1000;
            size+=8;
            dc-=2;
            break;
        case 8:
            x = 488 + 32;
            score+= 1000;
            size+=8;
            dc-=2;
            break;
        case 9:
            x = 516;
            y = 160;
            score+= 2000;
            size+=8;
            dc-=4;
            break;
        case 10:
            x = 516;
            y = 160;
            score+= 2000;
            size+=8;
            dc-=4;
            break;
        case 11:
            x = 516;
            y = 176;
            score+= 3000;
            size+=8;
            dc-=4;
            break;
        case 12:
            x = 516;
            y = 176;
            score+= 3000;
            size+=8;
            dc-=4;
            break;
        default:
            x = 516;
            y = 192;
            score+= 5000;
            size+=8;
            dc-=4;
            break;
    }

    Sprite sp;
    sp.setTexture(this->pinkText);

    // Set scaling factor
    sp.setScale(3, 3);
    IntRect spriteRect(x, y, size, 16);
    sp.setTextureRect(spriteRect);

    sp.setPosition(Vector2f(3*(8*c + dc), 3*(8*r - 4)));
    window->draw(sp);
    if (this->firstFrameEat) {
        this->firstFrameEat = false;
        this->score += score;
    }
}

void GameData::drawOpeningScreen(RenderWindow* window, Texture assets, int fr) {
    // Draw "Character / Nickname"
    for (int i = 0; i < 20; i++) {
        Sprite s;
        s.setTexture(assets);

        // Set scaling factor
        s.setScale(3, 3);
        int c = 0;
        int r = 0;

        int x = 486; int y = 337;
        switch (i) {
            case 0:
                x = 419; y = 400;
                break;
            case 1:
                x = 464; y = 400;
                break;
            case 2:
                x = 401; y = 400;
                break;
            case 3:
                x = 437; y = 409;
                break;
            case 4:
                x = 401; y = 400;
                break;
            case 5:
                x = 419; y = 400;
                break;
            case 6:
                x = 455; y = 409;
                break;
            case 7:
                x = 437; y = 400;
                break;
            case 8:
                x = 437; y = 409;
                break;
            case 9:
                break;
            case 10:
                x = 491; y = 382;
                break;
            case 11:
                break;
            case 12:
                x = 401; y = 409;
                break;
            case 13:
                x = 473; y = 400;
                break;
            case 14:
                x = 419; y = 400;
                break;
            case 15:
                x = 491; y = 400;
                break;
            case 16:
                x = 401; y = 409;
                break;
            case 17:
                x = 401; y = 400;
                break;
            case 18:
                x = 509; y = 400;
                break;
            case 19:
                x = 437; y = 400;
                break;
        }
        
        IntRect spriteRect(x, y, 8, 8);
        s.setTextureRect(spriteRect);

        c = 7 + i;
        r = 5;

        s.setPosition(Vector2f(3*(8*c), 3*(8*r)));
        window->draw(s);
    }

    if (fr < 2) {return;}

    // Draw Blinky and text

    for (int i = 0; i < 1; i++) {
        Sprite s;
        s.setTexture(assets);

        // Set scaling factor
        s.setScale(3, 3);
        int c = 0;
        int r = 0;

        int x = 401-400; int y = 83;
        
        IntRect spriteRect(x, y, 16, 16);
        s.setTextureRect(spriteRect);

        c = 4;
        r = 7;

        s.setPosition(Vector2f(3*(8*c), 3*(8*r - 4)));
        window->draw(s);
    }

    if (fr < 4) {return;}

    for (int i = 0; i < 11; i++) {
        Sprite s;
        s.setTexture(assets);

        // Set scaling factor
        s.setScale(3, 3);
        int c = 0;
        int r = 0;

        int x = 486+400; int y = 337+372;
        switch (i) {
            case 0:
                x = 500; y = 382;
                break;
            case 1:
                x = 446; y = 409;
                break;
            case 2:
                x = 464; y = 400;
                break;
            case 3:
                x = 401; y = 400;
                break;
            case 4:
                x = 428; y = 400;
                break;
            case 5:
                x = 410; y = 409;
                break;
            case 6:
                x = 482; y = 409;
                break;
            case 7:
                break;
            case 8:
                break;
            case 9:
                break;
            case 10:
                break;
        }

        x-=400;
        y-=372;
        
        IntRect spriteRect(x, y, 8, 8);
        s.setTextureRect(spriteRect);

        c = 7 + i;
        r = 7;

        s.setPosition(Vector2f(3*(8*c), 3*(8*r)));
        window->draw(s);
    }

    if (fr < 5) {return;}

    for (int i = 0; i < 8; i++) {
        Sprite s;
        s.setTexture(assets);

        // Set scaling factor
        s.setScale(3, 3);
        int c = 0;
        int r = 0;

        int x = 486+400; int y = 337+372;
        switch (i) {
            case 0:
                x = 491; y = 391;
                break;
            case 1:
                x = 410; y = 400;
                break;
            case 2:
                x = 500; y = 400;
                break;
            case 3:
                x = 473; y = 400;
                break;
            case 4:
                x = 401; y = 409;
                break;
            case 5:
                x = 491; y = 400;
                break;
            case 6:
                x = 500; y = 409;
                break;
            case 7:
                x = 491; y = 391;
                break;
        }

        x-=400;
        y-=372;
        
        IntRect spriteRect(x, y, 8, 8);
        s.setTextureRect(spriteRect);

        c = 18 + i;
        r = 7;

        s.setPosition(Vector2f(3*(8*c), 3*(8*r)));
        window->draw(s);
    }

    if (fr < 6) {return;}

    // Draw Pinky and text

    for (int i = 0; i < 1; i++) {
        Sprite s;
        s.setTexture(assets);

        // Set scaling factor
        s.setScale(3, 3);
        int c = 0;
        int r = 0;

        int x = 401-200; int y = 83;
        
        IntRect spriteRect(x, y, 16, 16);
        s.setTextureRect(spriteRect);

        c = 4;
        r = 10;

        s.setPosition(Vector2f(3*(8*c), 3*(8*r - 4)));
        window->draw(s);
    }

    if (fr < 8) {return;}

    for (int i = 0; i < 11; i++) {
        Sprite s;
        s.setTexture(assets);

        // Set scaling factor
        s.setScale(3, 3);
        int c = 0;
        int r = 0;

        int x = 486+200; int y = 337+372;
        switch (i) {
            case 0:
                x = 500; y = 382;
                break;
            case 1:
                x = 446; y = 409;
                break;
            case 2:
                x = 419; y = 409;
                break;
            case 3:
                x = 437; y = 400;
                break;
            case 4:
                x = 437; y = 400;
                break;
            case 5:
                x = 428; y = 400;
                break;
            case 6:
                x = 500; y = 409;
                break;
            case 7:
                break;
            case 8:
                break;
            case 9:
                break;
            case 10:
                break;
        }

        x-=200;
        y-=372;
        
        IntRect spriteRect(x, y, 8, 8);
        s.setTextureRect(spriteRect);

        c = 7 + i;
        r = 10;

        s.setPosition(Vector2f(3*(8*c), 3*(8*r)));
        window->draw(s);
    }

    if (fr < 9) {return;}

    for (int i = 0; i < 8; i++) {
        Sprite s;
        s.setTexture(assets);

        // Set scaling factor
        s.setScale(3, 3);
        int c = 0;
        int r = 0;

        int x = 486+200; int y = 337+372;
        switch (i) {
            case 0:
                x = 491; y = 391;
                break;
            case 1:
                x = 419; y = 409;
                break;
            case 2:
                x = 473; y = 400;
                break;
            case 3:
                x = 401; y = 409;
                break;
            case 4:
                x = 491; y = 400;
                break;
            case 5:
                x = 500; y = 409;
                break;
            case 6:
                x = 491; y = 391;
                break;
            case 7:
                break;
        }

        x-=200;
        y-=372;
        
        IntRect spriteRect(x, y, 8, 8);
        s.setTextureRect(spriteRect);

        c = 18 + i;
        r = 10;

        s.setPosition(Vector2f(3*(8*c), 3*(8*r)));
        window->draw(s);
    }

    if (fr < 10) {return;}

    // Draw Inky and text

    for (int i = 0; i < 1; i++) {
        Sprite s;
        s.setTexture(assets);

        // Set scaling factor
        s.setScale(3, 3);
        int c = 0;
        int r = 0;

        int x = 401-0; int y = 83;
        
        IntRect spriteRect(x, y, 16, 16);
        s.setTextureRect(spriteRect);

        c = 4;
        r = 13;

        s.setPosition(Vector2f(3*(8*c), 3*(8*r - 4)));
        window->draw(s);
    }

    if (fr < 12) {return;}

    for (int i = 0; i < 11; i++) {
        Sprite s;
        s.setTexture(assets);

        // Set scaling factor
        s.setScale(3, 3);
        int c = 0;
        int r = 0;

        int x = 486; int y = 337+372;
        switch (i) {
            case 0:
                x = 500; y = 382;
                break;
            case 1:
                x = 410; y = 400;
                break;
            case 2:
                x = 401; y = 400;
                break;
            case 3:
                x = 446; y = 409;
                break;
            case 4:
                x = 464; y = 400;
                break;
            case 5:
                x = 446; y = 400;
                break;
            case 6:
                x = 464; y = 409;
                break;
            case 7:
                x = 500; y = 400;
                break;
            case 8:
                break;
            case 9:
                break;
            case 10:
                break;
        }

        y-=372;
        
        IntRect spriteRect(x, y, 8, 8);
        s.setTextureRect(spriteRect);

        c = 7 + i;
        r = 13;

        s.setPosition(Vector2f(3*(8*c), 3*(8*r)));
        window->draw(s);
    }

    if (fr < 13) {return;}

    for (int i = 0; i < 8; i++) {
        Sprite s;
        s.setTexture(assets);

        // Set scaling factor
        s.setScale(3, 3);
        int c = 0;
        int r = 0;

        int x = 486; int y = 337+372;
        switch (i) {
            case 0:
                x = 491; y = 391;
                break;
            case 1:
                x = 473; y = 400;
                break;
            case 2:
                x = 401; y = 409;
                break;
            case 3:
                x = 491; y = 400;
                break;
            case 4:
                x = 500; y = 409;
                break;
            case 5:
                x = 491; y = 391;
                break;
            case 6:
                break;
            case 7:
                break;
        }

        y-=372;
        
        IntRect spriteRect(x, y, 8, 8);
        s.setTextureRect(spriteRect);

        c = 18 + i;
        r = 13;

        s.setPosition(Vector2f(3*(8*c), 3*(8*r)));
        window->draw(s);
    }

    if (fr < 14) {return;}

    // Draw Clyde and text

    for (int i = 0; i < 1; i++) {
        Sprite s;
        s.setTexture(assets);

        // Set scaling factor
        s.setScale(3, 3);
        int c = 0;
        int r = 0;

        int x = 401+200; int y = 83;
        
        IntRect spriteRect(x, y, 16, 16);
        s.setTextureRect(spriteRect);

        c = 4;
        r = 16;

        s.setPosition(Vector2f(3*(8*c), 3*(8*r - 4)));
        window->draw(s);
    }

    if (fr < 16) {return;}

    for (int i = 0; i < 11; i++) {
        Sprite s;
        s.setTexture(assets);

        // Set scaling factor
        s.setScale(3, 3);
        int c = 0;
        int r = 0;

        int x = 486-200; int y = 337+372;
        switch (i) {
            case 0:
                x = 500; y = 382;
                break;
            case 1:
                x = 419; y = 409;
                break;
            case 2:
                x = 410; y = 409;
                break;
            case 3:
                x = 491; y = 400;
                break;
            case 4:
                x = 437; y = 400;
                break;
            case 5:
                x = 500; y = 409;
                break;
            case 6:
                break;
            case 7:
                break;
            case 8:
                break;
            case 9:
                break;
            case 10:
                break;
        }

        x+=200;
        y-=372;
        
        IntRect spriteRect(x, y, 8, 8);
        s.setTextureRect(spriteRect);

        c = 7 + i;
        r = 16;

        s.setPosition(Vector2f(3*(8*c), 3*(8*r)));
        window->draw(s);
    }

    if (fr < 17) {return;}

    for (int i = 0; i < 8; i++) {
        Sprite s;
        s.setTexture(assets);

        // Set scaling factor
        s.setScale(3, 3);
        int c = 0;
        int r = 0;

        int x = 486-200; int y = 337+372;
        switch (i) {
            case 0:
                x = 491; y = 391;
                break;
            case 1:
                x = 419; y = 400;
                break;
            case 2:
                x = 500; y = 400;
                break;
            case 3:
                x = 500; y = 409;
                break;
            case 4:
                x = 428; y = 400;
                break;
            case 5:
                x = 437; y = 400;
                break;
            case 6:
                x = 491; y = 391;
                break;
            case 7:
                break;
        }

        x+=200;
        y-=372;
        
        IntRect spriteRect(x, y, 8, 8);
        s.setTextureRect(spriteRect);

        c = 18 + i;
        r = 16;

        s.setPosition(Vector2f(3*(8*c), 3*(8*r)));
        window->draw(s);
    }

    if (fr < 19) {return;}

    // Draw pellets and text

    for (int i = 0; i < 8; i++) {
        Sprite s;
        s.setTexture(assets);

        // Set scaling factor
        s.setScale(3, 3);
        int c = 0;
        int r = 0;

        int x = 486; int y = 337;
        switch (i) {
            case 0:
                x = 736; y = 196;
                break;
            case 1:
                break;
            case 2:
                x = 610; y = 373;
                break;
            case 3:
                x = 601; y = 373;
                break;
            case 4:
                break;
            case 5:
                x = 664; y = 382;
                break;
            case 6:
                x = 673; y = 382;
                break;
            case 7:
                x = 682; y = 382;
                break;
        }
        
        IntRect spriteRect(x, y, 8, 8);
        s.setTextureRect(spriteRect);

        c = 10 + i;
        r = 24;

        s.setPosition(Vector2f(3*(8*c), 3*(8*r)));
        window->draw(s);
    }

    for (int i = 0; i < 8; i++) {
        Sprite s;
        s.setTexture(assets);

        // Set scaling factor
        s.setScale(3, 3);
        int c = 0;
        int r = 0;

        int x = 486; int y = 337;
        switch (i) {
            case 0:
                if (fr < 23) {x = 736; y = 214;}
                break;
            case 1:
                break;
            case 2:
                x = 646; y = 373;
                break;
            case 3:
                x = 601; y = 373;
                break;
            case 4:
                break;
            case 5:
                x = 664; y = 382;
                break;
            case 6:
                x = 673; y = 382;
                break;
            case 7:
                x = 682; y = 382;
                break;
        }
        
        IntRect spriteRect(x, y, 8, 8);
        s.setTextureRect(spriteRect);

        c = 10 + i;
        r = 26;

        s.setPosition(Vector2f(3*(8*c), 3*(8*r)));
        window->draw(s);
    }

    if (fr < 21) {return;}

    // Draw 1980 logo and big pellet

    for (int i = 0; i < 22; i++) {
        Sprite s;
        s.setTexture(assets);

        // Set scaling factor
        s.setScale(3, 3);
        int c = 0;
        int r = 0;

        int x = 486+200; int y = 337+372;
        switch (i) {
            case 0:
                x = 500; y = 391;
                break;
            case 1:
                break;
            case 2:
                x = 410; y = 391;
                break;
            case 3:
                x = 482; y = 391;
                break;
            case 4:
                x = 473; y = 391;
                break;
            case 5:
                x = 401; y = 391;
                break;
            case 6:
                break;
            case 7:
                x = 509; y = 400;
                break;
            case 8:
                x = 473; y = 400;
                break;
            case 9:
                x = 428; y = 400;
                break;
            case 10:
                x = 482; y = 409;
                break;
            case 11:
                x = 401; y = 400;
                break;
            case 12:
                x = 500; y = 409;
                break;
            case 13:
                break;
            case 14:
                x = 509; y = 400;
                break;
            case 15:
                x = 446; y = 400;
                break;
            case 16:
                x = 455; y = 400;
                break;
            case 17:
                x = 509; y = 382;
                break;
            case 18:
                x = 419; y = 400;
                break;
            case 19:
                x = 410; y = 409;
                break;
            case 20:
                x = 509; y = 382;
                break;
            case 21:
                if (fr < 23) {x = 736+200; y = 214+372;}
                break;
        }
        
        x-=200;
        y-=372;

        IntRect spriteRect(x, y, 8, 8);
        s.setTextureRect(spriteRect);

        c = 4 + i;
        r = 31;

        if (i == 21) {
            c = 4;
            r = 20;
        }

        s.setPosition(Vector2f(3*(8*c), 3*(8*r)));
        window->draw(s);
    }

}

void GameData::drawOpeningOverlay(RenderWindow* window, Texture assets, int fr) {
    int pr = 20*8;
    int pc = 28*8;
    int dc = 0;
    int dr = 0;
    int x = 0; 
    int y = 0;
    if (fr < 24*8) {
        for (int i = 0; i < 6; i++) {
            Sprite sprite;
            sprite.setTexture(assets);

            // Set scaling factor
            sprite.setScale(3, 3);
            switch (i) {
                case 0:
                    x = 503;
                    y = 337;
                    break;
                case 1:
                    x = 69;
                    y = 83;
                    break;
                case 2:
                    x = 269;
                    y = 83;
                    break;
                case 3:
                    x = 469;
                    y = 83;
                    break;
                case 4:
                    x = 669;
                    y = 83;
                    break;
                case 5:
                    x = 736;
                    y = 214;
                    break;
            }

            if (i == 0) {
                sprite.rotate(180.0f);
                if ((fr % 8 > 1) && (fr % 8 < 4)) {
                    y+=17;
                } else if ((fr % 8 > 3) && (fr % 8 < 6)) {
                    y-=17;
                }
                IntRect spriteRect(x, y, 16, 16);
                sprite.setTextureRect(spriteRect);
            } else if (i != 5) {
                dc = 16*i + 2 - 15 - (fr / 48);
                dr = -15;
                if (fr % 8 < 4) {x+=17;}
                IntRect spriteRect(x, y, 16, 16);
                sprite.setTextureRect(spriteRect);
            } else {
                IntRect spriteRect(x, y, 8, 8);
                sprite.setTextureRect(spriteRect);
            }

            sprite.setPosition(Vector2f(3*(pc - fr + 15 + dc), 3*(pr + 15 + dr)));
            if (i != 5) {
                window->draw(sprite);
            } else if ((fr % (6*3)) < (3*3)) {
                sprite.setPosition(Vector2f(3*(8*4), 3*(8*20)));
                window->draw(sprite);
                sprite.setPosition(Vector2f(3*(8*10), 3*(8*26)));
                window->draw(sprite);
            }
        }
    } else if (fr < 24*8+4) {
        for (int i = 0; i < 6; i++) {
            Sprite sprite;
            sprite.setTexture(assets);

            // Set scaling factor
            sprite.setScale(3, 3);
            switch (i) {
                case 0:
                    x = 503;
                    y = 337;
                    break;
                case 1:
                    x = 201;
                    y = 540;
                    break;
                case 2:
                    x = 201;
                    y = 540;
                    break;
                case 3:
                    x = 201;
                    y = 540;
                    break;
                case 4:
                    x = 201;
                    y = 540;
                    break;
                case 5:
                    x = 736;
                    y = 214;
                    break;
            }

            if (i == 0) {
                sprite.rotate(180.0f);
                if ((fr % 8 > 1) && (fr % 8 < 4)) {
                    y+=17;
                } else if ((fr % 8 > 3) && (fr % 8 < 6)) {
                    y-=17;
                }
                IntRect spriteRect(x, y, 16, 16);
                sprite.setTextureRect(spriteRect);
                sprite.setPosition(Vector2f(3*(pc - fr + 15 + dc), 3*(pr + 15 + dr)));
            } else if (i != 5) {
                dc = 16*i + 2;
                if (fr % 8 < 4) {x+=17;}
                IntRect spriteRect(x, y, 16, 16);
                sprite.setTextureRect(spriteRect);
                sprite.setPosition(Vector2f(3*(pc + fr/2 - (24*8 + 24*4) + dc), 3*pr));
            } else {
                IntRect spriteRect(x, y, 8, 8);
                sprite.setTextureRect(spriteRect);
            }

            if (i != 5) {
                window->draw(sprite);
            } else if ((fr % (6*3)) < (3*3)) {
                sprite.setPosition(Vector2f(3*(8*10), 3*(8*26)));
                window->draw(sprite);
            }
        }
    } else if (fr < 80*8) {
        int df = fr;
        int lagBehind = 0;
        int dFreeze = 6*8;
        int fr1 = 28*8;
        int fr2 = 32*8 + dFreeze;
        int fr3 = 36*8 + dFreeze + dFreeze;
        int fr4 = 40*8 + dFreeze + dFreeze + dFreeze;
        for (int i = 0; i < 6; i++) {
            Sprite sprite;
            sprite.setTexture(assets);

            // Set scaling factor
            sprite.setScale(3, 3);
            switch (i) {
                case 0:
                    x = 503;
                    y = 337;
                    break;
                case 1:
                    x = 201;
                    y = 540;
                    break;
                case 2:
                    x = 201;
                    y = 540;
                    break;
                case 3:
                    x = 201;
                    y = 540;
                    break;
                case 4:
                    x = 201;
                    y = 540;
                    break;
                case 5:
                    x = 736;
                    y = 214;
                    break;
            }

            if ((fr1 <= fr) && (fr < fr1+dFreeze)) {
                df = fr1;
                if (i == 0) {x = 486; y = 337;}
                if (i == 1) {x = 435; y = 354;}
            } else if ((fr2 <= fr) && (fr < fr2+dFreeze)) {
                df = fr2;
                if (i == 0) {x = 486; y = 337;}
                if (i == 2) {x = 452; y = 354;}
            } else if ((fr3 <= fr) && (fr < fr3+dFreeze)) {
                df = fr3;
                if (i == 0) {x = 486; y = 337;}
                if (i == 3) {x = 469; y = 354;}
            } else if ((fr4 <= fr) && (fr < fr4+dFreeze)) {
                df = fr4;
                if (i == 0) {x = 486; y = 337;}
                if (i == 4) {x = 486; y = 354;}
            } else {
                if ((fr % 8 < 4) && (i != 0) && (i != 5)) {x+=17;}
            }
            
            if (fr >= fr1+dFreeze) {
                lagBehind = dFreeze;
                if (i == 1) {x = 486; y = 337;}
            }
            if (fr >= fr2+dFreeze) {
                lagBehind = 2*dFreeze;
                if (i == 2) {x = 486; y = 337;}
            }
            if (fr >= fr3+dFreeze) {
                lagBehind = 3*dFreeze;
                if (i == 3) {x = 486; y = 337;}
            }
            if (fr >= fr4+dFreeze) {
                lagBehind = 4*dFreeze;
                if (i == 4) {x = 486; y = 337;}
            }

            if (i == 0) {
                if ((fr % 8 > 1) && (fr % 8 < 4) && ((x != 486) || (y != 337))) {
                    y+=17;
                } else if ((fr % 8 > 3) && (fr % 8 < 6) && ((x != 486) || (y != 337))) {
                    y-=17;
                }
                IntRect spriteRect(x, y, 16, 16);
                sprite.setTextureRect(spriteRect);
                sprite.setPosition(Vector2f(3*(pc + (df-lagBehind) - 24*16 - 4), 3*pr));
            } else if (i != 5) {
                dc = 16*i + 2;
                IntRect spriteRect(x, y, 16, 16);
                sprite.setTextureRect(spriteRect);
                sprite.setPosition(Vector2f(3*(pc + (df-lagBehind) / 2 - (24*8 + 24*4) + dc), 3*pr));
            } else {
                IntRect spriteRect(x, y, 8, 8);
                sprite.setTextureRect(spriteRect);
            }

            if (i != 5) {
                window->draw(sprite);
            } else if ((fr % (6*3)) < (3*3)) {
                sprite.setPosition(Vector2f(3*(8*10), 3*(8*26)));
                window->draw(sprite);
            }
        }
    } else {
        for (int i = 5; i < 6; i++) {
            Sprite sprite;
            sprite.setTexture(assets);

            // Set scaling factor
            sprite.setScale(3, 3);
            x = 736;
            y = 214;
            
            IntRect spriteRect(x, y, 8, 8);
            sprite.setTextureRect(spriteRect);

            if ((fr % (6*3)) < (3*3)) {
                sprite.setPosition(Vector2f(3*(8*10), 3*(8*26)));
                window->draw(sprite);
            }
        }
    }
}

void GameData::debug(RenderWindow* window) {
    window->clear();
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            // Define the texture rectangle for the sprite
            Color color;
            CircleShape cir(2);
            switch (blocks[r][c].getType()) {
                case 0:
                    color = Color::Cyan;
                    break;
                case 1:
                    color = Color::Black;
                    break;
                case 2:
                    color = Color::Yellow;
                    break;
                case 3: 
                    color = Color::White;
                    break;
                case 4: 
                    color = Color::Blue;
                    break;
                case 7: 
                    color = Color::Green;
                    break;
                default: 
                    color = Color::Red;
                    break;
            }

            cir.setFillColor(color);
            // Position the sprite on the screen
            cir.setPosition(Vector2f(blocks[r][c].getX() + 3, blocks[r][c].getY() + 3));

            // Set scaling factor
            cir.setScale(this->scalingFactor, this->scalingFactor);
            window->draw(cir);
        }
    }
}

void GameData::addWall(int r, int c, int texture) {
    if (texture != 30) {
        this->changeBlock(r, c, 1);
    }
    this->blocks[r][c].setTexture(texture);
}

void GameData::addWalls() {
    int texture;
//Texture 0: Left double wall
    texture = 0;
    //Top upper left
    for (int i = 4; i < 12; i++) {
        this->addWall(i, 0, texture);
    }
    //Upper left rectangle
    for (int i = 13; i < 16; i++) {
        this->addWall(i, 5, texture);
    }
    //Lower left rectangle
    for (int i = 19; i < 22; i++) {
        this->addWall(i, 5, texture);
    }
    //Lower left higher wall
    for (int i = 23; i < 27; i++) {
        this->addWall(i, 0, texture);
    }
    //Lower left lower wall
    for (int i = 29; i < 33; i++) {
        this->addWall(i, 0, texture);
    }
    //Ghost cage left wall
    for (int i = 16; i < 19; i++) {
        this->addWall(i, 17, texture);
    }

//Texture 1: Right double wall
    texture = 1;
    //Top upper right
    for (int i = 4; i < 12; i++) {
        this->addWall(i, 27, texture);
    }
    //Upper right rectangle
    for (int i = 13; i < 16; i++) {
        this->addWall(i, 22, texture);
    }
    //Lower right rectangle
    for (int i = 19; i < 22; i++) {
        this->addWall(i, 22, texture);
    }
    //Lower right higher wall
    for (int i = 23; i < 27; i++) {
        this->addWall(i, 27, texture);
    }
    //Lower right lower wall
    for (int i = 29; i < 33; i++) {
        this->addWall(i, 27, texture);
    }
    //Ghost cage right wall
    for (int i = 16; i < 19; i++) {
        this->addWall(i, 10, texture);
    }

//Texture 2: Top double wall
    texture = 2;
    //Top screen left
    for (int i = 1; i < 13; i++) {
        this->addWall(3, i, texture);
    }
    //Top screen right
    for (int i = 15; i < 27; i++) {
        this->addWall(3, i, texture);
    }
    //Upper left rectangle
    for (int i = 0; i < 5; i++) {
        this->addWall(16, i, texture);
    }
    //Lower left rectangle
    for (int i = 1; i < 5; i++) {
        this->addWall(22, i, texture);
    }
    //Upper right rectangle
    for (int i = 23; i < 28; i++) {
        this->addWall(16, i, texture);
    }
    //Lower right rectangle
    for (int i = 23; i < 27; i++) {
        this->addWall(22, i, texture);
    }
    //Ghost cage bottom wall
    for (int i = 11; i < 17; i++) {
        this->addWall(19, i, texture);
    }
    
//Texture 3: Bottom double wall
    texture = 3;
    //Bottom screen
    for (int i = 1; i < 27; i++) {
        this->addWall(33, i, texture);
    }
    //Upper left rectangle
    for (int i = 1; i < 5; i++) {
        this->addWall(12, i, texture);
    }
    //Lower left rectangle
    for (int i = 0; i < 5; i++) {
        this->addWall(18, i, texture);
    }
    //Upper right rectangle
    for (int i = 23; i < 27; i++) {
        this->addWall(12, i, texture);
    }
    //Lower right rectangle
    for (int i = 23; i < 28; i++) {
        this->addWall(18, i, texture);
    }
    //Ghost cage top wall left
    for (int i = 11; i < 13; i++) {
        this->addWall(15, i, texture);
    }
    //Ghost cage top wall right
    for (int i = 15; i < 17; i++) {
        this->addWall(15, i, texture);
    }

//Texture 4: Double bottom left corner
    texture = 4;
    this->addWall(12, 0, texture);
    this->addWall(33, 0, texture);
//Texture 5: Double bottom right corner
    texture = 5;
    this->addWall(12, 27, texture);
    this->addWall(33, 27, texture);
//Texture 20 (also): Double bottom left small corner
    texture = 20;
    this->addWall(12, 5, texture);
    this->addWall(18, 5, texture);
//Texture 7: Double bottom right small corner
    texture = 7;
    this->addWall(12, 22, texture);
    this->addWall(18, 22, texture);
//Texture 8: Double top left corner
    texture = 8;
    this->addWall(3, 0, texture);
    this->addWall(22, 0, texture);
//Texture 9: Double top right corner
    texture = 9;
    this->addWall(3, 27, texture);
    this->addWall(22, 27, texture);
//Texture 24 (also): Double top left small corner
    texture = 24;
    this->addWall(16, 5, texture);
    this->addWall(22, 5, texture);
//Texture 11: Double top right small corner
    texture = 11;
    this->addWall(16, 22, texture);
    this->addWall(22, 22, texture);
//Texture 12: Bottom left branch corner
    texture = 12;
    this->addWall(27, 0, texture);
//Texture 13: Bottom right branch corner
    texture = 13;
    this->addWall(27, 27, texture);
//Texture 14: Top left branch corner (left wall)
    texture = 14;
    this->addWall(28, 0, texture);
//Texture 6: Top left branch corner (top wall)
    texture = 6;
    this->addWall(3, 14, texture);
//Texture 10: Top right branch corner (right wall)
    texture = 10;
    this->addWall(28, 27, texture);
//Texture 15: Top right branch corner (top wall)
    texture = 15;
    this->addWall(3, 13, texture);

//Texture 62: Single horizontal wall bottom
    texture = 62;
    //Top left bumper
    this->addWall(7, 3, texture);
    this->addWall(7, 4, texture);
    //Top center left bumper
    this->addWall(7, 8, texture);
    this->addWall(7, 9, texture);
    this->addWall(7, 10, texture);
    //Top lower left bumper
    this->addWall(10, 3, texture);
    this->addWall(10, 4, texture);
    //Top right bumper
    this->addWall(7, 23, texture);
    this->addWall(7, 24, texture);
    //Top center right bumper
    this->addWall(7, 17, texture);
    this->addWall(7, 18, texture);
    this->addWall(7, 19, texture);
    //Top lower right bumper
    this->addWall(10, 23, texture);
    this->addWall(10, 24, texture);
    //Top center bumper
    for (int i = 11; i < 13; i++) {
        this->addWall(10, i, texture);
    }
    for (int i = 15; i < 17; i++) {
        this->addWall(10, i, texture);
    }
    //Northern left bumper
    this->addWall(13, 9, texture);
    this->addWall(13, 10, texture);
    //Northern right bumper
    this->addWall(13, 17, texture);
    this->addWall(13, 18, texture);
    //Southern central bumper
    for (int i = 11; i < 13; i++) {
        this->addWall(22, i, texture);
    }
    for (int i = 15; i < 17; i++) {
        this->addWall(22, i, texture);
    }
    //Southern left bumper
    for (int i = 8; i < 11; i++) {
        this->addWall(25, i, texture);
    }
    //Southern far left bumper
    for (int i = 3; i < 4; i++) {
        this->addWall(25, i, texture);
    }
    //Southern right bumper
    for (int i = 17; i < 20; i++) {
        this->addWall(25, i, texture);
    }
    //Southern far right bumper
    for (int i = 24; i < 25; i++) {
        this->addWall(25, i, texture);
    }
    //Bottom center bumper
    for (int i = 11; i < 13; i++) {
        this->addWall(28, i, texture);
    }
    for (int i = 15; i < 17; i++) {
        this->addWall(28, i, texture);
    }
    //Bottom left bumper
    for (int i = 3; i < 11; i++) {
        this->addWall(31, i, texture);
    }
    //Bottom right bumper
    for (int i = 17; i < 25; i++) {
        this->addWall(31, i, texture);
    }
    //Left wall bump
    this->addWall(28, 1, texture);
    //Right wall bump
    this->addWall(28, 26, texture);

//Texture 16: Single horizontal wall top
    texture = 16;
    //Top left bumper
    this->addWall(5, 3, texture);
    this->addWall(5, 4, texture);
    //Top center left bumper
    this->addWall(5, 8, texture);
    this->addWall(5, 9, texture);
    this->addWall(5, 10, texture);
    //Top lower left bumper
    this->addWall(9, 3, texture);
    this->addWall(9, 4, texture);
    //Top right bumper
    this->addWall(5, 23, texture);
    this->addWall(5, 24, texture);
    //Top center right bumper
    this->addWall(5, 17, texture);
    this->addWall(5, 18, texture);
    this->addWall(5, 19, texture);
    //Top lower right bumper
    this->addWall(9, 23, texture);
    this->addWall(9, 24, texture);
    //Top center bumper
    for (int i = 11; i < 17; i++) {
        this->addWall(9, i, texture);
    }
    //Northern left bumper
    this->addWall(12, 9, texture);
    this->addWall(12, 10, texture);
    //Northern right bumper
    this->addWall(12, 17, texture);
    this->addWall(12, 18, texture);
    //Southern central bumper
    for (int i = 11; i < 17; i++) {
        this->addWall(21, i, texture);
    }
    //Southern left bumper
    for (int i = 8; i < 11; i++) {
        this->addWall(24, i, texture);
    }
    //Southern far left bumper
    for (int i = 3; i < 5; i++) {
        this->addWall(24, i, texture);
    }
    //Southern right bumper
    for (int i = 17; i < 20; i++) {
        this->addWall(24, i, texture);
    }
    //Southern far right bumper
    for (int i = 23; i < 25; i++) {
        this->addWall(24, i, texture);
    }
    //Bottom center bumper
    for (int i = 11; i < 17; i++) {
        this->addWall(27, i, texture);
    }
    //Bottom left bumper
    for (int i = 3; i < 7; i++) {
        this->addWall(30, i, texture);
    }
    for (int i = 9; i < 11; i++) {
        this->addWall(30, i, texture);
    }
    //Bottom right bumper
    for (int i = 17; i < 19; i++) {
        this->addWall(30, i, texture);
    }
    for (int i = 21; i < 25; i++) {
        this->addWall(30, i, texture);
    }
    //Left wall bump
    this->addWall(27, 1, texture);
    //Right wall bump
    this->addWall(27, 26, texture);


//Texture 61: Single vertical wall right
    texture = 61;
    //Top center bump
    for (int i = 4; i < 7; i++) {
         this->addWall(i, 14, texture);
    }
    //Top far left bumper
    this->addWall(6, 5, texture);
    //Top left bumper
    this->addWall(6, 11, texture);
    //Top far right bumper
    this->addWall(6, 25, texture);
    //Top right bumper
    this->addWall(6, 20, texture);
    //Upper center bumper
    for (int i = 11; i < 13; i++) {
         this->addWall(i, 14, texture);
    }
    //Upper left bumper
    for (int i = 10; i < 12; i++) {
         this->addWall(i, 8, texture);
    }
    for (int i = 14; i < 16; i++) {
         this->addWall(i, 8, texture);
    }
    //Upper right bumper
    for (int i = 10; i < 16; i++) {
        this->addWall(i, 20, texture);
    }
    //Lower center bumper
    for (int i = 23; i < 25; i++) {
         this->addWall(i, 14, texture);
    }
    //Lower left bumper
    for (int i = 19; i < 22; i++) {
        this->addWall(i, 8, texture);
    }
    //Lower right bumper
    for (int i = 19; i < 22; i++) {
        this->addWall(i, 20, texture);
    }
    //Lower far left bumper
    for (int i = 25; i < 28; i++) {
         this->addWall(i, 5, texture);
    }
    //Lower far right bumper
    for (int i = 26; i < 28; i++) {
         this->addWall(i, 23, texture);
    }
    //Bottom center bumper
    for (int i = 29; i < 31; i++) {
         this->addWall(i, 14, texture);
    }
    //Bottom left bumper
    for (int i = 28; i < 30; i++) {
         this->addWall(i, 8, texture);
    }
    //Bottom right bumper
    for (int i = 28; i < 30; i++) {
         this->addWall(i, 20, texture);
    }

//Texture 17: Single vertical wall left
    texture = 17;
    //Top center bump
    for (int i = 4; i < 7; i++) {
        this->addWall(i, 13, texture);
    }
    //Top far left bumper
    this->addWall(6, 2, texture);
    //Top left bumper
    this->addWall(6, 7, texture);
    //Top far right bumper
    this->addWall(6, 22, texture);
    //Top right bumper
    this->addWall(6, 16, texture);
    //Upper center bumper
    for (int i = 11; i < 13; i++) {
        this->addWall(i, 13, texture);
    }
    //Upper left bumper
    for (int i = 10; i < 16; i++) {
        this->addWall(i, 7, texture);
    }
    //Upper right bumper
    for (int i = 10; i < 12; i++) {
         this->addWall(i, 19, texture);
    }
    for (int i = 14; i < 16; i++) {
         this->addWall(i, 19, texture);
    }
    //Lower center bumper
    for (int i = 23; i < 25; i++) {
        this->addWall(i, 13, texture);
    }
    //Lower left bumper
    for (int i = 19; i < 22; i++) {
        this->addWall(i, 7, texture);
    }
    //Lower right bumper
    for (int i = 19; i < 22; i++) {
        this->addWall(i, 19, texture);
    }
    //Lower far left bumper
    for (int i = 26; i < 28; i++) {
        this->addWall(i, 4, texture);
    }
    //Lower far right bumper
    for (int i = 25; i < 28; i++) {
        this->addWall(i, 22, texture);
    }
    //Bottom center bumper
    for (int i = 29; i < 31; i++) {
        this->addWall(i, 13, texture);
    }
    //Bottom left bumper
    for (int i = 28; i < 30; i++) {
        this->addWall(i, 7, texture);
    }
    //Bottom right bumper
    for (int i = 28; i < 30; i++) {
        this->addWall(i, 19, texture);
    }

//Texture 18: Single bottom left corner
    texture = 18;
    //Bottom left and right bumpers
    this->addWall(30, 8, texture);
    this->addWall(30, 20, texture);
    //Central T-bumpers
    //Far lower paddles
    //Far lower side paddles
    //Side bumps
    //Lower paddles
    //Upper sideways T-bumpers
    this->addWall(12, 8, texture);
    //Upper extreme bumpers
    //Top bump
    //Top central bumpers
    //Top extreme bumpers

//Texture 19: Single bottom right corner
    texture = 19;
    //Bottom left and right bumpers
    this->addWall(30, 7, texture);
    this->addWall(30, 19, texture);
    //Central T-bumpers
    //Far lower paddles
    //Far lower side paddles
    //Side bumps
    //Lower paddles
    //Upper sideways T-bumpers
    this->addWall(12, 19, texture);
    //Upper extreme bumpers
    //Top bump
    //Top central bumpers
    //Top extreme bumpers

//Texture 20: Single bottom left small corner
    texture = 20;
    //Bottom left and right bumpers
    this->addWall(30, 11, texture);
    this->addWall(27, 8, texture);
    this->addWall(30, 25, texture);
    this->addWall(27, 20, texture);
    //Central T-bumpers
    this->addWall(27, 17, texture);
    this->addWall(21, 17, texture);
    this->addWall(9, 17, texture);
    //Far lower paddles
    this->addWall(24, 11, texture);
    this->addWall(24, 20, texture);
    //Far lower side paddles
    this->addWall(24, 5, texture);
    this->addWall(24, 25, texture);
    //Side bumps
    this->addWall(27, 2, texture);
    //Lower paddles
    this->addWall(18, 8, texture);
    this->addWall(18, 20, texture);
    //Upper sideways T-bumpers
    this->addWall(9, 8, texture);
    this->addWall(9, 20, texture);
    this->addWall(12, 11, texture);
    //Upper extreme bumpers
    this->addWall(9, 5, texture);
    this->addWall(9, 25, texture);
    //Top bump
    //Top central bumpers
    this->addWall(5, 11, texture);
    this->addWall(5, 20, texture);
    //Top extreme bumpers
    this->addWall(5, 5, texture);
    this->addWall(5, 25, texture);

//Texture 21: Single bottom right small corner
    texture = 21;
    //Bottom left and right bumpers
    this->addWall(30, 2, texture);
    this->addWall(27, 7, texture);
    this->addWall(30, 16, texture);
    this->addWall(27, 19, texture);
    //Central T-bumpers
    this->addWall(27, 10, texture);
    this->addWall(21, 10, texture);
    this->addWall(9, 10, texture);
    //Far lower paddles
    this->addWall(24, 7, texture);
    this->addWall(24, 16, texture);
    //Far lower side paddles
    this->addWall(24, 2, texture);
    this->addWall(24, 22, texture);
    //Side bumps
    this->addWall(27, 25, texture);
    //Lower paddles
    this->addWall(18, 7, texture);
    this->addWall(18, 19, texture);
    //Upper sideways T-bumpers
    this->addWall(9, 7, texture);
    this->addWall(9, 19, texture);
    this->addWall(12, 16, texture);
    //Upper extreme bumpers
    this->addWall(9, 2, texture);
    this->addWall(9, 22, texture);
    //Top bump
    //Top central bumpers
    this->addWall(5, 7, texture);
    this->addWall(5, 16, texture);
    //Top extreme bumpers
    this->addWall(5, 2, texture);
    this->addWall(5, 22, texture);

//Texture 22: Single top left corner
    texture = 22;
    //Bottom left and right bumpers
    //Central T-bumpers
    this->addWall(28, 14, texture);
    this->addWall(22, 14, texture);
    this->addWall(10, 14, texture);
    //Far lower paddles
    //Far lower side paddles
    this->addWall(25, 23, texture);
    //Side bumps
    //Lower paddles
    //Upper sideways T-bumpers
    this->addWall(13, 8, texture);
    //Upper extreme bumpers
    //Top bump
    //Top central bumpers
    //Top extreme bumpers

//Texture 23: Single top right corner
    texture = 23;
    //Bottom left and right bumpers
    //Central T-bumpers
    this->addWall(28, 13, texture);
    this->addWall(22, 13, texture);
    this->addWall(10, 13, texture);
    //Far lower paddles
    //Far lower side paddles
    this->addWall(25, 4, texture);
    //Side bumps
    //Lower paddles
    //Upper sideways T-bumpers
    this->addWall(13, 19, texture);
    //Upper extreme bumpers
    //Top bump
    //Top central bumpers
    //Top extreme bumpers

//Texture 24: Single top left small corner  
    texture = 24;
    //Bottom left and right bumpers
    this->addWall(31, 11, texture);
    this->addWall(31, 25, texture);
    //Central T-bumpers
    this->addWall(31, 14, texture);
    this->addWall(28, 17, texture);
    this->addWall(25, 14, texture);
    this->addWall(22, 17, texture);
    this->addWall(13, 14, texture);
    this->addWall(10, 17, texture);
    //Far lower paddles
    this->addWall(25, 11, texture);
    this->addWall(25, 20, texture);
    //Far lower side paddles
    this->addWall(28, 5, texture);
    this->addWall(28, 23, texture);
    this->addWall(25, 25, texture);
    //Side bumps
    this->addWall(28, 2, texture);
    //Lower paddles
    this->addWall(22, 8, texture);
    this->addWall(22, 20, texture);
    //Upper sideways T-bumpers
    this->addWall(16, 8, texture);
    this->addWall(16, 20, texture);
    this->addWall(13, 11, texture);
    //Upper extreme bumpers
    this->addWall(10, 5, texture);
    this->addWall(10, 25, texture);
    //Top bump
    this->addWall(7, 14, texture);
    //Top central bumpers
    this->addWall(7, 11, texture);
    this->addWall(7, 20, texture);
    //Top extreme bumpers
    this->addWall(7, 5, texture);
    this->addWall(7, 25, texture);

//Texture 25: Single top right small corner
    texture = 25;
    //Bottom left and right bumpers
    this->addWall(31, 2, texture);
    this->addWall(31, 16, texture);
    //Central T-bumpers
    this->addWall(31, 13, texture);
    this->addWall(28, 10, texture);
    this->addWall(25, 13, texture);
    this->addWall(22, 10, texture);
    this->addWall(13, 13, texture);
    this->addWall(10, 10, texture);
    //Far lower paddles
    this->addWall(25, 7, texture);
    this->addWall(25, 16, texture);
    //Far lower side paddles
    this->addWall(28, 4, texture);
    this->addWall(25, 2, texture);
    this->addWall(28, 22, texture);
    //Side bumps
    this->addWall(28, 25, texture);
    //Lower paddles
    this->addWall(22, 7, texture);
    this->addWall(22, 19, texture);
    //Upper sideways T-bumpers
    this->addWall(16, 7, texture);
    this->addWall(16, 19, texture);
    this->addWall(13, 16, texture);
    //Upper extreme bumpers
    this->addWall(10, 2, texture);
    this->addWall(10, 22, texture);
    //Top bump
    this->addWall(7, 13, texture);
    //Top central bumpers
    this->addWall(7, 7, texture);
    this->addWall(7, 16, texture);
    //Top extreme bumpers
    this->addWall(7, 2, texture);
    this->addWall(7, 22, texture);

//Texture 26: Top left square corner
    texture = 26;
    this->addWall(19, 17, texture);

//Texture 27: Top right square corner
    texture = 27;
    this->addWall(19, 10, texture);

//Texture 28: Bottom left square corner
    texture = 28;
    this->addWall(15, 17, texture);

//Texture 29: Bottom right square corner
    texture = 29;
    this->addWall(15, 10, texture);

//Texture 30: Ghost Barrier
    texture = 30;
    this->addWall(15, 13, texture);
    this->addWall(15, 14, texture);

}

void GameData::setElroy() {
    if (hasDied && clydeState == 5) {return;}
    if (numDots < 120) {this->isElroyI = false; this->isElroyII = false; return;}
    if (isElroyII) {return;}
    int l = levelNum;
    int d = 240 - numDots;
    if (l == 1) {
        if (d <= 20) {
            this->isElroyI = true;
        }
        if (d <= 10) {
            this->isElroyII = true;
        }
        return;
    }
    if (l == 2) {
        if (d <= 30) {
            this->isElroyI = true;
        }
        if (d <= 15) {
            this->isElroyII = true;
        }
        return;
    }
    if ((l > 2) && (l < 6)) {
        if (d <= 40) {
            this->isElroyI = true;
        }
        if (d <= 20) {
            this->isElroyII = true;
        }
        return;
    }
    if ((l > 5) && (l < 9)) {
        if (d <= 50) {
            this->isElroyI = true;
        }
        if (d <= 25) {
            this->isElroyII = true;
        }
        return;
    }
    if ((l > 8) && (l < 12)) {
        if (d <= 60) {
            this->isElroyI = true;
        }
        if (d <= 30) {
            this->isElroyII = true;
        }
        return;
    }
    if ((l > 11) && (l < 15)) {
        if (d <= 80) {
            this->isElroyI = true;
        }
        if (d <= 40) {
            this->isElroyII = true;
        }
        return;
    }
    if ((l > 14) && (l < 19)) {
        if (d <= 100) {
            this->isElroyI = true;
        }
        if (d <= 50) {
            this->isElroyII = true;
        }
        return;
    }
    if (d <= 120) {
        this->isElroyI = true;
    }
    if (d <= 60) {
        this->isElroyII = true;
    }
    return;
}

void GameData::readHighScore() {
    std::ifstream infile("HighScores.txt");
    if (!infile) {
        this->highScore = 0;
        return;
    }

    infile >> this->highScore;
    infile.close();
}

void GameData::writeHighScore() {
    int NUMDIGITS = 6;

    std::ofstream outfile("HighScores.txt");
    if (outfile) {
        outfile << std::setfill('0') << std::setw(NUMDIGITS) << this->score;
        outfile.close();
    } else {
        std::cout << "Failed to write high score to file" << std::endl;
    }
}

GameData::~GameData() {
        // Release memory for each row of the 2D array
        for (int r = 0; r < rows; r++) {
            delete[] blocks[r];
        }

        // Release memory for the array of pointers
        delete[] blocks;
}

