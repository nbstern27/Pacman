#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "player.h"
#include "ghost.h"

using namespace sf;

//mingw32-make


const int PIXELSPERBLOCK = 8;
const int SCALINGFACTOR = 3;

const int SCREENWIDTH = 224 * SCALINGFACTOR;
const int SCREENHEIGHT = 288 * SCALINGFACTOR;

const int BLOCKSX = 28; // Original packman value
const int BLOCKSY = 36; // Original packman value
const int BLOCKWIDTH = PIXELSPERBLOCK * SCALINGFACTOR;
const int BLOCKHEIGHT = PIXELSPERBLOCK * SCALINGFACTOR;

const float LOGICRATE = 480.0f; // Rate at which logic updates
const float FPS = 240.0f; // Frame rate
const float BASESPEED = 80.0f; // Units of pixels per second, pacman base speed
const float BASESPEEDGHOST = 80.0f; // Units of pixels per second, ghost base speed

const float TIMEPERLOGICUPDATE = 1.0f / LOGICRATE;
const float TIMEPERFRAME = 1.0f / FPS;

RenderWindow window(VideoMode(SCREENWIDTH, SCREENHEIGHT), "Pac-Man");
Clock clockCounter;
float deltaTime = 0;
float accumulatedTimeGraphics = 0;
float accumulatedTimeMove = 0;
float accumulatedTimeMoveInky = 0;
float accumulatedTimeMoveBlinky = 0;
float accumulatedTimeMovePinky = 0;
float accumulatedTimeMoveClyde = 0;
float accumulatedTimeLogic = 0;
float accumulatedTimeTotal = 0;
float fruitTimer = 0;
float eatenFruitTimer = 0;

int secondCount = 0; // Goes from 0 to FPS number
int powerUpTimer = 0;
int frightTime = 6; // Starting frightened time of ghosts in seconds
int ghostCombo = 200;

const int STARTR = 26; // Starting row of player
const int STARTC = 14; // Starting column of player

const int R0 = 17, R1 = 14, R2 = 17, R3 = 17, C0 = 11, C1 = 13, C2 = 13, C3 = 15; // Starting locations of ghosts
bool extraLife = false;

int playGame();
void startScreenAnimation(GameData* game, Player* player, Ghost** ghosts, bool first, bool newLevel);
void boardFlashAnimation(GameData* game, Player* player, Ghost** ghosts);
void ghostStateChange(GameData* game, Ghost* I, Ghost* B, Ghost* P, Ghost* C);
float getSpeed(GameData* game, int id, int c, int r, bool vulnerable);
int getFrightenedTime(GameData* game);
bool moveEntities(Player* p, Ghost* I, Ghost* B, Ghost* P, Ghost* C, GameData* g);
void loadSounds();

// Lookup table for scatter and chase times in seconds
float ghostTimes[3][7] = {
    {7, 20, 7, 20, 5, 20, 5},
    {7, 20, 7, 20, 5, 17*60 + 13 + (14 / FPS), 1 / FPS},
    {5, 20, 5, 20, 5, 17*60 + 13 + (14 / FPS), 1 / FPS}
};
int ghostTimeIndex = 0;

SoundBuffer soundDeath;
SoundBuffer soundEatFruit;
SoundBuffer soundEatGhost;
SoundBuffer soundEatPellet;
SoundBuffer soundExtraLife;
SoundBuffer soundIntermission;
SoundBuffer soundIntroMusic;
SoundBuffer soundPowerPellet;
SoundBuffer soundGhostRetreat;
SoundBuffer soundGhostMove;
SoundBuffer siren2;
SoundBuffer siren3;
SoundBuffer siren4;
SoundBuffer siren5;
Sound deathSound;
Sound eatFruitSound;
Sound eatGhostSound;
Sound eatPelletSound;
Sound extraLifeSound;
Sound intermissionSound;
Sound introSound;
Sound powerPelletSound;
Sound ghostRetreatSound;
Sound ghostMoveSound;
Sound ghostMoveSound2;
Sound ghostMoveSound3;
Sound ghostMoveSound4;
Sound ghostMoveSound5;
Sound sounds[5] = {ghostMoveSound, ghostMoveSound2, ghostMoveSound3, ghostMoveSound4, ghostMoveSound5};
int soundIndex = 0;

SoundBuffer m1;
SoundBuffer m2;
Sound munch1;
Sound munch2;
bool currMunch = false;

int main() {
    if (sf::Joystick::isConnected(0)) {
        std::cout << "Joystick is Connected!" << std::endl;
    }
    
    while (true) {
        if (playGame() == -1) {
            return 1;
        }
    }
}

int playGame() {    
    GameData * game = new GameData(SCREENWIDTH/BLOCKWIDTH, SCREENHEIGHT/BLOCKHEIGHT, BLOCKWIDTH, BLOCKHEIGHT);
    Player * player = new Player(STARTR, STARTC, game);
    Ghost * inky = new Ghost(R0, C0, game, 0);
    Ghost * blinky = new Ghost(R1, C1, game, 1);
    Ghost * pinky = new Ghost(R2, C2, game, 2);
    Ghost * clyde = new Ghost(R3, C3, game, 3);
    Ghost* ghosts[4] = {inky, blinky, pinky, clyde};
    bool first = true;

    game->setScalingFactor(SCALINGFACTOR);

    loadSounds();

    deathSound.setBuffer(soundDeath);
    eatFruitSound.setBuffer(soundEatFruit);
    eatGhostSound.setBuffer(soundEatGhost);
    eatPelletSound.setBuffer(soundEatPellet);
    extraLifeSound.setBuffer(soundExtraLife);
    intermissionSound.setBuffer(soundIntermission);
    introSound.setBuffer(soundIntroMusic);
    sounds[0].setBuffer(soundGhostMove);
    sounds[1].setBuffer(siren2);
    sounds[2].setBuffer(siren3);
    sounds[3].setBuffer(siren4);
    sounds[4].setBuffer(siren5);
    ghostRetreatSound.setBuffer(soundGhostRetreat);
    powerPelletSound.setBuffer(soundPowerPellet);
    munch1.setBuffer(m1);
    munch2.setBuffer(m2);

    while (window.isOpen()) {

        // Play First Screen Before Game Starts
        if (first) {
            Event e;
            int phaseOfAnimation = 0;
            int frame = 0;
            float halfSecond = 0.5f;
            while (!Keyboard::isKeyPressed(Keyboard::Space)) {
                deltaTime = clockCounter.restart().asSeconds();
                accumulatedTimeGraphics += deltaTime;
                accumulatedTimeLogic += deltaTime;
                // Update Phase of Animation
                if ((accumulatedTimeLogic >= halfSecond) && (phaseOfAnimation < 23)) {
                    accumulatedTimeLogic = 0;
                    phaseOfAnimation++;
                }
                if ((accumulatedTimeLogic >= 1 / BASESPEED) && (phaseOfAnimation >= 23)) {
                    accumulatedTimeLogic = 0;
                    frame = (frame + 1) % (80*8*2);
                }
                // Update Display on Clock Cycles
                if (accumulatedTimeGraphics >= TIMEPERFRAME) {
                    window.clear();
                    accumulatedTimeGraphics = 0;
                    game->drawOpeningScreen(&window, player->playerAssets, phaseOfAnimation);
                    if (phaseOfAnimation >= 23) {
                        game->drawOpeningOverlay(&window, player->playerAssets, frame);
                    }
                    game->drawOverlay(&window, player->playerAssets, false);
                    window.display();
                } 

                while (window.pollEvent(e)) {
                    switch (e.type) {
                        case Event::Closed:
                            window.close();
                            return -1;
                    }
                }
            }
        }



        if (first) {startScreenAnimation(game, player, ghosts, true, false); first = false;}
        deltaTime = clockCounter.restart().asSeconds();
        int newRow, newCol;

        if ((Keyboard::isKeyPressed(Keyboard::W) || (Keyboard::isKeyPressed(Keyboard::Up))) || (Joystick::isButtonPressed(0, 0)) && ((player->px + 5) % 8 == 0)) {
            newCol = 8*(player->c);
            newRow = 8*(player->r - 1);

            newCol = ((8*game->getCols()+newCol)) % (game->getCols()*8) / 8;
            newRow = ((8*game->getRows()+newRow)) % (game->getRows()*8) / 8;
            if (!game->checkCollision(newRow, newCol, 4)) {
                player->direc[0] = 0;
                player->direc[1] = -1;
            }
        }
        if ((Keyboard::isKeyPressed(Keyboard::S) || (Keyboard::isKeyPressed(Keyboard::Down)) || (Joystick::isButtonPressed(0, 1))) && ((player->px + 5) % 8 == 0)) {
            newCol = 8*(player->c);
            newRow = 8*(player->r + 1);

            newCol = ((8*game->getCols()+newCol)) % (game->getCols()*8) / 8;
            newRow = ((8*game->getRows()+newRow)) % (game->getRows()*8) / 8;
            if (!game->checkCollision(newRow, newCol, 4)) {
                player->direc[0] = 0;
                player->direc[1] = 1;
            }
        }
        if ((Keyboard::isKeyPressed(Keyboard::D) || (Keyboard::isKeyPressed(Keyboard::Right)) || (Joystick::isButtonPressed(0, 3))) && ((player->py + 5) % 8 == 0)) {
            newCol = 8*(player->c + 1);
            newRow = 8*(player->r);

            newCol = ((8*game->getCols()+newCol)) % (game->getCols()*8) / 8;
            newRow = ((8*game->getRows()+newRow)) % (game->getRows()*8) / 8;
            if (!game->checkCollision(newRow, newCol, 4)) {
                player->direc[0] = 1;
                player->direc[1] = 0;
            }
        }
        if ((Keyboard::isKeyPressed(Keyboard::A) || (Keyboard::isKeyPressed(Keyboard::Left)) || (Joystick::isButtonPressed(0, 2))) && ((player->py + 5) % 8 == 0)) {
            newCol = 8*(player->c - 1);
            newRow = 8*(player->r);

            newCol = ((8*game->getCols()+newCol)) % (game->getCols()*8) / 8;
            newRow = ((8*game->getRows()+newRow)) % (game->getRows()*8) / 8;
            if (!game->checkCollision(newRow, newCol, 4)) {
                player->direc[0] = -1;
                player->direc[1] = 0;
            }
        }
        
        // Event handling
        Event event;
        while (window.pollEvent(event)) {
            switch (event.type) {
                case Event::Closed:
                    window.close();
                    return -1;
            }
        }

        // Switch ghost move sound after certain number of pellets eaten
        game->numDots == 240 ? soundIndex = 4 : soundIndex = game->numDots / 48;

        // Play ghost moving sounds
        if (inky->vulnerable && pinky->vulnerable && blinky->vulnerable && clyde->vulnerable) {
            if (ghostRetreatSound.getStatus() == Sound::Playing) {
                ghostRetreatSound.stop();
            }
            if (sounds[soundIndex].getStatus() == Sound::Playing) {
                sounds[soundIndex].stop();
            }
            if (powerPelletSound.getStatus() != Sound::Playing) {
                powerPelletSound.play();
            }
        } else if (inky->getState() == 3 || pinky->getState() == 3 || blinky->getState() == 3 || clyde->getState() == 3) {
            if (ghostRetreatSound.getStatus() != Sound::Playing) {
                ghostRetreatSound.play();
            }
            if (powerPelletSound.getStatus() == Sound::Playing) {
                powerPelletSound.stop();
            }
            if (sounds[soundIndex].getStatus() == Sound::Playing) {
                sounds[soundIndex].stop();
            }
        } else {
            if (ghostRetreatSound.getStatus() == Sound::Playing) {
                ghostRetreatSound.stop();
            }
            if (powerPelletSound.getStatus() == Sound::Playing) {
                powerPelletSound.stop();
            }
            if (sounds[soundIndex].getStatus() != Sound::Playing) {
                sounds[soundIndex].play();
            }
        }

        // Give player another live at 10,000 points
        if (game->score > 10000 && !extraLife) {
            game->lives++;
            extraLife = true;
            extraLifeSound.play();
        }

        accumulatedTimeGraphics += deltaTime;
        accumulatedTimeMove += deltaTime;
        accumulatedTimeMoveInky += deltaTime;
        accumulatedTimeMoveBlinky += deltaTime;
        accumulatedTimeMovePinky += deltaTime;
        accumulatedTimeMoveClyde += deltaTime;
        accumulatedTimeLogic += deltaTime;
        if (game->isFruit) {
            fruitTimer += deltaTime;
        }
        if (game->eatenFruit) {
            eatenFruitTimer += deltaTime;
        }
        if (!game->isPoweredUp) {
            accumulatedTimeTotal += deltaTime;
        }


        // Update Movement on Movement Cycles
        if (accumulatedTimeMove >= 1.0f / (getSpeed(game, 0, player->c, player->r, false)*BASESPEED)) {
            accumulatedTimeMove = 0;
            player->updateMovement(game);
        } 

        // Update Inky Movement on Inky Movement Cycles
        if (accumulatedTimeMoveInky >= 1.0f / (getSpeed(game, 1, inky->getC(), inky->getR(), inky->vulnerable)*BASESPEEDGHOST)) {
            accumulatedTimeMoveInky = 0;
            inky->updateMovement(game);
        } 

        // Update Pinky Movement on Inky Movement Cycles
        if (accumulatedTimeMovePinky >= 1.0f / (getSpeed(game, 1, pinky->getC(), pinky->getR(), pinky->vulnerable)*BASESPEEDGHOST)) {
            accumulatedTimeMovePinky = 0;
            pinky->updateMovement(game);
        } 

        // Update Inky Movement on Inky Movement Cycles
        if (accumulatedTimeMoveClyde >= 1.0f / (getSpeed(game, 1, clyde->getC(), clyde->getR(), clyde->vulnerable)*BASESPEEDGHOST)) {
            accumulatedTimeMoveClyde = 0;
            clyde->updateMovement(game);
        } 

        // Update Blinky Movement on Blinky Movement Cycles
        if (accumulatedTimeMoveBlinky >= 1.0f / (getSpeed(game, 1, blinky->getC(),  blinky->getR(), blinky->vulnerable)*BASESPEEDGHOST)) {
            accumulatedTimeMoveBlinky = 0;
            blinky->updateMovement(game);
        } 

        // Update Logic on Logic Cycles
        if (accumulatedTimeLogic >= TIMEPERLOGICUPDATE) {
            accumulatedTimeLogic = 0;
            game->lastDotTimer++;
            inky->pacC = player->c, inky->pacR = player->r; inky->pacDir = player->direc;
            blinky->pacC = player->c, blinky->pacR = player->r; blinky->pacDir = player->direc;
            pinky->pacC = player->c, pinky->pacR = player->r; pinky->pacDir = player->direc;
            clyde->pacC = player->c, clyde->pacR = player->r; clyde->pacDir = player->direc;
            game->blinkX = blinky->getC();
            game->blinkY = blinky->getR();
            game->clydeState = clyde->getState();
            if (game->levelNum < 5 && game->lastDotTimer >= 4 * LOGICRATE) {
                game->tempDots = 0;
                game->lastDotTimer = 0;
                if (pinky->getState() == 5) {
                    pinky->leaveHouse = true;
                } else if (inky->getState() == 5) {
                    inky->leaveHouse = true;
                } else if (clyde->getState() == 5) {
                    clyde->leaveHouse = true;
                } 
            } else if (game->levelNum >= 5 && game->lastDotTimer >= 3 * LOGICRATE) {
                game->tempDots = 0;
                game->lastDotTimer = 0;
                if (pinky->getState() == 5) {
                    pinky->leaveHouse = true;
                } else if (inky->getState() == 5) {
                    inky->leaveHouse = true;
                } else if (clyde->getState() == 5) {
                    clyde->leaveHouse = true;
                } 
            } else if (!game->dotsSwitch) {
                if (pinky->getState() == 5) {
                    pinky->dotCount = game->tempDots;
                } else if (inky->getState() == 5) {
                    inky->dotCount = game->tempDots;
                } else if (clyde->getState() == 5) {
                    clyde->dotCount = game->tempDots;
                } else {
                    game->tempDots = 0;
                }
            }

            // Spawn fruit check
            if (game->numDots == 70) {
                game->isFruit = true;
            }
            if (game->numDots == 170) {
                game->isFruit = true;
            }

            // Despawn fruit after 10 seconds
            if (fruitTimer >= 10) {
                game->isFruit = false;
                fruitTimer = 0;
            }

            // Eat fruit score display
            if (eatenFruitTimer >= 2) { 
                game->eatenFruit = false;
                eatenFruitTimer = 0;
            }

            // Level is over check
            if (game->numDots == 240) {
                boardFlashAnimation(game, player, ghosts);
                inky->reset(R0, C0, game);
                blinky->reset(R1, C1, game);
                pinky->reset(R2, C2, game);
                clyde->reset(R3, C3, game);
                game->resetBoard(true);
                player->respawn(STARTR, STARTC);
                ghostTimeIndex = 0;
                accumulatedTimeGraphics = 0;
                accumulatedTimeMove = 0;
                accumulatedTimeMoveInky = 0;
                accumulatedTimeMoveBlinky = 0;
                accumulatedTimeMovePinky = 0;
                accumulatedTimeMoveClyde = 0;
                accumulatedTimeLogic = 0;
                accumulatedTimeTotal = 0;
                fruitTimer = 0;
                eatenFruitTimer = 0;
                game->eatenFruit = false;
                game->isFruit = false;
                startScreenAnimation(game, player, ghosts, false, true);

                secondCount = 0;
                powerUpTimer = 0;
            }

            // Update movements
            if (moveEntities(player, inky, blinky, pinky, clyde, game)) {
                ghostTimeIndex = 0;
                accumulatedTimeGraphics = 0;
                accumulatedTimeMove = 0;
                accumulatedTimeMoveInky = 0;
                accumulatedTimeMoveBlinky = 0;
                accumulatedTimeMovePinky = 0;
                accumulatedTimeMoveClyde = 0;
                accumulatedTimeLogic = 0;
                accumulatedTimeTotal = 0;
                secondCount = 0;
                powerUpTimer = 0;
                game->eatenFruit = false;
                game->isFruit = false;
                fruitTimer = 0;
                eatenFruitTimer = 0;
                if (game->lives == 0) {
                    if (game->score > game->highScore) {
                        game->writeHighScore();
                    }

                    while (!Keyboard::isKeyPressed(Keyboard::Space)) {
                        deltaTime = clockCounter.restart().asSeconds();
                        accumulatedTimeGraphics += deltaTime;
                        // Update Display on Clock Cycles
                        if (accumulatedTimeGraphics >= TIMEPERFRAME) {
                            accumulatedTimeGraphics = 0;
                            game->updateDisplay(&window, true);
                            game->drawGameOver(&window, player->playerAssets);
                            game->drawOverlay(&window, player->playerAssets, true);
                            window.display();
                        } 

                        while (window.pollEvent(event)) {
                            switch (event.type) {
                                case Event::Closed:
                                    window.close();
                                    return -1;
                            }
                        }
                    }

                    return 0;
                }
                inky->reset(R0, C0, game);
                blinky->reset(R1, C1, game);
                pinky->reset(R2, C2, game);
                clyde->reset(R3, C3, game);
                game->resetBoard(false);
                player->respawn(STARTR, STARTC);
                startScreenAnimation(game, player, ghosts, false, false);
            } 
            // Powerup timer
            if (game->setPowerUpTimer) {
                ghostCombo = 200;
                inky->flash = false;
                blinky->flash = false;
                pinky->flash = false;
                clyde->flash = false;
                game->setPowerUpTimer = false;
                game->isPoweredUp = true;
                powerUpTimer = 0;
                frightTime = getFrightenedTime(game);
                if (inky->getState() == 5) {inky->vulnerable = true;} else if (inky->getState() != 3) {inky->setState(2);}
                if (blinky->getState() == 5) {blinky->vulnerable = true;} else if (blinky->getState() != 3) {blinky->setState(2);}
                if (pinky->getState() == 5) {pinky->vulnerable = true;} else if (pinky->getState() != 3) {pinky->setState(2);}
                if (clyde->getState() == 5) {clyde->vulnerable = true;} else if (clyde->getState() != 3) {clyde->setState(2);}
            }
            if (powerUpTimer >= frightTime * LOGICRATE) {
                game->isPoweredUp = false;
                powerUpTimer = 0;
                inky->vulnerable = false;
                blinky->vulnerable = false;
                pinky->vulnerable = false;
                clyde->vulnerable = false;
                ghostCombo = 200;
                if (inky->getState() == 2) {inky->setState(game->ghostState);}
                if (blinky->getState() == 2) {blinky->setState(game->ghostState);}
                if (pinky->getState() == 2) {pinky->setState(game->ghostState);}
                if (clyde->getState() == 2) {clyde->setState(game->ghostState);}
            }
            if (game->isPoweredUp) {
                // Flash ghosts since powerup is almost over
                if (frightTime == 1) {
                    if (powerUpTimer >= frightTime * LOGICRATE * (0.64f)) {
                        inky->flash = true;
                        blinky->flash = true;
                        pinky->flash = true;
                        clyde->flash = true;
                        if (powerUpTimer >= frightTime * LOGICRATE * (0.71f)) {
                            inky->flash = false;
                            blinky->flash = false;
                            pinky->flash = false;
                            clyde->flash = false;
                        }
                        if (powerUpTimer >= frightTime * LOGICRATE * (0.78)) {
                            inky->flash = true;
                            blinky->flash = true;
                            pinky->flash = true;
                            clyde->flash = true;
                        }
                        if (powerUpTimer >= frightTime * LOGICRATE * (0.85f)) {
                            inky->flash = false;
                            blinky->flash = false;
                            pinky->flash = false;
                            clyde->flash = false;
                        }
                        if (powerUpTimer >= frightTime * LOGICRATE * (0.92f)) {
                            inky->flash = true;
                            blinky->flash = true;
                            pinky->flash = true;
                            clyde->flash = true;
                        }
                    }
                } else {
                    if (powerUpTimer >= frightTime * LOGICRATE * (0.64f)) {
                        inky->flash = true;
                        blinky->flash = true;
                        pinky->flash = true;
                        clyde->flash = true;
                        if (powerUpTimer >= frightTime * LOGICRATE * (0.68f)) {
                            inky->flash = false;
                            blinky->flash = false;
                            pinky->flash = false;
                            clyde->flash = false;
                        }
                        if (powerUpTimer >= frightTime * LOGICRATE * (0.72f)) {
                            inky->flash = true;
                            blinky->flash = true;
                            pinky->flash = true;
                            clyde->flash = true;
                        }
                        if (powerUpTimer >= frightTime * LOGICRATE * (0.76f)) {
                            inky->flash = false;
                            blinky->flash = false;
                            pinky->flash = false;
                            clyde->flash = false;
                        }
                        if (powerUpTimer >= frightTime * LOGICRATE * (0.80f)) {
                            inky->flash = true;
                            blinky->flash = true;
                            pinky->flash = true;
                            clyde->flash = true;
                        }
                        if (powerUpTimer >= frightTime * LOGICRATE * (0.84f)) {
                            inky->flash = false;
                            blinky->flash = false;
                            pinky->flash = false;
                            clyde->flash = false;
                        }
                        if (powerUpTimer >= frightTime * LOGICRATE * (0.88f)) {
                            inky->flash = true;
                            blinky->flash = true;
                            pinky->flash = true;
                            clyde->flash = true;
                        }
                        if (powerUpTimer >= frightTime * LOGICRATE * (0.92f)) {
                            inky->flash = false;
                            blinky->flash = false;
                            pinky->flash = false;
                            clyde->flash = false;
                        }
                        if (powerUpTimer >= frightTime * LOGICRATE * (0.96f)) {
                            inky->flash = true;
                            blinky->flash = true;
                            pinky->flash = true;
                            clyde->flash = true;
                        }
                    }
                }
                powerUpTimer++;
            }

            // Ghost state handler
            ghostStateChange(game, inky, blinky, pinky, clyde);
        }  

        // Update Display on Clock Cycles
        if (accumulatedTimeGraphics >= TIMEPERFRAME) {
            accumulatedTimeGraphics = 0;
            if (secondCount < FPS / 6) {
                game->updateDisplay(&window, false);
            } else {
                game->updateDisplay(&window, true);
            }

            if (secondCount++ >= FPS / 3) {
                secondCount = 0;
            }
        
            player->drawPacman(&window);
            inky->drawGhost(&window, false);
            blinky->drawGhost(&window, false);
            pinky->drawGhost(&window, false);
            clyde->drawGhost(&window, false);
            game->drawOverlay(&window, player->playerAssets, true);

            //game->debug(&window);
            window.display();
        } 
    }

    return -1;
}

bool checkCollision(int pC, int pR, int* pDir, int gC, int gR, int* gDir) {
    int pStartC = pC;
    int pEndC = pC + pDir[0];
    if (pDir[0] < 0) {
        pStartC = pC + pDir[0];
        pEndC = pC;
    }
    int pStartR = pR;
    int pEndR = pR + pDir[1];
    if (pDir[1] < 0) {
        pStartR = pR + pDir[1];
        pEndR = pR;
    }
    int gStartC = gC;
    int gEndC = gC + gDir[0];
    if (gDir[0] < 0) {
        gStartC = gC + gDir[0];
        gEndC = gC;
    }
    int gStartR = gR;
    int gEndR = gR + gDir[1];
    if (gDir[1] < 0) {
        gStartR = gR + gDir[1];
        gEndR = gR;
    }
    return (pStartC <= gEndC && pEndC >= gStartC && pStartR <= gEndR && pEndR >= gStartR);
}

void startScreenAnimation(GameData* game, Player* player, Ghost** ghosts, bool first, bool newLevel) {
    introSound.play();
    accumulatedTimeGraphics = 0;
    int frame = 0;
    float n = 2.2;
    float m = 2;
    Event event;
    while (window.pollEvent(event)) {
        switch (event.type) {
            case Event::Closed:
                window.close();
        }
    }

    if (first) {
        // Start screen
        while (frame < n*FPS) {
            deltaTime = clockCounter.restart().asSeconds();
            accumulatedTimeGraphics += deltaTime;
            if (accumulatedTimeGraphics >= TIMEPERFRAME) {
                accumulatedTimeGraphics = 0;
                game->updateDisplay(&window, false);
                game->drawPlayerOne(&window, player->playerAssets);
                game->drawReady(&window, player->playerAssets);
                game->drawOverlay(&window, player->playerAssets, true);
                window.display();
                frame++;
            } 
        } 
    } else {
            m+=n;
    }

    // Spawn in
    if (!newLevel) {
        game->lives--;
    }
    accumulatedTimeGraphics = 0;
    frame = 0;
    while (frame < m*FPS) {
        deltaTime = clockCounter.restart().asSeconds();
        accumulatedTimeGraphics += deltaTime;
        if (accumulatedTimeGraphics >= TIMEPERFRAME) {
            accumulatedTimeGraphics = 0;
            game->updateDisplay(&window, false);
            game->drawReady(&window, player->playerAssets);
            player->drawPacman(&window);
            for (int i = 0; i < 4; i++) {
                ghosts[i]->drawGhost(&window, true);
            }
            game->drawOverlay(&window, player->playerAssets, true);
            window.display();
            frame++;
        } 
    }
}

void boardFlashAnimation(GameData* game, Player* player, Ghost** ghosts) {
    if (ghostRetreatSound.getStatus() == Sound::Playing) {
        ghostRetreatSound.stop();
    }
    if (powerPelletSound.getStatus() == Sound::Playing) {
        powerPelletSound.stop();
    }
    if (sounds[soundIndex].getStatus() == Sound::Playing) {
        sounds[soundIndex].stop();
    }
    if (munch1.getStatus() == Sound::Playing) {
        munch1.stop();
    }
    if (munch2.getStatus() == Sound::Playing) {
        munch2.stop();
    }
    if (extraLifeSound.getStatus() == Sound::Playing) {
        extraLifeSound.stop();
    }
    if (eatFruitSound.getStatus() == Sound::Playing) {
        eatFruitSound.stop();
    }
    if (eatGhostSound.getStatus() == Sound::Playing) {
        eatGhostSound.stop();
    }
    accumulatedTimeGraphics = 0;
    int frame = 0;
    // Freeze frame
    while (frame < 2*FPS/3) {
        deltaTime = clockCounter.restart().asSeconds();
        accumulatedTimeGraphics += deltaTime;
        if (accumulatedTimeGraphics >= TIMEPERFRAME) {
            accumulatedTimeGraphics = 0;
            game->updateDisplay(&window, false);
            player->drawPacman(&window);
            for (int i = 0; i < 4; i++) {
                ghosts[i]->drawGhost(&window, false);
            }
            game->drawOverlay(&window, player->playerAssets, true);
            window.display();
            frame++;
        } 
    }

    for (int i = 0; i < 4; i++) {
        accumulatedTimeGraphics = 0;
        frame = 0;
        // Flash background
        while (frame < FPS/4) {
            deltaTime = clockCounter.restart().asSeconds();
            accumulatedTimeGraphics += deltaTime;
            if (accumulatedTimeGraphics >= TIMEPERFRAME) {
                accumulatedTimeGraphics = 0;
                game->drawFlash(&window);
                game->drawOverlay(&window, player->playerAssets, true);
                window.display();
                frame++;
            } 
        }

        accumulatedTimeGraphics = 0;
        frame = 0;

        // Normal background
        while (frame < FPS/4) {
            deltaTime = clockCounter.restart().asSeconds();
            accumulatedTimeGraphics += deltaTime;
            if (accumulatedTimeGraphics >= TIMEPERFRAME) {
                accumulatedTimeGraphics = 0;
                game->updateDisplay(&window, true);
                game->drawOverlay(&window, player->playerAssets, true);
                window.display();
                frame++;
            } 
        }
    }
    return;
}

void playerDeathAnimation(GameData* game, Player* player, Ghost** ghosts) {
    accumulatedTimeGraphics = 0;
    int frame = 0;

    if (ghostRetreatSound.getStatus() == Sound::Playing) {
        ghostRetreatSound.stop();
    }
    if (powerPelletSound.getStatus() == Sound::Playing) {
        powerPelletSound.stop();
    }
    if (sounds[soundIndex].getStatus() == Sound::Playing) {
        sounds[soundIndex].stop();
    }

    // Freeze frame
    while (frame < 2*FPS/3) {
        deltaTime = clockCounter.restart().asSeconds();
        accumulatedTimeGraphics += deltaTime;
        if (accumulatedTimeGraphics >= TIMEPERFRAME) {
            accumulatedTimeGraphics = 0;
            if (secondCount < FPS / 6) {
                game->updateDisplay(&window, false);
            } else {
                game->updateDisplay(&window, true);
            }

            if (secondCount++ >= FPS / 3) {
                secondCount = 0;
            }

            player->drawPacman(&window);
            for (int i = 0; i < 4; i++) {
                ghosts[i]->drawGhost(&window, false);
            }
            game->drawOverlay(&window, player->playerAssets, true);
            window.display();
            frame++;
        } 
    }

    deathSound.play();
    // Animation of Pacman dying
    accumulatedTimeGraphics = 0;
    frame = 0;
    while (frame < FPS) {
        deltaTime = clockCounter.restart().asSeconds();
        accumulatedTimeGraphics += deltaTime;
        if (accumulatedTimeGraphics >= TIMEPERFRAME) {
            accumulatedTimeGraphics = 0;
            if (secondCount < FPS / 6) {
                game->updateDisplay(&window, false);
            } else {
                game->updateDisplay(&window, true);
            }

            if (secondCount++ >= FPS / 3) {
                secondCount = 0;
            }

            game->drawOverlay(&window, player->playerAssets, true);
            player->drawDeath(&window, frame);
            window.display();
            frame++;
        } 
    }

    accumulatedTimeGraphics = 0;
    frame = 0;
    // Freeze frame
    while (frame < FPS) {
        deltaTime = clockCounter.restart().asSeconds();
        accumulatedTimeGraphics += deltaTime;
        if (accumulatedTimeGraphics >= TIMEPERFRAME) {
            accumulatedTimeGraphics = 0;
            if (secondCount < FPS / 6) {
                game->updateDisplay(&window, false);
            } else {
                game->updateDisplay(&window, true);
            }

            if (secondCount++ >= FPS / 3) {
                secondCount = 0;
            }

            game->drawOverlay(&window, player->playerAssets, true);
            window.display();
            frame++;
        } 
    }
}

void eatGhostAnimation(GameData* game, Player* player) {
    accumulatedTimeGraphics = 0;
    int frame = 0;
    while (frame < 3*FPS/4) {
        deltaTime = clockCounter.restart().asSeconds();
        accumulatedTimeGraphics += deltaTime;
        if (accumulatedTimeGraphics >= TIMEPERFRAME) {
            accumulatedTimeGraphics = 0;
            if (secondCount < FPS / 6) {
                game->updateDisplay(&window, false);
            } else {
                game->updateDisplay(&window, true);
            }

            if (secondCount++ >= FPS / 3) {
                secondCount = 0;
            }

            game->drawOverlay(&window, player->playerAssets, true);
            player->drawGhostCombo(&window, ghostCombo);

            window.display();
            frame++;
        } 
    }
    ghostCombo*=2;
}

bool moveEntities(Player * p, Ghost * I, Ghost * B, Ghost * P, Ghost * C, GameData* g) {
    bool playerDeathFlag = false;
    int zeroDir[2] = {0, 0};
    Ghost* ghosts[4] = {I, B, P, C};
    // Check Pacman and Inky overlap
    if (checkCollision(p->c, p->r, g->qMovep, I->getC(), I->getR(), g->qMoveI)) {
        // Collision!
        if (I->vulnerable) {
            I->setState(3);
            g->score+=ghostCombo;
            eatGhostSound.play();
            eatGhostAnimation(g, p);
        } else if (I->getState() != 3){
            // Player dies
            playerDeathFlag = true;
        }  
    } 
    // Check Pacman and Binky overlap
    if (checkCollision(p->c, p->r, g->qMovep, B->getC(), B->getR(), g->qMoveB)) {
        // Collision!
        if (B->vulnerable) {
            B->setState(3);
            g->score+=ghostCombo;
            eatGhostSound.play();
            eatGhostAnimation(g, p);
        } else if (B->getState() != 3){
            // Player dies
            playerDeathFlag = true;
        }  
    }
    // Check Pacman and Pinky overlap
    if (checkCollision(p->c, p->r, g->qMovep, P->getC(), P->getR(), g->qMoveP)) {
        // Collision!
        if (P->vulnerable) {
            P->setState(3);
            g->score+=ghostCombo;
            eatGhostSound.play();
            eatGhostAnimation(g, p);
        } else if (P->getState() != 3){
            // Player dies
            playerDeathFlag = true;
        }  
    }
    // Check Pacman and Clyde overlap
    if (checkCollision(p->c, p->r, g->qMovep, C->getC(), C->getR(), g->qMoveC)) {
        // Collision!
        if (C->vulnerable) {
            C->setState(3);
            g->score+=ghostCombo;
            eatGhostSound.play();
            eatGhostAnimation(g, p);
        } else if (C->getState() != 3){
            // Player dies
            playerDeathFlag = true;
        }  
    }

    if ((!playerDeathFlag) && ((g->qMovep[0] != 0) || (g->qMovep[1] != 0))) {
        // Safe for Pacman to move

        // Get fruit
        if ((g->isFruit) && (checkCollision(p->c, p->r, g->qMovep, 13, 20, zeroDir) || checkCollision(p->c, p->r, g->qMovep, 14, 20, zeroDir))) {
            g->isFruit = false;
            g->eatenFruit = true;
            g->firstFrameEat = true;
            eatFruitSound.play();
            fruitTimer = 0;
        }

        if (g->getBlocks()[p->r+g->qMovep[1]][p->c+g->qMovep[0]].getType() == 3 || g->getBlocks()[p->r+g->qMovep[1]][p->c+g->qMovep[0]].getType() == 6) {
            if (munch1.getStatus() != Sound::Playing && munch2.getStatus() != Sound::Playing) {
                if (currMunch) {
                    munch2.play();
                    currMunch = false;
                } else {
                    munch1.play();
                    currMunch = true;
                }
            }
        } else {
            if (munch1.getStatus() != Sound::Playing && munch2.getStatus() != Sound::Playing) {
                if (!currMunch) {
                    munch1.play();
                    currMunch = true;
                }
            }
        }

        p->commenceMove(g, p->r+g->qMovep[1], p->c+g->qMovep[0]); g->qMovep[0] = 0; g->qMovep[1] = 0;
        return false;
    } else if (playerDeathFlag) {
        g->changeBlock(p->r, p->c, 0);
        playerDeathAnimation(g, p, ghosts);
    }

    I->commenceMove(g, I->getR()+g->qMoveI[1], I->getC()+g->qMoveI[0]); g->qMoveI[0] = 0; g->qMoveI[1] = 0;
    B->commenceMove(g, B->getR()+g->qMoveB[1], B->getC()+g->qMoveB[0]); g->qMoveB[0] = 0; g->qMoveB[1] = 0;
    P->commenceMove(g, P->getR()+g->qMoveP[1], P->getC()+g->qMoveP[0]); g->qMoveP[0] = 0; g->qMoveP[1] = 0;
    C->commenceMove(g, C->getR()+g->qMoveC[1], C->getC()+g->qMoveC[0]); g->qMoveC[0] = 0; g->qMoveC[1] = 0;

    return playerDeathFlag;
}
  
void ghostStateChange(GameData* game, Ghost* I, Ghost* B, Ghost* P, Ghost* C) {
    int l;
    if (game->levelNum == 1) {
        l = 0;
    } else if (game->levelNum < 5) {
        l = 1;
    } else {
        l = 2;
    }

    float reqTime = 0;
    bool maxTime = false;
    if (ghostTimeIndex < 7) {
        reqTime = ghostTimes[l][ghostTimeIndex];
    } else {
        maxTime = true;
    }

    if ((accumulatedTimeTotal > reqTime) && (!maxTime)) {
        // Switch from scatter to chase mode or chase to scatter mode
        accumulatedTimeTotal = 0;
        if (I->getState() < 2) {I->setState((I->getState() + 1)%2);}
        if (B->getState() < 2) {B->setState((B->getState() + 1)%2);}
        if (P->getState() < 2) {P->setState((P->getState() + 1)%2);}
        if (C->getState() < 2) {C->setState((C->getState() + 1)%2);}
        game->ghostState = (game->ghostState + 1) %2;
        if (ghostTimeIndex < 7) {
            ghostTimeIndex++;
        }
    }
}

float getSpeed(GameData* game, int id, int c, int r, bool vulnerable) {
    int l = game->levelNum;
    // Pacman
    if (id == 0) {
        if (game->isPoweredUp) {
            if (l < 5) {
                return 0.95f;
            } else {
                return 1.0f;
            }
        } else {
            if (l == 1) {
                return 0.8f;
            } else if ((l < 5) || (l > 20)) {
                return 0.9f;
            } else {
                return 1.0f;
            }
        }
    }

    // Ghosts in tunnel
    if (((c < 5) || (c > 22)) && (r == 17)) {
        if (l == 1) {
            return 0.4f;
        } else if (l < 5) {
            return 0.45f;
        } else {
            return 0.5f;
        }
    }

    // Ghosts leaving house
    if (((c == 13) || (c == 14)) && (r == 15)) {
        if (l == 1) {
            return 0.3f;
        } else if (l < 5) {
            return 0.35f;
        } else {
            return 0.4f;
        }
    }

    // Ghost vulnerable
    if (vulnerable) {
        if (l == 1) {
            return 0.5f;
        } else if (l < 5) {
            return 0.55f;
        } else {
            return 0.6f;
        }
    }

    // Blinky
    if (id == 2) {
        if (game->isElroyII) {
            if (l == 1) {
                return 0.85f;
            } else if (l < 5) {
                return 0.95f;
            } else {
                return 1.05f;
            }
        } else if (game->isElroyI) {
            if (l == 1) {
                return 0.8f;
            } else if (l < 5) {
                return 0.9f;
            } else {
                return 1.0f;
            }
        }
    }

    // Ghosts besides Blinky or not Elroy modes
    if (l == 1) {
        return 0.75f;
    } else if (l < 5) {
        return 0.85f;
    } else {
        return 0.95f;
    }
}

int getFrightenedTime(GameData* game) {
    int l = game->levelNum;
    if (l == 1) {
        return 6;
    }
    if ((l == 2) || (l == 6) || (l = 10)) {
        return 5;
    }
    if (l == 3) {
        return 4;
    }
    if ((l == 4) || (l == 14)) {
        return 3;
    }
    if ((l == 5) || (l == 7) || (l == 8) || (l = 11)) {
        return 2;
    }
    if ((l == 9) || (l == 12) || (l == 13) || (l = 15) || (l = 16) || (l = 18)) {
        return 1;
    }
    return 0;

}

void loadSounds() {
    if (!soundDeath.loadFromFile("soundDeath.wav"))
    {
        std::cout << "Error loading sound file" << std::endl;
    }
    if (!soundEatFruit.loadFromFile("soundEatFruit.wav"))
    {
        std::cout << "Error loading sound file" << std::endl;
    }
    if (!soundEatGhost.loadFromFile("soundEatGhost.wav"))
    {
        std::cout << "Error loading sound file" << std::endl;
    }
    if (!soundEatPellet.loadFromFile("soundEatPellet.wav"))
    {
        std::cout << "Error loading sound file" << std::endl;
    }
    if (!soundExtraLife.loadFromFile("soundExtraLife.wav"))
    {
        std::cout << "Error loading sound file" << std::endl;
    }
    if (!soundIntermission.loadFromFile("soundIntermission.wav"))
    {
        std::cout << "Error loading sound file" << std::endl;
    }
    if (!soundIntroMusic.loadFromFile("soundIntroMusic.wav"))
    {
        std::cout << "Error loading sound file" << std::endl;
    }
    if (!m1.loadFromFile("munch_1.wav"))
    {
        std::cout << "Error loading sound file" << std::endl;
    }
    if (!m2.loadFromFile("munch_2.wav"))
    {
        std::cout << "Error loading sound file" << std::endl;
    }
    if (!soundGhostMove.loadFromFile("siren_1.wav"))
    {
        std::cout << "Error loading sound file" << std::endl;
    }
    if (!siren2.loadFromFile("siren_2.wav"))
    {
        std::cout << "Error loading sound file" << std::endl;
    }
    if (!siren3.loadFromFile("siren_3.wav"))
    {
        std::cout << "Error loading sound file" << std::endl;
    }
    if (!siren4.loadFromFile("siren_4.wav"))
    {
        std::cout << "Error loading sound file" << std::endl;
    }
    if (!siren5.loadFromFile("siren_5.wav"))
    {
        std::cout << "Error loading sound file" << std::endl;
    }
    if (!soundGhostRetreat.loadFromFile("retreating.wav"))
    {
        std::cout << "Error loading sound file" << std::endl;
    }
    if (!soundPowerPellet.loadFromFile("power_pellet.wav"))
    {
        std::cout << "Error loading sound file" << std::endl;
    }
}