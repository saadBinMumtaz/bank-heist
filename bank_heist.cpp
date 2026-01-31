#include <iostream>
#include <windows.h>
#include <iomanip>
#include <mmsystem.h>
#include <conio.h>
#include <ctime>
#include <cstdlib>
#include <string>
#pragma comment(lib, "winmm.lib")

using namespace std;

// Definitions
#define SCREEN_WIDTH 150
#define SCREEN_HEIGHT 43
#define MAZE_WIDTH (SCREEN_WIDTH * 8 / 10)
#define MAZE_HEIGHT (SCREEN_HEIGHT * 8 / 10)
#define ROBBER_WIDTH 5
#define ROBBER_HEIGHT 3
#define PLATFORM_LENGTH 10
#define BANK_LEFT 2
#define BANK_RIGHT 17
#define BANK_TOP 24
#define BANK_BOTTOM 34
#define VAULT_X 45
#define VAULT_Y 15
#define VAULT_WIDTH 15
#define VAULT_HEIGHT 10
#define LASER_LENGTH 15
#define NUM_LASERS 8
#define NUM_COINS 20
#define RIDDLE_TIME_LIMIT 60
#define COIN_TIME_LIMIT 120

const int MAZE_WIDTH3 = 81;
const int MAZE_HEIGHT3 = 25;
const int TIMER_SECONDS = 60;

// Structure for position
struct Position
{
    int x, y;
};

// Game state for keys (maze level)
struct MazeGameState
{
    int keysCollected;
    bool key1Found;
    bool key2Found;
    bool key3Found;
};

struct Entity
{
    int x, y, dx, dy;
    bool active;
    int health;
};

struct Platform
{
    int x, y, dx;
};

struct GameState
{
    int keys, health, score;
    bool levelComplete;
    int animFrame;
};

struct Laser
{
    int x, y, direction;
    bool active;
};

struct Level2State
{
    int coinsCollected;
    bool vaultUnlocked, gameWon, gameLost;
    time_t startTime;
};

struct Riddle
{
    string question, answer, hint;
};
bool visited[MAZE_HEIGHT3][MAZE_WIDTH3] = {false};

int maze[MAZE_HEIGHT3][MAZE_WIDTH3] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};

// Function prototypes
void setColor(int color);
void hideCursor();
void gotoxy(int x, int y);
char getCharAtxy(short int x, short int y);
void playSound(const string &soundType);
void printBg();
void printBank();
void drawUI(GameState &state);
void printPlatform(Platform &p);
void printRobber(Entity &robber, int frame);
void eraseRobber(Entity &robber);
void moveRobberRight(Entity &robber);
void moveRobberLeft(Entity &robber);
void jumpRobber(Entity &robber, bool &isJumping, int &jumpCounter);
void printHelicopter(Entity &heli, int frame);
void eraseHelicopter(Entity &heli);
void moveHelicopter(Entity &heli);
void printPolice(Entity &police, int frame);
void erasePolice(Entity &police);
void movePolice(Entity &police, Platform platforms[], int platformCount, Entity &helicopter);
void printBox(Entity &box);
void eraseBox(Entity &box);
void moveBox(Entity &box, Platform platforms[], int platformCount, Entity &helicopter);
void printBullet(Entity &bullet);
void eraseBullet(Entity &bullet);
void moveBullet(Entity &bullet);
void shootBullet(Entity bullets[], int &bulletCount, Entity &robber);
void printHeliBullet(Entity &bullet);
void eraseHeliBullet(Entity &bullet);
void moveHeliBullet(Entity &bullet);
void heliShoot(Entity heliBullets[], int &heliBulletCount, Entity &heli);
void printKey(int keyPositions[], int keyPlace, int frame);
void eraseKey(int keyPositions[], int keyPlace);
void keyCollect(int keyPositions[], int &keyPlace, Entity &robber, GameState &state);
void checkBulletCollision(Entity bullets[], int &bulletCount, Entity &police, Entity &box, GameState &state);
void checkHeliBulletCollision(Entity heliBullets[], int &heliBulletCount, Entity &robber, GameState &state);
void checkLevelComplete(Entity &robber, GameState &state, Entity &police, Entity &box);
bool isBankWallChar(char ch);
bool isInsideBank(int x, int y);
bool willCollideWithBank(Entity &r, int newX, int newY);
bool isRobberChar(char ch);
bool canMoveLeft(Entity &r);
bool canMoveRight(Entity &r);
bool canMoveUp(Entity &r);
void erasePlatform(Platform &p);
void movePlatform(Platform &p, int leftLimit, int rightLimit);
bool isStandingOnPlatform(Entity &r, Platform platforms[], int count, int &platformDX);
bool hasGroundBelow(Entity &r);
bool isCollidingWithPlatform(Entity &e, Platform platforms[], int count);
bool isCollidingWithHelicopter(Entity &e, Entity &helicopter);
void printWelcomeScreen();
void printVault();
void clearVault();
void printCoin(Entity &coin);
void clearCoin(Entity &coin);
void printLaser(Laser &laser);
void clearLaser(Laser &laser);
void printExitDoor();
void drawLevel2UI(GameState &state, Level2State &l2state, int timeRemaining);
void clearRobber(Entity &robber);
Riddle getRiddle();
bool solveVaultRiddle();
void playCoinSound();
void playLaserSound();
void drawMaze();
void drawRobber(Position pos, int frame);
void drawTimer(int timeLeft);
void playBeep(int frequency, int duration);
bool checkWin(Position robber, Position car);
void winAnimation(Position robber, Position car);
void loseAnimation(Position robber);
void screenFlash();
void drawUI3();
void drawKeyStatus(MazeGameState &state);
void printVaultMaze();

// IMPLEMENTATIONS
void setColor(int color)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void hideCursor()
{
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO ci;
    GetConsoleCursorInfo(h, &ci);
    ci.bVisible = false;
    SetConsoleCursorInfo(h, &ci);
}

void gotoxy(int x, int y)
{
    COORD p = {(SHORT)x, (SHORT)y};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), p);
}

char getCharAtxy(short int x, short int y)
{
    CHAR_INFO ci;
    COORD xy = {0, 0};
    SMALL_RECT rect = {x, y, x, y};
    COORD sz = {1, 1};
    return ReadConsoleOutput(GetStdHandle(STD_OUTPUT_HANDLE), &ci, sz, xy, &rect) ? ci.Char.AsciiChar : ' ';
}

void printWelcomeScreen()
{
    system("cls");
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    setColor(14);
    string t="\t\t\t\t\t\t";
    cout << "\n\n\n\n\n\n\n\n\n\n\n";
    cout << t <<"██████╗  █████╗ ███╗   ██╗██╗  ██╗    ██╗  ██╗███████╗██╗███████╗████████╗" << endl;
    cout << t <<"██╔══██╗██╔══██╗████╗  ██║██║ ██╔╝    ██║  ██║██╔════╝██║██╔════╝╚══██╔══╝" << endl;
    cout << t <<"██████╔╝███████║██╔██╗ ██║█████╔╝     ███████║█████╗  ██║███████╗   ██║   " << endl;
    cout << t <<"██╔══██╗██╔══██║██║╚██╗██║██╔═██╗     ██╔══██║██╔══╝  ██║╚════██║   ██║   " << endl;
    cout << t <<"██████╔╝██║  ██║██║ ╚████║██║  ██╗    ██║  ██║███████╗██║███████║   ██║   " << endl;
    cout << t <<"╚═════╝ ╚═╝  ╚═╝╚═╝  ╚═══╝╚═╝  ╚═╝    ╚═╝  ╚═╝╚══════╝╚═╝╚══════╝   ╚═╝   " << endl;
    getch();
    
setColor(10);
getch();

cout << "\n\n\n\n" << t <<"\t\t" << "Loading ";
cout<<"\n\n"<<t<<"\t\t";

for (int i = 0; i < 30; i++)
{
    cout << "█";
    Sleep(100);
}


    setColor(14);

    system("cls");
    cout<<"\n\n\n";
    cout << t <<"=============================================\n";
    cout << t <<"               Overview\n";
    cout << t <<"=============================================\n\n";
    
    cout << t <<"One job. Three levels. No second chances.\n\n";
 
    cout << t <<"You are the city's most wanted robber, breaking\n";
    cout << t <<"into the most secure bank ever built.\n";
    cout << t <<"Survive enemies, deadly traps, and the clock\n";
    cout << t <<"or lose everything.\n\n";
  
    cout << t <<"LEVEL 1: ASSAULT\n";
    cout << t <<"- Fight cops and a helicopter drone\n";
    cout << t <<"- Collect 3 keys to enter the bank\n\n";
   
    cout << t <<"LEVEL 2: VAULT\n";
    cout << t <<"- Dodge deadly laser traps\n";
    cout << t <<"- Solve the vault riddle\n";
    cout << t <<"- Collect 20 coins before the alarm\n\n";
    
    cout << t <<"LEVEL 3: ESCAPE\n";
    cout << t <<"- Escape the maze in 60 seconds\n";
    cout << t <<"- Collect final keys or get caught\n\n";
    
    cout << t <<"Fast. Brutal. Unforgiving.\n";
    cout << t <<"Only legends escape alive.\n\n";

    cout << t <<"Controls: Arrow Keys | Space/S to Shoot | ESC to Quit\n";
    cout << t <<"Difficulty: Brutal\n";
    cout << t <<"=============================================\n";

    _getch();
    system("cls");
}

void playSound(const string &soundType)
{
    if (soundType == "jump")
        Beep(600, 100);
    else if (soundType == "shoot")
        Beep(400, 80);
    else if (soundType == "key")
    {
        Beep(800, 150);
        Beep(1000, 100);
    }
    else if (soundType == "explosion")
    {
        Beep(200, 100);
        Beep(150, 100);
        Beep(100, 100);
    }
    else if (soundType == "hit")
        Beep(300, 50);
    else if (soundType == "damage")
        Beep(250, 150);
}

void playCoinSound()
{
    Beep(800, 100);
}
void playLaserSound()
{
    Beep(1500, 150);
}

bool isBankWallChar(char ch)
{
    return (ch == '|' || ch == '/' || ch == '\\' || ch == '_' || ch == '-' || ch == char(219) || ch == char(254) || ch == '$');
}

bool isInsideBank(int x, int y)
{
    return (x > BANK_LEFT && x < BANK_RIGHT && y > BANK_TOP && y < BANK_BOTTOM);
}

bool willCollideWithBank(Entity &r, int newX, int newY)
{
    for (int i = 0; i < ROBBER_WIDTH; i++)
        for (int j = 0; j < ROBBER_HEIGHT; j++)
            if (isInsideBank(newX + i, newY + j))
                return true;
    return false;
}

bool isRobberChar(char ch)
{
    return (ch == '0' || ch == 'O' || ch == '/' || ch == '\\' || ch == '|' || ch == '-');
}

bool canMoveLeft(Entity &r)
{
    if (willCollideWithBank(r, r.x - 2, r.y))
        return false;
    for (int i = 0; i < ROBBER_HEIGHT; i++)
    {
        char leftChar = getCharAtxy(r.x - 1, r.y + i);
        if (leftChar == char(219) || isBankWallChar(leftChar))
            return false;
    }
    return true;
}

bool canMoveRight(Entity &r)
{
    if (willCollideWithBank(r, r.x + 2, r.y))
        return false;
    for (int i = 0; i < ROBBER_HEIGHT; i++)
    {
        char rightChar = getCharAtxy(r.x + ROBBER_WIDTH, r.y + i);
        if (rightChar == char(219) || isBankWallChar(rightChar))
            return false;
    }
    return true;
}

bool canMoveUp(Entity &r)
{
    if (willCollideWithBank(r, r.x, r.y - 1))
        return false;
    for (int i = 0; i < ROBBER_WIDTH; i++)
    {
        char topChar = getCharAtxy(r.x + i, r.y - 1);
        if (topChar == char(219) || topChar == char(205) || isBankWallChar(topChar))
            return false;
    }
    return true;
}

void erasePlatform(Platform &p)
{
    for (int i = 0; i < PLATFORM_LENGTH; i++)
    {
        char ch = getCharAtxy(p.x + i, p.y);
        if (ch != char(219) && ch != '|' && !isRobberChar(ch) && !isBankWallChar(ch) && ch != 15 && ch != 'o' && ch != char(25) && ch != char(24))
        {
            gotoxy(p.x + i, p.y);
            cout << " ";
        }
    }
}

void movePlatform(Platform &p, int leftLimit, int rightLimit)
{
    erasePlatform(p);
    p.x += p.dx;
    if (p.x <= leftLimit || p.x + PLATFORM_LENGTH >= rightLimit)
        p.dx *= -1;
    printPlatform(p);
}

bool isStandingOnPlatform(Entity &r, Platform platforms[], int count, int &platformDX)
{
    for (int i = 0; i < count; i++)
    {
        if (r.y + ROBBER_HEIGHT == platforms[i].y &&
            r.x + 2 >= platforms[i].x &&
            r.x + 2 <= platforms[i].x + PLATFORM_LENGTH)
        {
            platformDX = platforms[i].dx;
            return true;
        }
    }
    platformDX = 0;
    return false;
}

bool hasGroundBelow(Entity &r)
{
    if (r.y + ROBBER_HEIGHT >= MAZE_HEIGHT)
        return true;
    for (int i = 0; i < ROBBER_WIDTH; i++)
    {
        char below = getCharAtxy(r.x + i, r.y + ROBBER_HEIGHT);
        if (below == char(205) || below == char(219))
            return true;
    }
    return false;
}

bool isCollidingWithPlatform(Entity &e, Platform platforms[], int count)
{
    for (int i = 0; i < count; i++)
    {
        if (e.x < platforms[i].x + PLATFORM_LENGTH && e.x + 5 > platforms[i].x &&
            e.y < platforms[i].y + 1 && e.y + 2 > platforms[i].y - 1)
            return true;
    }
    return false;
}

bool isCollidingWithHelicopter(Entity &e, Entity &helicopter)
{
    if (!helicopter.active)
        return false;
    return (e.x < helicopter.x + 25 && e.x + 5 > helicopter.x &&
            e.y < helicopter.y + 6 && e.y + 2 > helicopter.y);
}

void printBg()
{
    setColor(11);
    for (int x = 0; x < MAZE_WIDTH; x++)
        cout << char(219);
    cout << endl;
    for (int y = 0; y < MAZE_HEIGHT; y++)
    {
        cout << char(219);
        for (int x = 0; x < MAZE_WIDTH - 2; x++)
            cout << " ";
        cout << char(219) << endl;
    }
    for (int x = 0; x < MAZE_WIDTH; x++)
        cout << char(219);
    setColor(15);
}

void printBank()
{
    int x = BANK_LEFT, y = BANK_TOP;
    setColor(6);
    gotoxy(x, y);
    cout << "   " << char(220) << char(220) << char(220) << char(220) << char(220) << char(220) << char(220) << char(220) << char(220) << char(220) << char(220);
    gotoxy(x, y + 1);
    cout << "  " << char(219) << char(220) << char(220) << char(220) << char(220) << char(220) << char(220) << char(220) << char(220) << char(220) << char(220) << char(220) << char(219);
    gotoxy(x, y + 2);
    cout << " " << char(219) << char(220) << char(220) << char(220) << char(220) << char(220) << char(220) << char(220) << char(220) << char(220) << char(220) << char(220) << char(220) << char(220) << char(219);
    setColor(14);
    gotoxy(x, y + 3);
    cout << "  " << char(219) << "           " << char(219);
    gotoxy(x, y + 4);
    cout << "  " << char(219);
    setColor(10);
    cout << "  " << char(36) << " BANK " << char(36) << " ";
    setColor(14);
    cout << char(219);
    gotoxy(x, y + 5);
    cout << "  " << char(219) << "           " << char(219);
    setColor(6);
    gotoxy(x, y + 6);
    cout << "  " << char(219) << char(219) << char(219) << char(219) << char(219) << char(219) << char(219) << char(219) << char(219) << char(219) << char(219) << char(219);
    gotoxy(x, y + 7);
    cout << "  " << char(219) << char(223) << char(223) << char(223) << char(223) << char(223) << char(223) << char(223) << char(223) << char(223) << char(223) << char(219);
    gotoxy(x, y + 8);
    cout << "  " << char(219) << char(219) << char(219) << char(219) << char(219) << char(219) << char(219) << char(219) << char(219) << char(219) << char(219) << char(219);
    gotoxy(x, y + 9);
    cout << "  " << char(219) << char(254) << char(254) << char(254) << char(254) << char(254) << char(254) << char(254) << char(254) << char(254) << char(254) << char(219);
    setColor(15);
}
void printVaultMaze()
{
    int x = MAZE_WIDTH3 + 8, y = 23;
    setColor(6);
    gotoxy(x, y);
    cout << "   " << char(220) << char(220) << char(220) << char(220) << char(220) << char(220) << char(220) << char(220) << char(220) << char(220) << char(220);
    gotoxy(x, y + 1);
    cout << "  " << char(219) << char(220) << char(220) << char(220) << char(220) << char(220) << char(220) << char(220) << char(220) << char(220) << char(220) << char(220) << char(219);
    gotoxy(x, y + 2);
    cout << " " << char(219) << char(220) << char(220) << char(220) << char(220) << char(220) << char(220) << char(220) << char(220) << char(220) << char(220) << char(220) << char(220) << char(220) << char(219);
    setColor(14);
    gotoxy(x, y + 3);
    cout << "  " << char(219) << "           " << char(219);
    gotoxy(x, y + 4);
    cout << "  " << char(219);
    setColor(10);
    cout << "  " << char(36) << " VAULT " << char(36) << " ";
    setColor(14);
    cout << char(219);
    gotoxy(x, y + 5);
    cout << "  " << char(219) << "           " << char(219);
    setColor(6);
    gotoxy(x, y + 6);
    cout << "  " << char(219) << char(219) << char(219) << char(219) << char(219) << char(219) << char(219) << char(219) << char(219) << char(219) << char(219) << char(219);
    gotoxy(x, y + 7);
    cout << "  " << char(219) << char(223) << char(223) << char(223) << char(223) << char(223) << char(223) << char(223) << char(223) << char(223) << char(223) << char(219);
    gotoxy(x, y + 8);
    cout << "  " << char(219) << char(219) << char(219) << char(219) << char(219) << char(219) << char(219) << char(219) << char(219) << char(219) << char(219) << char(219);
    gotoxy(x, y + 9);
    cout << "  " << char(219) << char(254) << char(254) << char(254) << char(254) << char(254) << char(254) << char(254) << char(254) << char(254) << char(254) << char(219);
    setColor(15);
}
void printPlatform(Platform &p)
{
    setColor(14);
    gotoxy(p.x, p.y);
    for (int i = 0; i < PLATFORM_LENGTH; i++)
        cout << char(205);
    setColor(15);
}

void printRobber(Entity &robber, int frame)
{
    setColor(4);
    gotoxy(robber.x, robber.y);
    cout << (frame % 2 == 0 ? " O   " : " 0   ");
    gotoxy(robber.x, robber.y + 1);
    cout << "/|\\";
    gotoxy(robber.x, robber.y + 2);
    cout << (frame % 2 == 0 ? "/ \\ " : " / \\");
    setColor(15);
}

void eraseRobber(Entity &robber)
{
    for (int i = 0; i < 3; i++)
    {
        gotoxy(robber.x, robber.y + i);
        cout << "     ";
    }
}

void clearRobber(Entity &robber)
{
    for (int i = 0; i < 3; i++)
    {
        gotoxy(robber.x, robber.y + i);
        cout << "    ";
    }
}

void moveRobberRight(Entity &robber)
{
    if (canMoveRight(robber))
    {
        eraseRobber(robber);
        robber.x += 2;
        printRobber(robber, 0);
    }
}

void moveRobberLeft(Entity &robber)
{
    if (canMoveLeft(robber))
    {
        eraseRobber(robber);
        robber.x -= 2;
        printRobber(robber, 0);
    }
}

void jumpRobber(Entity &robber, bool &isJumping, int &jumpCounter)
{
    eraseRobber(robber);
    if (jumpCounter < 8)
    {
        if (canMoveUp(robber))
            robber.y -= 1;
        else
            jumpCounter = 8;
    }
    else
    {
        if (!hasGroundBelow(robber))
            robber.y += 1;
        else
        {
            isJumping = false;
            jumpCounter = 0;
        }
    }
    jumpCounter++;
    if (jumpCounter > 16)
    {
        isJumping = false;
        jumpCounter = 0;
    }
    printRobber(robber, 0);
}

void printHelicopter(Entity &heli, int frame)
{
    setColor(8);
    gotoxy(heli.x, heli.y);
    cout << (frame % 2 == 0 ? "      \\:/" : "      /:\\");
    setColor(15);
    gotoxy(heli.x, heli.y + 1);
    cout << "  _____|______    ";
    gotoxy(heli.x, heli.y + 2);
    cout << " /   |        \\   ";
    gotoxy(heli.x, heli.y + 3);
    cout << " \\___|         \\------\\ ";
    gotoxy(heli.x, heli.y + 4);
    cout << "  \\____________ \\ -------\\";
    gotoxy(heli.x, heli.y + 5);
    cout << "       |\\  |\\   ";
    setColor(15);
}

void eraseHelicopter(Entity &heli)
{
    for (int i = 0; i < 6; i++)
    {
        gotoxy(heli.x, heli.y + i);
        cout << "                           ";
    }
}

void moveHelicopter(Entity &heli)
{
    if (!heli.active)
        return;
    eraseHelicopter(heli);
    heli.x += heli.dx;
    if (heli.x < 2)
        heli.x = MAZE_WIDTH - 30;
    if (heli.x > MAZE_WIDTH - 30)
        heli.x = 2;
    printHelicopter(heli, 0);
}

void printPolice(Entity &police, int frame)
{
    setColor(1);
    gotoxy(police.x, police.y);
    cout << (frame % 2 == 0 ? "  (O)  " : "  (@)  ");
    gotoxy(police.x, police.y + 1);
    cout << "  /:\\ ";
    gotoxy(police.x, police.y + 2);
    cout << "  / \\  ";
    setColor(15);
}

void erasePolice(Entity &police)
{
    for (int i = 0; i < 3; i++)
    {
        gotoxy(police.x, police.y + i);
        cout << "       ";
    }
}

void movePolice(Entity &police, Platform platforms[], int platformCount, Entity &helicopter)
{
    erasePolice(police);
    int nextY = police.y + police.dy;
    Entity temp = police;
    temp.y = nextY;
    if (isCollidingWithPlatform(temp, platforms, platformCount) || isCollidingWithHelicopter(temp, helicopter))
        police.dy *= -1;
    police.y += police.dy;
    if (police.y >= MAZE_HEIGHT - 6 || police.y <= 2)
        police.dy *= -1;
    printPolice(police, 0);
}

void printBox(Entity &box)
{
    setColor(6);
    gotoxy(box.x, box.y);
    cout << char(219) << char(254) << char(219) << char(254) << char(219);
    gotoxy(box.x, box.y + 1);
    cout << char(219) << char(196) << char(196) << char(196) << char(219);
    setColor(15);
}

void eraseBox(Entity &box)
{
    for (int i = 0; i < 2; i++)
    {
        gotoxy(box.x, box.y + i);
        cout << "     ";
    }
}

void moveBox(Entity &box, Platform platforms[], int platformCount, Entity &helicopter)
{
    eraseBox(box);
    int nextX = box.x + box.dx, nextY = box.y + box.dy;
    Entity temp = box;
    temp.x = nextX;
    temp.y = nextY;
    if (isCollidingWithPlatform(temp, platforms, platformCount) || isCollidingWithHelicopter(temp, helicopter))
    {
        box.dx *= -1;
        box.dy *= -1;
    }
    box.x += box.dx;
    box.y += box.dy;
    if (box.y <= 5)
    {
        box.dy = 1;
        box.dx = -1;
    }
    else if (box.y >= 22)
    {
        box.dy = -1;
        box.dx = 1;
    }
    if (box.x <= 3 || box.x >= MAZE_WIDTH - 8)
        box.dx *= -1;
    printBox(box);
}

void printBullet(Entity &bullet)
{
    setColor(14);
    gotoxy(bullet.x, bullet.y);
    cout << char(24);
    setColor(15);
}

void eraseBullet(Entity &bullet)
{
    char ch = getCharAtxy(bullet.x, bullet.y);
    if (ch != char(219) && ch != char(205))
    {
        gotoxy(bullet.x, bullet.y);
        cout << " ";
    }
}

void moveBullet(Entity &bullet)
{
    eraseBullet(bullet);
    bullet.y -= 1;
    if (bullet.y < 2)
    {
        bullet.active = false;
        return;
    }
    char nextChar = getCharAtxy(bullet.x, bullet.y);
    if (nextChar == char(219) || nextChar == char(205))
    {
        bullet.active = false;
        return;
    }
    printBullet(bullet);
}

void shootBullet(Entity bullets[], int &bulletCount, Entity &robber)
{
    for (int i = 0; i < 10; i++)
    {
        if (!bullets[i].active)
        {
            bullets[i].x = robber.x + 2;
            bullets[i].y = robber.y - 1;
            bullets[i].active = true;
            bulletCount++;
            break;
        }
    }
}

void printHeliBullet(Entity &bullet)
{
    setColor(12);
    gotoxy(bullet.x, bullet.y);
    cout << char(25);
    setColor(15);
}

void eraseHeliBullet(Entity &bullet)
{
    char ch = getCharAtxy(bullet.x, bullet.y);
    if (ch != char(219) && ch != char(205))
    {
        gotoxy(bullet.x, bullet.y);
        cout << " ";
    }
}

void moveHeliBullet(Entity &bullet)
{
    eraseHeliBullet(bullet);
    bullet.y += 1;
    char checkChar = getCharAtxy(bullet.x, bullet.y);
    if (bullet.y > MAZE_HEIGHT - 2 || checkChar == char(219) || checkChar == char(205) ||
        checkChar == '_' || checkChar == '\\' || checkChar == '/')
    {
        bullet.active = false;
        eraseHeliBullet(bullet);
        return;
    }
    printHeliBullet(bullet);
}

void heliShoot(Entity heliBullets[], int &heliBulletCount, Entity &heli)
{
    for (int i = 0; i < 5; i++)
    {
        if (!heliBullets[i].active)
        {
            heliBullets[i].x = heli.x + 10;
            heliBullets[i].y = heli.y + 6;
            heliBullets[i].active = true;
            heliBulletCount++;
            break;
        }
    }
}

void printKey(int keyPositions[], int keyPlace, int frame)
{
    int x = keyPositions[keyPlace * 2];
    int y = keyPositions[keyPlace * 2 + 1];
    if (frame % 2 == 0)
        setColor(14);
    else
        setColor(11);
    gotoxy(x, y);
    cout << char(15) << char(196);
    gotoxy(x, y + 1);
    cout << "o ";
    setColor(15);
}

void eraseKey(int keyPositions[], int keyPlace)
{
    int x = keyPositions[keyPlace * 2];
    int y = keyPositions[keyPlace * 2 + 1];
    gotoxy(x, y);
    cout << "  ";
    gotoxy(x, y + 1);
    cout << "  ";
}

void keyCollect(int keyPositions[], int &keyPlace, Entity &robber, GameState &state)
{
    if (keyPlace >= 3)
        return;
    int kX = keyPositions[keyPlace * 2];
    int kY = keyPositions[keyPlace * 2 + 1];
    printKey(keyPositions, keyPlace, state.animFrame);
    if (robber.x < kX + 3 && robber.x + ROBBER_WIDTH > kX &&
        robber.y < kY + 2 && robber.y + ROBBER_HEIGHT > kY)
    {
        eraseKey(keyPositions, keyPlace);
        keyPlace++;
        state.keys++;
        state.score += 30;
        playSound("key");
    }
}

void checkBulletCollision(Entity bullets[], int &bulletCount, Entity &police, Entity &box, GameState &state)
{
    for (int i = 0; i < 10; i++)
    {
        if (!bullets[i].active)
            continue;
        if (police.active && bullets[i].x >= police.x && bullets[i].x <= police.x + 6 &&
            bullets[i].y >= police.y && bullets[i].y <= police.y + 2)
        {
            eraseBullet(bullets[i]);
            bullets[i].active = false;
            police.health--;
            playSound("hit");
            if (police.health <= 0)
            {
                erasePolice(police);
                police.active = false;
                state.score += 50;
                playSound("explosion");
            }
        }
        if (box.active && bullets[i].x >= box.x && bullets[i].x <= box.x + 4 &&
            bullets[i].y >= box.y && bullets[i].y <= box.y + 1)
        {
            eraseBullet(bullets[i]);
            bullets[i].active = false;
            box.health--;
            playSound("hit");
            if (box.health <= 0)
            {
                eraseBox(box);
                box.active = false;
                state.score += 50;
                playSound("explosion");
            }
        }
    }
}

void checkHeliBulletCollision(Entity heliBullets[], int &heliBulletCount, Entity &robber, GameState &state)
{
    for (int i = 0; i < 5; i++)
    {
        if (!heliBullets[i].active)
            continue;

        if (heliBullets[i].x >= robber.x && heliBullets[i].x <= robber.x + ROBBER_WIDTH &&
            heliBullets[i].y >= robber.y && heliBullets[i].y <= robber.y + ROBBER_HEIGHT)
        {
            eraseHeliBullet(heliBullets[i]);
            heliBullets[i].active = false;
            state.health -= 10;
            playSound("damage");
        }
    }
}

void checkLevelComplete(Entity &robber, GameState &state, Entity &police, Entity &box)
{
    // Must complete ALL objectives: kill drone (box), kill police, collect 3 keys, then touch bank
    if (state.keys >= 3 && !police.active && !box.active)
    {
        // Check if robber is near or touching the bank
        bool nearBank = false;
        for (int i = 0; i < ROBBER_WIDTH; i++)
        {
            for (int j = 0; j < ROBBER_HEIGHT; j++)
            {
                int checkX = robber.x + i;
                int checkY = robber.y + j;

                // Check if inside bank area
                if (checkX >= BANK_LEFT && checkX <= BANK_RIGHT &&
                    checkY >= BANK_TOP && checkY <= BANK_BOTTOM)
                {
                    nearBank = true;
                    break;
                }

                // Check if touching bank walls (proximity)
                if (checkX >= BANK_LEFT - 3 && checkX <= BANK_RIGHT + 3 &&
                    checkY >= BANK_TOP - 2 && checkY <= BANK_BOTTOM + 2)
                {
                    char nearBankChar = getCharAtxy(checkX, checkY);
                    if (isBankWallChar(nearBankChar))
                    {
                        nearBank = true;
                        break;
                    }
                }
            }
            if (nearBank)
                break;
        }

        if (nearBank)
        {
            // Show completion message
            setColor(10);
            gotoxy(35, 15);
            cout << "                                        ";
            gotoxy(35, 15);
            cout << char(16) << " ALL OBJECTIVES COMPLETE! ENTERING BANK! " << char(17);
            Sleep(1000);
            state.levelComplete = true;
        }
    }
    else if (state.keys >= 3)
    {
        // Has keys but enemies still alive - show reminder
        static int reminderTimer = 0;
        reminderTimer++;
        if (reminderTimer % 50 == 0) // Show every ~5 seconds
        {
            setColor(14);
            gotoxy(25, 18);
            cout << "                                                  ";
            gotoxy(25, 18);
            if (police.active && box.active)
                cout << char(16) << " Eliminate the DRONE and POLICE first! " << char(17);
            else if (police.active)
                cout << char(16) << " Eliminate the POLICE first! " << char(17);
            else if (box.active)
                cout << char(16) << " Eliminate the DRONE first! " << char(17);
        }
    }
}

void drawUI(GameState &state)
{
    int uiX = MAZE_WIDTH + 2, uiY = 2;
    setColor(13);
    gotoxy(uiX, uiY);
    cout << char(201);
    for (int i = 0; i < 30; i++)
        cout << char(205);
    cout << char(187);
    gotoxy(uiX, uiY + 1);
    cout << char(186);
    setColor(14);
    cout << "     " << char(2) << " BANK HEIST STATUS " << char(2) << "    ";
    setColor(13);
    cout << char(186);
    gotoxy(uiX, uiY + 2);
    cout << char(204);
    for (int i = 0; i < 30; i++)
        cout << char(205);
    cout << char(185);
    gotoxy(uiX, uiY + 3);
    cout << char(186);
    setColor(11);
    cout << "  " << char(3) << " HEALTH ";
    setColor(15);
    int healthBars = state.health / 10;
    for (int i = 0; i < healthBars; i++)
    {
        if (state.health >= 70)
            setColor(10);
        else if (state.health >= 40)
            setColor(14);
        else
            setColor(12);
        cout << char(219);
    }
    setColor(8);
    for (int i = healthBars; i < 10; i++)
        cout << char(176);
    setColor(13);
    cout << "  ";
    gotoxy(uiX + 31, uiY + 3);
    cout << char(186);
    gotoxy(uiX, uiY + 4);
    cout << char(186);
    setColor(15);
    cout << "           ";
    if (state.health >= 70)
        setColor(10);
    else if (state.health >= 40)
        setColor(14);
    else
        setColor(12);
    cout << setw(3) << state.health << "%";
    setColor(15);
    cout << "            ";
    setColor(13);
    gotoxy(uiX + 31, uiY + 4);
    cout << char(186);
    gotoxy(uiX, uiY + 5);
    cout << char(186);
    setColor(14);
    cout << "  " << char(15) << " KEYS COLLECTED        ";
    setColor(13);
    gotoxy(uiX + 31, uiY + 5);
    cout << char(186);
    gotoxy(uiX, uiY + 6);
    cout << char(186) << "     ";
    for (int i = 0; i < 3; i++)
    {
        if (i < state.keys)
        {
            setColor(14);
            cout << char(15);
        }
        else
        {
            setColor(8);
            cout << char(15);
        }
        cout << " ";
    }
    setColor(15);
    cout << "[";
    setColor(14);
    cout << state.keys;
    setColor(15);
    cout << "/3]       ";
    setColor(13);
    gotoxy(uiX + 31, uiY + 6);
    cout << char(186);
    gotoxy(uiX, uiY + 7);
    cout << char(186);
    setColor(10);
    cout << "  " << char(4) << " SCORE                 ";
    setColor(13);
    gotoxy(uiX + 31, uiY + 7);
    cout << char(186);
    gotoxy(uiX, uiY + 8);
    cout << char(186);
    setColor(14);
    cout << "      " << setw(8) << state.score << " pts     ";
    setColor(13);
    gotoxy(uiX + 31, uiY + 8);
    cout << char(186);
    gotoxy(uiX, uiY + 9);
    cout << char(204);
    for (int i = 0; i < 30; i++)
        cout << char(205);
    cout << char(185);
    gotoxy(uiX, uiY + 10);
    cout << char(186);
    setColor(11);
    cout << "        CONTROLS          ";
    setColor(13);
    gotoxy(uiX + 31, uiY + 10);
    cout << char(186);
    gotoxy(uiX, uiY + 11);
    cout << char(204);
    for (int i = 0; i < 30; i++)
        cout << char(205);
    cout << char(185);
    gotoxy(uiX, uiY + 12);
    cout << char(186);
    setColor(15);
    cout << "  ";
    setColor(14);
    cout << char(27) << "/" << char(26);
    setColor(15);
    cout << "       : Move         ";
    setColor(13);
    gotoxy(uiX + 31, uiY + 12);
    cout << char(186);
    gotoxy(uiX, uiY + 13);
    cout << char(186);
    setColor(15);
    cout << "  ";
    setColor(14);
    cout << "SPACE";
    setColor(15);
    cout << "     : Jump         ";
    setColor(13);
    gotoxy(uiX + 31, uiY + 13);
    cout << char(186);
    gotoxy(uiX, uiY + 14);
    cout << char(186);
    setColor(15);
    cout << "  ";
    setColor(14);
    cout << "S";
    setColor(15);
    cout << "         : Shoot        ";
    setColor(13);
    gotoxy(uiX + 31, uiY + 14);
    cout << char(186);
    gotoxy(uiX, uiY + 15);
    cout << char(204);
    for (int i = 0; i < 30; i++)
        cout << char(205);
    cout << char(185);
    gotoxy(uiX, uiY + 16);
    cout << char(186);
    setColor(10);
    cout << " " << char(16) << " Defeat all enemies!    ";
    setColor(13);
    gotoxy(uiX + 31, uiY + 16);
    cout << char(186);
    gotoxy(uiX, uiY + 17);
    cout << char(186);
    setColor(14);
    cout << " " << char(16) << " Collect 3 keys!        ";
    setColor(13);
    gotoxy(uiX + 31, uiY + 17);
    cout << char(186);
    gotoxy(uiX, uiY + 18);
    cout << char(186);
    setColor(12);
    cout << " " << char(16) << " Touch the bank!        ";
    setColor(13);
    gotoxy(uiX + 31, uiY + 18);
    cout << char(186);
    gotoxy(uiX, uiY + 19);
    cout << char(200);
    for (int i = 0; i < 30; i++)
        cout << char(205);
    cout << char(188);
    setColor(15);
}
// LEVEL 2 FUNCTIONS

void printVault()
{
    int x = VAULT_X, y = VAULT_Y;
    setColor(6);
    gotoxy(x, y);
    cout << "   " << char(220) << char(220) << char(220) << char(220) << char(220) << char(220) << char(220) << char(220) << char(220) << char(220) << char(220);
    gotoxy(x, y + 1);
    cout << "  " << char(219) << char(220) << char(220) << char(220) << char(220) << char(220) << char(220) << char(220) << char(220) << char(220) << char(220) << char(220) << char(219);
    gotoxy(x, y + 2);
    cout << " " << char(219) << char(220) << char(220) << char(220) << char(220) << char(220) << char(220) << char(220) << char(220) << char(220) << char(220) << char(220) << char(220) << char(220) << char(219);
    setColor(14);
    gotoxy(x, y + 3);
    cout << "  " << char(219) << "           " << char(219);
    gotoxy(x, y + 4);
    cout << "  " << char(219);
    setColor(10);
    cout << "  " << char(36) << " VAULT " << char(36) << " ";
    setColor(14);
    cout << char(219);
    gotoxy(x, y + 5);
    cout << "  " << char(219) << "           " << char(219);
    setColor(6);
    gotoxy(x, y + 6);
    cout << "  " << char(219) << char(219) << char(219) << char(219) << char(219) << char(219) << char(219) << char(219) << char(219) << char(219) << char(219) << char(219);
    gotoxy(x, y + 7);
    cout << "  " << char(219) << char(223) << char(223) << char(223) << char(223) << char(223) << char(223) << char(223) << char(223) << char(223) << char(223) << char(219);
    gotoxy(x, y + 8);
    cout << "  " << char(219) << char(219) << char(219) << char(219) << char(219) << char(219) << char(219) << char(219) << char(219) << char(219) << char(219) << char(219);
    gotoxy(x, y + 9);
    cout << "  " << char(219) << char(254) << char(254) << char(254) << char(254) << char(254) << char(254) << char(254) << char(254) << char(254) << char(254) << char(219);
    setColor(15);
}

void clearVault()
{
    int x = VAULT_X, y = VAULT_Y;
    for (int i = 0; i < 10; i++)
    {
        gotoxy(x, y + i);
        cout << "                ";
    }
}

void printCoin(Entity &coin)
{
    if (coin.active)
    {
        setColor(14);
        gotoxy(coin.x, coin.y);
        cout << char(36);
        setColor(15);
    }
}

void clearCoin(Entity &coin)
{
    gotoxy(coin.x, coin.y);
    cout << " ";
}

void printLaser(Laser &laser)
{
    if (laser.active)
    {
        setColor(12);
        gotoxy(laser.x, laser.y);
        for (int i = 0; i < LASER_LENGTH; i++)
            cout << char(205);
        setColor(15);
    }
}

void clearLaser(Laser &laser)
{
    gotoxy(laser.x, laser.y);
    for (int i = 0; i < LASER_LENGTH; i++)
        cout << " ";
}

void printExitDoor()
{
    int x = MAZE_WIDTH - 15, y = MAZE_HEIGHT - 8;
    setColor(15);
    for (int i = 0; i < 8; i++)
    {
        gotoxy(x, y + i);
        cout << char(219) << char(219);
    }
    for (int i = 0; i < 8; i++)
    {
        gotoxy(x + 8, y + i);
        cout << char(219) << char(219);
    }
    gotoxy(x, y);
    for (int i = 0; i < 10; i++)
        cout << char(219);
    gotoxy(x + 2, y + 3);
    setColor(10);
    cout << " EXIT ";
    gotoxy(x + 2, y + 4);
    cout << " DOOR ";
    setColor(15);
}

void drawLevel2UI(GameState &state, Level2State &l2state, int timeRemaining)
{
    int uiX = MAZE_WIDTH + 2, uiY = 2;
    setColor(13);
    gotoxy(uiX, uiY);
    cout << char(201);
    for (int i = 0; i < 30; i++)
        cout << char(205);
    cout << char(187);
    gotoxy(uiX, uiY + 1);
    cout << char(186);
    setColor(14);
    cout << "   " << char(2) << " BANK HEIST STATUS " << char(2) << "   ";
    setColor(13);
    gotoxy(uiX + 31, uiY + 1);
    cout << char(186);
    gotoxy(uiX, uiY + 2);
    cout << char(204);
    for (int i = 0; i < 30; i++)
        cout << char(205);
    cout << char(185);
    gotoxy(uiX, uiY + 3);
    cout << char(186);
    setColor(11);
    cout << "  " << char(3) << " HEALTH ";
    setColor(15);
    int healthBars = state.health / 10;
    for (int i = 0; i < healthBars; i++)
    {
        if (state.health >= 70)
            setColor(10);
        else if (state.health >= 40)
            setColor(14);
        else
            setColor(12);
        cout << char(219);
    }
    setColor(8);
    for (int i = healthBars; i < 10; i++)
        cout << char(176);
    setColor(13);
    cout << "  ";
    gotoxy(uiX + 31, uiY + 3);
    cout << char(186);
    gotoxy(uiX, uiY + 4);
    cout << char(186);
    setColor(15);
    cout << "           ";
    if (state.health >= 70)
        setColor(10);
    else if (state.health >= 40)
        setColor(14);
    else
        setColor(12);
    cout << setw(3) << state.health << "%";
    setColor(15);
    cout << "            ";
    setColor(13);
    gotoxy(uiX + 31, uiY + 4);
    cout << char(186);
    gotoxy(uiX, uiY + 5);
    cout << char(186);
    setColor(14);
    cout << "  " << char(36) << " COINS COLLECTED       ";
    setColor(13);
    gotoxy(uiX + 31, uiY + 5);
    cout << char(186);
    gotoxy(uiX, uiY + 6);
    cout << char(186) << "     ";
    setColor(14);
    cout << "[" << l2state.coinsCollected << "/" << NUM_COINS << "]";
    setColor(15);
    if (l2state.coinsCollected < 10)
        cout << "               ";
    else
        cout << "              ";
    setColor(13);
    gotoxy(uiX + 31, uiY + 6);
    cout << char(186);
    gotoxy(uiX, uiY + 7);
    cout << char(186);
    setColor(10);
    cout << "  " << char(4) << " SCORE                 ";
    setColor(13);
    gotoxy(uiX + 31, uiY + 7);
    cout << char(186);
    gotoxy(uiX, uiY + 8);
    cout << char(186);
    setColor(14);
    cout << "      " << setw(8) << state.score << " pts   ";
    setColor(13);
    gotoxy(uiX + 31, uiY + 8);
    cout << char(186);
    if (timeRemaining >= 0)
    {
        gotoxy(uiX, uiY + 9);
        cout << char(186);
        setColor(12);
        cout << "  " << char(1) << " TIME LEFT             ";
        setColor(13);
        gotoxy(uiX + 31, uiY + 9);
        cout << char(186);
        gotoxy(uiX, uiY + 10);
        cout << char(186);
        if (timeRemaining <= 10)
            setColor(12);
        else if (timeRemaining <= 30)
            setColor(14);
        else
            setColor(10);
        cout << "       " << setw(3) << timeRemaining << " seconds      ";
        setColor(13);
        gotoxy(uiX + 31, uiY + 10);
        cout << char(186);
        gotoxy(uiX, uiY + 11);
    }
    else
    {
        gotoxy(uiX, uiY + 9);
    }
    cout << char(204);
    for (int i = 0; i < 30; i++)
        cout << char(205);
    cout << char(185);
    int yOffset = (timeRemaining >= 0) ? 12 : 10;
    gotoxy(uiX, uiY + yOffset);
    cout << char(186);
    setColor(11);
    cout << "        CONTROLS          ";
    setColor(13);
    gotoxy(uiX + 31, uiY + yOffset);
    cout << char(186);
    gotoxy(uiX, uiY + yOffset + 1);
    cout << char(204);
    for (int i = 0; i < 30; i++)
        cout << char(205);
    cout << char(185);
    gotoxy(uiX, uiY + yOffset + 2);
    cout << char(186);
    setColor(15);
    cout << "  ";
    setColor(14);
    cout << char(24) << char(25) << char(26) << char(27);
    setColor(15);
    cout << "    : Move           ";
    setColor(13);
    gotoxy(uiX + 31, uiY + yOffset + 2);
    cout << char(186);
    gotoxy(uiX, uiY + yOffset + 3);
    cout << char(186);
    setColor(15);
    cout << "  ";
    setColor(14);
    cout << "ESC";
    setColor(15);
    cout << "      : Exit Game     ";
    setColor(13);
    gotoxy(uiX + 31, uiY + yOffset + 3);
    cout << char(186);
    gotoxy(uiX, uiY + yOffset + 4);
    cout << char(204);
    for (int i = 0; i < 30; i++)
        cout << char(205);
    cout << char(185);
    gotoxy(uiX, uiY + yOffset + 5);
    cout << char(186);
    setColor(10);
    cout << " " << char(16) << " Reach the Vault!      ";
    setColor(13);
    gotoxy(uiX + 31, uiY + yOffset + 5);
    cout << char(186);
    gotoxy(uiX, uiY + yOffset + 6);
    cout << char(186);
    setColor(14);
    cout << " " << char(16) << " Solve the Riddle!     ";
    setColor(13);
    gotoxy(uiX + 31, uiY + yOffset + 6);
    cout << char(186);
    gotoxy(uiX, uiY + yOffset + 7);
    cout << char(186);
    setColor(12);
    cout << " " << char(16) << " Collect All Coins!    ";
    setColor(13);
    gotoxy(uiX + 31, uiY + yOffset + 7);
    cout << char(186);
    gotoxy(uiX, uiY + yOffset + 8);
    cout << char(186);
    setColor(11);
    cout << " " << char(16) << " Escape via Exit!      ";
    setColor(13);
    gotoxy(uiX + 31, uiY + yOffset + 8);
    cout << char(186);
    gotoxy(uiX, uiY + yOffset + 9);
    cout << char(200);
    for (int i = 0; i < 30; i++)
        cout << char(205);
    cout << char(188);
    setColor(15);
}

Riddle getRiddle()
{
    Riddle riddles[5] = {
        {"What has keys but can't open locks?", "PIANO", "Hint: It's a musical instrument (5 letters)"},
        {"I have cities but no houses, forests but no trees. What am I?", "MAP", "Hint: You use it for navigation (3 letters)"},
        {"What gets wet while drying?", "TOWEL", "Hint: You use it after a shower (5 letters)"},
        {"The more you take, the more you leave behind. What am I?", "FOOTSTEPS", "Hint: You make them when you walk (9 letters)"},
        {"What can travel around the world while staying in a corner?", "STAMP", "Hint: You put it on mail (5 letters)"}};
    int index = rand() % 5;
    return riddles[index];
}
bool solveVaultRiddle()
{
    // Flush buffer to prevent leftover keys
    while (_kbhit())
        _getch();
    Sleep(200); // Small delay to ensure keys are released

    system("cls");
    Riddle riddle = getRiddle();
    int boxWidth = 70;
    int startX = 25;
    int attempts = 3;
    string answer;
    time_t startTime = time(0);

    while (attempts > 0)
    {
        time_t currentTime = time(0);
        int elapsed = (int)difftime(currentTime, startTime);
        int timeLeft = RIDDLE_TIME_LIMIT - elapsed;

        if (timeLeft <= 0)
        {
            system("cls");
            gotoxy(startX + 10, 15);
            setColor(12);
            cout << char(254) << char(254) << " TIME'S UP! VAULT LOCKED! " << char(254) << char(254);
            Sleep(2000);
            return false;
        }

        setColor(14);
        gotoxy(startX, 8);
        cout << char(201);
        for (int i = 0; i < boxWidth; i++)
            cout << char(205);
        cout << char(187);

        gotoxy(startX, 9);
        cout << char(186);
        setColor(10);
        string header = " VAULT SECURITY SYSTEM ";
        int headerPad = (boxWidth - header.length()) / 2;
        for (int i = 0; i < headerPad; i++)
            cout << " ";
        cout << header;
        for (int i = 0; i < boxWidth - headerPad - header.length(); i++)
            cout << " ";
        setColor(14);
        cout << char(186);

        gotoxy(startX, 10);
        cout << char(204);
        for (int i = 0; i < boxWidth; i++)
            cout << char(205);
        cout << char(185);

        gotoxy(startX, 11);
        cout << char(186);
        if (timeLeft <= 10)
            setColor(12);
        else if (timeLeft <= 30)
            setColor(14);
        else
            setColor(10);
        string timeStr = "  TIME REMAINING: " + to_string(timeLeft) + " seconds";
        cout << timeStr;
        for (int i = 0; i < boxWidth - timeStr.length(); i++)
            cout << " ";
        setColor(14);
        cout << char(186);

        gotoxy(startX, 12);
        cout << char(186);
        setColor(15);
        cout << "  RIDDLE: ";
        setColor(11);
        cout << riddle.question;
        int spaces1 = boxWidth - 10 - riddle.question.length();
        for (int i = 0; i < spaces1; i++)
            cout << " ";
        setColor(14);
        cout << char(186);

        gotoxy(startX, 13);
        cout << char(186);
        setColor(13);
        cout << "  " << riddle.hint;
        int spaces2 = boxWidth - 2 - riddle.hint.length();
        for (int i = 0; i < spaces2; i++)
            cout << " ";
        setColor(14);
        cout << char(186);

        gotoxy(startX, 14);
        cout << char(204);
        for (int i = 0; i < boxWidth; i++)
            cout << char(205);
        cout << char(185);

        gotoxy(startX, 15);
        cout << char(186);
        setColor(12);
        string attemptsStr = "  Attempts Remaining: " + to_string(attempts) + " ";
        cout << attemptsStr;
        setColor(8);
        cout << "[";
        for (int i = 0; i < attempts; i++)
        {
            setColor(12);
            cout << char(3);
        }
        for (int i = attempts; i < 3; i++)
        {
            setColor(8);
            cout << char(3);
        }
        setColor(8);
        cout << "]";
        int spaces3 = boxWidth - attemptsStr.length() - 5;
        for (int i = 0; i < spaces3; i++)
            cout << " ";
        setColor(14);
        cout << char(186);

        gotoxy(startX, 16);
        cout << char(186);
        setColor(15);
        cout << "  Your Answer: ";
        for (int i = 0; i < boxWidth - 15; i++)
            cout << " ";
        setColor(14);
        cout << char(186);

        gotoxy(startX, 17);
        cout << char(200);
        for (int i = 0; i < boxWidth; i++)
            cout << char(205);
        cout << char(188);

        gotoxy(startX + 16, 16);
        setColor(15);
        answer = "";
        bool inputDone = false;

        // Clear keyboard buffer before accepting input
        while (_kbhit())
            _getch();

        while (!inputDone)
        {
            if (_kbhit())
            {
                char ch = _getch();
                if (ch == 13)
                {
                    inputDone = true;
                }
                else if (ch == 8 && answer.length() > 0)
                {
                    answer.pop_back();
                    cout << "\b \b";
                }
                else if (ch >= 32 && ch <= 126 && answer.length() < 30)
                {
                    answer += toupper(ch);
                    cout << (char)toupper(ch);
                }

                // Small delay after processing each key to prevent rapid repeats
                Sleep(100);
            }
            Sleep(10);
        }

        if (answer == riddle.answer)
        {
            gotoxy(startX + 15, 19);
            setColor(10);
            cout << char(16) << char(16) << " CORRECT! VAULT UNLOCKED! " << char(17) << char(17);
            Beep(1000, 200);
            Sleep(2000);
            return true;
        }
        else
        {
            attempts--;
            if (attempts > 0)
            {
                gotoxy(startX + 18, 19);
                setColor(12);
                cout << char(254) << " INCORRECT! Try again... " << char(254);
                Beep(400, 300);
                Sleep(1500);
                gotoxy(startX + 18, 19);
                cout << "                              ";
            }
        }
    }

    gotoxy(startX + 10, 19);
    setColor(12);
    cout << char(254) << char(254) << " VAULT LOCKED! MISSION FAILED! " << char(254) << char(254);
    Sleep(2000);
    return false;
}

int main()
{
    srand(time(0));
    hideCursor();
    printWelcomeScreen();
    system("chcp 437 > nul");
    system("cls");
    hideCursor();
    system("color 0E");

    GameState state = {0, 100, 0, false, 0};

    // LEVEL 1 SETUP

    Entity robber = {20, MAZE_HEIGHT - ROBBER_HEIGHT, 0, 0, true, 0};
    Entity helicopter = {50, 2, -2, 0, true, 0};
    Entity police = {100, 6, 0, 1, true, 3};
    Entity box = {60, 10, 1, -1, true, 3};

    Entity bullets[10];
    for (int i = 0; i < 10; i++)
        bullets[i].active = false;
    int bulletCount = 0;

    Entity heliBullets[5];
    for (int i = 0; i < 5; i++)
        heliBullets[i].active = false;
    int heliBulletCount = 0;

    int keyPositions[6] = {
        MAZE_WIDTH / 2, MAZE_HEIGHT - 10,
        MAZE_WIDTH - 20, MAZE_HEIGHT / 2,
        MAZE_WIDTH / 3, MAZE_HEIGHT / 3};
    int keyPlace = 0;

    Platform platforms[] = {
        {20, 26, 1}, {38, 23, -1}, {58, 20, 1}, {78, 17, -1}, {58, 14, 1}, {38, 11, -1}, {15, 8, 1}};
    int platformCount = 7;

    bool isJumping = false;
    int jumpCounter = 0;
    int heliShootTimer = 0;

    printBg();
    printBank();

    for (int i = 0; i < platformCount; i++)
    {
        printPlatform(platforms[i]);
    }

    printRobber(robber, 0);

    // LEVEL 1 GAME LOOP
    while (true)
    {
        state.animFrame = (state.animFrame + 1) % 4;

        if (state.levelComplete)
        {
            setColor(10);
            gotoxy(35, 15);
            cout << "LEVEL 1 COMPLETE! Starting Level 2...";
            Sleep(2000);
            break;
        }

        if (state.health <= 0)
        {
            setColor(12);
            gotoxy(35, 15);
            cout << "MISSION FAILED! Press ESC to exit...";
            if (GetAsyncKeyState(VK_ESCAPE))
                return 0;
            Sleep(100);
            continue;
        }

        if (GetAsyncKeyState(VK_LEFT))
            moveRobberLeft(robber);
        if (GetAsyncKeyState(VK_RIGHT))
            moveRobberRight(robber);
        if (GetAsyncKeyState(VK_SPACE) && !isJumping)
        {
            isJumping = true;
            jumpCounter = 0;
            playSound("jump");
        }
        if (GetAsyncKeyState('S'))
        {
            shootBullet(bullets, bulletCount, robber);
            playSound("shoot");
            Sleep(150);
        }

        if (isJumping)
            jumpRobber(robber, isJumping, jumpCounter);
        else
        {
            int carryDX = 0;
            bool onPlatform = isStandingOnPlatform(robber, platforms, platformCount, carryDX);
            bool groundBelow = hasGroundBelow(robber);

            if (onPlatform)
            {
                eraseRobber(robber);
                int newX = robber.x + carryDX;
                if (!willCollideWithBank(robber, newX, robber.y))
                    robber.x = newX;
                if (robber.x < 2)
                    robber.x = 2;
                if (robber.x + ROBBER_WIDTH >= MAZE_WIDTH - 1)
                    robber.x = MAZE_WIDTH - 1 - ROBBER_WIDTH;
                printRobber(robber, state.animFrame);
            }
            else if (!groundBelow)
            {
                eraseRobber(robber);
                int newY = robber.y + 1;
                if (!willCollideWithBank(robber, robber.x, newY))
                    robber.y = newY;
                if (robber.y + ROBBER_HEIGHT > MAZE_HEIGHT)
                    robber.y = MAZE_HEIGHT - ROBBER_HEIGHT;
                printRobber(robber, state.animFrame);
            }
        }

        moveHelicopter(helicopter);
        if (police.active)
            movePolice(police, platforms, platformCount, helicopter);
        if (box.active)
            moveBox(box, platforms, platformCount, helicopter);

        heliShootTimer++;
        if (heliShootTimer > 8 && helicopter.active)
        {
            heliShoot(heliBullets, heliBulletCount, helicopter);
            heliShootTimer = 0;
        }

        for (int i = 0; i < 10; i++)
            if (bullets[i].active)
                moveBullet(bullets[i]);
        for (int i = 0; i < 5; i++)
            if (heliBullets[i].active)
                moveHeliBullet(heliBullets[i]);

        checkBulletCollision(bullets, bulletCount, police, box, state);
        checkHeliBulletCollision(heliBullets, heliBulletCount, robber, state);
        keyCollect(keyPositions, keyPlace, robber, state);
        checkLevelComplete(robber, state, police, box);

        for (int i = 0; i < platformCount; i++)
            movePlatform(platforms[i], 2, MAZE_WIDTH - 2);

        printBank();
        drawUI(state);
        Sleep(100);
    }

    //=========== LEVEL 2 =============

    system("cls");

    Level2State l2state;
    l2state.coinsCollected = 0;
    l2state.vaultUnlocked = false;
    l2state.gameWon = false;
    l2state.gameLost = false;
    l2state.startTime = 0;

    robber.x = 10;
    robber.y = 5;
    robber.active = true;

    Entity coins[NUM_COINS];
    Laser lasers[NUM_LASERS];

    for (int i = 0; i < NUM_COINS; i++)
        coins[i].active = false;
    for (int i = 0; i < NUM_LASERS; i++)
        lasers[i].active = false;

    hideCursor();
    system("cls");

    // Initialize positions
    Position robber3 = {1, 1};
    Position car = {MAZE_WIDTH3 + 8, 23}; // Car outside maze at exit

    // Initialize game state
    MazeGameState gameState;
    gameState.keysCollected = 0;
    gameState.key1Found = false;
    gameState.key2Found = false;
    gameState.key3Found = false;

    // Mark start as visited
    visited[robber3.y][robber3.x] = true;

    // Timer variables
    time_t startTime = time(NULL);
    int lastBeep = 60;
    bool sirenPlaying = false;
    int frame = 0;

    // Draw initial screen
    drawMaze();
    drawUI3();
    printVaultMaze();

    // Game loop
    bool gameRunning = true;
    bool won = false;

    while (gameRunning)
    {
        // Calculate time remaining
        int elapsed = (int)difftime(time(NULL), startTime);
        int timeLeft = TIMER_SECONDS - elapsed;

        // Draw robber
        drawRobber(robber3, frame);

        // Draw timer
        drawTimer(timeLeft);

        // Draw key status
        drawKeyStatus(gameState);

        // Timer pressure effects
        if (timeLeft <= 0)
        {
            gameRunning = false;
            won = false;
            break;
        }

        // Siren in last 10 seconds
        if (timeLeft <= 10 && !sirenPlaying)
        {
            sirenPlaying = true;
        }

        if (sirenPlaying && timeLeft > 0)
        {
            if (frame % 4 == 0)
            {
                playBeep(1000, 50);
            }
        }

        // Check for ESC key
        // if(GetAsyncKeyState(VK_ESCAPE)) {
        //     gameRunning = false;
        //     break;
        // }

        Position newPos = robber3;
        bool moved = false;

        if (GetAsyncKeyState(VK_UP))
        {
            newPos.y--;
            moved = true;
        }
        else if (GetAsyncKeyState(VK_DOWN))
        {
            newPos.y++;
            moved = true;
        }

        if (GetAsyncKeyState(VK_LEFT))
        {
            newPos.x--;
            moved = true;
        }
        else if (GetAsyncKeyState(VK_RIGHT))
        {
            newPos.x++;
            moved = true;
        }

        if (moved)
        {
            // Clear old position (single character)
            gotoxy(robber3.x, robber3.y);
            // Redraw what was there (wall or path)
            if (robber3.y >= 0 && robber3.y < MAZE_HEIGHT3 && robber3.x >= 0 && robber3.x < MAZE_WIDTH3)
            {
                if (maze[robber3.y][robber3.x] == 1)
                {
                    setColor(14);
                    cout << (char)219;
                }
                else if (visited[robber3.y][robber3.x] && maze[robber3.y][robber3.x] == 0)
                {
                    setColor(8);
                    cout << (char)176;
                }
                else if (maze[robber3.y][robber3.x] == 3)
                {
                    setColor(10);
                    cout << (char)254;
                }
                else
                {
                    cout << " ";
                }
            }

            // Check collision
            if (newPos.x >= 0 && newPos.x < MAZE_WIDTH3 &&
                newPos.y >= 0 && newPos.y < MAZE_HEIGHT3)
            {

                int cell = maze[newPos.y][newPos.x];

                if (cell == 0 || cell == 3 || cell == 4)
                {
                    // Footstep sound
                    playBeep(200, 20);

                    // Check for key collection
                    if (cell == 4)
                    {
                        // Determine which key was collected based on position
                        if (newPos.y == 5 && newPos.x == 14)
                        { // Key 1 (top-left)
                            if (!gameState.key1Found)
                            {
                                gameState.key1Found = true;
                                gameState.keysCollected++;
                                playBeep(1000, 150);
                                maze[newPos.y][newPos.x] = 0;
                            }
                        }
                        else if (newPos.y == 13 && newPos.x == 78)
                        {
                            if (!gameState.key2Found)
                            {
                                gameState.key2Found = true;
                                gameState.keysCollected++;
                                playBeep(1000, 150);
                                maze[newPos.y][newPos.x] = 0;
                            }
                        }
                        else if (newPos.y == 22 && newPos.x == 40)
                        {
                            if (!gameState.key3Found)
                            {
                                gameState.key3Found = true;
                                gameState.keysCollected++;
                                playBeep(1000, 150);
                                maze[newPos.y][newPos.x] = 0;
                            }
                        }
                    }

                    // Check exit-if all keys collected
                    if (cell == 3)
                    {
                        if (gameState.keysCollected == 3)
                        {
                            won = true;
                            gameRunning = false;
                        }
                        else
                        {
                            // Show message - need all keys
                            gotoxy(MAZE_WIDTH3 + 5, 20);
                            setColor(12);
                            cout << "LOCKED! Need all 3 keys!";
                            Sleep(800);
                            gotoxy(MAZE_WIDTH3 + 5, 20);
                            cout << "                         ";
                        }
                    }

                    // Move robber and mark as visited
                    robber3 = newPos;
                    visited[robber3.y][robber3.x] = true;
                }
            }

            frame++;
            Sleep(50);
        }

        Sleep(20);
    }

    // End game
    if (won)
    {
        winAnimation(robber3, car);
    }
    else
    {
        loseAnimation(robber3);
    }

    // Final screen
    gotoxy(0, MAZE_HEIGHT3 + 2);
    setColor(15);
    cout << "\n\nPress any key to continue...";
    _getch();

    // Clear screen and set up Level 2 (vault level)
    system("cls");
    printBg();
    printVault();
    printExitDoor();
    printRobber(robber, 0);
    drawLevel2UI(state, l2state, -1);

    // return 0;
    // int frame = 0;
    bool nearVault = false;
    bool laserHitCooldown = false;
    int cooldownCounter = 0;

    // LEVEL 2 GAME LOOP
    while (!l2state.gameWon && !l2state.gameLost)
    {
        if (l2state.vaultUnlocked)
        {
            int elapsed = (int)difftime(time(0), l2state.startTime);
            int timeLeft = COIN_TIME_LIMIT - elapsed;

            if (timeLeft <= 0)
            {
                l2state.gameLost = true;
                break;
            }

            drawLevel2UI(state, l2state, timeLeft);
        }

        bool moved = false;
        int newX = robber.x, newY = robber.y;

        if (GetAsyncKeyState(VK_UP))
        {
            newY--;
            moved = true;
        }
        else if (GetAsyncKeyState(VK_DOWN))
        {
            newY++;
            moved = true;
        }

        if (GetAsyncKeyState(VK_LEFT))
        {
            newX -= 2;
            moved = true;
        }
        else if (GetAsyncKeyState(VK_RIGHT))
        {
            newX += 2;
            moved = true;
        }

        if (moved)
        {
            if (newX > 2 && newX < MAZE_WIDTH - 5 && newY > 1 && newY < MAZE_HEIGHT - 3)
            {
                if (l2state.vaultUnlocked ||
                    !(newX >= VAULT_X - 2 && newX <= VAULT_X + VAULT_WIDTH &&
                      newY >= VAULT_Y - 2 && newY <= VAULT_Y + VAULT_HEIGHT))
                {
                    clearRobber(robber);
                    robber.x = newX;
                    robber.y = newY;
                    printRobber(robber, 0);
                }
            }
            Sleep(80);
        }

        // Check if near vault
        if (!l2state.vaultUnlocked &&
            robber.x >= VAULT_X - 5 && robber.x <= VAULT_X + VAULT_WIDTH + 2 &&
            robber.y >= VAULT_Y - 3 && robber.y <= VAULT_Y + VAULT_HEIGHT + 2)
        {
            if (!nearVault)
            {
                nearVault = true;
                if (solveVaultRiddle())
                {
                    l2state.vaultUnlocked = true;
                    state.score += 500;
                    l2state.startTime = time(0);

                    system("cls");
                    printBg();
                    printExitDoor();

                    // Spawn coins
                    for (int i = 0; i < NUM_COINS; i++)
                    {
                        coins[i].x = 5 + rand() % (MAZE_WIDTH - 10);
                        coins[i].y = 3 + rand() % (MAZE_HEIGHT - 6);
                        coins[i].active = true;
                    }

                    // Spawn lasers
                    for (int i = 0; i < NUM_LASERS; i++)
                    {
                        lasers[i].x = 10 + i * 10;
                        lasers[i].y = 5 + rand() % 20;
                        lasers[i].direction = (rand() % 2 == 0) ? 1 : -1;
                        lasers[i].active = true;
                    }
                }
                else
                {
                    l2state.gameLost = true;
                }
            }
        }
        else
        {
            nearVault = false;
        }

        if (l2state.vaultUnlocked && frame % 3 == 0)
        {
            for (int i = 0; i < NUM_LASERS; i++)
            {
                if (lasers[i].active)
                {
                    clearLaser(lasers[i]);
                    lasers[i].y += lasers[i].direction;
                    if (lasers[i].y <= 2 || lasers[i].y >= MAZE_HEIGHT - 3)
                        lasers[i].direction *= -1;
                }
            }
        }

        if (!l2state.vaultUnlocked)
        {
            drawLevel2UI(state, l2state, -1);
        }

        if (l2state.vaultUnlocked)
        {
            for (int i = 0; i < NUM_COINS; i++)
                printCoin(coins[i]);
            for (int i = 0; i < NUM_LASERS; i++)
                printLaser(lasers[i]);
            printExitDoor();
        }

        // Check coin collection
        for (int i = 0; i < NUM_COINS; i++)
        {
            if (coins[i].active &&
                abs(robber.x - coins[i].x) <= 2 &&
                abs(robber.y - coins[i].y) <= 1)
            {
                coins[i].active = false;
                clearCoin(coins[i]);
                l2state.coinsCollected++;
                state.score += 100;
                playCoinSound();
            }
        }

        // Cooldown management
        if (laserHitCooldown)
        {
            cooldownCounter++;
            if (cooldownCounter >= 20)
            {
                laserHitCooldown = false;
                cooldownCounter = 0;
            }
        }

        // Check laser collision
        if (!laserHitCooldown)
        {
            for (int i = 0; i < NUM_LASERS; i++)
            {
                if (lasers[i].active &&
                    robber.y == lasers[i].y &&
                    robber.x >= lasers[i].x &&
                    robber.x <= lasers[i].x + LASER_LENGTH)
                {
                    state.health -= 15;
                    playLaserSound();
                    laserHitCooldown = true;

                    if (state.health <= 0)
                    {
                        state.health = 0;
                        l2state.gameLost = true;
                    }

                    setColor(12);
                    gotoxy(robber.x, robber.y);
                    cout << " O  ";
                    Sleep(100);
                    break;
                }
            }
        }

        // Check exit door
        if (l2state.vaultUnlocked && l2state.coinsCollected == NUM_COINS)
        {
            if (robber.x >= MAZE_WIDTH - 16 && robber.x <= MAZE_WIDTH - 6 &&
                robber.y >= MAZE_HEIGHT - 9 && robber.y <= MAZE_HEIGHT - 2)
            {
                l2state.gameWon = true;
            }
        }

        frame++;
        Sleep(50);
    }
    // GAME OVER SCREEN

    system("cls");
    gotoxy(40, 15);

    if (l2state.gameWon)
    {
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);
        setColor(10);
        cout << "╔══════════════════════════════════════╗" << endl;
        gotoxy(40, 16);
        cout << "║  *** HEIST SUCCESSFUL! ***           ║" << endl;
        gotoxy(40, 17);
        cout << "║  You escaped with the loot!          ║" << endl;
        gotoxy(40, 18);
        cout << "║  Final Score: " << setw(8) << state.score << " pts           ║" << endl;
        gotoxy(40, 19);
        cout << "╚══════════════════════════════════════╝" << endl;
        getch();
        Beep(1000, 500);
    }
    else if (l2state.gameLost)
    {
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);
        setColor(12);
        cout << "╔══════════════════════════════════════╗" << endl;
        gotoxy(40, 16);
        cout << "║     *** MISSION FAILED! ***          ║" << endl;
        gotoxy(40, 17);
        if (state.health <= 0)
            cout << "║  You were caught by lasers!          ║" << endl;
        else
            cout << "║  Time ran out!                       ║" << endl;
        gotoxy(40, 18);
        cout << "║  Score: " << setw(8) << state.score << " pts               ║" << endl;
        gotoxy(40, 19);
        cout << "╚══════════════════════════════════════╝" << endl;
        Beep(400, 500);
    }

    setColor(15);
    gotoxy(40, 21);
    cout << "Press any key to exit...";
    _getch();

    return 0;
}

// Draw maze
void drawMaze()
{
    for (int y = 0; y < MAZE_HEIGHT3; y++)
    {
        for (int x = 0; x < MAZE_WIDTH3; x++)
        {
            gotoxy(x, y);

            if (maze[y][x] == 1)
            {
                setColor(14); // Yellow walls
                cout << (char)219;
            }
            else if (maze[y][x] == 3)
            {
                setColor(10);      // Green real exit
                cout << (char)254; // Small square for exit
            }
            else if (maze[y][x] == 4)
            {
                setColor(11);     // Cyan key
                cout << (char)11; // Key symbol ♂
            }
            else
            {
                cout << " ";
            }
        }
    }
    setColor(15);
}

// Draw robber
void drawRobber(Position pos, int frame)
{
    setColor(12); // Red robber
    gotoxy(pos.x, pos.y);
    cout << (frame % 2 == 0 ? (char)2 : (char)1);
    setColor(15);
}

// Draw timer UI
void drawTimer(int timeLeft)
{
    int uiX = MAZE_WIDTH3 + 5, uiY = 2;

    // Border
    setColor(13);
    gotoxy(uiX, uiY);
    cout << (char)201;
    for (int i = 0; i < 28; i++)
        cout << (char)205;
    cout << (char)187;

    gotoxy(uiX, uiY + 1);
    cout << (char)186;
    setColor(14);
    cout << "   " << (char)2 << " ESCAPE STATUS " << (char)2 << "   ";
    setColor(13);
    gotoxy(uiX + 29, uiY + 1);
    cout << (char)186;

    gotoxy(uiX, uiY + 2);
    cout << (char)204;
    for (int i = 0; i < 28; i++)
        cout << (char)205;
    cout << (char)185;

    // Timer
    gotoxy(uiX, uiY + 3);
    cout << (char)186;
    if (timeLeft <= 5)
        setColor(12 | 128); // Blinking red
    else if (timeLeft <= 10)
        setColor(12); // Red
    else if (timeLeft <= 30)
        setColor(14); // Yellow
    else
        setColor(10); // Green

    cout << "   " << (char)1 << " TIME: " << timeLeft << " sec     ";

    setColor(13);
    gotoxy(uiX + 29, uiY + 3);
    cout << (char)186;

    gotoxy(uiX, uiY + 4);
    cout << (char)200;
    for (int i = 0; i < 28; i++)
        cout << (char)205;
    cout << (char)188;

    setColor(15);
}

// Draw UI instructions
void drawUI3()
{
    int uiX = MAZE_WIDTH3 + 5, uiY = 8;

    setColor(13);
    gotoxy(uiX, uiY);
    cout << (char)201;
    for (int i = 0; i < 28; i++)
        cout << (char)205;
    cout << (char)187;

    gotoxy(uiX, uiY + 1);
    cout << (char)186;
    setColor(11);
    cout << "       CONTROLS          ";
    setColor(13);
    gotoxy(uiX + 29, uiY + 1);
    cout << (char)186;

    gotoxy(uiX, uiY + 2);
    cout << (char)204;
    for (int i = 0; i < 28; i++)
        cout << (char)205;
    cout << (char)185;

    gotoxy(uiX, uiY + 3);
    cout << (char)186;
    setColor(14);
    cout << "  " << (char)24 << (char)25 << (char)26 << (char)27 << " : Move            ";
    setColor(13);
    gotoxy(uiX + 29, uiY + 3);
    cout << (char)186;

    gotoxy(uiX, uiY + 4);
    cout << (char)186;
    setColor(14);
    cout << "  ESC : Exit            ";
    setColor(13);
    gotoxy(uiX + 29, uiY + 4);
    cout << (char)186;

    gotoxy(uiX, uiY + 5);
    cout << (char)200;
    for (int i = 0; i < 28; i++)
        cout << (char)205;
    cout << (char)188;

    // Objectives
    gotoxy(uiX, uiY + 7);
    setColor(11);
    cout << (char)16 << " Collect 3 keys!";

    gotoxy(uiX, uiY + 8);
    setColor(10);
    cout << (char)16 << " Find the exit!";

    gotoxy(uiX, uiY + 9);
    setColor(12);
    cout << (char)16 << " Watch the timer!";

    setColor(15);
}

// Draw key status
void drawKeyStatus(MazeGameState &state)
{
    int uiX = MAZE_WIDTH3 + 5, uiY = 15;

    setColor(13);
    gotoxy(uiX, uiY);
    cout << (char)201;
    for (int i = 0; i < 28; i++)
        cout << (char)205;
    cout << (char)187;

    gotoxy(uiX, uiY + 1);
    cout << (char)186;
    setColor(11);
    cout << "      KEYS COLLECTED     ";
    setColor(13);
    gotoxy(uiX + 29, uiY + 1);
    cout << (char)186;

    gotoxy(uiX, uiY + 2);
    cout << (char)204;
    for (int i = 0; i < 28; i++)
        cout << (char)205;
    cout << (char)185;

    // Key 1
    gotoxy(uiX, uiY + 3);
    cout << (char)186 << "  ";
    if (state.key1Found)
    {
        setColor(10);
        cout << (char)251 << " Key 1: FOUND       ";
    }
    else
    {
        setColor(8);
        cout << (char)251 << " Key 1: ...         ";
    }
    setColor(13);
    gotoxy(uiX + 29, uiY + 3);
    cout << (char)186;

    // Key 2
    gotoxy(uiX, uiY + 4);
    cout << (char)186 << "  ";
    if (state.key2Found)
    {
        setColor(10);
        cout << (char)251 << " Key 2: FOUND       ";
    }
    else
    {
        setColor(8);
        cout << (char)251 << " Key 2: ...         ";
    }
    setColor(13);
    gotoxy(uiX + 29, uiY + 4);
    cout << (char)186;

    // Key 3
    gotoxy(uiX, uiY + 5);
    cout << (char)186 << "  ";
    if (state.key3Found)
    {
        setColor(10);
        cout << (char)251 << " Key 3: FOUND       ";
    }
    else
    {
        setColor(8);
        cout << (char)251 << " Key 3: ...         ";
    }
    setColor(13);
    gotoxy(uiX + 29, uiY + 5);
    cout << (char)186;

    gotoxy(uiX, uiY + 6);
    cout << (char)200;
    for (int i = 0; i < 28; i++)
        cout << (char)205;
    cout << (char)188;

    setColor(15);
}

// Play beep sound
void playBeep(int frequency, int duration)
{
    Beep(frequency, duration);
}

// Screen flash effect
void screenFlash()
{
    system("color F0");
    Sleep(100);
    system("color 07");
}

// Win animation
void winAnimation(Position robber, Position car)
{
    system("cls");

    // Victory screen
    system("cls");
    setColor(10);
    gotoxy(30, 10);
    cout << (char)201;  
    for(int i = 0; i < 32; i++) cout << (char)205;
    cout << (char)187;  
    gotoxy(30, 11);
    cout << (char)186 << "                                " << (char)186; 
    gotoxy(30, 12);
    cout << (char)186 << "    " << (char)1 << "YOU ENTERED THE VAULT! " << (char)1 << "   " << (char)186;
    gotoxy(30, 13);
    cout << (char)186 << "                                " << (char)186;
    gotoxy(30, 14);
    cout << (char)186 << "                                " << (char)186;
    gotoxy(30, 15);
    cout << (char)186 << "    Continue your loot          " << (char)186;
    gotoxy(30, 16);
    cout << (char)186 << "                                " << (char)186;
    gotoxy(30, 17);
    cout << (char)200;  
    for(int i = 0; i < 32; i++) cout << (char)205;  
    cout << (char)188;  

    setColor(15);

    // Celebration beeps
    for (int i = 0; i < 3; i++)
    {
        playBeep(800, 200);
        Sleep(100);
        playBeep(1000, 200);
        Sleep(100);
    }
}

// Lose animation
void loseAnimation(Position robber)
{
    system("cls");

    // Police surrounding
    setColor(9);
    gotoxy(35, 10);
    cout << "     POLICE";
    gotoxy(25, 12);
    cout << "POLICE";
    gotoxy(50, 12);
    cout << "POLICE";
    gotoxy(35, 14);
    cout << "     POLICE";

    // Robber in center
    setColor(12);
    gotoxy(38, 12);
    cout << "O";
    gotoxy(37, 13);
    cout << "/|\\";
    gotoxy(37, 14);
    cout << "/ \\";

    // Sirens
    for (int i = 0; i < 5; i++)
    {
        playBeep(800, 100);
        Sleep(50);
        playBeep(1000, 100);
        Sleep(50);
    }

    Sleep(1000);

    // Caught screen
    system("cls");
    setColor(12);
    gotoxy(30, 10);
    cout << (char)201;  
    for(int i = 0; i < 32; i++) cout << (char)205;  
    cout << (char)187;  
    gotoxy(30, 11);
    cout << (char)186 << "                                " << (char)186; 
    gotoxy(30, 12);
    cout << (char)186 << "      " << (char)15 << " YOU GOT CAUGHT! " << (char)15 << "      " << (char)186;
    gotoxy(30, 13);
    cout << (char)186 << "                                " << (char)186;
    gotoxy(30, 14);
    cout << (char)186 << "    Time ran out! The police    " << (char)186;
    gotoxy(30, 15);
    cout << (char)186 << "    surrounded you. BUSTED!     " << (char)186;
    gotoxy(30, 16);
    cout << (char)186 << "                                " << (char)186;
    gotoxy(30, 17);
    cout << (char)200;  
    for(int i = 0; i < 32; i++) cout << (char)205;  
    cout << (char)188;  

    setColor(15);
}