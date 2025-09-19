#include <stdio.h>
#include <stdlib.h>

#include <windows.h>
#define SLEEP(ms) Sleep(ms) // lisää viiveen

#define ROWS 7
#define COLUMNS 7 // nämä määrittää pelikentän koon.

typedef struct {
    int current;
    int next;
} cell;

void print_board(cell board[ROWS][COLUMNS]) {  // tämä printtaa tauluun joko 0 tai . riippuen onko solu elossa vai ei
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLUMNS; c++) {
            if (board[r][c].current == 1)
                printf("O");
            else
                printf(".");
        }
        printf("\n");
    }
    printf("\n");
}

int count_neighbors(cell board[ROWS][COLUMNS], int r, int c) {  // tarkistaa kaikki 8 ympäröivää solua, varmistaa ettei mee yli rajojen ja kertoo montako naapuria on hengis
    int count = 0;
    for (int dr = -1; dr <= 1; dr++) {
        for (int dc = -1; dc <= 1; dc++) {
            if (dr == 0 && dc == 0) continue;
            int nr = r + dr;
            int nc = c + dc;
            if (nr >= 0 && nr < ROWS && nc >= 0 && nc < COLUMNS) {
                if (board[nr][nc].current == 1)
                    count++;
            }
        }
    }
    return count;
}

void update_board(cell board[ROWS][COLUMNS]) { // tuo game of life pelin säännöt
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLUMNS; c++) {
            int neighbors = count_neighbors(board, r, c);
            if (board[r][c].current == 1) {
                if (neighbors < 2 || neighbors > 3)
                    board[r][c].next = 0;
                else
                    board[r][c].next = 1;
            } else {
                if (neighbors == 3)
                    board[r][c].next = 1;
                else
                    board[r][c].next = 0;
            }
        }
    }

    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLUMNS; c++) {
            board[r][c].current = board[r][c].next;
        }
    }
}

int main(void) { //tekee "pelipöydän". kaikki alottaa kuolleena.
    FILE *fp;
    char state_c;
    int state, c, r;
    cell board[ROWS][COLUMNS] = {0};

    c = 0;
    r = 0;

    fp = fopen("game.txt", "r");  //avaa "game.txt" tiedoston 
    if (!fp) {
        printf("Could not open file.\n");
        return 1;
    }

    while (fscanf(fp, "%c", &state_c) == 1) { // lukee tiedostosta arvot ja täyttää rivi riviltä.
        if (state_c == '\n' || state_c == '\r')
            continue;
        if (state_c == '1' || state_c == 'O')
            state = 1;
        else
            state = 0;
        board[r][c].current = state;
        c++;
        if (c >= COLUMNS) {
            r++;
            c = 0;
        }
    }
    fclose(fp);

    int gen = 0;       
    while (1) {  // infinite loop kunnes pysäytetään.
        system("clear||cls"); // clear terminal
        printf("Generation %d:\n", gen++);
        print_board(board);
        update_board(board);
        SLEEP(1000); // odottaa 1 sekunnin
    }

    return 0;
}
