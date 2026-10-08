/*
 * Sudoku Solver — Recursive Depth-First Search with Backtracking
 *
 * Reads puzzle1.txt through puzzle5.txt.
 * Empty cells are represented by '_' in the input files.
 * Finds and prints ALL valid solutions for each puzzle.
 *
 * Design:
 *   SudokuBoard  — owns the 9×9 grid, handles I/O and display.
 *   SudokuSolver — owns a SudokuBoard copy and performs recursive DFS.
 */

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <array>
#include <sstream>

// ─────────────────────────────────────────────────────────────────────────────
// Constants
// ─────────────────────────────────────────────────────────────────────────────
static constexpr int GRID_SIZE  = 9;   // 9×9 board
static constexpr int BOX_SIZE   = 3;   // 3×3 sub-grid
static constexpr int EMPTY_CELL = 0;   // Internal sentinel for blank cells

// ─────────────────────────────────────────────────────────────────────────────
// Class: SudokuBoard
//   Stores the 9×9 grid and knows how to read from a file and print itself.
// ─────────────────────────────────────────────────────────────────────────────
class SudokuBoard {
public:
    // Default-construct to an all-empty board.
    SudokuBoard()
        : grid_(GRID_SIZE, std::vector<int>(GRID_SIZE, EMPTY_CELL)) {}

    // ── File I/O ─────────────────────────────────────────────────────────────

    // Load the board from the given file path.
    // Returns true on success, false if the file cannot be opened or is malformed.
    bool loadFromFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Cannot open file \"" << filename << "\".\n";
            return false;
        }

        for (int row = 0; row < GRID_SIZE; ++row) {
            std::string line;
            // Skip blank lines between rows (e.g., trailing newlines at EOF).
            while (line.empty() && std::getline(file, line)) {}

            if (line.empty()) {
                std::cerr << "Error: File \"" << filename
                          << "\" has fewer than 9 data rows.\n";
                return false;
            }

            std::istringstream ss(line);
            for (int col = 0; col < GRID_SIZE; ++col) {
                std::string token;
                if (!(ss >> token)) {
                    std::cerr << "Error: Row " << row + 1
                              << " in \"" << filename << "\" has fewer than 9 tokens.\n";
                    return false;
                }
                if (token == "_") {
                    grid_[row][col] = EMPTY_CELL;
                } else {
                    int digit = std::stoi(token);
                    grid_[row][col] = digit;
                }
            }
        }
        return true;
    }

    // ── Display ───────────────────────────────────────────────────────────────

    // Print the board to stdout.
    // Empty cells (EMPTY_CELL) are shown as '_'; digits are shown as-is.
    void print() const {
        for (int row = 0; row < GRID_SIZE; ++row) {
            for (int col = 0; col < GRID_SIZE; ++col) {
                if (col > 0) std::cout << ' ';
                if (grid_[row][col] == EMPTY_CELL)
                    std::cout << '_';
                else
                    std::cout << grid_[row][col];
            }
            std::cout << '\n';
        }
    }

    // ── Grid access ──────────────────────────────────────────────────────────

    int  get(int row, int col) const { return grid_[row][col]; }
    void set(int row, int col, int val) { grid_[row][col] = val; }

private:
    std::vector<std::vector<int>> grid_;  // [row][col], values 0–9 (0 = empty)
};

// ─────────────────────────────────────────────────────────────────────────────
// Class: SudokuSolver
//   Takes a board, finds ALL solutions via recursive DFS + backtracking,
//   and prints each one.
// ─────────────────────────────────────────────────────────────────────────────
class SudokuSolver {
public:
    // Initialise the solver with a copy of the starting board.
    explicit SudokuSolver(const SudokuBoard& board)
        : board_(board), solutionCount_(0) {}

    // Entry point: solve the puzzle and print every solution found.
    // Returns the total number of solutions discovered.
    int solve() {
        solutionCount_ = 0;
        dfs();                        // Begin exhaustive recursive search
        return solutionCount_;
    }

private:
    SudokuBoard board_;        // Working grid (mutated during search, restored on backtrack)
    int         solutionCount_; // Cumulative count of complete solutions found

    // ── Validation helpers ────────────────────────────────────────────────────

    // Return true if placing `digit` at (row, col) is legal according to
    // Sudoku rules: no duplicate in the same row, column, or 3×3 box.
    bool isValid(int row, int col, int digit) const {
        // Check the entire row.
        for (int c = 0; c < GRID_SIZE; ++c) {
            if (board_.get(row, c) == digit) return false;
        }

        // Check the entire column.
        for (int r = 0; r < GRID_SIZE; ++r) {
            if (board_.get(r, col) == digit) return false;
        }

        // Check the 3×3 sub-grid that contains (row, col).
        int boxRowStart = (row / BOX_SIZE) * BOX_SIZE;  // top-left row of box
        int boxColStart = (col / BOX_SIZE) * BOX_SIZE;  // top-left col of box
        for (int r = boxRowStart; r < boxRowStart + BOX_SIZE; ++r) {
            for (int c = boxColStart; c < boxColStart + BOX_SIZE; ++c) {
                if (board_.get(r, c) == digit) return false;
            }
        }

        return true;  // digit is a legal candidate here
    }

    // ── Core DFS / backtracking ───────────────────────────────────────────────

    // Recursively fills empty cells in reading order (left-to-right, top-to-bottom).
    // When the board is completely filled, records and prints the solution.
    // Backtracks by resetting a cell to EMPTY_CELL whenever no digit works.
    void dfs() {
        // Scan for the next empty cell.
        for (int row = 0; row < GRID_SIZE; ++row) {
            for (int col = 0; col < GRID_SIZE; ++col) {
                if (board_.get(row, col) == EMPTY_CELL) {
                    // Try every candidate digit 1–9.
                    for (int digit = 1; digit <= GRID_SIZE; ++digit) {
                        if (isValid(row, col, digit)) {
                            board_.set(row, col, digit);   // Place digit (choose)
                            dfs();                         // Recurse deeper (explore)
                            board_.set(row, col, EMPTY_CELL); // Undo placement (backtrack)
                        }
                    }
                    // No digit worked here → dead end; unwind the call stack.
                    return;
                }
            }
        }

        // If we reach here, every cell is filled → a complete solution was found.
        ++solutionCount_;
        std::cout << "Solution " << solutionCount_ << ":\n";
        board_.print();
        std::cout << '\n';
    }
};

// ─────────────────────────────────────────────────────────────────────────────
// main()
//   Iterates over the five puzzle files sequentially, prints each puzzle and
//   its solution(s), or reports that no solution exists.
// ─────────────────────────────────────────────────────────────────────────────
int main() {
    const std::array<std::string, 5> puzzleFiles = {
        "puzzle1.txt", "puzzle2.txt", "puzzle3.txt",
        "puzzle4.txt", "puzzle5.txt"
    };

    for (const std::string& filename : puzzleFiles) {
        std::cout << "========================================\n";
        std::cout << filename << "\n";
        std::cout << "========================================\n";

        // ── Load puzzle ───────────────────────────────────────────────────────
        SudokuBoard board;
        if (!board.loadFromFile(filename)) {
            // Error message already printed inside loadFromFile; move on.
            std::cout << '\n';
            continue;
        }

        // ── Print original (unsolved) puzzle ──────────────────────────────────
        std::cout << "Puzzle:\n";
        board.print();
        std::cout << '\n';

        // ── Solve and print all solutions ─────────────────────────────────────
        SudokuSolver solver(board);
        int count = solver.solve();

        if (count == 0) {
            std::cout << "No solution found\n";
        } else {
            std::cout << "Solutions found: " << count << "\n";
        }
        std::cout << '\n';
    }

    return 0;
}
