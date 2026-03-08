#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <termios.h>
#include <unistd.h>

#define COIN '$'
#define TRAP '^'
#define WALL '#'
#define DOOR '+'
#define PLAYER '@'
#define EMPTY ' '
#define THROWER '&'
#define MAX_H 200
#define MAX_W 400

int playerX, playerY;
int coins = 0;
int gameOver = 0;
int cnt_coins = 10;
int cnt_traps = 5;
int cnt_walls = 6;
int cnt_thrower = 1;
//int level = 0;
int level_coins = 0;
int height = 21;
int width = 43;
char map[MAX_H][MAX_W];
int level_pos[2] = {0 , 0};

int mrandom(int min, int max) {
    return min + rand() % (max - min + 1);
}

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
            if ((y == (height-1) / 2  && x == 0 && level_pos[0] > 0) || 
                (x == (width-1) / 2 && y == 0) || 
                (x == (width-1) / 2 && y == (height-1) && level_pos[1] > 0) || 
                (y == (height-1) / 2 && x == (width-1))){
                    map[y][x] = DOOR;
            }
            else if (x == 0 || x == width-1 || y == 0 || y == height-1) {
                map[y][x] = WALL;
            } else {
                map[y][x] = EMPTY;
            }
        }
    }
    
 
    map[playerY][playerX] = PLAYER;
    
    srand(time(0));
    for (int i = 0; i < cnt_thrower; i++) {
        int x, y;
        x = mrandom(0, 2);
        if (x == 0){
            x = 0;
            do {
                y = mrandom(1, height - 1);
            } while (map[y][x] == '+');
            
        } else {
            x = width - 1;
            do {
                y = mrandom(1, height - 1);
            } while (map[y][x] == '+'); 
        }
        map[y][x] = THROWER;
    }

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
    for (int i = 0; i < cnt_walls; i++) {
        int x, y;
        do {
            x = 1 + rand() % (width-2);
            y = 1 + rand() % (height-2);
        } while (map[y][x] != EMPTY);
        map[y][x] = WALL;
    }
}

void drawMap() {
    system("clear");
    
    printf("Coins: %d\n", coins);
    printf("Level: %d, %d\n", level_pos[0], level_pos[1]);
    printf("Use WASD to move.\n Press 'q' to quit.\n\n");
    
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
    
    if (map[newY][newX] == WALL) return;
    if (map[newY][newX] == TRAP) {
        gameOver = 1;
        return;
    }
    if (map[newY][newX] == DOOR) {
        //cnt_coins++;
        cnt_traps = mrandom(5, 20);
        cnt_walls = mrandom(10, 35);
        if (playerX == 1){
            level_pos[0]--;
            playerX = width - 2;
        } else if (playerX == width-2) {
            level_pos[0]++;
            playerX = 1;
        }

        if (playerY == 1){
            level_pos[1]++;
            playerY = height - 2;
        } else if (playerY == height-2) {
            level_pos[1]--;
            playerY = 1;
        }

        initMap();
        return;
    }
    
    if (map[newY][newX] == COIN) {
        coins += mrandom(1, 5);
        //level_coins++;
    }


    
    map[playerY][playerX] = EMPTY;
    playerX = newX;
    playerY = newY;
    map[playerY][playerX] = PLAYER;
}

void checkScore() {
    FILE *fp;
    char name[] = "record.txt";
    fp = fopen(name, "a+");
    fprintf(fp,"+");
    fclose(fp);


}
int main() {
    
    setTerminalMode(1);
    playerX = width / 2;
    playerY = height / 2;
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
    checkScore();
    
    setTerminalMode(0);
    drawMap();
    printf("\nGame over! Your score: %d coins (Level %d, %d)\n", coins, level_pos[0], level_pos[1]);
    
    return 0;
}