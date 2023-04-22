#include <stdio.h>
#include <stdlib.h>
#define TRUE 1
#define FALSE 0
#define MAX_WIDTH 10
#define MAX_HEIGHT 15
#define INDEX(x,y,board) y*board->width+x
#define RANDOM(min,max) (rand()%(max-min))+min
#define TO_CHAR(n) n+'0'

typedef int t_bool;

typedef struct
{
    t_bool is_mine, is_hidden;
    int nearby_mines;
    char mark;
}
t_game_piece;

typedef struct
{
    t_game_piece* values;
    int width, height;
}
t_game_board;

typedef struct
{
    int x, y;
}
t_position;

void Set_Piece_Is_Mine(t_game_board* board, int x, int y, t_bool value)
{
    board->values[INDEX(x, y, board)].is_mine = value;
}

void Set_Piece_Is_Hidden(t_game_board* board, int x, int y, t_bool value)
{
    board->values[INDEX(x, y, board)].is_hidden = value;
}

void Set_Piece_Mark(t_game_board* board, int x, int y, char value)
{
    board->values[INDEX(x, y, board)].mark = value;
}

void Set_Piece_Nearby_Mines(t_game_board* board, int x, int y, int value)
{
    board->values[INDEX(x, y, board)].nearby_mines = value;
}

t_bool Piece_Is_Mine(const t_game_board* board, int x, int y)
{
    return board->values[INDEX(x, y, board)].is_mine;
}

t_bool Piece_Is_Hidden(const t_game_board* board, int x, int y)
{
    return board->values[INDEX(x, y, board)].is_hidden;
}

int Piece_Nearby_Mines(const t_game_board* board, int x, int y)
{
    return board->values[INDEX(x, y, board)].nearby_mines;
}

t_game_board Game_Board_Ctor(int width, int height)
{
    t_game_board board;

    board.values = (t_game_piece*)malloc(width * height * sizeof(t_game_piece));
    board.height = height;
    board.width = width;

    for (int i = 0; i < height; ++i)
        for (int j = 0; j < width; ++j)
        {
            Set_Piece_Is_Hidden(&board, j, i, TRUE);
            Set_Piece_Is_Mine(&board, j, i, FALSE);
            Set_Piece_Mark(&board, j, i, 'x');
        }

    return board;
}

void Free_Game_board(t_game_board* board)
{
    free(board->values);
}

char Valid_Character()
{
    char input;

    while (TRUE)
    {
        printf("Veuillez entrer un caractère (D, M, ?, or Q): ");
        scanf(" %c", &input);

        if (input == 'D' || input == 'M' || input == '?' || input == 'Q')
            return input;
    }
}

int Valid_Integer(int min, int max, char* msg)
{
    int input;

    while (TRUE)
    {
        printf("Pour %s, veuillez entrer un nombre entre [%d, %d): ", msg, min, max);
        scanf("%d", &input);

        if (input >= min && input < max)
            return input;
    }
}

void Init_Mines(t_game_board* board, int mines)
{
    while(mines != 0)
    {
        int x = RANDOM(0, board->width);
        int y = RANDOM(0, board->height);

        if (Piece_Is_Mine(board, x, y))
        {
            Set_Piece_Nearby_Mines(board, x, y, 99);
            Set_Piece_Is_Mine(board, x, y, TRUE);
            --mines;
        }
    }
}

void Init_Empty_Spaces(t_game_board* board)
{
    for (int i = 0; i < board->height; ++i)
        for (int j = 0; j < board->width; ++j)
        {
            t_game_piece* piece = &board->values[INDEX(j, i, board)];
            if (piece->is_mine) continue;

            piece->nearby_mines = 0;
            for (int y = i - 1; y <= i + 1; ++y)
                for (int x = j - 1; x <= j + 1; ++x)
                    if (y >= 0 && y < board->height && x >= 0 && x < board->width)
                    {
                        t_game_piece* neighbor = &board->values[INDEX(x, y, board)];
                        if (neighbor->is_mine) ++piece->nearby_mines;
                    }
        }
}

void Print_Board(const t_game_board* board)
{
    printf("   ");

    for (int x = 0; x < board->width; ++x)
        printf(" %2d ", x);

    printf("\n");
    printf("  +");

    for (int x = 0; x < board->width; ++x)
        printf("---+");

    printf("\n");

    for (int y = 0; y < board->height; ++y)
    {
        printf("%2d|", y);

        for (int x = 0; x < board->width; ++x)
            printf(" %2c |", board->values[INDEX(x, y, board)].mark);

        printf("\n");
        printf("  +");

        for (int x = 0; x < board->width; ++x)
            printf("---+");

        printf("\n");
    }
}

t_position Valid_Choice(const t_game_board* board)
{
    t_position pos = {0};

    do
    {
        pos.x = Valid_Integer(0, board->width, "la coordonnée x");
        pos.y = Valid_Integer(0, board->height, "la coordonnée y");
    }
    while(!Piece_Is_Hidden(board, pos.x, pos.y));

    return pos;
}

int Spaces_Left(const t_game_board* board)
{
    int count = 0;

    for (int i = 0; i < board->width * board->height; ++i)
        if (board->values[i].mark == '?' || board->values[i].mark == 'x')
            ++count;

    return count;
}

void Reveal_Neighbors(t_game_board* board, t_position pos)
{
    if (pos.x < 0 || pos.x >= board->width || pos.y < 0 || pos.y >= board->height)
        return;

    for (int i = -1; i <= 1; ++i)
        for (int j = -1; j <= 1; ++j)
        {
            t_position neighbor = {pos.x + i, pos.y + j};

            if (neighbor.x < 0 || neighbor.x >= board->width || neighbor.y < 0 || neighbor.y >= board->height)
                continue;

            Set_Piece_Is_Hidden(board, neighbor.x, neighbor.y, FALSE);
            Set_Piece_Mark(board, neighbor.x, neighbor.y, TO_CHAR(Piece_Nearby_Mines(board, neighbor.x, neighbor.y)));
        }
}

int Play_Choice(t_game_board* board, t_position pos)
{
    if (Piece_Is_Mine(board, pos.x, pos.y))
    {
        Set_Piece_Mark(board, pos.x, pos.y, 'M');
        return -1;
    }

    int mines = Piece_Nearby_Mines(board, pos.x, pos.y);
    Set_Piece_Mark(board, pos.x, pos.y, TO_CHAR(mines));

    if (mines == 0) Reveal_Neighbors(board, pos);
    return 0;
}

// J'ai mal géré mon temps et j'ai donc pas réussi à terminer. Désolé :-(
void main(){}
