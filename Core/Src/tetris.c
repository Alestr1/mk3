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
    for(int y=0; y<4; y++) {
        for(int x=0; x<4; x++) {
            if(temp[y][x]) {
                int nx = s->x + x + dx;
                int ny = s->y + y + dy;
                if(nx < 0 || nx >= BOARD_W || ny >= BOARD_H) return 1;
                if(ny >= 0 && s->board[ny][nx]) return 1;
            }
        }
    }
    return 0;
}

void Tetris_Init(TetrisState* s) {
    for(int y=0; y<BOARD_H; y++)
        for(int x=0; x<BOARD_W; x++)
            s->board[y][x] = 0;
    s->score = 0;
    s->speed = 500;
    Tetris_Spawn(s);
}

void Tetris_Spawn(TetrisState* s) {
    int rnd = rand() % 7;
    for(int y=0; y<4; y++)
        for(int x=0; x<4; x++)
            s->shape[y][x] = FIGURES[rnd][y][x];
    s->x = 3; s->y = -1;
}

void Tetris_Drop(TetrisState* s) {
    while(!check_collision(s, 0, 1, s->shape)) s->y++;
}

void Tetris_Move(TetrisState* s, int dx, int dy) {
    if(!check_collision(s, dx, dy, s->shape)) {
        s->x += dx; s->y += dy;
    }
}

void Tetris_Rotate(TetrisState* s) {
    uint8_t temp[4][4];
    for(int y=0; y<4; y++)
        for(int x=0; x<4; x++)
            temp[y][x] = s->shape[3-x][y];
    if(!check_collision(s, 0, 0, temp)) {
        for(int y=0; y<4; y++)
            for(int x=0; x<4; x++)
                s->shape[y][x] = temp[y][x];
    }
}

int Tetris_Step(TetrisState* s) {
    if(!check_collision(s, 0, 1, s->shape)) {
        s->y++;
    } else {
        for(int y=0; y<4; y++)
            for(int x=0; x<4; x++)
                if(s->shape[y][x]) {
                    int ny = s->y + y;
                    if(ny < 0) return 1;
                    s->board[ny][s->x + x] = 1;
                }
        // Очистка линий
        for(int y=BOARD_H-1; y>=0; y--) {
            int full = 1;
            for(int x=0; x<BOARD_W; x++) if(!s->board[y][x]) full = 0;
            if(full) {
                s->score++;
                if(s->speed > 100) s->speed -= 20;
                for(int yy=y; yy>0; yy--)
                    for(int x=0; x<BOARD_W; x++) s->board[yy][x] = s->board[yy-1][x];
                y++;
            }
        }
        Tetris_Spawn(s);
    }
    return 0;
}

void Tetris_Render(TetrisState* s) {
    oled_Fill(Black);


    for(int y=0; y<BOARD_H; y++) {
        for(int x=0; x<BOARD_W; x++) {
            int draw = s->board[y][x];
            if(!draw && s->y >= 0) {
                int rx = x - s->x, ry = y - s->y;
                if(rx>=0 && rx<4 && ry>=0 && ry<4 && s->shape[ry][rx]) draw = 1;
            }
            if(draw) oled_DrawSquare(x*3, x*3+2, y*3, y*3+2, White);
        }
    }

    oled_DrawVLine(0, 60, 32, White);

    // UI
    char buf[10];
    oled_SetCursor(36, 2);
    sprintf(buf, "S:%d", s->score);
    oled_WriteString(buf, Font_7x10, White);

    oled_SetCursor(36, 20);
    oled_WriteString("Ctrl:", Font_7x10, White);
    oled_SetCursor(36, 32);
    oled_WriteString("2:Rotate", Font_7x10, White);
    oled_SetCursor(36, 42);
    oled_WriteString("1:Left", Font_7x10, White);
    oled_SetCursor(36, 52);
    oled_WriteString("3:Right", Font_7x10, White);

    oled_UpdateScreen();
}

