/*
 * 棋盘逻辑
 */


#ifndef REVERSI_GAME_H
#define REVERSI_GAME_H

/*---------------------------------------
 *一些关于黑白棋的宏定义
 */
#define BOARD_SIZE          8//棋盘大小
// 使用64位unsigned long long表示棋盘的状态
// 因此需要使用一些宏定义的常数方便位运算
#define INT64_1             0x8000000000000000//用于棋盘落子状态计算
#define INT64_BLACK         0x8000000000000000//用于黑棋落子状态计算
#define INT64_WHITE         0x7FFFFFFFFFFFFFFF//用于白棋落子状态计算
#define INT64_COMP          0xffffffffffffffff//用于白棋落子状态计算
#define New_Board_isEmpty   0x0000001818000000//棋盘落子初始状态
#define New_Board_color     0x0000000810000000//棋盘落子颜色初始状态

//结构体chess_board，表示棋盘状态
struct chess_board {
    int this_color;//此状态应该下棋的颜色
    unsigned long long board_color;//表示棋盘上落子的颜色状态
    unsigned long long board_isOccupied;//表示棋盘上落子的状态
    int is_end;//判断是否结束，如果未结束=0，黑棋获胜为1，白棋获胜为2
};
//枚举，表示棋子状态
enum chess_type {
    none = 0,
    black = 1,
    white = 2,
};

int get_black_num(unsigned long long board);

int get_chess_num(unsigned long long board);

struct chess_board *get_next_state(struct chess_board *game, int temp_position_x, int temp_position_y);

struct chess_board *get_next_ai_state(struct chess_board *game);

int is_empty(unsigned long long board_isFull, int i, int j);

int is_black(unsigned long long board_isOccupied, unsigned long long board_color, int i, int j);

int reverse_the_chess(int player_color, unsigned long long *board_isOccupied, unsigned long long *board_color,
                      int temp_position_x, int temp_position_y);

int is_valid_step(int player_color, unsigned long long board_isOccupied, unsigned long long board_color,
                  int temp_position_x, int temp_position_y);

int can_continue(int color, unsigned long long board_isOccupied, unsigned long long board_color);

#endif //REVERSI_GAME_H
