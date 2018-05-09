/*
 * 棋盘逻辑
 */

#include <stdio.h>
#include <stdlib.h>
#include "chess_board.h"

/*
 * 判断是否为棋盘上的有效位置
 * 输入：横坐标，纵坐标
 */
int is_valid_position(int i, int j) {
    return i >= 0 && i < BOARD_SIZE && j >= 0 && j < BOARD_SIZE;
}
/*
 * 获取另一种颜色，输入黑子输出白子，输入白子输出黑子。
 * 输入：颜色
 */
int get_another_color(int this_color) {
    return this_color == black ? white : black;
}

/*
 * 棋子位置可以落子则返回1，否则返回0
 * 输入：棋盘落子位置，横坐标，纵坐标
 */
int is_empty(unsigned long long board_isFull, int i, int j) {
    if (!is_valid_position(i, j))return 0;
    if (board_isFull << (i * BOARD_SIZE + j) & INT64_1) {
        return 0;
    } else {
        return 1;
    }
}

/*
 * 判断此坐标位置是否是黑子
 * 输入：棋盘落子位置，棋盘落子颜色，横坐标，纵坐标
 */
int is_black(unsigned long long board_isOccupied, unsigned long long board_color, int i, int j) {//此位置有黑子返回1，否则返回0
    if (!is_valid_position(i, j))return 0;
    if (is_empty(board_isOccupied, i, j)) {
        return 0;
    }
    if (board_color << i * BOARD_SIZE + j & INT64_1) {
        return 1;
    } else {
        return 0;
    }
}
/*
 * 判断此坐标位置是否是白子
 * 输入：棋盘落子位置，棋盘落子颜色，横坐标，纵坐标
 */
int is_white(unsigned long long board_isOccupied, unsigned long long board_color, int i, int j) {//此位置有白子返回1，否则返回0
    if (!is_valid_position(i, j))return 0;
    if (is_empty(board_isOccupied, i, j)) {
        return 0;
    }
    if (board_color << (i * BOARD_SIZE + j) & INT64_1) {
        return 0;
    } else {
        return 1;
    }
}

/*
 * 判断此坐标位置是否是与输入颜色不同色的棋子
 * 输入：棋盘落子位置，棋盘落子颜色，横坐标，纵坐标
 */
int is_different_color(int current_color, unsigned long long board_isOccupied, unsigned long long board_color,
                       int temp_position_x, int temp_position_y) {//此位置有其他颜色的棋子返回1，否则返回0
    if (current_color == black) {
        return is_white(board_isOccupied, board_color, temp_position_x, temp_position_y);
    } else {
        return is_black(board_isOccupied, board_color, temp_position_x, temp_position_y);
    }

}


/*
 * 判断此坐标位置是否是与输入颜色同色的棋子
 * 输入：当前颜色，棋盘落子位置，棋盘落子颜色，横坐标，纵坐标
 */
int is_same_color(int current_color, unsigned long long board_isOccupied, unsigned long long board_color,
                  int temp_position_x, int temp_position_y) {
    if (is_valid_position(temp_position_x, temp_position_y)) {
        if (current_color == black) {
            return is_black(board_isOccupied, board_color, temp_position_x, temp_position_y);
        } else {
            return is_white(board_isOccupied, board_color, temp_position_x, temp_position_y);
        }
    }
    else {
        return 0;
    }
}



/*
 * 根据当前横坐标，获取向输入方向位移1格的棋子横坐标
 */
int get_next_position_x(int dir, int i) {
    switch (dir) {//从正上方开始，顺时针
        case 1:
        case 2:
        case 3:
            i++;
            break;
        case 5:
        case 6:
        case 7:
            i--;
            break;
        default:
            break;
    }
    return i;
}

/*
 * 根据当前纵坐标，获取向输入方向位移1格的棋子纵坐标
 */
int get_next_position_y(int dir, int j) {
    switch (dir) {//从正上方开始，顺时针
        case 0:
        case 1:
        case 7:
            j--;
            break;
        case 4:
        case 3:
        case 5:
            j++;
            break;
        default:
            break;
    }
    return j;
}

/*
 * 获取当前局势下，输入颜色是否有合法位置可以落子
 */
int can_continue(int color, unsigned long long board_isOccupied, unsigned long long board_color) {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (is_valid_step(color, board_isOccupied, board_color, i, j)) {
                return 1;
            }
        }
    }
    return 0;
}

int get_chess_num(unsigned long long board){
    int chess_num=0;
    while (board) {
        chess_num++;
        board= board & (board - 1);
    }
    return chess_num;
}

int get_black_num(unsigned long long board){
    int black_num=0;
    while (board) {
        black_num++;
        board = board & (board - 1);
    }
    return black_num;
}
/*
 * 获取当前局势下的赢家
 */
int get_winner(struct chess_board *game) {
    int chess_num = 0, white_num, black_num = 0;
    chess_num=get_chess_num(game->board_isOccupied);
    black_num=get_black_num(game->board_color);
    white_num = chess_num - black_num;
    //1表示黑棋获胜，2表示白棋获胜，3表示平局
    if(white_num==black_num){
        game->is_end=3;
    }else{
        game->is_end = white_num > black_num ? white : black;
    }
    return game->is_end;
}

/*
 * 根据落子位置，翻转相应的对手方棋子
 */
int reverse_the_chess(int player_color, unsigned long long *board_isOccupied, unsigned long long *board_color,
                      int temp_position_x, int temp_position_y) {
    int next_position_x, next_position_y;
    for (int dir = 0; dir < 8; dir++) {
        next_position_x = get_next_position_x(dir, temp_position_x);
        next_position_y = get_next_position_y(dir, temp_position_y);
        if (is_valid_position(next_position_x, next_position_y)) {//此方向下一步棋在棋盘内
            if (is_different_color(player_color, *board_isOccupied, *board_color, next_position_x,
                                   next_position_y)) {//遇到一个对方的棋子
                while (is_different_color(player_color, *board_isOccupied, *board_color, next_position_x,
                                          next_position_y) &&
                       is_valid_position(temp_position_x, temp_position_y)) {//继续走直到遇到非对方的棋子，可能是空,可能是墙，也可能是自己的棋子
                    next_position_x = get_next_position_x(dir, next_position_x);
                    next_position_y = get_next_position_y(dir, next_position_y);
                }
                if (is_empty(*board_isOccupied, next_position_x, next_position_y) ||
                    !is_valid_position(next_position_x, next_position_y)) {//并不是自己的棋子，或者遇到了墙，不能翻转
                    continue;
                } else {
                    while ((next_position_x != temp_position_x) || (
                            next_position_y != temp_position_y)) {//翻转当前的棋子直到回到当前的落子
                        next_position_x = get_next_position_x((dir + 4) % 8, next_position_x);
                        next_position_y = get_next_position_y((dir + 4) % 8, next_position_y);
                        if (player_color == black) {
                            *board_color |= INT64_BLACK >> ((next_position_x) * BOARD_SIZE + next_position_y);
                        } else {//将黑色位变成白色位
                            *board_color = ((INT64_WHITE >> ((next_position_x) * BOARD_SIZE + next_position_y)) |
                                            (INT64_COMP << (BOARD_SIZE * BOARD_SIZE - (next_position_x) * BOARD_SIZE -
                                                            next_position_y))) & (*board_color);
                        }
                    }
                }
            }
        }
    }
    return 1;
}

/*
 * 获取当前局势下，输入坐标是否为玩家的合法落子位置
 */
int is_valid_step(int player_color, unsigned long long board_isOccupied, unsigned long long board_color,
                  int temp_position_x, int temp_position_y) {//todo:x和y其实只需要用一个数表示
    if (!is_empty(board_isOccupied, temp_position_x, temp_position_y)) {
        return 0;
    }
    if (is_valid_position(temp_position_x, temp_position_y) &&
        is_empty(board_isOccupied, temp_position_x, temp_position_y)) {
//        printf("-----------\n");
//        printf("x=%d y=%d\n", temp_position_x, temp_position_y);
        int next_position_x, next_position_y;
        for (int dir = 0; dir < 8; dir++) {
            next_position_x = get_next_position_x(dir, temp_position_x);
            next_position_y = get_next_position_y(dir, temp_position_y);
//            printf("dir=%d x=%d y=%d\n", dir, temp_position_x, temp_position_y);
//            printf("next_x=%d next_y=%d\n", next_position_x, next_position_y);
            if (is_valid_position(next_position_x, next_position_y)) {//此方向下一步棋在棋盘内
                if (is_different_color(player_color, board_isOccupied, board_color, next_position_x,
                                       next_position_y)) {//遇到一个对方的棋子
                    while (is_different_color(player_color, board_isOccupied, board_color, next_position_x,
                                              next_position_y)) {//继续走直到遇到非对方的棋子，可能是空，也可能是自己的棋子
//                        printf("dir=%d x=%d y=%d\n", dir, temp_position_x, temp_position_y);
//                        printf("next_x=%d next_y=%d\n", next_position_x, next_position_y);
                        next_position_x = get_next_position_x(dir, next_position_x);
                        next_position_y = get_next_position_y(dir, next_position_y);
                    }
                    if (is_empty(board_isOccupied, next_position_x, next_position_y) ||
                        (!is_valid_position(next_position_x, next_position_y))) {//并不是自己的棋子，不能翻转
                        continue;
                    } else {
                        return 1;
                    }
                }
            } else {
                continue;
            }

        }
    } else {//超出棋盘之外
        return 0;
    }
    return 0;
}

/*
 * 根据落子位置，获取落子后的棋盘状态
 */
struct chess_board *get_next_state(struct chess_board *game, int temp_position_x, int temp_position_y) {
    if (is_valid_step(game->this_color, game->board_isOccupied, game->board_color, temp_position_x,
                      temp_position_y)) {
        //合法，在八个方向翻转对方的棋子
        reverse_the_chess(game->this_color, &(game->board_isOccupied),
                          &(game->board_color), temp_position_x, temp_position_y);
        game->board_isOccupied |= INT64_1 >> temp_position_x * BOARD_SIZE + temp_position_y;
        if (game->this_color == black) {
            game->board_color |= INT64_BLACK >> temp_position_x * BOARD_SIZE + temp_position_y;
        } else {
            game->board_color |= 0 >> temp_position_x * BOARD_SIZE + temp_position_y;
        }
    }

    if (can_continue(get_another_color(game->this_color), game->board_isOccupied, game->board_color))//对方有棋可下
        game->this_color = get_another_color(game->this_color);
    else {//对方无棋可下
        if (!can_continue(game->this_color, game->board_isOccupied, game->board_color)) {//自己也无棋可下
            game->this_color = none;
            game->is_end = get_winner(game);
        }
    }
    return game;
}

/*
 * 根据当前局势为当前玩家随机落子，蒙特卡洛模拟
 */
struct chess_board *get_next_ai_state(struct chess_board *game) {
    int a[30];
    int b[30];
    int k=0;
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (is_valid_step(game->this_color, game->board_isOccupied, game->board_color, i, j)) {
                a[k]=i;
                b[k]=j;
                k++;
            }
        }
    }
    int c= rand()%k;
    game = get_next_state(game, a[c], b[c]);
    return game;
}