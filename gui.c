#include <stdio.h>
#include "gui.h"
#include "art.h"

void clearBoard(void)
{
    printf("\033[2J");
    printf("\033[%d;%dH", 0, 0);
}

void drawBoard(char board[][8])
{
    // makes the cursor "wrap around"
    // highlightedPiece[1] = (highlightedPiece[1] + 9) % 9;
    // highlightedPiece[0] = (highlightedPiece[0] + 8) % 8;

    // print grid
    // attron(COLOR_PAIR(BLACKPAIR));
    // for (int i = 0 ; i < 8 ; i++ )
    // {
    //     mvaddstr(top + 0 + 3 * i, left, "+-----+-----+-----+-----+-----+-----+-----+-----+");
    //     mvaddstr(top + 1 + 3 * i, left, "|     |     |     |     |     |     |     |     |");
    //     mvaddstr(top + 2 + 3 * i, left, "|     |     |     |     |     |     |     |     |");
    // }
    // mvaddstr(top + 8 * 3, left, "+-----+-----+-----+-----+-----+-----+-----+-----+" );
    // attroff(COLOR_PAIR(BLACKPAIR));
    // printf("+-----+-----+-----+-----+-----+-----+-----+-----+\n");
    // printf("|     |     |     |     |     |     |     |     |\n");
    // printf("|     |     |     |     |     |     |     |     |\n");

    // print pieces
    for (int i = 0 ; i < 8 ; i++ )
    {
        for (int j = 0 ; j < 8 ; j++ )
        {
            if (board[j][i] != ' ' && board[j][i] != 'C' && board[j][i] != 'c')
            {
                // use makeshift ascii art dictionary to print the top of the piece
                int pieceIndex = 0;
                for (int k = 0; k < 7; k ++)
                {
                    if (pieces_index[k] == board[j][i] || pieces_index[k] == board[j][i] + 32)
                    {
                        pieceIndex = k;
                    }
                }
                printf("%d\n", pieceIndex);
                // mvaddstr(top + 22 - 3 * i, left + 2 + 6 * j, pieces_ascii[pieceIndex]);

                // print the bottom of the piece
                // all bottoms are the same
                // mvaddstr(top + 23 - 3 * i, left + 2 + 6 * j, "(_)");

                // color to distinguish between black and white pieces
                // if (getColor(board[j][i]) == 'w')
                // {
                //     attron(COLOR_PAIR(WHITEPAIR));
                //     mvaddstr(top + 23 - 3 * i, left + 3 + 6 * j, "_");
                //     attroff(COLOR_PAIR(WHITEPAIR));
                // }
                // else
                // {
                //     attron(COLOR_PAIR(BLACKPAIR));
                //     mvaddstr(top + 23 - 3 * i, left + 3 + 6 * j, "_");
                //     attroff(COLOR_PAIR(BLACKPAIR));
                // }
                //
                // // if the piece is highlighted or select it, color it accordingly
                // if (highlightedPiece[0] == j && highlightedPiece[1] == i)
                // {
                //     attron(COLOR_PAIR(BLUEPAIR));
                //     mvaddstr(top + 22 - 3 * i, left + 2 + 6 * j, pieces_ascii[pieceIndex]);
                //     mvaddstr(top + 23 - 3 * i, left + 2 + 6 * j, "(_)");
                //     attroff(COLOR_PAIR(BLUEPAIR));
                // }
                // if (selectedPiece[0] == j && selectedPiece[1] == i)
                // {
                //     attron(COLOR_PAIR(WHITEPAIR));
                //     mvaddstr(top + 22 - 3 * i, left + 2 + 6 * j, pieces_ascii[pieceIndex]);
                //     mvaddstr(top + 23 - 3 * i, left + 2 + 6 * j, "(_)");
                //     attroff(COLOR_PAIR(BLUEPAIR));
                // }
            }
            // if an empty square is highlighted
            // else if (highlightedPiece[0] == j && highlightedPiece[1] == i)
            // {
            //     attron(COLOR_PAIR(BLUEPAIR));
            //     mvaddstr(top + 22 - 3 * i, left + 2 + 6 * j, "   ");
            //     mvaddstr(top + 23 - 3 * i, left + 2 + 6 * j, "   ");
            //     attroff(COLOR_PAIR(BLUEPAIR));
            // }
        }
    }

    // gives a few evaluations above the board
    // char message[128] = "hello";
    // sprintf(message, "Position Eval: %i \t\t Computer Eval: %i \t\t\t", evaluate(&realPosition), computer_eval);
    // mvaddstr(top - 1, left, message);

    // displays difficulty level selection

    // if (highlightedPiece[1] == 8)
    // {
    //     if (highlightedPiece[0] % 3 == 0)
    //     {
    //         attron(COLOR_PAIR(BLUEPAIR));
    //         mvaddstr(top + 25, left + 12, "Easy");
    //         attroff(COLOR_PAIR(BLUEPAIR));
    //         highlightedDifficulty = 3;
    //     }
    //     else
    //     {
    //         attron(COLOR_PAIR(BLACKPAIR));
    //         mvaddstr(top + 25, left + 12, "Easy");
    //         attroff(COLOR_PAIR(BLACKPAIR));
    //     }
    //     mvaddstr(top + 25, left + 16, "    ");
    //     if (highlightedPiece[0] % 3 == 1)
    //     {
    //         attron(COLOR_PAIR(BLUEPAIR));
    //         mvaddstr(top + 25, left + 20, "Medium");
    //         attroff(COLOR_PAIR(BLUEPAIR));
    //         highlightedDifficulty = 4;
    //     }
    //     else
    //     {
    //         attron(COLOR_PAIR(BLACKPAIR));
    //         mvaddstr(top + 25, left + 20, "Medium");
    //         attroff(COLOR_PAIR(BLACKPAIR));
    //     }
    //     mvaddstr(top + 25, left + 26, "    ");
    //     if (highlightedPiece[0] % 3 == 2)
    //     {
    //         attron(COLOR_PAIR(BLUEPAIR));
    //         mvaddstr(top + 25, left + 30, "Hard");
    //         attroff(COLOR_PAIR(BLUEPAIR));
    //         highlightedDifficulty = 5;
    //     }
    //     else
    //     {
    //         attron(COLOR_PAIR(BLACKPAIR));
    //         mvaddstr(top + 25, left + 30, "Hard");
    //         attroff(COLOR_PAIR(BLACKPAIR));
    //     }
    // }
    // else
    // {
    //     attron(COLOR_PAIR(BLACKPAIR));
    //     mvaddstr(top + 25, left + 12, "Easy    Medium    Hard");
    //     attroff(COLOR_PAIR(BLACKPAIR));
    // }

    // move actual cursor away
    // move(0, 0);

}
