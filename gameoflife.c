#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <time.h>

#define SLEEP(ms) Sleep(ms)

typedef struct {
    int current; // 0 = dead, 1..n = population
    int next;
    int land;    // 0 = grass, 1 = water, 2 = stone
} cell;

#define ROWS 25
#define COLUMNS 60
#define MAX_POPULATIONS 5

int POPULATIONS = 5;
int SIM_SPEED = 500; // default medium speed

// Colors for populations (distinct from grass)
int POP_COLORS[MAX_POPULATIONS] = {4, 12, 6, 5, 3}; 
int LAND_COLORS[3] = {2 << 4, 1 << 4, 8 << 4};     // grass, water, stone

// Symbols
const char* POP_SYMBOLS[MAX_POPULATIONS] = {"☺", "★", "✓", "♡", "♲"};
const char* LAND_SYMBOLS[3] = {".", "≈", "▲"}; // grass, water, stone

// Function prototypes
void print_board(cell board[ROWS][COLUMNS]);
int count_neighbors(cell board[ROWS][COLUMNS], int r, int c, int pop);
void update_board(cell board[ROWS][COLUMNS]);
void start_menu();
void generate_land(cell board[ROWS][COLUMNS]);

int main() {
    SetConsoleOutputCP(CP_UTF8); // Enable UTF-8
    cell board[ROWS][COLUMNS] = {0};

    start_menu();
    srand((unsigned int)time(NULL));

    generate_land(board);

    // Random population initialization (can live only on grass)
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLUMNS; c++) {
            if (board[r][c].land == 0) {
                int rand_pop = rand() % (POPULATIONS + 1);
                board[r][c].current = rand_pop;
            } else {
                board[r][c].current = 0;
            }
        }
    }

    int gen = 0;
    while (1) {
        system("cls||clear");
        printf("Generation %d:\n", gen++);
        print_board(board);
        update_board(board);
        SLEEP(SIM_SPEED);
    }

    return 0;
}

// ---------------- Functions ----------------

void print_board(cell board[ROWS][COLUMNS]) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    printf("+");
    for (int c = 0; c < COLUMNS; c++) printf("-");
    printf("+\n");

    for (int r = 0; r < ROWS; r++) {
        printf("|");
        for (int c = 0; c < COLUMNS; c++) {
            int pop = board[r][c].current;
            int land = board[r][c].land;

            int bg_color = LAND_COLORS[land];
            int fg_color = 7;

            if (pop >= 1 && pop <= POPULATIONS) {
                fg_color = POP_COLORS[pop-1];
                SetConsoleTextAttribute(hConsole, fg_color + bg_color);
                printf("%s", POP_SYMBOLS[pop-1]);
            } else {
                SetConsoleTextAttribute(hConsole, fg_color + bg_color);
                printf("%s", LAND_SYMBOLS[land]);
            }
        }
        SetConsoleTextAttribute(hConsole, 7);
        printf("|\n");
    }

    printf("+");
    for (int c = 0; c < COLUMNS; c++) printf("-");
    printf("+\n\n");
}

int count_neighbors(cell board[ROWS][COLUMNS], int r, int c, int pop) {
    int count = 0;
    for (int dr = -1; dr <= 1; dr++) {
        for (int dc = -1; dc <= 1; dc++) {
            if (dr == 0 && dc == 0) continue;
            int nr = r + dr;
            int nc = c + dc;
            if (nr >= 0 && nr < ROWS && nc >= 0 && nc < COLUMNS) {
                if (board[nr][nc].current == pop && board[nr][nc].land == 0)
                    count++;
            }
        }
    }
    return count;
}

void update_board(cell board[ROWS][COLUMNS]) {
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLUMNS; c++) {
            int current_pop = board[r][c].current;
            int new_state = 0;

            if (board[r][c].land != 0) { // water or stone
                board[r][c].next = 0;
                continue;
            }

            for (int p = 1; p <= POPULATIONS; p++) {
                int neighbors = count_neighbors(board, r, c, p);
                if (current_pop == p) {
                    if (neighbors == 2 || neighbors == 3)
                        new_state = p;
                } else if (current_pop == 0 && neighbors == 3) {
                    new_state = p;
                }
            }

            board[r][c].next = new_state;
        }
    }

    for (int r = 0; r < ROWS; r++)
        for (int c = 0; c < COLUMNS; c++)
            board[r][c].current = board[r][c].next;
}

// ---------------- Menu ----------------

void start_menu() {
    int choice = 1;
    int speed_choice = 2; // default medium
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    printf("=== Game of Life ===\n\n");

    // Population selection
    printf("Select number of populations:\n");
    for (int i = 1; i <= MAX_POPULATIONS; i++) {
        SetConsoleTextAttribute(hConsole, POP_COLORS[i-1] | 0x70); // fg color + white bg
        printf("%d) %d population%s\n", i, i, i == 1 ? "" : "s");
        SetConsoleTextAttribute(hConsole, 7);
    }
    printf("\nEnter choice (1-%d): ", MAX_POPULATIONS);
    scanf("%d", &choice);
    if (choice < 1) choice = 1;
    if (choice > MAX_POPULATIONS) choice = MAX_POPULATIONS;
    POPULATIONS = choice;

    // Speed selection
    printf("\nSelect simulation speed:\n");
    printf("1) Slow\n2) Medium\n3) Fast\n");
    printf("Enter choice (1-3): ");
    scanf("%d", &speed_choice);
    if (speed_choice < 1) speed_choice = 1;
    if (speed_choice > 3) speed_choice = 3;

    switch(speed_choice) {
        case 1: SIM_SPEED = 1000; break;
        case 2: SIM_SPEED = 500; break;
        case 3: SIM_SPEED = 200; break;
    }

    getchar(); // consume newline
    printf("\nPress Enter to start...");
    getchar();
}

// ---------------- Island Map Generator ----------------

void generate_land(cell board[ROWS][COLUMNS]) {
    // Fill map with water
    for (int r = 0; r < ROWS; r++)
        for (int c = 0; c < COLUMNS; c++)
            board[r][c].land = 1;

    // Ellipse parameters
    double center_r = ROWS / 2.0;
    double center_c = COLUMNS / 2.0;
    double radius_r = ROWS * 0.35;
    double radius_c = COLUMNS * 0.4;

    // Fill island using ellipse
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLUMNS; c++) {
            double norm_r = (r - center_r) / radius_r;
            double norm_c = (c - center_c) / radius_c;
            if (norm_r*norm_r + norm_c*norm_c <= 1.0) {
                board[r][c].land = 0; // grass
            }
        }
    }

    // Stone clusters inside island
    int stone_clusters = ROWS * COLUMNS / 50; // adjust density
    for (int i = 0; i < stone_clusters; i++) {
        int r = rand() % ROWS;
        int c = rand() % COLUMNS;
        if (board[r][c].land == 0) { // only inside island
            int cluster_size = 1 + rand() % 2;
            for (int dr = -cluster_size; dr <= cluster_size; dr++) {
                for (int dc = -cluster_size; dc <= cluster_size; dc++) {
                    int nr = r + dr;
                    int nc = c + dc;
                    if (nr >= 0 && nr < ROWS && nc >= 0 && nc < COLUMNS) {
                        if (board[nr][nc].land == 0 && rand() % 2) {
                            board[nr][nc].land = 2; // stone
                        }
                    }
                }
            }
        }
    }

    // Stones near shoreline
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLUMNS; c++) {
            if (board[r][c].land == 0) { // grass
                for (int dr = -1; dr <= 1; dr++) {
                    for (int dc = -1; dc <= 1; dc++) {
                        int nr = r + dr;
                        int nc = c + dc;
                        if (nr >= 0 && nr < ROWS && nc >= 0 && nc < COLUMNS) {
                            if (board[nr][nc].land == 1 && rand() % 2 == 0) {
                                board[r][c].land = 2; // stone near water
                            }
                        }
                    }
                }
            }
        }
    }
}
