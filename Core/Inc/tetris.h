#ifndef TETRIS_H
#define TETRIS_H

#include <stdint.h>

#define BOARD_W 10
#define BOARD_H 20


#define KEY_LEFT   0x01
#define KEY_ROTATE 0x02
#define KEY_RIGHT  0x04
#define KEY_DOWN   0x11
#define KEY_DROP   0x12

typedef struct {
    uint8_t board[BOARD_H][BOARD_W];
    uint8_t shape[4][4];
    int x, y;
    int score;
    uint32_t speed;
    uint8_t state;
} TetrisState;

void Tetris_Init(TetrisState* s);
void Tetris_Spawn(TetrisState* s);
void Tetris_Move(TetrisState* s, int dx, int dy);
void Tetris_Rotate(TetrisState* s);
int  Tetris_Step(TetrisState* s);
void Tetris_Render(TetrisState* s);
void Tetris_Drop(TetrisState* s);
#endif
