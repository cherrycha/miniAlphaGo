/*
 * 蒙特卡洛树搜索逻辑
 */


#include <malloc.h>
#include <mem.h>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include "MCTS.h"
#include "chess_board.h"

#define Computation_Budget 10000;

/*
 * 根据当前局势初始化蒙特卡洛树根节点
 */
struct MCTS_node *new_MCTS_root(struct chess_board *game) {
    struct MCTS_node *this_node = (struct MCTS_node *) malloc(sizeof(struct MCTS_node));
    this_node->state = (struct chess_board *) malloc(sizeof(struct chess_board));
    this_node->children_tail = NULL;
    this_node->this_tail = NULL;
//    printf("-----new root------\n");
//    printf("malloc begin\n");
    memcpy(this_node->state, game, sizeof(struct chess_board));//先返回一个和parent一模一样的棋局
//    printf("x=%llx y=%llx\n", this_node->state->board_isOccupied,this_node->state->board_color);
    this_node->parent = NULL;
    this_node->children_head = NULL;
    this_node->next_sibling = NULL;
    this_node->stimul_times = 0;
    this_node->win_times = 0;
//    printf("malloc over\n");
    return this_node;
}

/*
 * 输入：父节点指针
 * 为父节点初始化一个蒙特卡洛树叶节点
 * 输出：叶节点指针
 */
struct MCTS_node *new_MCTS_leaf(struct MCTS_node *parent) {
    struct MCTS_node *this_node = new_MCTS_root(parent->state);
    this_node->parent = parent;
    return this_node;
}

/*
 * 输入：当前结点指针，父节点模拟次数
 * 计算当前节点的ucb值
 * 输出：当前节点的ucb值
 */
float get_ucb(struct MCTS_node *this_node, int N_total) {
    float N, W;
    N = this_node->stimul_times;
    W = this_node->win_times;
    if (N == 0) {
        return INFINITY - 1;
    }
//    printf("%f\n",(float) (W / N + sqrt(log(N_total) * C / N)));
    return (float) (W / N + sqrt(log(N_total) * C / N));
}


/*
 * 输入：父结点指针
 * 为父节点进行一次expansion操作
 */
void expansion(struct MCTS_node *parent) {
//    printf("----expansion-----\n");
    struct MCTS_node *children_head = NULL;
    struct MCTS_node *children_this = NULL;
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (is_valid_step(parent->state->this_color, parent->state->board_isOccupied, parent->state->board_color, i,
                              j)) {
                if (children_this == NULL) {
                    children_head = new_MCTS_leaf(parent);
//                    printf("parent==null:%d\n",children_head->parent==NULL);
                    children_head->state = get_next_state(children_head->state, i, j);
//                    printf("head:next_x=%llx next_y=%llx\n", children_head->state->board_isOccupied,children_head->state->board_color);
                    children_this = children_head;
                } else {
                    struct MCTS_node *children_next = new_MCTS_leaf(parent);
                    children_next->state = get_next_state(children_next->state, i, j);
//                    printf("next_x=%llx next_y=%llx\n", children_next->state->board_isOccupied,children_next->state->board_color);
                    children_this->next_sibling = children_next;
                    children_this = children_next;
                }
            }
        }
    }
    parent->children_head = children_head;
    parent->this_tail = NULL;//待拓展节点
    parent->children_tail = children_this;
//    return parent;
}


/*
 * 输入：当前结点指针
 * 为当前节点进行一次蒙特卡洛模拟
 * 输出：获胜玩家
 */
int rollout(struct MCTS_node *parent) {//优先级可以在这里加
    int winner;
    struct MCTS_node *this_node = (struct MCTS_node *) malloc(sizeof(struct MCTS_node));
    this_node->state = (struct chess_board *) malloc(sizeof(struct chess_board));
    memcpy(this_node->state, parent->state, sizeof(*parent->state));
    while (!this_node->state->is_end) {
        this_node->state = get_next_ai_state(this_node->state);
    }
    winner = this_node->state->is_end;
    free(this_node);
    return winner;
}


/*
 * 输入：当前结点指针，模拟获胜玩家
 * 为当前节点进行一次backpropagation
 */
void backpropagation(struct MCTS_node *node, int winner) {
//    printf("%d",node->parent == NULL);
    while (node->parent != NULL) {
        node->stimul_times += 1;
//        printf("stimul_times:%d\n",node->stimul_times);
        if (winner == node->parent->state->this_color) {
            node->win_times += 1;
        }
        node = node->parent;
    }
    node->stimul_times += 1;
//    return 1;
}


/*
 * 输入：根结点指针
 * 选择ucb最优节点
 * 输出：最优节点指针
 */
struct MCTS_node *selection_noncur(struct MCTS_node *root) {
    struct MCTS_node *best_node = root;
    struct MCTS_node *this_node = root->children_head;
    int N = root->stimul_times;
    float best_ucb = -1;
    float max_stimul = -1;
    while (this_node != NULL) {
//        printf("node:%d/%d\n",this_node->win_times,this_node->stimul_times);
        if (this_node->stimul_times > max_stimul) {
            best_node = this_node;
            max_stimul = this_node->stimul_times;
        }
//        float this_ucb = get_ucb(this_node, N);
//        if (this_ucb > best_ucb) {
//            best_ucb = this_ucb;
//            best_node = this_node;
//        }
        this_node = this_node->next_sibling;
    }
    printf("p:%d/%d,%f\n", best_node->win_times, best_node->stimul_times,
           ((float) best_node->win_times) / ((float) best_node->stimul_times));
//    printf("x=%llx y=%llx\n", best_node->state->board_isOccupied,best_node->state->board_color);
    return best_node;
}

int isTerminal(struct MCTS_node *node) {
    if (node->children_head == NULL) {//还未生成子节点，未拓展完毕
        expansion(node);
    }
    return node->state->is_end != 0;//棋局是否结束，即terminal
//    if(node->children_head==NULL){//还未生成子节点，未拓展完毕
//        expansion(node);//为它生成所有子节点
//        printf("expansion_success:%d",node->children_head==NULL);
//        return 1;
//    }else if(node->this_tail==node->children_tail){//已经拓展完毕了
//        return 0;
//    }
//    return 0;

}

/*
 * 选取ucb值最优的子节点
 */
struct MCTS_node *best_child(struct MCTS_node *parent, int player_color) {
//    printf("------------best-child-----------\n");
    struct MCTS_node *best_node = parent;
    struct MCTS_node *this_node;
    float best_ucb_same = -1;
    float best_ucb_diff = INFINITY;
    int N = parent->stimul_times;
    this_node = parent->children_head;//访问子节点，选择ucb最优
    while (this_node != NULL) {
//        printf("--------!!!----------\n");
        float this_ucb = get_ucb(this_node, N);
//        printf("ucb:%f\n",get_ucb(this_node,N));
        if (parent->state->this_color == player_color) {
//            printf("max\n");
            if (this_ucb > best_ucb_same) {
                best_ucb_same = this_ucb;
                best_node = this_node;
            }
        } else {
//            printf("min\n");
            if (this_ucb < best_ucb_diff) {
                best_ucb_diff = this_ucb;
                best_node = this_node;
            }
        }
        this_node = this_node->next_sibling;
    }
//    printf("-------return-----best-child-----------\n");
    return best_node;
}

/*
 * 根据策略进行节点拓展及蒙特卡洛模拟
 */
struct MCTS_node *treePolicy(struct MCTS_node *root) {
    struct MCTS_node *best_node = root;
    struct MCTS_node *this_node;
//    printf("----treepolicy--------\n");
//    while (best_node->children_head != NULL) {//当当前最佳节点不是叶节点的时候
    while (!isTerminal(best_node)) {//当当前最佳节点non-terminal的时候
        if (best_node->this_tail != best_node->children_tail) {//若未拓展完毕
//            printf("here1\n");
            if (best_node->this_tail == NULL) {//一个都没拓展
                this_node = best_node->children_head;
                best_node->this_tail = best_node->children_head;
            } else {
                this_node = best_node->this_tail;//选择当前应该拓展的节点
                best_node->this_tail = best_node->this_tail->next_sibling;//更新下一个拓展节点
            }
            return this_node;//拓展新的节点并返回
        } else {//所有子节点都被访问过了
//            printf("here2\n");
            best_node = best_child(best_node, root->state->this_color);
        }
    }
//    printf("ucb:%f",get_ucb(best_node,best_node->parent->stimul_times));
    return best_node;
}


/*
 * 释放节点内存
 * 输入：节点指针
 */
void free_node(struct MCTS_node *root) {
    free(root->state);
    free(root);
}

/*
 * 释放内存
 * 输入：根结点指针
 */
int free_mem(struct MCTS_node *root) {
    if (root->children_head != NULL) {
        free_mem(root->children_head);
    } else if (root->next_sibling != NULL) {
        free_mem(root->next_sibling);
    }
    free_node(root);
    return 1;
}

/*
 * 功能：进行一次蒙特卡洛树搜索
 * 输入：当前局势
 */
double search(struct chess_board *game) {
    clock_t start, finish;
    double duration = 0;
    start = clock();
    struct MCTS_node *root = new_MCTS_root(game);
//    expansion(root);
    int i = 0;
//    printf("----search-----\n");
    while (i < SEARCH_TIMES && (duration < SEARCH_TIME - 0.1)) {//在search budget限制内循环模拟
        struct MCTS_node *best_node = treePolicy(root);//根据treePolicy选择最优叶节点
//        printf("tree_finish\n");
        int winner = rollout(best_node);
        backpropagation(best_node, winner);
        if (i % 100 == 0) {
            printf("i=%d\n", i);
        }
        i++;
        finish = clock();
        duration = (double) (finish - start) / CLOCKS_PER_SEC;
    }
    finish = clock();
    duration = (double) (finish - start) / CLOCKS_PER_SEC;

    struct chess_board *next_state = selection_noncur(root)->state;

    unsigned long long this_step = game->board_isOccupied ^next_state->board_isOccupied;
    int j = -1;
    while (this_step != 0) {
        this_step <<= 1;
        j++;
    }
    printf("%d %d\n", j / BOARD_SIZE, j % BOARD_SIZE);
    memcpy(game, next_state, sizeof(struct chess_board));
    printf("--------over----------\n");
    free_mem(root);
    return duration;
}