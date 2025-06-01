#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <termios.h>
#include <unistd.h>

#define COIN '$'
#define TRAP '^'
#define PLAYER '@'
#define EMPTY ' '
#define MAX_H 200
#define MAX_W 400

int playerX, playerY;
int coins = 0;
int gameOver = 0;
int cnt_coins = 10;
int cnt_traps = 5;
int level = 0;
int level_coins = 0;
int height = 15;
int width = 30;
char map[MAX_H][MAX_W];

void setTerminalMode(int enable) {
    static struct termios oldt, newt;
    if (enable) {
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    } else {
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    }
}

void initMap() {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (x == 0 || x == width-1 || y == 0 || y == height-1) {
                map[y][x] = '#';
            } else {
                map[y][x] = EMPTY;
            }
        }
    }
    
    playerX = width / 2;
    playerY = height / 2;
    map[playerY][playerX] = PLAYER;
    
    srand(time(0));
    for (int i = 0; i < cnt_coins; i++) {
        int x, y;
        do {
            x = 1 + rand() % (width-2);
            y = 1 + rand() % (height-2);
        } while (map[y][x] != EMPTY);
        map[y][x] = COIN;
    }
    
    for (int i = 0; i < cnt_traps; i++) {
        int x, y;
        do {
            x = 1 + rand() % (width-2);
            y = 1 + rand() % (height-2);
        } while (map[y][x] != EMPTY);
        map[y][x] = TRAP;
    }
}

void drawMap() {
    system("clear");
    
    printf("Coins: %d\n", coins);
    printf("Level: %d\n", level);
    printf("Use WASD to move. Don't step on traps! Press 'q' to quit.\n\n");
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            printf("%c", map[y][x]);
        }
        printf("\n");
    }
}

void movePlayer(int dx, int dy) {
    int newX = playerX + dx;
    int newY = playerY + dy;
    
    if (map[newY][newX] == '#') return;
    if (map[newY][newX] == TRAP) {
        gameOver = 1;
        return;
    }
    
    if (map[newY][newX] == COIN) {
        coins++;
        level_coins++;
    }

    if (level_coins >= cnt_coins) {
        level++;
        level_coins = 0;
        cnt_coins++;
        cnt_traps += 4;
        
        if (width < MAX_W-1) width++;
        if (height < MAX_H-1) height++;
        
        initMap();
        return;
    }
    
    map[playerY][playerX] = EMPTY;
    playerX = newX;
    playerY = newY;
    map[playerY][playerX] = PLAYER;
}

int main() {
    setTerminalMode(1);
    initMap();
    
    while (!gameOver) {
        drawMap();
        
        char input;
        if (read(STDIN_FILENO, &input, 1) > 0) {
            switch(input) {
                case 'w': movePlayer(0, -1); break;
                case 'a': movePlayer(-1, 0); break;
                case 's': movePlayer(0, 1); break;
                case 'd': movePlayer(1, 0); break;
                case 'q': gameOver = 1; break;
            }
        }
    }
    
    setTerminalMode(0);
    drawMap();
    printf("\nGame over! Your score: %d coins (Level %d)\n", coins, level);
    
    return 0;
}