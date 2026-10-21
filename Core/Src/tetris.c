#include "tetris.h"
#include "oled.h"
#include "fonts.h"
#include <stdlib.h>
#include <stdio.h>

const uint8_t FIGURES[7][4][4] = {
    {{0,0,0,0},{1,1,1,1},{0,0,0,0},{0,0,0,0}}, // I
    {{0,0,0,0},{0,1,1,0},{0,1,1,0},{0,0,0,0}}, // O
    {{0,0,0,0},{0,1,0,0},{1,1,1,0},{0,0,0,0}}, // T
    {{0,0,0,0},{0,1,1,0},{1,1,0,0},{0,0,0,0}}, // S
    {{0,0,0,0},{1,1,0,0},{0,1,1,0},{0,0,0,0}}, // Z
    {{0,0,0,0},{1,0,0,0},{1,1,1,0},{0,0,0,0}}, // J
    {{0,0,0,0},{0,0,1,0},{1,1,1,0},{0,0,0,0}}, // L
};

static int check_collision(TetrisState* s, int dx, int dy, uint8_t temp[4][4]) {
    for(int y = 0; y < 4; y++) {
        for(int x = 0; x < 4; x++) {
            if(temp[y][x]) {
                int nx = s->x + x + dx;
                int ny = s->y + y + dy;

                // 1. Проверка границ поля (стены и пол)
                if(nx < 0 || nx >= BOARD_W || ny >= BOARD_H) return 1;

                // 2. Проверка столкновения с застывшими блоками (только если мы внутри поля по Y)
                if(ny >= 0 && s->board[ny][nx]) return 1;
            }
        }
    }
    return 0;
}

void Tetris_Init(TetrisState* s) {
    for(int y = 0; y < BOARD_H; y++)
        for(int x = 0; x < BOARD_W; x++)
            s->board[y][x] = 0;
    s->score = 0;
    s->speed = 500;
    Tetris_Spawn(s);
}

void Tetris_Spawn(TetrisState* s) {
    int rnd = rand() % 7;
    for(int y = 0; y < 4; y++)
        for(int x = 0; x < 4; x++)
            s->shape[y][x] = FIGURES[rnd][y][x];

    s->x = 3;
    s->y = 0;
}

void Tetris_Drop(TetrisState* s) {
    while(!check_collision(s, 0, 1, s->shape)) {
        s->y++;
    }
}

void Tetris_Move(TetrisState* s, int dx, int dy) {
    if(!check_collision(s, dx, dy, s->shape)) {
        s->x += dx;
        s->y += dy;
    }
}

void Tetris_Rotate(TetrisState* s) {
    uint8_t temp[4][4];
    for(int y = 0; y < 4; y++)
        for(int x = 0; x < 4; x++)
            temp[y][x] = s->shape[3-x][y];

    if(!check_collision(s, 0, 0, temp)) {
        for(int y = 0; y < 4; y++)
            for(int x = 0; x < 4; x++)
                s->shape[y][x] = temp[y][x];
    }
}

int Tetris_Step(TetrisState* s) {
    if(!check_collision(s, 0, 1, s->shape)) {
        s->y++;
    } else {
        for(int y = 0; y < 4; y++) {
            for(int x = 0; x < 4; x++) {
                if(s->shape[y][x]) {
                    int ny = s->y + y;
                    int nx = s->x + x;

                    if(ny < 0) return 1;

                    s->board[ny][nx] = 1;
                }
            }
        }

        for(int y = BOARD_H - 1; y >= 0; y--) {
            int full = 1;
            for(int x = 0; x < BOARD_W; x++) {
                if(!s->board[y][x]) { full = 0; break; }
            }

            if(full) {
                s->score++;
                if(s->speed > 100) s->speed -= 20;

                for(int yy = y; yy > 0; yy--) {
                    for(int x = 0; x < BOARD_W; x++) {
                        s->board[yy][x] = s->board[yy-1][x];
                    }
                }
                for(int x = 0; x < BOARD_W; x++) s->board[0][x] = 0;

                y++;
            }
        }

        Tetris_Spawn(s);

        if(check_collision(s, 0, 0, s->shape)) {
            return 1;
        }
    }
    return 0;
}

void Tetris_Render(TetrisState* s) {
    oled_Fill(Black);

    for(int y = 0; y < BOARD_H; y++) {
        for(int x = 0; x < BOARD_W; x++) {
            int draw = s->board[y][x];

            if(!draw && s->y >= 0) {
                int rx = x - s->x;
                int ry = y - s->y;
                if(rx >= 0 && rx < 4 && ry >= 0 && ry < 4 && s->shape[ry][rx]) {
                    draw = 1;
                }
            }

            if(draw) {
                oled_DrawSquare(x*3, x*3+2, y*3, y*3+2, White);
            }
        }
    }

    // Разделительная линия
    oled_DrawVLine(0, 60, 32, White);

    // UI: Счет
    char buf[16];
    oled_SetCursor(36, 2);
    sprintf(buf, "Score: %d", s->score);
    oled_WriteString(buf, Font_7x10, White);

    // UI: Управление
    oled_SetCursor(36, 16);
    oled_WriteString("Controls:", Font_7x10, White);

    oled_SetCursor(36, 26);
    oled_WriteString("1:L  2:Rot 3:R", Font_7x10, White);

    oled_SetCursor(36, 36);
    oled_WriteString("Row2-1: Down", Font_7x10, White);

    oled_SetCursor(36, 46);
    oled_WriteString("Row2-2: Drop", Font_7x10, White);

    oled_UpdateScreen();
}
