#include <stdio.h>
#include <stdbool.h>
#include <math.h>
// #include <windows.h>
#include "iGraphics.h"
#include "iSound.h"
#include <time.h>
#define screen_width 1000
#define screen_height 750
#define pi 3.14159
/*
gamestate:
0 = main menu
1 = game
2 = game over
3 = Highscore
4 = Options
5 = Help
6 = Load Game
7 = level Cleared screen
8 = Display saved games
9 = credits

110 = <main_game><level 1><base>
100 = <main_game><paused>  [new experimental notation -rafid]
*/

// variables/////////////////////////////////
int bgchk = 1, mbgchk = 1, mmchannel = -1, gamechannel = -1;
float ball_spd = 10.0;
int paddle_width = 150;
int paddle_height = 20;
int paddle_x = screen_width / 2 - paddle_width / 2;
int paddle_y = 15;
int ball_radius = 10;
int lives = 3;
int score = 0;
int dbx = 0;
bool isGameOver = false;
bool isFullscreen = false;
int offset_x = 0, offset_y = 0;
int gameState = 0;
char scoreText[100];
char lifeText[100];
int gomcheck = 0;
int selected_menu_idx = 1;
int prevGameState = 0;
bool isBallMoving = false;
bool mm_sound_check = true;
bool game_sound_check = true;
int level = 1;
bool loadingDone = false;
int levelClearedCounter = 0;
bool levelClearedAnimating = false;
int updateHighscoreFlag = 0;
int endingScreenFlag = 0;
int creditsroll = 0;
int creditscounter = 0;
int lastmusic = 0;
// Multiple balls system
#define MAX_BALLS 50
typedef struct
{
    float x, y;
    float dx, dy;
    bool isActive;
} Ball;
Ball balls[MAX_BALLS];
int activeBalls = 1;
/// block variables///////////////////////////
float block_width = 65;
float block_height = 30;
int block_padding = 3;
// block1 col = 00ffb5, 004d37
// block2 col = 00aaff, 00334d
// block3 col = ff4800, 4d1600
int blockGrid[15][15];
int levelGrid[10][15][15] = {
    {
        {5, 2, 1, 5, 2, 1, 5, 2, 1, 5, 2, 1, 5, 2, 1},
        {2, 1, 5, 2, 1, 5, 2, 1, 5, 2, 1, 5, 2, 1, 5},
        {1, 5, 2, 1, 5, 2, 1, 5, 2, 1, 5, 2, 1, 5, 2},
        {5, 2, 1, 5, 2, 1, 5, 2, 1, 5, 2, 1, 5, 2, 1},
        {2, 1, 5, 2, 1, 5, 2, 1, 5, 2, 1, 5, 2, 1, 5},
        {1, 5, 2, 1, 5, 2, 1, 5, 2, 1, 5, 2, 1, 5, 2},
        {5, 2, 1, 5, 2, 1, 5, 2, 1, 5, 2, 1, 5, 2, 1},
        {2, 1, 5, 2, 1, 5, 2, 1, 5, 2, 1, 5, 2, 1, 5},
        {1, 5, 2, 1, 5, 2, 1, 5, 2, 1, 5, 2, 1, 5, 2},
        {5, 2, 1, 5, 2, 1, 5, 2, 1, 5, 2, 1, 5, 2, 1},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    },
    {
        {5, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 5},
        {0, 5, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 5, 0},
        {0, 0, 5, 0, 0, 1, 1, 2, 1, 1, 0, 0, 5, 0, 0},
        {0, 0, 0, 5, 1, 1, 2, 2, 2, 1, 1, 5, 0, 0, 0},
        {0, 0, 0, 1, 1, 2, 2, 3, 2, 2, 1, 1, 0, 0, 0},
        {0, 0, 1, 1, 2, 2, 3, 3, 3, 2, 2, 1, 1, 0, 0},
        {0, 1, 1, 2, 2, 3, 3, 4, 3, 3, 2, 2, 1, 1, 0},
        {1, 1, 2, 2, 3, 3, 4, 4, 4, 3, 3, 2, 2, 1, 1},
        {0, 1, 1, 2, 2, 3, 3, 4, 3, 3, 2, 2, 1, 1, 0},
        {0, 0, 1, 1, 2, 2, 3, 3, 3, 2, 2, 1, 1, 0, 0},
        {0, 0, 0, 1, 1, 2, 2, 3, 2, 2, 1, 1, 0, 0, 0},
        {0, 0, 0, 5, 1, 1, 2, 2, 2, 1, 1, 5, 0, 0, 0},
        {0, 0, 5, 0, 0, 1, 1, 2, 1, 1, 0, 0, 5, 0, 0},
        {0, 5, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 5, 0},
        {5, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 5},
    },
    {
        {1, 1, 1, 2, 2, 2, 3, 3, 3, 1, 1, 1, 2, 2, 2},
        {1, 5, 1, 2, 5, 2, 3, 5, 3, 1, 5, 1, 2, 5, 2},
        {1, 1, 1, 2, 2, 2, 3, 3, 3, 1, 1, 1, 2, 2, 2},
        {2, 2, 2, 3, 3, 3, 1, 1, 1, 2, 2, 2, 3, 3, 3},
        {2, 5, 2, 3, 5, 3, 1, 5, 1, 2, 5, 2, 3, 5, 3},
        {2, 2, 2, 3, 3, 3, 1, 1, 1, 2, 2, 2, 3, 3, 3},
        {3, 3, 3, 1, 1, 1, 2, 2, 2, 3, 3, 3, 1, 1, 1},
        {3, 5, 3, 1, 5, 1, 2, 5, 2, 3, 5, 3, 1, 5, 1},
        {3, 3, 3, 1, 1, 1, 2, 2, 2, 3, 3, 3, 1, 1, 1},
        {1, 1, 1, 2, 2, 2, 3, 3, 3, 1, 1, 1, 2, 2, 2},
        {1, 5, 1, 2, 5, 2, 3, 5, 3, 1, 5, 1, 2, 5, 2},
        {1, 1, 1, 2, 2, 2, 3, 3, 3, 1, 1, 1, 2, 2, 2},
        {2, 2, 2, 3, 3, 3, 1, 1, 1, 2, 2, 2, 3, 3, 3},
        {2, 5, 2, 3, 5, 3, 1, 5, 1, 2, 5, 2, 3, 5, 3},
        {2, 2, 2, 3, 3, 3, 1, 1, 1, 2, 2, 2, 3, 3, 3},
    },
    {
        {1, 2, 3, 4, 1, 2, 3, 4, 1, 5, 3, 4, 1, 2, 3},
        {4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4},
        {5, 0, 1, 5, 3, 4, 1, 2, 3, 4, 1, 2, 3, 0, 3},
        {2, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 2},
        {1, 0, 3, 0, 1, 2, 3, 4, 1, 2, 3, 0, 3, 0, 5},
        {2, 0, 2, 0, 4, 0, 0, 0, 0, 0, 4, 0, 2, 0, 2},
        {3, 0, 1, 0, 3, 0, 1, 2, 3, 0, 5, 0, 1, 0, 3},
        {4, 0, 4, 0, 2, 0, 5, 0, 2, 0, 2, 0, 4, 0, 4},
        {1, 0, 3, 0, 1, 0, 3, 0, 1, 0, 1, 0, 3, 0, 1},
        {2, 0, 2, 0, 4, 0, 2, 0, 4, 0, 4, 0, 2, 0, 2},
        {3, 0, 1, 0, 3, 0, 1, 2, 3, 0, 3, 0, 1, 0, 3},
        {4, 0, 4, 0, 5, 0, 0, 0, 0, 0, 2, 0, 4, 0, 4},
        {1, 0, 3, 0, 1, 2, 3, 4, 1, 2, 3, 0, 3, 0, 1},
        {2, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 2},
        {3, 4, 3, 2, 1, 4, 3, 2, 1, 4, 3, 2, 1, 4, 1},
    },
    {
        {5, 0, 2, 0, 5, 0, 2, 0, 5, 0, 2, 0, 5, 0, 2},
        {0, 4, 0, 4, 0, 4, 0, 4, 0, 4, 0, 4, 0, 4, 0},
        {3, 0, 5, 0, 3, 0, 5, 0, 3, 0, 5, 0, 3, 0, 5},
        {0, 4, 0, 4, 0, 4, 0, 4, 0, 4, 0, 4, 0, 4, 0},
        {2, 0, 5, 0, 2, 0, 5, 0, 2, 0, 5, 0, 2, 0, 5},
        {0, 4, 0, 4, 0, 4, 0, 4, 0, 4, 0, 4, 0, 4, 0},
        {1, 0, 5, 0, 1, 0, 5, 0, 1, 0, 5, 0, 1, 0, 5},
        {0, 4, 0, 4, 0, 4, 0, 4, 0, 4, 0, 4, 0, 4, 0},
        {3, 0, 5, 0, 3, 0, 5, 0, 3, 0, 5, 0, 3, 0, 5},
        {0, 4, 0, 4, 0, 4, 0, 4, 0, 4, 0, 4, 0, 4, 0},
        {2, 0, 5, 0, 2, 0, 5, 0, 2, 0, 5, 0, 2, 0, 5},
        {0, 4, 0, 4, 0, 4, 0, 4, 0, 4, 0, 4, 0, 4, 0},
        {1, 0, 5, 0, 1, 0, 5, 0, 1, 0, 5, 0, 1, 0, 5},
        {0, 4, 0, 4, 0, 4, 0, 4, 0, 4, 0, 4, 0, 4, 0},
        {5, 0, 2, 0, 5, 0, 2, 0, 5, 0, 2, 0, 5, 0, 2},
    },
    {
        {0, 0, 0, 4, 4, 4, 0, 0, 0, 4, 4, 4, 0, 0, 0},
        {0, 5, 0, 0, 0, 0, 2, 3, 2, 0, 0, 0, 0, 5, 0},
        {0, 0, 3, 3, 1, 0, 0, 5, 0, 0, 1, 3, 3, 0, 0},
        {4, 0, 0, 0, 0, 4, 0, 1, 0, 4, 0, 0, 0, 0, 4},
        {4, 4, 2, 0, 4, 4, 4, 0, 4, 4, 4, 0, 2, 4, 4},
        {0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0},
        {2, 0, 4, 0, 1, 1, 0, 4, 0, 1, 1, 0, 4, 0, 2},
        {3, 0, 0, 4, 0, 5, 0, 0, 0, 5, 0, 4, 0, 0, 3},
        {2, 0, 4, 0, 1, 1, 0, 4, 0, 1, 1, 0, 4, 0, 2},
        {0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0},
        {4, 4, 2, 0, 4, 4, 4, 0, 4, 4, 4, 0, 2, 4, 4},
        {4, 0, 0, 0, 0, 4, 0, 1, 0, 4, 0, 0, 0, 0, 4},
        {0, 0, 3, 3, 1, 0, 0, 5, 0, 0, 1, 3, 3, 0, 0},
        {0, 5, 0, 0, 0, 0, 2, 3, 2, 0, 0, 0, 0, 5, 0},
        {0, 0, 0, 4, 4, 4, 0, 0, 0, 4, 4, 4, 0, 0, 0},
    },
    {
        {0, 0, 0, 4, 4, 4, 4, 4, 4, 4, 4, 4, 0, 0, 0},
        {0, 1, 5, 0, 0, 0, 5, 0, 5, 0, 0, 0, 5, 1, 0},
        {0, 0, 4, 2, 3, 0, 4, 0, 0, 0, 3, 2, 4, 0, 0},
        {4, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 4},
        {4, 0, 5, 0, 2, 4, 0, 5, 0, 4, 2, 0, 5, 0, 4},
        {4, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 4},
        {4, 4, 4, 4, 4, 4, 1, 4, 1, 4, 4, 4, 4, 4, 4},
        {4, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 4},
        {4, 0, 5, 0, 2, 4, 0, 5, 0, 4, 2, 0, 5, 0, 4},
        {4, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 4},
        {0, 0, 4, 2, 3, 0, 4, 0, 4, 0, 3, 2, 4, 0, 0},
        {0, 1, 5, 0, 0, 0, 5, 0, 5, 0, 0, 0, 5, 1, 0},
        {0, 0, 0, 4, 4, 4, 3, 1, 3, 4, 4, 4, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 4, 4, 4, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    },
    {
        {0, 0, 0, 4, 4, 4, 0, 0, 0, 4, 4, 4, 0, 0, 0},
        {0, 0, 5, 0, 0, 0, 2, 0, 2, 0, 0, 0, 5, 0, 0},
        {0, 2, 0, 3, 0, 4, 0, 0, 0, 4, 0, 3, 0, 2, 0},
        {4, 0, 4, 0, 4, 0, 1, 0, 1, 0, 4, 0, 4, 0, 4},
        {0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0},
        {0, 3, 0, 4, 0, 0, 3, 4, 3, 0, 0, 4, 0, 3, 0},
        {4, 0, 5, 0, 2, 0, 0, 4, 0, 0, 2, 0, 5, 0, 4},
        {0, 1, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 1, 0},
        {4, 0, 5, 0, 2, 0, 0, 4, 0, 0, 2, 0, 5, 0, 4},
        {0, 3, 0, 4, 0, 0, 3, 4, 3, 0, 0, 4, 0, 3, 0},
        {0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0},
        {4, 0, 4, 0, 4, 0, 1, 0, 1, 0, 4, 0, 4, 0, 4},
        {0, 2, 0, 3, 0, 4, 0, 0, 0, 4, 0, 3, 0, 2, 0},
        {0, 0, 5, 0, 0, 0, 2, 0, 2, 0, 0, 0, 5, 0, 0},
        {0, 0, 0, 4, 4, 4, 0, 0, 0, 4, 4, 4, 0, 0, 0},
    },
    {
        {0, 1, 2, 1, 0, 1, 3, 1, 3, 1, 0, 2, 1, 1, 0},
        {1, 0, 0, 0, 1, 0, 5, 0, 5, 0, 1, 0, 0, 0, 1},
        {2, 0, 3, 0, 2, 0, 1, 0, 1, 0, 2, 0, 3, 0, 2},
        {1, 5, 0, 4, 0, 2, 0, 3, 0, 2, 0, 4, 0, 5, 1},
        {0, 0, 2, 0, 1, 0, 5, 0, 5, 0, 1, 0, 2, 0, 0},
        {1, 3, 0, 2, 0, 3, 0, 2, 0, 3, 0, 2, 0, 3, 1},
        {2, 0, 5, 0, 0, 0, 1, 0, 1, 0, 0, 0, 5, 0, 2},
        {1, 0, 0, 1, 3, 1, 0, 5, 0, 1, 3, 1, 0, 0, 1},
        {2, 0, 5, 0, 0, 0, 1, 0, 1, 0, 0, 0, 5, 0, 2},
        {1, 3, 0, 2, 0, 3, 0, 2, 0, 3, 0, 2, 0, 3, 1},
        {0, 0, 2, 0, 1, 0, 5, 0, 5, 0, 1, 0, 2, 0, 0},
        {1, 5, 0, 4, 0, 2, 0, 3, 0, 2, 0, 4, 0, 5, 1},
        {2, 0, 3, 0, 2, 0, 1, 0, 1, 0, 2, 0, 3, 0, 2},
        {1, 0, 0, 0, 1, 0, 5, 0, 5, 0, 1, 0, 0, 0, 1},
        {0, 1, 2, 1, 0, 1, 3, 1, 3, 1, 0, 2, 1, 1, 0},
    },
    {
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    },
};
///////////////////////////////powerups//////////////////////////////////////////////
typedef struct
{
    float x, y;
    int type;
    bool isActive;
} pw;
pw powerUps[30];
////////////////////////////////////
FILE *fpr, *fpw;
#define MAX_SCORE 10
#define NAME_LEN 50
struct HighScores
{
    char name[NAME_LEN];
    int pts;
};
struct HighScores highscores[MAX_SCORE];
char playername[NAME_LEN];
int nameLength = 0;
int newrank = 0;

FILE *savefile;
#define MAX_SLOT 10

struct SavedData
{
    char timestamp[30];
    int score;
    int level;
    int lives;
    int blockState[15][15];
};
struct SavedData savedData[MAX_SLOT];

///////////////////////////////////////////////////////////////
void resetGame(void);
void mainMenu(void);
void pauseMenu(void);
void drawBlocks(void);
void displayOptions(void);
void ballMotion(void);
void toggleFullscreen(void);
void toggleMenuMusic(void);
void toggleGameMusic(void);
void loadScreen(int gamestate);
void checkCollision(int ballIdx);
int playOrResumeSound(int *channelVar, const char *filename, bool loop, int volume);
bool isLevelCleared();
void loadNextLevel();
void explode(int i, int j, bool playSound);
void activePower(int n);
void loadHighscore(void);
void displayHighscore(void);
void updateHighscore(char new_name[], int new_score);
void loadData(void);
void displaySavedGames(void);
int saveGame(int slotIdx);
void deleteSlot(int slotIndex);
void endgame(void);
void displayCredits(void);
///////////////////////////////////////////////////////////////

void iDraw()
{
    iClear();
    glPushMatrix();
    glTranslatef(offset_x, offset_y, 0);
    // main menu
    if (gameState == 0)
    {
        mainMenu();
        if (mbgchk && mm_sound_check)
        {
            playOrResumeSound(&mmchannel, "assets/sounds/mus_menu6.wav", true, 40);
            mbgchk = 0;
        }
        if (!mm_sound_check && mmchannel != -1)
        {
            iPauseSound(mmchannel);
        }

        iSetColor(255, 255, 255);

        if (selected_menu_idx == 1)
        {
            iShowImage(360, 370, "assets/images/mm_newgame_red.png");
        }

        else if (selected_menu_idx == 2)
        {
            iShowImage(360, 300, "assets/images/mm_loadgame_red.png");
        }

        else if (selected_menu_idx == 3)
        {
            iShowImage(360, 230, "assets/images/mm_highscore_red.png");
        }

        else if (selected_menu_idx == 4)
        {
            iShowImage(360, 160, "assets/images/mm_options_red.png");
        }

        else if (selected_menu_idx == 5)
        {
            iShowImage(360, 90, "assets/images/mm_help_red.png");
        }

        else if (selected_menu_idx == 6)
        {
            iShowImage(360, 20, "assets/images/mm_exit_red.png");
        }
    }

    // main game (level 1)
    if (gameState == 101)
    {
        iClear();
        iShowImage(0, 0, "assets/images/1.png");
        if (paddle_width == 150)
            iShowImage(paddle_x + dbx, paddle_y, "assets/images/paddle2.png");
        else if (paddle_width == 80)
            iShowImage(paddle_x + dbx, paddle_y, "assets/images/paddle_small.png");
        else if (paddle_width == 200)
            iShowImage(paddle_x + dbx, paddle_y, "assets/images/paddle_long.png");
        iSetColor(213, 105, 43);
        for (int i = 0; i < MAX_BALLS; i++)
        {
            if (balls[i].isActive)
            {
                iFilledCircle(balls[i].x, balls[i].y, ball_radius);
            }
        }
        drawBlocks();
        iSetColor(255, 0, 0);
        iShowImage(20, screen_height - 40, "assets/images/score.png");
        sprintf(scoreText, "%d", score);
        iTextTTF(150, screen_height - 25, scoreText, "assets/fonts/GrenzeGotisch-VariableFont_wght.ttf", 30);
        iShowImage(850, screen_height - 40, "assets/images/lives.png");
        sprintf(lifeText, "%d", lives);
        iTextTTF(950, screen_height - 25, lifeText, "assets/fonts/GrenzeGotisch-VariableFont_wght.ttf", 30);
        char levelText[20];
        sprintf(levelText, "Level: %d", level);
        iTextTTF(440, screen_height - 30, levelText, "assets/fonts/Bungee-Regular.ttf", 30);
        if (bgchk && game_sound_check)
        {
            playOrResumeSound(&gamechannel, "assets/sounds/gamebg1.wav", true, 40);
            bgchk = 0;
        }
        if (!game_sound_check && gamechannel != -1)
        {
            iPauseSound(gamechannel);
        }

        if (lives < 1 && !isGameOver)
        {
            iPlaySound("assets/sounds/mus_gameover.wav", false, 30);
            isGameOver = true;
            level = 1;
            gameState = 2;
        }
        for (int p = 0; p < 30; p++)
        {
            if (powerUps[p].isActive)
            {
                // Apply offset for fullscreen mode
                int draw_x = powerUps[p].x;
                int draw_y = powerUps[p].y;

                switch (powerUps[p].type)
                {
                case 1:
                    iShowImage(draw_x, draw_y, "assets/images/powerups/life+.png");
                    break;
                case 2:
                    iShowImage(draw_x, draw_y, "assets/images/powerups/spd--.png");
                    break;
                case 3:
                    iShowImage(draw_x, draw_y, "assets/images/powerups/paddle_wide.png");
                    break;
                case 4:
                    iShowImage(draw_x, draw_y, "assets/images/powerups/life-.png");
                    break;
                case 5:
                    iShowImage(draw_x, draw_y, "assets/images/powerups/spd++.png");
                    break;
                case 6:
                    iShowImage(draw_x, draw_y, "assets/images/powerups/paddle_small.png");
                    break;
                case 7:
                    iShowImage(draw_x, draw_y, "assets/images/powerups/gameover.png");
                    break;
                case 8:
                    iShowImage(draw_x, draw_y, "assets/images/powerups/ballmulti.png");
                    break;
                default:
                    break;
                }
            }
        }

        if (level == 10)
        {
            iSetColor(204, 204, 255);
            iTextTTF(200, screen_height - 200, "HERE YOU DESERVE IT!", "assets/fonts/BitcountGridDouble_Cursive-Light.ttf", 50);
        }
    }

    // pause menu
    else if (gameState == 100)
    {
        pauseMenu();
        if (gamechannel != -1)
            iPauseSound(gamechannel);
        if (selected_menu_idx == 0)
        {
            iShowImage(50, 435, "assets/images/pm_resume_yellow.png");
        }
        else if (selected_menu_idx == 1)
        {
            iShowImage(50, 375, "assets/images/pm_save_yellow.png");
        }
        else if (selected_menu_idx == 2)
        {
            iShowImage(50, 315, "assets/images/pm_exittomm_yellow.png");
        }
        else if (selected_menu_idx == 3)
        {
            iShowImage(50, 255, "assets/images/pm_exittod_yellow.png");
        }
    }

    // game over
    else if (gameState == 2)
    {
        if (gamechannel != -1)
        {
            iStopSound(gamechannel);
            gamechannel = -1;
        }
        if (updateHighscoreFlag == 0) // check but nothing else
        {
            for (int i = 0; i < MAX_SCORE; i++)
            {
                if (score > highscores[i].pts)
                {
                    updateHighscoreFlag = 1;
                    break;
                }
                updateHighscoreFlag = 2;
            }
        }

        else if (updateHighscoreFlag == 1) // taking input
        {
            iShowImage(0, 0, "assets/images/gominputmenu.png");
            iSetColor(255, 255, 255);
            iTextTTF(265, 442, playername, "assets/fonts/JetBrainsMono-Regular.ttf", 30);
        }
        else if (creditsroll)
        {
            creditscounter = 0;
            gameState = 9;
        }
        else
        {
            iShowImage(0, 0, "assets/images/gameover1.jpg");
            iSetColor(0, 0, 0);
            iFilledRectangle(10, 10, 980, 70);
            iShowImage(63, 20, "assets/images/gom_mainmenu_blue.png");
            iShowImage(375, 20, "assets/images/gom_tryagain_blue.png");
            iShowImage(687, 20, "assets/images/gom_exit_blue.png");
            if (gomcheck == 0)
                iShowImage(63, 20, "assets/images/gom_mainmenu_red.png");
            if (gomcheck == 1)
                iShowImage(375, 20, "assets/images/gom_tryagain_red.png");
            if (gomcheck == 2)
                iShowImage(687, 20, "assets/images/gom_exit_red.png");

            iSetColor(0, 0, 0);
            iFilledRectangle(265, 150, 500, 70);
            iShowImage(345, 160, "assets/images/scoregom.png");
            iSetColor(255, 255, 255);
            iTextAdvanced(550, 167, scoreText, 0.3, 4);
        }
    }

    else if (gameState == 3) // highscore
    {
        static bool highscoresLoaded = false;
        if (!highscoresLoaded)
        {
            loadHighscore();
            highscoresLoaded = true;
        }

        for (int i = 0; i < MAX_SCORE; i++)
        {
            int pts;
            char name[NAME_LEN];
            if (fscanf(fpr, "%s %d\n", name, &pts) == 2)
            {
                strcpy(highscores[i].name, name);
                highscores[i].pts = pts;
            }
        }
        displayHighscore();
    }
    else if (gameState == 4)
    {
        displayOptions();
        if (selected_menu_idx == 0)
        {
            iShowImage(50, screen_height - 310, "assets/images/opn-opnbg1.png");
            iSetColor(255, 0, 0);
            iTextTTF(70, screen_height - 290, "FULLSCREEN", "assets/fonts/RubikDoodleShadow-Regular.ttf", 33);
        }
        else if (selected_menu_idx == 1)
        {
            if (mm_sound_check)
            {
                iShowImage(50, screen_height - 380, "assets/images/opn-opnbg1.png");
                iSetColor(255, 0, 0);
                iTextTTF(70, screen_height - 360, "MAIN MENU MUSIC : ON", "assets/fonts/RubikDoodleShadow-Regular.ttf", 33);
            }
            else
            {
                iShowImage(50, screen_height - 380, "assets/images/opn-opnbg1.png");
                iSetColor(255, 0, 0);
                iTextTTF(70, screen_height - 360, "MAIN MENU MUSIC : OFF", "assets/fonts/RubikDoodleShadow-Regular.ttf", 33);
            }
        }
        else if (selected_menu_idx == 2)
        {
            if (game_sound_check)
            {
                iShowImage(50, screen_height - 450, "assets/images/opn-opnbg1.png");
                iSetColor(255, 0, 0);
                iTextTTF(70, screen_height - 430, "GAME MUSIC : ON", "assets/fonts/RubikDoodleShadow-Regular.ttf", 33);
            }
            else
            {
                iShowImage(50, screen_height - 450, "assets/images/opn-opnbg1.png");
                iSetColor(255, 0, 0);
                iTextTTF(70, screen_height - 430, "GAME MUSIC : OFF", "assets/fonts/RubikDoodleShadow-Regular.ttf", 33);
            }
        }
        else if (selected_menu_idx == 3)
        {
            iShowImage(50, screen_height - 520, "assets/images/opn-opnbg1.png");
            iSetColor(255, 0, 0);
            iTextTTF(70, screen_height - 500, "BACK", "assets/fonts/RubikDoodleShadow-Regular.ttf", 33);
        }
    }
    else if (gameState == 5)
    {
        iShowImage(0, 0, "assets/images/background2.jpg");
        iShowImage(100, 50, "assets/images/help.png");
    }

    else if (gameState == 6) // load game
    {
        displaySavedGames();
    }
    else if (gameState == 7) // levelcleared screen
    {
        if (endingScreenFlag == 0)
        {
            if (!levelClearedAnimating)
            {
                levelClearedCounter = 0;
                levelClearedAnimating = true;
                iPauseTimer(0);
            }

            static int frameDelay = 0;
            frameDelay++;

            if (frameDelay < 90)
            {
                iSetColor(237, 184, 38);
                iTextTTF(290, 350, "LEVEL CLEARED!", "assets/fonts/Bungee-Regular.ttf", 50);
            }

            if (frameDelay > 200)
            {
                frameDelay = 0;
                levelClearedCounter++;
            }

            if (levelClearedCounter >= 5)
            {
                levelClearedAnimating = false;
                frameDelay = 0;
                loadNextLevel();
            }
        }
        else if (endingScreenFlag == 1)
        {
            if (!levelClearedAnimating)
            {
                levelClearedCounter = 0;
                levelClearedAnimating = true;
                iPauseTimer(0);
            }

            static int frameDelay = 0;
            frameDelay++;

            if (frameDelay < 100)
            {
                iSetColor(237, 184, 38);
                iTextTTF(270, 350, "GAME COMPLETED!", "assets/fonts/Bungee-Regular.ttf", 50);
            }

            if (frameDelay > 200)
            {
                frameDelay = 0;
                levelClearedCounter++;
            }

            if (levelClearedCounter >= 5)
            {
                levelClearedAnimating = false;
                frameDelay = 0;
                endgame();
            }
        }
    }
    else if (gameState == 8)
    {
        displaySavedGames();
    }

    else if (gameState == 9) // credits
    {
        if (!lastmusic)
        {
            iStopAllSounds();
            iPlaySound("assets/sounds/last_goodbye.mp3");
            lastmusic = 1;
        }

        displayCredits();
    }
    glPopMatrix();
}

void iMouseMove(int mx, int my)
{
    mx -= offset_x;
    my -= offset_y;
    if (gameState == 0)
    {

        if (my < 90 && my > 20)
        {
            selected_menu_idx = 6;
        }
        else if (my < 160 && my > 90)
        {
            selected_menu_idx = 5;
        }
        else if (my < 230 && my > 160)
        {
            selected_menu_idx = 4;
        }
        else if (my < 300 && my > 230)
        {
            selected_menu_idx = 3;
        }
        else if (my < 370 && my > 300)
        {
            selected_menu_idx = 2;
        }
        else if (my < 440 && my > 370)
        {
            selected_menu_idx = 1;
        }
    }
    if (gameState == 101)
    {
        if (mx < paddle_width / 2)
            mx = paddle_width / 2;
        if (mx > screen_width - paddle_width / 2)
            mx = screen_width - paddle_width / 2;
        paddle_x = mx - paddle_width / 2;
        if (!isBallMoving)
            balls[0].x = paddle_x + dbx + paddle_width / 2;
    }
    if (gameState == 100)
    {
        if (my < 315 && my > 255)
        {
            selected_menu_idx = 3;
        }
        else if (my < 375 && my > 315)
        {
            selected_menu_idx = 2;
        }
        else if (my < 435 && my > 375)
        {
            selected_menu_idx = 1;
        }
        else if (my < 495 && my > 435)
        {
            selected_menu_idx = 0;
        }
    }
    if (gameState == 2)
    {
        if (updateHighscoreFlag != 1)
        {
            if (mx >= 63 && mx <= 313)
                gomcheck = 0;
            if (mx >= 375 && mx <= 625)
                gomcheck = 1;
            if (mx >= 687 && mx <= 937)
                gomcheck = 2;
        }
    }
    if (gameState == 3)
    {
    }
    if (gameState == 4)
    {
        if (my < screen_height - 240 && my > screen_height - 310)
        {
            selected_menu_idx = 0;
        }
        else if (my < screen_height - 310 && my > screen_height - 380)
        {
            selected_menu_idx = 1;
        }
        else if (my < screen_height - 380 && my > screen_height - 450)
        {
            selected_menu_idx = 2;
        }
        else if (my < screen_height - 450 && my > screen_height - 520)
        {
            selected_menu_idx = 3;
        }
    }
    if (gameState == 6 || gameState == 8) // load game
    {
        if (my < screen_height - 260 - 0 * 40 && my > screen_height - 260 - 1 * 40)
        {
            selected_menu_idx = 1;
        }
        else if (my < screen_height - 260 - 1 * 40 && my > screen_height - 260 - 2 * 40)
        {
            selected_menu_idx = 2;
        }
        else if (my < screen_height - 260 - 2 * 40 && my > screen_height - 260 - 3 * 40)
        {
            selected_menu_idx = 3;
        }
        else if (my < screen_height - 260 - 3 * 40 && my > screen_height - 260 - 4 * 40)
        {
            selected_menu_idx = 4;
        }
        else if (my < screen_height - 260 - 4 * 40 && my > screen_height - 260 - 5 * 40)
        {
            selected_menu_idx = 5;
        }
        else if (my < screen_height - 260 - 5 * 40 && my > screen_height - 260 - 6 * 40)
        {
            selected_menu_idx = 6;
        }
        else if (my < screen_height - 260 - 6 * 40 && my > screen_height - 260 - 7 * 40)
        {
            selected_menu_idx = 7;
        }
        else if (my < screen_height - 260 - 7 * 40 && my > screen_height - 260 - 8 * 40)
        {
            selected_menu_idx = 8;
        }
        else if (my < screen_height - 260 - 8 * 40 && my > screen_height - 260 - 9 * 40)
        {
            selected_menu_idx = 9;
        }
        else if (my < screen_height - 260 - 9 * 40 && my > screen_height - 260 - 10 * 40)
        {
            selected_menu_idx = 10;
        }
    }
}

void iMouseDrag(int mx, int my)
{
}

void iMouse(int button, int state, int mx, int my)
{
    mx -= offset_x;
    my -= offset_y;
    if (gameState == 0)
    {
        if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
        {
            if (selected_menu_idx == 1)
            {
                // start game
                level = 1;
                resetGame();
                gameState = 101;
                iResumeTimer(0);
                iStopAllSounds();
            }
            else if (selected_menu_idx == 2)
            {
                // load game
                prevGameState = gameState;
                gameState = 6;
                selected_menu_idx = 0;
            }
            else if (selected_menu_idx == 3)
            {
                // high score
                displayHighscore();
                prevGameState = gameState;
            }
            else if (selected_menu_idx == 4)
            {
                // options
                prevGameState = gameState;
                displayOptions();
            }
            else if (selected_menu_idx == 5)
            {
                // help menu
                gameState = 5;
            }
            else if (selected_menu_idx == 6)
            {
                exit(0);
            }
        }
    }
    else if (gameState == 100)
    {
        if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
        {
            if (selected_menu_idx == 0)
            {
                gameState = 101;
                iResumeTimer(0);
                if (gamechannel != -1)
                    iResumeSound(gamechannel); // Resume the paused music
            }
            if (selected_menu_idx == 1)
            {
                gameState = 8;
            }
            if (selected_menu_idx == 2)
            {
                resetGame();
                mbgchk = 1;
                iStopSound(gamechannel);
                gameState = 0;
            }
            if (selected_menu_idx == 3)
            {
                exit(0);
            }
        }
    }
    else if (gameState == 2)
    {
        if (updateHighscoreFlag != 1)
        {
            if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
            {
                if (gomcheck == 0)
                {
                    iStopAllSounds();
                    mbgchk = 1;
                    gameState = 0;
                }
                else if (gomcheck == 1)
                {
                    resetGame();
                    isGameOver = false;
                    iStopAllSounds();
                }
                else if (gomcheck == 2)
                {
                    exit(0);
                }
            }
        }
    }
    else if (gameState == 4)
    {
        if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
        {
            if (selected_menu_idx == 0)
            {
                toggleFullscreen();
            }
            else if (selected_menu_idx == 1)
            {
                // toggle main menu music if not from pausemenu
                if (prevGameState != 100)
                {
                    mm_sound_check = !mm_sound_check;
                    toggleMenuMusic();
                }
            }
            else if (selected_menu_idx == 2)
            {
                // toggle game music
                game_sound_check = !game_sound_check;
                toggleGameMusic();
            }
            else if (selected_menu_idx == 3)
            {
                gameState = 0;
            }
        }
    }
    else if (gameState == 6) // load game
    {
        if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
        {
            if (selected_menu_idx > 0)
            {
                loadData();
                if (strlen(savedData[selected_menu_idx - 1].timestamp) != 0)
                {
                    resetGame();
                    score = savedData[selected_menu_idx - 1].score;
                    lives = savedData[selected_menu_idx - 1].lives;
                    level = savedData[selected_menu_idx - 1].level;
                    for (int i = 0; i < 15; i++)
                    {
                        for (int j = 0; j < 15; j++)
                        {
                            blockGrid[i][j] = savedData[selected_menu_idx - 1].blockState[i][j];
                        }
                    }
                    iResumeTimer(0);
                    iStopAllSounds();
                }
            }
        }
    }
}

void iMouseWheel(int dir, int mx, int my)
{
}

void iKeyboard(unsigned char key)
{
    if (gameState == 0) // main menu
    {
        switch (key)
        {
        case 'w':
        case 'W':
            selected_menu_idx = (selected_menu_idx + 6) % 7;
            if (selected_menu_idx == 0)
                selected_menu_idx = 1;
            break;

        case 's':
        case 'S':
            selected_menu_idx = (selected_menu_idx + 1) % 7;
            if (selected_menu_idx == 0)
                selected_menu_idx = 6;
            break;

        case '\r':
            if (selected_menu_idx == 1)
            {
                // main game
                level = 1;
                resetGame();
                gameState = 101;
                iResumeTimer(0);
                iStopAllSounds();
            }

            else if (selected_menu_idx == 2)
            {
                // load game
                prevGameState = gameState;
                gameState = 6;
                selected_menu_idx = 0;
            }

            else if (selected_menu_idx == 3)
            {
                // high score
                displayHighscore();
                prevGameState = gameState;
            }

            else if (selected_menu_idx == 4)
            {
                // options
                prevGameState = gameState;
                displayOptions();
            }
            else if (selected_menu_idx == 5)
            {
                // help menu
                gameState = 5;
            }
            else if (selected_menu_idx == 6)
            {
                // exit
                exit(0);
            }
            selected_menu_idx = 0;
            break;
        default:
            break;
        }
    }

    if (gameState == 100) // paused
    {
        switch (key)
        {
        case 'w':
        case 'W':
            selected_menu_idx += 3;
            break;
        case 's':
        case 'S':
            selected_menu_idx++;
            break;
        default:
            break;
        }
        selected_menu_idx = selected_menu_idx % 4;

        switch (key)
        {
        case ' ':
        case '\r':

            if (selected_menu_idx == 0)
            {
                gameState = 101;
                iResumeTimer(0);
                if (gamechannel != -1)
                    iResumeSound(gamechannel); // Resume the paused music
                // Remove bgchk = 1; to prevent starting new music
            }
            if (selected_menu_idx == 1)
            {
                gameState = 8;
            }
            if (selected_menu_idx == 2)
            {
                resetGame();
                iStopAllSounds();
                mbgchk = 1;
                gameState = 0;
            }
            if (selected_menu_idx == 3)
            {
                exit(0);
            }
        }
    }

    if (gameState == 101) // game
    {
        switch (key)
        {
        case 'd':
        case 'D':
            paddle_x += 20;
            if (paddle_x > screen_width - paddle_width)
                paddle_x = screen_width - paddle_width;
            if (!isBallMoving)
                balls[0].x = paddle_x + paddle_width / 2;
            break;
        case 'a':
        case 'A':
            paddle_x -= 20;
            if (paddle_x < 0)
                paddle_x = 0;
            if (!isBallMoving)
                balls[0].x = paddle_x + paddle_width / 2;
            break;
        case ' ':
        {
            if (!isBallMoving)
            {
                float angle = pi / 4;
                balls[0].dx = ball_spd * cos(angle);
                balls[0].dy = ball_spd * sin(angle);
                isBallMoving = true;
            }
            break;
        }
        case 27:
            gameState = 100;
            iPauseTimer(0);
            break;

        default:
            break;
        }
    }

    if (gameState == 2) // game over
    {
        if (updateHighscoreFlag != 1)
        {
            switch (key)
            {
            case 'a':
            case 'A':
                gomcheck += 2;
                break;
            case 'd':
            case 'D':
                gomcheck += 1;
                break;
            default:
                break;
            }
            gomcheck = gomcheck % 3;

            switch (key)
            {
            case ' ':
            case '\r':

                if (gomcheck == 0)
                {
                    iStopAllSounds();
                    mbgchk = 1;
                    gameState = 0;
                }
                else if (gomcheck == 1)
                {
                    level = 1;
                    score = 0;
                    lives = 3;
                    resetGame();
                    gameState = 101;
                    isGameOver = false;
                    iStopAllSounds();
                }
                else if (gomcheck == 2)
                {
                    exit(0);
                }
                break;

            default:
                break;
            }
        }
        else
        {
            if (key == 13 && nameLength > 0 && playername[0] != '\0')
            { // Enter key

                updateHighscore(playername, score);
                gameState = 3;
                if (creditsroll)
                    prevGameState = 9; // credits
                else
                    prevGameState = 2;
                updateHighscoreFlag = 2;
            }
            else if (key == 8 && nameLength > 0)
            { // Backspace
                nameLength--;
                playername[nameLength] = '\0';
            }
            else if ((key >= 'A' && key <= 'Z') || (key >= 'a' && key <= 'z') || (key >= '0' && key <= '9'))
            {
                if (nameLength < NAME_LEN - 1)
                {
                    playername[nameLength++] = key;
                    playername[nameLength] = '\0';
                }
            }
        }
    }

    if (gameState == 3) // high score
    {
        displayHighscore();
        switch (key)
        {
        case 27:
            if (prevGameState == 100)
            {
                mbgchk = 1;
                gameState = 0;
                newrank = 0;
                break;
            }
            else if (prevGameState == 2)
            {
                mbgchk = 1;
                gameState = 2;
                newrank = 0;
            }
            else if (prevGameState == 9)
            {
                creditscounter = 0;
                gameState = 9;
            }
            else
            {
                mbgchk = 1;
                gameState = 0;
            }
        default:
            break;
        }
    }

    if (gameState == 4) // options menu
    {
        switch (key)
        {
        case 'w':
        case 'W':
            selected_menu_idx = (selected_menu_idx + 3) % 4;
            break;
        case 's':
        case 'S':
            selected_menu_idx = (selected_menu_idx + 1) % 4;
            break;
        case 27:
            mbgchk = 1;
            gameState = 0;
            prevGameState = 4;
            break;
        case ' ':
        case '\r':
            if (selected_menu_idx == 0)
            {
                toggleFullscreen();
            }
            else if (selected_menu_idx == 1)
            {
                if (prevGameState != 100)
                {
                    mm_sound_check = !mm_sound_check;
                    toggleMenuMusic();
                }
            }
            else if (selected_menu_idx == 2)
            {
                game_sound_check = !game_sound_check;
                toggleGameMusic();
            }
            else if (selected_menu_idx == 3)
            {
                gameState = prevGameState;
                prevGameState = 4;
            }
            break;

        default:
            break;
        }
    }

    if (gameState == 5) // help menu
    {
        switch (key)
        {
        case 27:
            mbgchk = 1;
            gameState = 0;
            break;
        default:
            break;
        }
    }

    if (gameState == 6) // load game
    {
        switch (key)
        {
        case 'w':
        case 'W':
            selected_menu_idx = (selected_menu_idx + 10) % 11;
            if (selected_menu_idx == 0)
                selected_menu_idx = 1;
            break;

        case 's':
        case 'S':
            selected_menu_idx = (selected_menu_idx + 1) % 11;
            if (selected_menu_idx == 0)
                selected_menu_idx = 10;
            break;

        case '\r':
            if (selected_menu_idx > 0)
            {
                loadData();
                if (strlen(savedData[selected_menu_idx - 1].timestamp) != 0)
                {
                    resetGame();
                    score = savedData[selected_menu_idx - 1].score;
                    lives = savedData[selected_menu_idx - 1].lives;
                    level = savedData[selected_menu_idx - 1].level;
                    for (int i = 0; i < 15; i++)
                    {
                        for (int j = 0; j < 15; j++)
                        {
                            blockGrid[i][j] = savedData[selected_menu_idx - 1].blockState[i][j];
                        }
                    }
                    iResumeTimer(0);
                    iStopAllSounds();
                }
            }
            break;
        case 'd':
        case 'D':
            // Delete selected slot
            if (selected_menu_idx > 0)
            {
                deleteSlot(selected_menu_idx - 1);
            }
            break;
        case 27:
            mbgchk = 1;
            gameState = 0;
            break;
        default:
            break;
        }
    }

    else if (gameState == 8)
    {
        switch (key)
        {
        case 'w':
        case 'W':
        {
            selected_menu_idx = (selected_menu_idx + 10) % 11;
            if (selected_menu_idx == 0)
                selected_menu_idx = 1;
            break;
        }
        case 's':
        case 'S':
        {
            selected_menu_idx = (selected_menu_idx + 1) % 11;
            if (selected_menu_idx == 0)
                selected_menu_idx = 10;
            break;
        }
        case 27:
        {
            gameState = 100;
            break;
        }
        case '\r':
        {
            saveGame(selected_menu_idx - 1);
            break;
        }
        case 'd':
        case 'D':
        {
            // Delete selected slot
            if (selected_menu_idx > 0)
            {
                deleteSlot(selected_menu_idx - 1);
            }
            break;
        }
        default:
            break;
        }
    }
}

void iSpecialKeyboard(unsigned char key)
{
    switch (key)
    {
    case GLUT_KEY_END:
        break;
    default:
        break;
    }
}

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    iSetTimer(10, ballMotion);
    loadHighscore();
    iInitializeSound();
    iInitialize(screen_width, screen_height, "Rebound by 2405051 and 2405042");
    return 0;
}

void resetGame(void)
{
    dbx = 0;
    isBallMoving = false;
    lives = 3;
    score = 0;
    bgchk = 1;
    isGameOver = false;
    gameState = 101;
    paddle_width = 150;
    ball_spd = 10;

    // Initialize balls
    activeBalls = 1;
    for (int i = 0; i < MAX_BALLS; i++)
    {
        balls[i].isActive = false;
        balls[i].dx = 0;
        balls[i].dy = 0;
    }

    // Initialize first ball
    balls[0].isActive = true;
    balls[0].x = paddle_x + dbx + paddle_width / 2;
    balls[0].y = paddle_height + paddle_y + ball_radius;
    balls[0].dx = 0;
    balls[0].dy = 0;
    for (int i = 0; i < 15; i++)
        for (int j = 0; j < 15; j++)
            blockGrid[i][j] = 0;
    for (int i = 0; i < 15; i++)
    {
        for (int j = 0; j < 15; j++)
        {
            blockGrid[i][j] = levelGrid[level - 1][i][j];
        }
    }

    for (int i = 0; i < 30; i++)
    {
        powerUps[i].isActive = false;
    }

    strcpy(playername, "");
    nameLength = 0;
    updateHighscoreFlag = 0;
    endingScreenFlag = 0;
}

void pauseMenu(void)
{
    if (gamechannel != -1)
    {
        iPauseSound(gamechannel);
    }
    iShowImage(50, 255, "assets/images/pm_exittod_white.png");
    iShowImage(50, 315, "assets/images/pm_exittomm_white.png");
    iShowImage(50, 375, "assets/images/pm_save_white.png");
    iShowImage(50, 435, "assets/images/pm_resume_white.png");
}

void mainMenu(void)
{
    iShowImage(0, 0, "assets/images/mainmenubg2.png");
    iShowImage(360, 20, "assets/images/mm_exit_white.png");
    iShowImage(360, 90, "assets/images/mm_help_white.png");
    iShowImage(360, 160, "assets/images/mm_options_white.png");
    iShowImage(360, 230, "assets/images/mm_highscore_white.png");
    iShowImage(360, 300, "assets/images/mm_loadgame_white.png");
    iShowImage(360, 370, "assets/images/mm_newgame_white.png");
}

void drawBlocks(void)
{
    const char block_images[5][30] = {
        "assets/images/blocks/1.png", // Bright Green
        "assets/images/blocks/2.png", // Bright Blue
        "assets/images/blocks/3.png", // Orange
        "assets/images/blocks/4.png", // Light Gray
        "assets/images/blocks/5.png"  // Bright Red (explosive)
    };
    for (int i = 0; i < 15; i++)
    {
        for (int j = 0; j < 15; j++)
        {
            if (blockGrid[i][j] > 0 && blockGrid[i][j] <= 5)
            {
                int block_x = 13 + j * block_width;
                int block_y = screen_height - (i + 1) * block_height - 70;
                iShowImage(block_x, block_y, block_images[blockGrid[i][j] - 1]);
            }
        }
    }
}

void displayOptions(void)
{
    gameState = 4;
    iShowImage(0, 0, "assets/images/background2.jpg");
    iShowImage(50, screen_height - 310, "assets/images/opn-opnbg1.png");
    iShowImage(50, screen_height - 380, "assets/images/opn-opnbg1.png");
    iShowImage(50, screen_height - 450, "assets/images/opn-opnbg1.png");
    iShowImage(50, screen_height - 520, "assets/images/opn-opnbg1.png");
    iSetColor(255, 255, 255);
    iTextTTF(70, screen_height - 290, "FULLSCREEN", "assets/fonts/RubikDoodleShadow-Regular.ttf", 31);
    if (mm_sound_check)
    {
        iTextTTF(70, screen_height - 360, "MAIN MENU MUSIC : ON", "assets/fonts/RubikDoodleShadow-Regular.ttf", 31);
    }
    else
    {
        iTextTTF(70, screen_height - 360, "MAIN MENU MUSIC : OFF", "assets/fonts/RubikDoodleShadow-Regular.ttf", 31);
    }
    if (game_sound_check)
    {
        iTextTTF(70, screen_height - 430, "GAME MUSIC : ON", "assets/fonts/RubikDoodleShadow-Regular.ttf", 31);
    }
    else
    {
        iTextTTF(70, screen_height - 430, "GAME MUSIC : OFF", "assets/fonts/RubikDoodleShadow-Regular.ttf", 31);
    }
    iTextTTF(70, screen_height - 500, "BACK", "assets/fonts/RubikDoodleShadow-Regular.ttf", 31);
}

void ballMotion(void)
{
    if (gameState != 101)
    {
        return;
    }
    // Count active balls at the start
    int currentActiveBalls = 0;

    for (int ballIdx = 0; ballIdx < MAX_BALLS; ballIdx++)
    {
        if (!balls[ballIdx].isActive)
            continue;

        currentActiveBalls++;

        if (!isBallMoving)
        {
            if (ballIdx == 0)
            {
                balls[ballIdx].x = (float)paddle_x + dbx + paddle_width / 2;
                balls[ballIdx].y = (float)paddle_height + paddle_y + ball_radius;
            }
            continue;
        }

        balls[ballIdx].x += balls[ballIdx].dx;
        balls[ballIdx].y += balls[ballIdx].dy;

        if (balls[ballIdx].y < paddle_y)
        {
            balls[ballIdx].isActive = false;
            currentActiveBalls--; 
            continue;           
        }

        checkCollision(ballIdx);

        // Wall collisions
        if (balls[ballIdx].x + ball_radius > screen_width || balls[ballIdx].x - ball_radius < 0)
        {
            if (balls[ballIdx].x + ball_radius > screen_width)
                balls[ballIdx].x = screen_width - ball_radius;
            else if (balls[ballIdx].x - ball_radius < 0)
                balls[ballIdx].x = ball_radius;
            balls[ballIdx].dx *= -1;
            if (game_sound_check)
                iPlaySound("assets/sounds/bounce_border.mp3", false);
        }

        if (balls[ballIdx].y + ball_radius > screen_height)
        {
            if (balls[ballIdx].y + ball_radius > screen_height)
                balls[ballIdx].y = screen_height - ball_radius;
            balls[ballIdx].dy *= (-1);
            if (game_sound_check)
                iPlaySound("assets/sounds/bounce_border.mp3", false);
        }

        // Paddle collision
        if (balls[ballIdx].dx != 0 && balls[ballIdx].dy != 0 &&
            balls[ballIdx].x > paddle_x + dbx && balls[ballIdx].x < paddle_x + paddle_width + dbx &&
            balls[ballIdx].y - ball_radius < paddle_height + paddle_y && balls[ballIdx].y - ball_radius > paddle_y)
        {
            score += 20;
            balls[ballIdx].y = paddle_y + paddle_height + ball_radius;
            float position = ((paddle_x + dbx + paddle_width / 2) - balls[ballIdx].x) / (paddle_width / 2);
            float angle = (pi / 2) + position * (pi / 3);
            if (game_sound_check)
                iPlaySound("assets/sounds/bounce_border.mp3", false);
            balls[ballIdx].dx = ball_spd * cos(angle);
            balls[ballIdx].dy = ball_spd * sin(angle);
        }
    }

    activeBalls = currentActiveBalls;

    if (activeBalls == 0 && isBallMoving)
    {
        lives--;
        ball_spd = 10;
        if (game_sound_check)
            iPlaySound("assets/sounds/lifelost.wav", false, 50);
        dbx = 0;

        if (lives <= 0)
        {
            isGameOver = true;
            gameState = 2;
            if (game_sound_check)
                iPlaySound("assets/sounds/mus_gameover.wav", false, 30);
        }
        else
        {
            isBallMoving = false;
            activeBalls = 1;
            balls[0].isActive = true;
            balls[0].x = paddle_x + dbx + paddle_width / 2;
            balls[0].y = paddle_height + paddle_y + ball_radius;
            balls[0].dx = 0;
            balls[0].dy = 0;

            for (int i = 1; i < MAX_BALLS; i++)
            {
                balls[i].isActive = false;
            }
        }
    }

    if (isLevelCleared())
    {
        level++;
        gameState = 7;
        if (level > 10)
        {
            endingScreenFlag = 1;
            lastmusic = 0;
        }
    }

    // Update power-ups
    for (int k = 0; k < 30; k++)
    {
        if (powerUps[k].isActive)
        {
            powerUps[k].y -= 5;
            if (((powerUps[k].x > paddle_x + dbx) && (powerUps[k].x < paddle_x + dbx + paddle_width)) &&
                (powerUps[k].y <= paddle_y + paddle_height) && (powerUps[k].y >= paddle_y))
            {
                // Power-up caught by paddle
                activePower(powerUps[k].type);
                powerUps[k].isActive = false;
            }
            else if (powerUps[k].y < paddle_y - 10)
            {
                powerUps[k].isActive = false;
            }
        }
    }
}
void toggleFullscreen(void)
{
    isFullscreen = !isFullscreen;
    if (isFullscreen)
    {
        // Save window position and size
        glutFullScreen();
        // Get screen dimensions
        int screenWidth = glutGet(GLUT_SCREEN_WIDTH);
        int screenHeight = glutGet(GLUT_SCREEN_HEIGHT);
        // Calculate offsets to center the game
        offset_x = (screenWidth - screen_width) / 2;
        offset_y = (screenHeight - screen_height) / 2;
    }
    else
    {
        // Reset to windowed mode
        glutReshapeWindow(screen_width, screen_height);
        glutPositionWindow(100, 100);
        offset_x = 0;
        offset_y = 0;
    }
}
void toggleMenuMusic()
{
    if (mm_sound_check)
    {
        playOrResumeSound(&mmchannel, "assets/sounds/mus_menu6.wav", true, 40);
    }
    else
    {
        if (mmchannel != -1)
            iPauseSound(mmchannel);
    }
}

void toggleGameMusic()
{
    if (game_sound_check)
    {
        playOrResumeSound(&gamechannel, "assets/sounds/gamebg1.wav", true, 40);
    }
    else
    {
        if (gamechannel != -1)
            iPauseSound(gamechannel);
    }
}

void loadScreen(int gamestate)
{
    iSetColor(0, 0, 0);
    iFilledRectangle(0, 0, screen_width, screen_height);
    iSetColor(0, 255, 255);
    iTextTTF(450, 100, "Loading Level ", "assets/fonts/RubikDoodleShadow-Regular.ttf", 33);
    int level = gamestate / 100;
    sprintf(scoreText, "%d", level);
    iTextTTF(600, 100, scoreText, "assets/fonts/RubikDoodleShadow-Regular.ttf", 33);
    iTextTTF(450, 50, "Please wait...", "assets/fonts/RubikDoodleShadow-Regular.ttf", 33);
}

void checkCollision(int ballIdx)
{
    if (balls[ballIdx].y <= 50)
        return;

    int i, j;
    for (i = 0; i < 15; i++)
    {
        for (j = 0; j < 15; j++)
        {
            if (blockGrid[i][j] > 0)
            {
                int block_x = 13 + j * block_width;
                int block_y = screen_height - (i + 1) * block_height - 70;

                // Find closest point on block to ball's center
                double closest_x = fmax(block_x, fmin(balls[ballIdx].x, block_x + block_width));
                double closest_y = fmax(block_y, fmin(balls[ballIdx].y, block_y + block_height));
                double distance = sqrt(pow(balls[ballIdx].x - closest_x, 2) + pow(balls[ballIdx].y - closest_y, 2));

                if (distance < ball_radius)
                {
                    // Determine collision side
                    double dx_left = balls[ballIdx].x - block_x;
                    double dx_right = (block_x + block_width) - balls[ballIdx].x;
                    double dy_top = (block_y + block_height) - balls[ballIdx].y;
                    double dy_bottom = balls[ballIdx].y - block_y;
                    double min_distance = fmin(fmin(dx_left, dx_right), fmin(dy_top, dy_bottom));

                    if (min_distance == dx_left || min_distance == dx_right)
                    {
                        balls[ballIdx].x = (min_distance == dx_left) ? block_x - ball_radius : block_x + block_width + ball_radius;
                        balls[ballIdx].dx = -balls[ballIdx].dx; // Horizontal bounce
                    }
                    else
                    {
                        balls[ballIdx].y = (min_distance == dy_top) ? block_y + block_height + ball_radius : block_y - ball_radius;
                        balls[ballIdx].dy = -balls[ballIdx].dy; // Vertical bounce
                    }

                    // Update block and score
                    if (blockGrid[i][j] == 5)
                    {
                        explode(i, j, true);
                    }
                    else if (blockGrid[i][j] < 4 && blockGrid[i][j] > 0)
                    {
                        blockGrid[i][j] -= 1;
                        score += 50;
                        if (rand() % 100 < 30 && isBallMoving)
                        {
                            for (int k = 0; k < 30; k++)
                            {
                                if (!powerUps[k].isActive)
                                {
                                    powerUps[k].isActive = true;
                                    powerUps[k].x = j * block_width + block_width / 2;
                                    powerUps[k].y = screen_height - (i + 1) * block_height;
                                    powerUps[k].type = rand() % 8 + 1; // Updated to include multi-ball (type 8)
                                    break;
                                }
                            }
                        }
                    }
                    iPlaySound("assets/sounds/bounce.wav", false, 30);

                    // Spawn power-up (30% chance)
                }
            }
        }
    }
}

int playOrResumeSound(int *channelVar, const char *filename, bool loop, int volume)
{
    if (*channelVar != -1 && Mix_Playing(*channelVar))
    {
        iResumeSound(*channelVar);
    }
    else
    {
        if (*channelVar != -1)
        {
            iStopSound(*channelVar);
        }
        *channelVar = iPlaySound(filename, loop, volume);
    }
    return *channelVar;
}
bool isLevelCleared()
{
    static int cachedBlockCount = -1;
    static bool lastResult = false;
    int currentBlocks = 0;
    for (int i = 0; i < 15; i++)
    {
        for (int j = 0; j < 15; j++)
        {
            if (blockGrid[i][j] != 0 && blockGrid[i][j] != 4)
                currentBlocks++;
        }
    }

    if (currentBlocks != cachedBlockCount)
    {
        cachedBlockCount = currentBlocks;
        lastResult = (currentBlocks == 0);
    }

    return lastResult;
}

void loadNextLevel()
{
    int lives_temp = lives;
    int score_temp = score;
    if (level > 10)
    {
        level = 1;
        isGameOver = true;
        gameState = 2;
    }
    gameState = 7;
    loadingDone = false;
    iPauseTimer(0);
    iPauseSound(gamechannel);
    resetGame();
    score = score_temp;
    lives = lives_temp;
    loadingDone = true;
    gameState = 101;
    iResumeTimer(0);
    bgchk = 1;
}

void explode(int i, int j, bool playSound = true)
{
    if (i < 0 || i >= 15 || j < 0 || j >= 15)
        return;

    if (blockGrid[i][j] == 0)
        return;

    int type = blockGrid[i][j];
    blockGrid[i][j] = 0;
    score += 50;

    if (type == 5)
    {
        for (int di = -1; di <= 1; di++)
        {
            for (int dj = -1; dj <= 1; dj++)
            {
                int ni = i + di;
                int nj = j + dj;
                if (ni >= 0 && ni < 15 && nj >= 0 && nj < 15)
                {
                    if (blockGrid[ni][nj] != 0)
                        explode(ni, nj, false);
                }
            }
        }
    }
    if (playSound)
        iPlaySound("assets/sounds/explosion.mp3", false, 20);
}
void activePower(int n)
{
    switch (n)
    {
    case 1: // life
        iPlaySound("assets/sounds/powerup.mp3", false, 30);

        if (lives < 5)
            lives++;
        break;
    case 2: // speed slow
        iPlaySound("assets/sounds/powerup.mp3", false, 30);
        ball_spd = 8;
        break;
    case 3: // paddle size increase
        iPlaySound("assets/sounds/powerup.mp3", false, 30);
        paddle_width = 200;
        break;
    case 4: // life decrease
        iPlaySound("assets/sounds/powerdown.wav", false, 100);
        lives--;
        break;
    case 5: // speed increase
        iPlaySound("assets/sounds/powerdown.wav", false, 100);
        ball_spd = 15;
        break;
    case 6: // paddle size decrease
        iPlaySound("assets/sounds/powerdown.wav", false, 100);
        paddle_width = 80;
        break;
    case 7: // game over

        iPlaySound("assets/sounds/powerdown.wav", false, 100);
        iPlaySound("assets/sounds/mus_gameover.wav", false, 30);
        isGameOver = true;
        level = 1;
        gameState = 2;
        break;
    case 8: // multi-ball
    {
        iPlaySound("assets/sounds/powerup.mp3", false, 30);

        // Find all active balls to spawn from
        int activeBallIndices[MAX_BALLS];
        int activeBallCount = 0;
        for (int j = 0; j < MAX_BALLS; j++)
        {
            if (balls[j].isActive)
            {
                activeBallIndices[activeBallCount] = j;
                activeBallCount++;
            }
        }

        for (int sourceIdx = 0; sourceIdx < activeBallCount; sourceIdx++)
        {
            for (int spawn = 0; spawn < 2; spawn++)
            {
                if (activeBalls >= MAX_BALLS)
                    break; // No more slots available

                for (int i = 0; i < MAX_BALLS; i++)
                {
                    if (!balls[i].isActive && activeBalls < MAX_BALLS)
                    {
                        int sourceBallIdx = activeBallIndices[sourceIdx];

                        balls[i].isActive = true;
                        balls[i].x = balls[sourceBallIdx].x; // Spawn from current active ball
                        balls[i].y = balls[sourceBallIdx].y;

                        // Generate random angle for variety
                        double baseAngle = (rand() % 120 + 30) * pi / 180; // 30-150 degrees
                        if (rand() % 2 == 0)
                            baseAngle = pi - baseAngle; // Mirror for variety

                        balls[i].dx = ball_spd * cos(baseAngle);
                        balls[i].dy = ball_spd * sin(baseAngle);
                        activeBalls++;
                        break; // Found slot, move to next source ball
                    }
                }
            }
            // Find next available slot
        }
        break;
    }
    default:
        break;
    }
}
void loadHighscore(void)
{
    fpr = fopen("assets/data/score.txt", "r");
    if (fpr == NULL)
    {
        printf("Error opening highscores file.\n");
        return;
    }

    for (int i = 0; i < MAX_SCORE; i++)
    {
        if (fscanf(fpr, "%s %d", highscores[i].name, &highscores[i].pts) != 2)
        {
            highscores[i].name[0] = '\0';
            highscores[i].pts = 0;
        }
    }
    fclose(fpr);
}

void displayHighscore(void)
{
    gameState = 3;

    static bool highscoresLoaded = false;
    if (!highscoresLoaded)
    {
        loadHighscore();
        highscoresLoaded = true;
    }

    int line = 0;
    iShowImage(0, 0, "assets/images/mainmenublurred.jpg");
    iSetColor(255, 255, 255); // 03bfeb
    iTextTTF(100, screen_height - 100 - line * 40, "High Scores:", "assets/fonts/SpecialGothicExpandedOne-Regular.ttf", 64);
    line += 3;

    iTextTTF(100, screen_height - 100 - line * 40, "RANK", "assets/fonts/Bungee-Regular.ttf", 30);
    iTextTTF(250, screen_height - 100 - line * 40, "PLAYER", "assets/fonts/Bungee-Regular.ttf", 30);
    iTextTTF(800, screen_height - 100 - line * 40, "SCORE", "assets/fonts/Bungee-Regular.ttf", 30);
    line += 2;
    for (int i = 0; i < MAX_SCORE; i++)
    {
        char i_str[3];
        sprintf(i_str, "%d", i + 1);
        iTextTTF(100, screen_height - 100 - line * 40, i_str, "assets/fonts/Bungee-Regular.ttf", 30);
        if (highscores[i].pts == 0)
        {
            line++;
            continue;
        }
        char pts_str[10];
        sprintf(pts_str, "%d", highscores[i].pts);
        if (i + 1 == newrank)
        {
            iSetColor(245, 245, 67); // f5f543
            iTextTTF(100, screen_height - 100 - line * 40, i_str, "assets/fonts/Bungee-Regular.ttf", 30);
        }
        iTextTTF(250, screen_height - 100 - line * 40, highscores[i].name, "assets/fonts/Bungee-Regular.ttf", 30);
        iTextTTF(800, screen_height - 100 - line * 40, pts_str, "assets/fonts/Bungee-Regular.ttf", 30);
        iSetColor(255, 255, 255);
        line++;
    }
    iSetColor(255, 0, 0);
    iTextTTF(100, 50, "Press ESC to go back", "assets/fonts/Bungee-Regular.ttf", 20);
}

void updateHighscore(char new_name[], int new_score)
{
    fpw = fopen("assets/data/score.txt", "w");
    if (fpw == NULL)
    {
        printf("Error opening highscores file for writing.\n");
        return;
    }

    for (int i = 0; i < MAX_SCORE; i++)
    {
        if (new_score > highscores[i].pts)
        {
            for (int j = MAX_SCORE - 1; j > i; j--)
            {
                strcpy(highscores[j].name, highscores[j - 1].name);
                highscores[j].pts = highscores[j - 1].pts;
            }
            highscores[i].pts = new_score;
            strcpy(highscores[i].name, new_name);
            newrank = i + 1;
            break;
        }
        else if (i == MAX_SCORE - 1 && new_score == highscores[i].pts)
        {
            for (int j = MAX_SCORE - 1; j <= i + 1; j--)
            {
                strcpy(highscores[j].name, highscores[j - 1].name);
                highscores[j].pts = highscores[j - 1].pts;
            }
            highscores[i].pts = new_score;
            strcpy(highscores[i].name, new_name);
            newrank = i + 1;
        }
    }
    for (int i = 0; i < MAX_SCORE; i++)
    {
        if (highscores[i].pts == 0 && strcmp(highscores[i].name, "") == 0)
            continue; // Skip empty entries
        fprintf(fpw, "%s %d\n", highscores[i].name, highscores[i].pts);
    }
    fclose(fpw);
}

void loadData(void)
{
    // Initialize all slots as empty first
    for (int i = 0; i < MAX_SLOT; i++)
    {
        strcpy(savedData[i].timestamp, "");
        savedData[i].score = 0;
        savedData[i].level = 0;
        savedData[i].lives = 0;
        for (int j = 0; j < 15; j++)
        {
            for (int k = 0; k < 15; k++)
            {
                savedData[i].blockState[j][k] = 0;
            }
        }
    }

    // read and load
    savefile = fopen("saves/savedgame.txt", "r");
    if (savefile == NULL)
    {
        printf("Error opening save file.\n");
        return;
    }

    for (int i = 0; i < MAX_SLOT; i++)
    {
        char tempTimestamp[30];
        if (fscanf(savefile, "%s", tempTimestamp) == 1)
        {
            if (strcmp(tempTimestamp, "EMPTY") == 0)
            {
                // This is an empty slot, read and discard the placeholder data
                int tempScore, tempLevel, tempLives;
                fscanf(savefile, "%d %d %d", &tempScore, &tempLevel, &tempLives);
                for (int j = 0; j < 15; j++)
                {
                    for (int k = 0; k < 15; k++)
                    {
                        int tempBlock;
                        fscanf(savefile, "%d", &tempBlock);
                    }
                }
                // Keep the slot empty (already initialized above)
            }
            else
            {
                // This is a real save, load the data
                strcpy(savedData[i].timestamp, tempTimestamp);
                fscanf(savefile, "%d %d %d", &savedData[i].score, &savedData[i].level, &savedData[i].lives);
                for (int j = 0; j < 15; j++)
                {
                    for (int k = 0; k < 15; k++)
                    {
                        fscanf(savefile, "%d", &savedData[i].blockState[j][k]);
                    }
                }
            }
        }
        else
        {
            // No more data to read, remaining slots stay empty
            break;
        }
    }
    fclose(savefile);
}

void displaySavedGames(void)
{
    loadData();
    int line = 0;
    iShowImage(0, 0, "assets/images/mainmenublurred.jpg");
    iSetColor(255, 255, 255);
    iTextTTF(100, screen_height - 100 - line * 40, "Saved Games:", "assets/fonts/Bungee-Regular.ttf", 64);
    line += 3;

    iTextTTF(100, screen_height - 100 - line * 40, "SLOT", "assets/fonts/Bungee-Regular.ttf", 30);
    iTextTTF(300, screen_height - 100 - line * 40, "TIME", "assets/fonts/Bungee-Regular.ttf", 30);
    line += 2;
    for (int i = 0; i < MAX_SLOT; i++)
    {
        char i_str[3];
        sprintf(i_str, "%d", i + 1);
        iTextTTF(100, screen_height - 100 - line * 40, i_str, "assets/fonts/Bungee-Regular.ttf", 30);
        if (i + 1 == selected_menu_idx)
            iSetColor(245, 245, 67);
        if (strlen(savedData[i].timestamp))
        {
            char modified_timestamp[25];
            strcpy(modified_timestamp, savedData[i].timestamp);
            modified_timestamp[strlen(modified_timestamp)] = '\0';
            iTextTTF(300, screen_height - 100 - line * 40, modified_timestamp, "assets/fonts/Bungee-Regular.ttf", 30);
        }
        else
        {
            iTextTTF(300, screen_height - 100 - line * 40, "<EMPTY_SLOT>", "assets/fonts/Bungee-Regular.ttf", 30);
        }
        iSetColor(255, 255, 255);
        line++;
    }
    iSetColor(255, 255, 255);
    iTextTTF(60, 30, "Controls: W/S = Navigate | D = Delete Slot | ENTER = Select | ESC = Back", "assets/fonts/RubikDoodleShadow-Regular.ttf", 25);
}

int saveGame(int slotIdx)
{
    loadData();

    time_t t = time(NULL);
    char *time_s = ctime(&t);
    time_s[3] = '_';
    time_s[7] = '_';
    time_s[10] = '_';
    time_s[19] = '_';
    time_s[24] = '\0';
    strcpy(savedData[slotIdx].timestamp, time_s);
    savedData[slotIdx].score = score;
    savedData[slotIdx].level = level;
    savedData[slotIdx].lives = lives;
    for (int j = 0; j < 15; j++)
    {
        for (int k = 0; k < 15; k++)
        {
            savedData[slotIdx].blockState[j][k] = blockGrid[j][k];
        }
    }

    // fprintf to file with consistent format
    savefile = fopen("saves/savedgame.txt", "w");
    if (savefile == NULL)
    {
        printf("Error opening save file for writing.\n");
        return 0;
    }
    for (int j = 0; j < MAX_SLOT; j++)
    {
        if (strlen(savedData[j].timestamp))
        {
            fprintf(savefile, "%s %d %d %d ", savedData[j].timestamp, savedData[j].score, savedData[j].level, savedData[j].lives);
            for (int k = 0; k < 15; k++)
            {
                for (int l = 0; l < 15; l++)
                {
                    fprintf(savefile, "%d ", savedData[j].blockState[k][l]);
                }
            }
            fprintf(savefile, "\n");
        }
        else
        {
            // Write empty slot placeholder
            fprintf(savefile, "EMPTY 0 0 0 ");
            for (int k = 0; k < 15; k++)
            {
                for (int l = 0; l < 15; l++)
                {
                    fprintf(savefile, "0 ");
                }
            }
            fprintf(savefile, "\n");
        }
    }

    fclose(savefile);
    return 1;
}

void deleteSlot(int slotIndex)
{
    if (slotIndex < 0 || slotIndex >= MAX_SLOT)
    {
        printf("Invalid slot index: %d\n", slotIndex);
        return;
    }

    // Load current data
    loadData();

    // Check if slot has data
    if (!strlen(savedData[slotIndex].timestamp))
    {
        printf("Slot %d is already empty.\n", slotIndex + 1);
        return;
    }

    // Clear the slot data
    strcpy(savedData[slotIndex].timestamp, "");
    savedData[slotIndex].score = 0;
    savedData[slotIndex].level = 0;
    savedData[slotIndex].lives = 0;

    // Clear block state
    for (int j = 0; j < 15; j++)
    {
        for (int k = 0; k < 15; k++)
        {
            savedData[slotIndex].blockState[j][k] = 0;
        }
    }

    // Rewrite the save file with all slots (including empty ones)
    savefile = fopen("saves/savedgame.txt", "w");
    if (savefile == NULL)
    {
        printf("Error opening save file for deletion.\n");
        return;
    }

    // Write all slots, including empty ones with placeholder data
    for (int j = 0; j < MAX_SLOT; j++)
    {
        if (strlen(savedData[j].timestamp))
        {
            // Write actual save data
            fprintf(savefile, "%s %d %d %d ", savedData[j].timestamp, savedData[j].score, savedData[j].level, savedData[j].lives);
            for (int k = 0; k < 15; k++)
            {
                for (int l = 0; l < 15; l++)
                {
                    fprintf(savefile, "%d ", savedData[j].blockState[k][l]);
                }
            }
            fprintf(savefile, "\n");
        }
        else
        {
            // Write empty slot placeholder
            fprintf(savefile, "EMPTY 0 0 0 ");
            for (int k = 0; k < 15; k++)
            {
                for (int l = 0; l < 15; l++)
                {
                    fprintf(savefile, "0 ");
                }
            }
            fprintf(savefile, "\n");
        }
    }

    fclose(savefile);
    printf("Slot %d deleted successfully!\n", slotIndex + 1);
}

void endgame(void)
{
    isGameOver = true;
    gameState = 2;
    creditsroll = 1;
}

void displayCredits(void)
{
    static int framedelay = 0;
    framedelay++;
    if (framedelay >= 3)
    {
        framedelay = 0;
        creditscounter++;
    }
    double alpha;
    if (creditscounter > 0 && creditscounter < 450)
    {
        alpha = (creditscounter > 450) ? 1.0 : (double)creditscounter / 200.0;
        int colorR = (int)(255 * alpha);
        int colorG = (int)(255 * alpha);
        int colorB = (int)(255 * alpha);
        iSetColor(colorR, colorG, 0);
        iTextTTF(320, 400, "A Project by", "assets/fonts/BitcountGridDouble_Cursive-Light.ttf", 48);
        iSetColor(colorR, colorG, colorB);
        iTextTTF(390, 350, "nahid 2405051", "assets/fonts/Bungee-Regular.ttf", 24);
        iTextTTF(390, 300, "rafid 2405042", "assets/fonts/Bungee-Regular.ttf", 24);
    }

    if (creditscounter >= 550 && creditscounter < 1000)
    {
        alpha = ((creditscounter - 550) > 100) ? 1.0 : (double)(creditscounter - 550) / 100.0;
        int colorR = (int)(255 * alpha);
        int colorG = (int)(255 * alpha);
        int colorB = (int)(255 * alpha);
        iSetColor(colorR, colorG, 0);
        iTextTTF(275, 400, "Inspiration from", "assets/fonts/BitcountGridDouble_Cursive-Light.ttf", 48);
        iSetColor(colorR, colorG, colorB);
        iTextTTF(425, 350, "Arkanoid", "assets/fonts/Bungee-Regular.ttf", 24);
        iTextTTF(415, 300, "Undertale", "assets/fonts/Bungee-Regular.ttf", 24);
    }
    if (creditscounter >= 1100 && creditscounter < 1550)
    {
        alpha = ((creditscounter - 1100) > 100) ? 1.0 : (double)(creditscounter - 1100) / 100.0;
        int colorR = (int)(255 * alpha);
        int colorG = (int)(255 * alpha);
        int colorB = (int)(255 * alpha);
        iSetColor(colorR, colorG, 0);
        iTextTTF(305, 400, "Resources from", "assets/fonts/BitcountGridDouble_Cursive-Light.ttf", 48);
        iSetColor(colorR, colorG, colorB);
        iTextTTF(330, 350, "non-copyright websites", "assets/fonts/Bungee-Regular.ttf", 24);
        iTextTTF(425, 300, "Undertale", "assets/fonts/Bungee-Regular.ttf", 24);
    }
    if (creditscounter >= 1650 && creditscounter <= 2210)
    {
        alpha = ((creditscounter - 1650) > 100) ? 1.0 : (double)(creditscounter - 1650) / 100.0;
        int colorR = (int)(255 * alpha);
        int colorG = (int)(255 * alpha);
        int colorB = (int)(255 * alpha);
        iSetColor(colorR, colorG, 0);
        iTextTTF(265, 400, "Special Thanks to", "assets/fonts/BitcountGridDouble_Cursive-Light.ttf", 48);
        iSetColor(colorR, colorG, colorB);
        iTextTTF(475, 350, "YOU", "assets/fonts/Bungee-Regular.ttf", 24);
        iTextTTF(390, 300, "For enjoying it.", "assets/fonts/Bungee-Regular.ttf", 24);
    }
    if (creditscounter > 2300)
    {
        creditscounter = 0;
        framedelay = 0;
        gameState = 0; // go back to main menu
        creditsroll = 0;
        level = 1;
        iStopAllSounds();
        iPlaySound("assets/sounds/mus_menu6.wav", true, 40);
    }
    creditscounter++;
}
