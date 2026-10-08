/**
 * @file main.cpp
 * @brief Complete Object-Oriented Sudoku Solver using Recursive Depth-First Search with Backtracking.
 *
 * Course: EECS 348 - Assignment 4
 *
 * This program reads 9x9 Sudoku puzzles from five text files (puzzle1.txt - puzzle5.txt),
 * displays each original puzzle board, and solves them exhaustively using recursive DFS
 * backtracking to find and print ALL valid solutions. If a puzzle has no solution, it prints
 * "No solution found".
 */

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

// ============================================================================
// Class: SudokuBoard
// ============================================================================
/**
 * @class SudokuBoard
 * @brief Encapsulates a 9x9 Sudoku grid with methods for file loading,
 *        validation, cell access, and formatted display.
 */
class SudokuBoard {
public:
    static constexpr int SIZE = 9;
    static constexpr int SUBGRID_SIZE = 3;
    static constexpr int EMPTY_CELL = 0;

    /**
     * @brief Default constructor: initializes a 9x9 grid with empty cells (0).
     */
    SudokuBoard() : grid_(SIZE, std::vector<int>(SIZE, EMPTY_CELL)) {}

    /**
     * @brief Constructs a board from an existing 2D grid vector.
     * @param grid 9x9 vector representing the Sudoku board.
     */
    explicit SudokuBoard(const std::vector<std::vector<int>>& grid) : grid_(grid) {}

    /**
     * @brief Loads a 9x9 Sudoku puzzle from a text file.
     *        Digits '1'-'9' represent clues; '_' represents an empty cell.
     * @param filename Path to the puzzle file.
     * @return true if the file was opened and 81 valid tokens were read; false otherwise.
     */
    bool loadFromFile(const std::string& filename) {
        std::ifstream inFile(filename);
        if (!inFile.is_open()) {
            std::cerr << "Error: Could not open file '" << filename << "'\n";
            return false;
        }

        std::string token;
        int count = 0;
        while (count < SIZE * SIZE && inFile >> token) {
            int row = count / SIZE;
            int col = count % SIZE;

            if (token == "_") {
                grid_[row][col] = EMPTY_CELL;
            } else if (token.length() == 1 && token[0] >= '1' && token[0] <= '9') {
                grid_[row][col] = token[0] - '0';
            } else {
                std::cerr << "Error: Invalid token '" << token 
                          << "' encountered in file '" << filename 
                          << "' at cell (" << row << ", " << col << ")\n";
                inFile.close();
                return false;
            }
            ++count;
        }

        inFile.close();

        if (count != SIZE * SIZE) {
            std::cerr << "Error: File '" << filename 
                      << "' contains incomplete data (expected 81 cells, read " 
                      << count << ").\n";
            return false;
        }

        return true;
    }

    /**
     * @brief Prints the 9x9 board to the specified output stream.
     *        Empty cells are printed as '_', and numbers are separated by spaces.
     * @param os Output stream (defaults to std::cout).
     */
    void print(std::ostream& os = std::cout) const {
        for (int r = 0; r < SIZE; ++r) {
            for (int c = 0; c < SIZE; ++c) {
                if (grid_[r][c] == EMPTY_CELL) {
                    os << "_ ";
                } else {
                    os << grid_[r][c] << " ";
                }
            }
            os << "\n";
        }
    }

    /**
     * @brief Retrieves the value at row and column.
     * @param row Row index (0-8).
     * @param col Column index (0-8).
     * @return The cell value (0 for empty, 1-9 for numbers).
     */
    int get(int row, int col) const {
        return grid_[row][col];
    }

    /**
     * @brief Sets the value at row and column.
     * @param row Row index (0-8).
     * @param col Column index (0-8).
     * @param val The value to set (0 for empty, 1-9 for numbers).
     */
    void set(int row, int col, int val) {
        grid_[row][col] = val;
    }

    /**
     * @brief Finds the first empty cell on the board in row-major order.
     * @param row Output reference storing the row of the empty cell.
     * @param col Output reference storing the column of the empty cell.
     * @return true if an empty cell was found; false if the board is completely filled.
     */
    bool findEmptyCell(int& row, int& col) const {
        for (int r = 0; r < SIZE; ++r) {
            for (int c = 0; c < SIZE; ++c) {
                if (grid_[r][c] == EMPTY_CELL) {
                    row = r;
                    col = c;
                    return true;
                }
            }
        }
        return false;
    }

    /**
     * @brief Checks if placing a candidate number at (row, col) violates Sudoku rules.
     *        Validates against the target row, column, and 3x3 subgrid.
     * @param row Row index (0-8).
     * @param col Column index (0-8).
     * @param num Candidate number (1-9).
     * @return true if valid to place; false if a conflict exists.
     */
    bool isValidPlacement(int row, int col, int num) const {
        // Check row and column constraints
        for (int i = 0; i < SIZE; ++i) {
            if (grid_[row][i] == num) {
                return false;
            }
            if (grid_[i][col] == num) {
                return false;
            }
        }

        // Check 3x3 subgrid constraints
        int startRow = (row / SUBGRID_SIZE) * SUBGRID_SIZE;
        int startCol = (col / SUBGRID_SIZE) * SUBGRID_SIZE;
        for (int r = 0; r < SUBGRID_SIZE; ++r) {
            for (int c = 0; c < SUBGRID_SIZE; ++c) {
                if (grid_[startRow + r][startCol + c] == num) {
                    return false;
                }
            }
        }

        return true;
    }

    /**
     * @brief Checks whether the initial pre-filled clues on the board are self-consistent.
     * @return true if no initial clues violate row, col, or subgrid uniqueness; false otherwise.
     */
    bool isInitialBoardValid() const {
        for (int r = 0; r < SIZE; ++r) {
            for (int c = 0; c < SIZE; ++c) {
                int val = grid_[r][c];
                if (val != EMPTY_CELL) {
                    // Check row uniqueness
                    for (int i = 0; i < SIZE; ++i) {
                        if (i != c && grid_[r][i] == val) {
                            return false;
                        }
                    }
                    // Check column uniqueness
                    for (int i = 0; i < SIZE; ++i) {
                        if (i != r && grid_[i][c] == val) {
                            return false;
                        }
                    }
                    // Check 3x3 subgrid uniqueness
                    int startRow = (r / SUBGRID_SIZE) * SUBGRID_SIZE;
                    int startCol = (c / SUBGRID_SIZE) * SUBGRID_SIZE;
                    for (int i = 0; i < SUBGRID_SIZE; ++i) {
                        for (int j = 0; j < SUBGRID_SIZE; ++j) {
                            int currR = startRow + i;
                            int currC = startCol + j;
                            if ((currR != r || currC != c) && grid_[currR][currC] == val) {
                                return false;
                            }
                        }
                    }
                }
            }
        }
        return true;
    }

private:
    std::vector<std::vector<int>> grid_;
};

// ============================================================================
// Class: SudokuSolver
// ============================================================================
/**
 * @class SudokuSolver
 * @brief Encapsulates the recursive Depth-First Search (DFS) with backtracking
 *        to find ALL valid solutions for a given SudokuBoard.
 */
class SudokuSolver {
public:
    /**
     * @brief Constructs a solver for the specified initial board.
     * @param board The initial Sudoku board.
     */
    explicit SudokuSolver(const SudokuBoard& board) : initialBoard_(board) {}

    /**
     * @brief Solves the puzzle exhaustively using recursive DFS with backtracking.
     *        Finds all possible solutions.
     * @return true if at least one solution was found; false otherwise.
     */
    bool solve() {
        solutions_.clear();

        // If the initial puzzle clues are already contradictory, no solution can exist
        if (!initialBoard_.isInitialBoardValid()) {
            return false;
        }

        SudokuBoard workingBoard = initialBoard_;
        backtrackDFS(workingBoard);

        return !solutions_.empty();
    }

    /**
     * @brief Returns the collection of all valid solutions found.
     * @return Const reference to the vector of solved SudokuBoard instances.
     */
    const std::vector<SudokuBoard>& getSolutions() const {
        return solutions_;
    }

    /**
     * @brief Returns the total number of solutions found.
     * @return Integer count of solutions.
     */
    int getSolutionCount() const {
        return static_cast<int>(solutions_.size());
    }

private:
    SudokuBoard initialBoard_;
    std::vector<SudokuBoard> solutions_;

    /**
     * @brief Core recursive Depth-First Search (DFS) algorithm with backtracking.
     *
     * Algorithm Details:
     * 1. Base Case: Locate the first unassigned (empty) cell. If no empty cells
     *    remain, the board has been completely and validly filled. Record the
     *    solution and return to continue searching for other possible solutions.
     * 2. Candidate Generation: For the identified empty cell, iterate through
     *    possible candidate digits (1 through 9).
     * 3. Safety Check: Verify if placing candidate `candidate` is valid (no conflict
     *    in the same row, column, or 3x3 subgrid).
     * 4. Recursive Step: If valid, tentatively place the number on the board
     *    and recursively call backtrackDFS() to explore this branch.
     * 5. Backtracking Step: Reset the cell back to EMPTY_CELL (0) after the
     *    recursive call returns, unmaking the tentative placement so other
     *    candidates or alternate branches can be evaluated.
     *
     * @param currentBoard The board state at the current recursion depth.
     */
    void backtrackDFS(SudokuBoard& currentBoard) {
        int row = -1;
        int col = -1;

        // Base case: If there are no empty cells left, a complete solution is found
        if (!currentBoard.findEmptyCell(row, col)) {
            solutions_.push_back(currentBoard);
            return; // Return to backtrack and search for further solutions
        }

        // Try candidate digits 1 through 9 in the empty cell (row, col)
        for (int candidate = 1; candidate <= 9; ++candidate) {
            // Check if placing candidate does not conflict with row, col, or 3x3 block
            if (currentBoard.isValidPlacement(row, col, candidate)) {
                // Tentatively assign the candidate
                currentBoard.set(row, col, candidate);

                // Recursively proceed with DFS
                backtrackDFS(currentBoard);

                // Backtrack: undo tentative assignment to explore next candidate/branch
                currentBoard.set(row, col, SudokuBoard::EMPTY_CELL);
            }
        }
    }
};

// ============================================================================
// Main Execution Function
// ============================================================================
int main(int argc, char* argv[]) {
    // List of input puzzle files to process sequentially
    std::vector<std::string> puzzleFiles;

    if (argc > 1) {
        // Allow command-line file overrides if supplied
        for (int i = 1; i < argc; ++i) {
            puzzleFiles.emplace_back(argv[i]);
        }
    } else {
        // Default required puzzle files
        puzzleFiles = {
            "puzzle1.txt",
            "puzzle2.txt",
            "puzzle3.txt",
            "puzzle4.txt",
            "puzzle5.txt"
        };
    }

    for (const std::string& filename : puzzleFiles) {
        std::cout << "========================================\n";
        std::cout << "File: " << filename << "\n";
        std::cout << "========================================\n";

        SudokuBoard board;
        // Attempt to load the puzzle from file with graceful error handling
        if (!board.loadFromFile(filename)) {
            std::cout << "Failed to load puzzle file: " << filename << "\n\n";
            continue;
        }

        // Print the original puzzle board
        std::cout << "Original Board:\n";
        board.print();
        std::cout << "\n";

        // Solve the puzzle using recursive DFS with backtracking
        SudokuSolver solver(board);
        solver.solve();

        const auto& solutions = solver.getSolutions();

        // Print solutions or notify that none exist
        if (solutions.empty()) {
            std::cout << "No solution found\n\n";
        } else {
            for (size_t i = 0; i < solutions.size(); ++i) {
                if (solutions.size() > 1) {
                    std::cout << "Solution " << (i + 1) << ":\n";
                } else {
                    std::cout << "Solution:\n";
                }
                solutions[i].print();
                std::cout << "\n";
            }
        }
    }

    return 0;
}