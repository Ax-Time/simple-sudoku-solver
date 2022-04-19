#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define SUDOKU_SIZE 81

int* read_sudoku(const char* path);
void write_sudoku(int* sudoku_grid, const char *path);
void solve_sudoku(int* sudoku_grid);
bool _solve_cell(int* sudoku_grid, int row, int col);
bool _row_constraints(int* sudoku_grid, int row);
bool _col_constraints(int* sudoku_grid, int col);
bool _quad_constraints(int* sudoku_grid, int row, int col);

int main(int argc, char **argv) {
    if(argc == 2) {  
        int* sudoku_grid = read_sudoku(argv[1]);
        solve_sudoku(sudoku_grid);
        write_sudoku(sudoku_grid, "solved.txt");
        free(sudoku_grid);
        printf("Check the file \"solved.txt\" to read the solution.\n");
        return 0;
    }
    else {
        printf("Please provide the input file as command line argument, use a syntax like this:\n$<program> <path_to_input_file.txt>");
        return -1;
    }
}

int* read_sudoku(const char* path) {
    FILE* fp = fopen(path, "r");
    if(!fp) {
        printf("Could not open file\n");
        return NULL;
    }
    int* sudoku_grid = malloc(sizeof(int) * SUDOKU_SIZE);
    if(!sudoku_grid) {
        printf("Could not allocate memory\n");
        fclose(fp);
        return NULL;
    }
    int ch;
    int grid_index = 0;
    while((ch = fgetc(fp)) != EOF) {
        if(ch == '.' || (ch >= '1' && ch <= '9')) 
            sudoku_grid[grid_index++] = (ch == '.') ? 0 : (ch - '0');
    }
    fclose(fp);
    return sudoku_grid;
}


void write_sudoku(int* sudoku_grid, const char *path) {
    FILE* fp;
    if(strcmp(path, "stdout") == 0) fp = stdout;
    else fp = fopen(path, "w");
    if(!fp) {
        printf("Could not open file\n");
        return;
    }

    for(int i = 0; i < 9; i++) {
        for(int j = 0; j < 9; j++) {
            fprintf(fp, "%d ", sudoku_grid[i*9+j]);
        }
        fprintf(fp, "\n");
    }   
    if(fp != stdout) fclose(fp);
}


void solve_sudoku(int* sudoku_grid) {
    if(!_solve_cell(sudoku_grid, 0, 0))
        printf("a solution could not be found!\n");
    else printf("solution found\n");
}

bool _solve_cell(int* sudoku_grid, int row, int col) {
    // If row is out of boundaries, sudoku has been solved
    if(row == 9) return true;

    int next_row = (col == 8) ? (row+1) : row;
    int next_col = (col == 8) ? 0 : (col+1);

    // If cell is already solved, go to next one (unless it is the last cell)
    if(sudoku_grid[row*9+col] != 0) {
        return _solve_cell(sudoku_grid, next_row, next_col);
    } else {
        // Cell not yet solved
        for(int try = 1; try <= 9; try++) {
            // try to assign the value
            sudoku_grid[row*9+col] = try;

            // Check constraints
            if(
                _row_constraints(sudoku_grid, row) &&
                _col_constraints(sudoku_grid, col) &&
                _quad_constraints(sudoku_grid, row, col)
            ) {
                // Assignment satisfies constraints, call recursively on next cell
                if(_solve_cell(sudoku_grid, next_row, next_col)) {
                    // Assignment found, return true
                    return true; 
                }
            }

            // This assignment either violates constraints or brings to an impossible solution
            // Therefore undo the assignment and try an other value
            sudoku_grid[row*9+col] = 0;
        }

        // A solution cannot be found, return false
        return false;
    }
}

bool _row_constraints(int* sudoku_grid, int row) {
    bool map[10] = {false};
    for(int col = 0; col < 9; col++) {
        if(sudoku_grid[row*9+col] != 0) {
            if(map[sudoku_grid[row*9+col]]) return false;
            else map[sudoku_grid[row*9+col]] = true;
        }
    }
    return true;
}

bool _col_constraints(int* sudoku_grid, int col) {
    bool map[10] = {false};
    for(int row = 0; row < 9; row++) {
        if(sudoku_grid[row*9+col] != 0) {
            if(map[sudoku_grid[row*9+col]]) return false;
            else map[sudoku_grid[row*9+col]] = true;
        }
    }
    return true;
}

bool _quad_constraints(int* sudoku_grid, int row, int col) {
    bool map[10] = {false};
    int quad_row = (row >= 6) ? 6 : (row >= 3) ? 3 : 0;
    int quad_col = (col >= 6) ? 6 : (col >= 3) ? 3 : 0;
    for(int i = quad_row; i < quad_row + 3; i++) {
        for(int j = quad_col; j < quad_col + 3; j++) {
            if(sudoku_grid[i*9+j] != 0) {
                if(map[sudoku_grid[i*9+j]]) return false;
                else map[sudoku_grid[i*9+j]] = true;
            }
        }
    }
    return true;
}