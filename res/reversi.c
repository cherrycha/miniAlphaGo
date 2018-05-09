/*
 * 图形界面逻辑
 */

#include <Windows.h>
#include <stdio.h>
#include <time.h>
#include "resource.h"
#include "chess_board.h"
#include "MCTS.h"

struct chess_board *game;
struct chess_board *game_backup;
double time_used;
double time_this_step;
int white_num;
int black_num;

// 必须要进行前导声明
LRESULT CALLBACK WindowProc(
        HWND hwnd,
        UINT uMsg,
        WPARAM wParam,
        LPARAM lParam
);

int this_player = black;
int next_player = black;
int ai_player = white;


void show_time(double time) {
    char buf[100];
    sprintf(buf, "%f", time);
    char buf2[3] = "s.";
    MessageBox(0, strcat(buf, buf2), "Time used", MB_OK);
}

/*
 * 初始化游戏
 */
struct chess_board *init_game(HWND hwnd) {
    srand(time(NULL));//初始化随机数种子
    time_used = 0;
    time_this_step = 0;
    white_num=2;
    black_num=2;
//    struct chess_board *this_game = (struct chess_board *) malloc(sizeof(struct chess_board));
    game = (struct chess_board *) malloc(sizeof(struct chess_board));
    game_backup = (struct chess_board *) malloc(sizeof(struct chess_board));
    this_player = next_player;
    game->this_color = black;
    game->is_end = 0;
    ai_player = (this_player == black ? white : black);
    game->board_isOccupied = New_Board_isEmpty;
    game->board_color = New_Board_color;
    if (black == ai_player) {
        time_this_step = search(game);
        time_used += time_this_step;
        black_num=get_black_num(game->board_color);
        white_num=get_black_num(game->board_isOccupied)-black_num;
    }
    InvalidateRect(hwnd, NULL, TRUE);
    UpdateWindow(hwnd);
    return game;
}

//窗口函数
int CALLBACK WinMain(
        HINSTANCE hInstance,
        HINSTANCE hPrevInstance,
        LPSTR lpCmdLine,
        int nCmdShow
) {
    HACCEL haccel;
    HICON hIcon1;
    hIcon1 = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
    //创建菜单
    haccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR1));
    //类名
    LPCSTR cls_Name = "MyClass";
    //设计窗口类
    WNDCLASS wc = {sizeof(WNDCLASS)};
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hCursor = LoadCursor(hInstance, IDC_ARROW);
    wc.hIcon = hIcon1;
    wc.lpszMenuName = NULL;
    wc.style = CS_HREDRAW | CS_VREDRAW;//水平和垂直重画
    wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
    wc.lpfnWndProc = WindowProc;
    wc.lpszClassName = cls_Name;
    wc.hInstance = hInstance;
    // 注册窗口类
    RegisterClass(&wc);

    //创建窗口
    HWND hwnd = CreateWindow(
            cls_Name,           //类名，要和刚才注册的一致
            "Welcome to my Reversi!",  //窗口标题文字
            WS_OVERLAPPEDWINDOW, //窗口外观样式
            0,             //窗口相对于父级的X坐标
            0,             //窗口相对于父级的Y坐标
            BOARD_SIZE * 100,                //窗口的宽度
            BOARD_SIZE * 100,                //窗口的高度
            NULL,               //没有父窗口，为NULL
            LoadMenu(hInstance, MAKEINTRESOURCE(IDR_MENU1)),
            hInstance,          //当前应用程序的实例句柄
            NULL);              //没有附加数据，为NULL
    if (hwnd == NULL) //检查窗口是否创建成功
        return 0;

    game = init_game(hwnd);
//    SetTimer(hwnd, 1, 50, NULL); //一个0.1秒触发一次的定时器,防止闪屏
    // 显示窗口
    ShowWindow(hwnd, SW_SHOW);
    // 更新窗口
    UpdateWindow(hwnd);
    // 消息循环
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        if (!TranslateAccelerator(hwnd, haccel, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    return 0;
}


LRESULT CALLBACK WindowProc(
        HWND hwnd,
        UINT uMsg,
        WPARAM wParam,
        LPARAM lParam
) {
    auto hmm = GetMenu(hwnd);
    auto hfmn = GetSubMenu(hmm, 1);
    //消息处理
    switch (uMsg) {
        case WM_COMMAND: {
            switch (LOWORD(wParam)) {
                case ID_BACK: {
                    memcpy(game, game_backup, sizeof(struct chess_board));
                    InvalidateRect(hwnd, NULL, TRUE);
                    UpdateWindow(hwnd);
                }
                    break;
                case ID_HINT: {
                }
                    break;
                case ID_PLAYER_BLACK: {
                    next_player = black;
                    if (IDOK == MessageBox(hwnd, "Do you want to start a new game right now?(BLACK CHESS)", "Hint",
                                           MB_OKCANCEL | MB_ICONWARNING | MB_DEFBUTTON1)) {
                        free(game);
                        game = init_game(hwnd);//重新开始游戏
                    }
                }
                    break;
                case ID_PLAYER_WHITE: {
                    next_player = white;
                    if (IDOK == MessageBox(hwnd, "Do you want to start a new game right now?(WHITE CHESS)", "Hint",
                                           MB_OKCANCEL | MB_ICONWARNING | MB_DEFBUTTON1)) {
                        free(game);
                        game = init_game(hwnd);//重新开始游戏
                    }
                }
                    break;
                    //重新开始游戏
                case ID_NEW_GAME:
                    if (IDOK == MessageBox(hwnd, "Do you want to Start a new game?", "Hint",
                                           MB_OKCANCEL | MB_ICONWARNING | MB_DEFBUTTON1)) {//默认确定
                        free(game);
                        game = init_game(hwnd);
                    }
                    break;
                    //退出游戏
                case ID_EXIT_GAME:
                    if (IDOK == MessageBox(hwnd, "Do you want to exit?", "Hint",
                                           MB_OKCANCEL | MB_ICONWARNING | MB_DEFBUTTON2)) {//默认取消
                        SendMessageA(hwnd, WM_DESTROY, NULL, NULL);
                    }
                    break;
                case ID_HELP:
                    MessageBox(hwnd, "Please use the mouse to play ^_^!", "Help", MB_OK);
                    break;
                default:
                    break;
            }
        }
            break;
            //鼠标左键按下
        case WM_LBUTTONDOWN:
//            mouse_flag = 1;
            break;
            //鼠标左键抬起
        case WM_LBUTTONUP: {
//            mouse_flag = 0;
            LPRECT temp_window_pos = malloc(sizeof(RECT));
            GetWindowRect(hwnd, temp_window_pos);
            int window_size_x = temp_window_pos->right - temp_window_pos->left;
            int window_size_y = temp_window_pos->bottom - temp_window_pos->top;
            int board_size = window_size_x > window_size_y ? window_size_y : window_size_x;
            int cell_size = board_size / (BOARD_SIZE + 1);
            int temp_position_x, temp_position_y;
            temp_position_x = LOWORD(lParam) / cell_size;//鼠标的横坐标
            temp_position_y = HIWORD(lParam) / cell_size;//鼠标的纵坐标
            if (is_valid_step(game->this_color, game->board_isOccupied, game->board_color, temp_position_x,
                              temp_position_y)) {
                memcpy(game_backup, game, sizeof(struct chess_board));
                game = get_next_state(game, temp_position_x, temp_position_y);
                InvalidateRect(hwnd, NULL, FALSE);
                UpdateWindow(hwnd);
                if (game->is_end > 0) {
                    if (this_player == game->is_end) {
                        MessageBox(hwnd, "You Win!", "Game Over",
                                           MB_OKCANCEL | MB_ICONWARNING | MB_DEFBUTTON1);
                    } else if (ai_player == game->is_end) {
                        MessageBox(hwnd, "You Lose!", "Game Over",
                                           MB_OKCANCEL | MB_ICONWARNING | MB_DEFBUTTON1);
                    } else {
                        MessageBox(hwnd, "Dogfall!", "Game Over",
                                   MB_OKCANCEL | MB_ICONWARNING | MB_DEFBUTTON1);
                    }
                    show_time(time_used);
                    printf("%f", time_used);
                    free(game);
                    game = init_game(hwnd);
                } else {
                    if (game->this_color == ai_player) {
                        time_this_step = search(game);
                        time_used += time_this_step;
                        black_num=get_black_num(game->board_color);
                        white_num=get_black_num(game->board_isOccupied)-black_num;
                        while (game->this_color == ai_player) {
                            MessageBox(hwnd, "You pass!", "Please continue",
                                       MB_OK | MB_ICONWARNING | MB_DEFBUTTON1);
//                            get_next_ai_state(game);
                            time_this_step = search(game);
                            time_used += time_this_step;
                            black_num=get_black_num(game->board_color);
                            white_num=get_black_num(game->board_isOccupied)-black_num;
                            InvalidateRect(hwnd, NULL, TRUE);
                            UpdateWindow(hwnd);

                        }
                    } else {
                        if (game->this_color == this_player) {
                            MessageBox(hwnd, "AI pass!", "Please continue",
                                       MB_OK | MB_ICONWARNING | MB_DEFBUTTON1);
                        }
                    }
                    InvalidateRect(hwnd, NULL, FALSE);
                    UpdateWindow(hwnd);
                    if (game->is_end > 0) {
                        if (this_player == game->is_end) {
                            MessageBox(hwnd, "You Win!", "Game Over",
                                       MB_OKCANCEL | MB_ICONWARNING | MB_DEFBUTTON1);
                            MessageBox(hwnd, "", "Game Over",
                                       MB_OKCANCEL | MB_ICONWARNING | MB_DEFBUTTON1);
                        } else if (ai_player == game->is_end) {
                            MessageBox(hwnd, "You Lose!", "Game Over",
                                       MB_OKCANCEL | MB_ICONWARNING | MB_DEFBUTTON1);
                        } else {
                            MessageBox(hwnd, "Dogfall!", "Game Over",
                                       MB_OKCANCEL | MB_ICONWARNING | MB_DEFBUTTON1);
                        }
                        show_time(time_used);
                        printf("%f", time_used);
                        free(game);
                        game = init_game(hwnd);
                    }
                }

            } else {
                MessageBox(hwnd, "You can't put your chess here.", "Hint",
                           MB_OK | MB_ICONWARNING | MB_DEFBUTTON2);
            }
            InvalidateRect(hwnd, NULL, TRUE);
            UpdateWindow(hwnd);
        }
            //鼠标移动事件
        case WM_MOUSEMOVE: {
            LPRECT temp_window_pos = malloc(sizeof(RECT));
            GetWindowRect(hwnd, temp_window_pos);
            if (LOWORD(lParam) > temp_window_pos->left && LOWORD(lParam) < temp_window_pos->right &&
                HIWORD(lParam) > temp_window_pos->top && HIWORD(lParam) < temp_window_pos->bottom) {
                SetCursor(LoadCursor(NULL, IDC_ARROW));
            }
        }
            break;
            //窗口绘图
        case WM_PAINT: {
            HDC hdc;
            HBRUSH hBrush;
            PAINTSTRUCT ps;
            hdc = BeginPaint(hwnd, &ps); //为指定句柄hwnd窗口进行绘画准备工作
            LPRECT temp_window_pos = malloc(sizeof(RECT));//获取窗体大小以便自适应
            GetWindowRect(hwnd, temp_window_pos);
            int window_size_x = temp_window_pos->right - temp_window_pos->left;
            int window_size_y = temp_window_pos->bottom - temp_window_pos->top;
            int board_size = window_size_x > window_size_y ? window_size_y : window_size_x;
            int cell_size = (board_size-40) / (BOARD_SIZE + 1);
            for (int i = 0; i < BOARD_SIZE + 1; i++) {
                MoveToEx(hdc, 0, i * cell_size, NULL);
                LineTo(hdc, cell_size * BOARD_SIZE, i * cell_size);
                MoveToEx(hdc, i * cell_size, 0, NULL);
                LineTo(hdc, i * cell_size, cell_size * BOARD_SIZE);
            }
            for (int i = 0; i < BOARD_SIZE; i++) {
                for (int j = 0; j < BOARD_SIZE; j++) {
                    if (!is_empty(game->board_isOccupied, i, j)) {
                        if (is_black(game->board_isOccupied, game->board_color, i, j)) {
                            hBrush = (HBRUSH) GetStockObject(BLACK_BRUSH);
                        } else {
                            hBrush = (HBRUSH) GetStockObject(WHITE_BRUSH);
                        }
                        SelectObject(hdc, hBrush);
                        Ellipse(hdc, i * cell_size, j * cell_size, (i + 1) * cell_size, (j + 1) * cell_size);
                    }
                }
            }
            char buff[100];
            sprintf(buff,"time:%fs.",time_this_step);
            TextOut( hdc, 0,8 * cell_size +2, buff, strlen(buff));
            sprintf(buff,"black chess number:%d.",black_num);
            TextOut( hdc, 0,8 * cell_size +20, buff, strlen(buff));
            sprintf(buff,"white chess number:%d.",white_num);
            TextOut( hdc, 0,8 * cell_size +40, buff, strlen(buff));
            if(game->this_color==this_player){
                TextOut( hdc, 8*cell_size+4,0, "Your Turn.", strlen( "Your Turn."));
            }else{
                TextOut( hdc, 8*cell_size+4,0, "  AI    Turn.", strlen( "  AI    Turn."));
            }
            EndPaint(hwnd, &ps);
        }
            break;
            //退出窗口
        case WM_DESTROY: {
            PostQuitMessage(0);
            return 0;
        }
        default: {
        }
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}