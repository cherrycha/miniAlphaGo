# This is a reversi by ZhaYue 3150103588

## compile command
windres resource.rc resource.o
gcc -mwindows chess_board.c MCTS.c reversi.c  resource.h  resource.o -o game

## 调整模拟次数或时间
- 请调整MCTS.h中的SEARCH_TIMES（次数）或SEARCH_TIME（时间）

## 可以优化的
- 加入多线程操作
- 每次不再新建根节点