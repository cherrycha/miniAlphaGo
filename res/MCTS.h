/*
 * 蒙特卡洛树搜索逻辑
 */

#ifndef REVERSI_MCTS_H
#define REVERSI_MCTS_H
#define C 2
#define MAX 3000000
#define SEARCH_TIMES 200000//200000//最大搜索次数
#define SEARCH_TIME 30          //最长搜索时间
#define INFINITE 3000000


struct MCTS_node{
    struct chess_board * state;//棋盘状态
    struct MCTS_node* parent;//父节点
    struct MCTS_node* children_head;//第一个儿子节点
    struct MCTS_node* children_tail;//最后一个儿子节点
    struct MCTS_node* this_tail;//当前被模拟过的最后一个儿子节点
    struct MCTS_node* next_sibling;//下一个兄弟节点
    int stimul_times ;//模拟次数
    int win_times;//获胜次数
};

double search(struct chess_board * game);


#endif //REVERSI_MCTS_H
