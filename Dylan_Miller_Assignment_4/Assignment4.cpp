// ---------------------------------------------------------------------------
// Name: EECS 348 Assignment 4
// Sudoku Solver
//
// Purpose: Takes in puzzles from the user and some initial puzzles 1-5 and produces the solved sudoku board in terminal
//
// Input:   puzzle(1-5).txt, and any puzzles passed in via user
//
// Output:  Original boards and all valid solved Sudoku boards displayed to the console
//
// Collaboraters: None
// Other Sources: Gemini's code (used it as a reference for making Sudoku solver better)
//
// Author: Dylan Miller
//
// Created: 6/10/2026
// Revised: 7/10/2026
//
// Revisions:   created file
//              added comprehensive inline comments to every line for code maintainability
//              edited the output to include the number of solutions at the end of each solved puzzle
//              edited the way the sudoku board solutions are saved and allocated by using arrays and Gridarrays
// ---------------------------------------------------------------------------
// Preprocessor directive to include standard input/output stream objects like std::cout and std::cerr
#include <iostream> // Included for standard console streaming operations
// Preprocessor directive to include file stream objects like std::ifstream for reading puzzle files
#include <fstream> // Included for file input stream operations
// Preprocessor directive to include the dynamic contiguous array container std::vector
#include <vector> // Included for std::vector dynamic array storage
// Preprocessor directive to include fixed-size contiguous array container std::array
#include <array> // Included for std::array contiguous stack storage
// Preprocessor directive to include the standard string class for file paths and token reading
#include <string> // Included for std::string handling
// ============================================================================
// Class: SudokuBoard
// ============================================================================
// Class definition encapsulating a 9x9 Sudoku board state and board operations
class SudokuBoard { // Begin definition of SudokuBoard class
public: // Public access specifier exposing interface constants, constructors, and methods
    // Compile-time constant defining the total width and height dimension of the board
    static constexpr int SIZE = 9; // Grid dimension (9x9)
    // Compile-time constant defining the width and height dimension of each 3x3 subgrid block
    static constexpr int SUBGRID_SIZE = 3; // Subgrid block dimension (3x3)
    // Compile-time constant sentinel value indicating an unassigned or blank cell
    static constexpr int EMPTY_CELL = 0; // Value representing an empty cell
    // Type alias for fixed 9x9 contiguous array, eliminating dynamic heap allocations
    using GridArray = std::array<std::array<int, SIZE>, SIZE>; // Contiguous 9x9 array type
    // Default constructor initializing an empty 9x9 grid with zero in every cell
    SudokuBoard() // Begin default constructor declaration
        : grid_{} { // Member initializer list value-initializing 9x9 contiguous array to zero
    } // End of default constructor body
    // Parameterized constructor initializing the board from an existing 2D integer vector
    explicit SudokuBoard(const std::vector<std::vector<int>>& grid) // Explicit constructor taking 2D vector by const reference
        : grid_{} { // Member initializer list initializing array to zero
        // Loop over each row up to SIZE or vector row count
        for (int r = 0; r < SIZE && r < static_cast<int>(grid.size()); ++r) { // Iterate rows
            // Loop over each column up to SIZE or vector column count
            for (int c = 0; c < SIZE && c < static_cast<int>(grid[r].size()); ++c) { // Iterate columns
                // Copy cell value from 2D vector to fixed contiguous array
                grid_[r][c] = grid[r][c]; // Copy cell value
            } // End of column loop
        } // End of row loop
    } // End of explicit constructor body
    // Parameterized constructor initializing the board directly from a contiguous 9x9 array
    explicit SudokuBoard(const GridArray& grid) // Explicit constructor taking contiguous array
        : grid_(grid) { // Member initializer list directly copying contiguous array
    } // End of explicit array constructor body
    // Member function to load a 9x9 puzzle configuration from a specified text file
    bool loadFromFile(const std::string& filename) { // Begin loadFromFile method definition
        // Create an input file stream object and attempt to open the specified file
        std::ifstream inFile(filename); // Open target file stream
        // Verify whether the file stream successfully opened
        if (!inFile.is_open()) { // Check for failed file opening
            // Output an error message to the standard error stream indicating file open failure
            std::cerr << "Error: Could not open file '" << filename << "'\n"; // Print file open error
            // Return false to notify caller that file loading failed
            return false; // Exit function returning failure
        } // End of file open check if-statement
        // Declare a string variable to store each whitespace-delimited token read from file
        std::string token; // Temporary buffer for parsed token
        // Initialize an integer counter to keep track of the number of valid tokens read
        int count = 0; // Counter for loaded grid cells
        // Loop while fewer than 81 cells have been read and extraction of next token succeeds
        while (count < SIZE * SIZE && inFile >> token) { // Read up to 81 tokens
            // Compute the row index for the current token using integer division
            int row = count / SIZE; // Calculate row index (0 to 8)
            // Compute the column index for the current token using modulo division
            int col = count % SIZE; // Calculate column index (0 to 8)
            // Check if the parsed token represents an empty cell underscore
            if (token == "_") { // Check for empty cell marker
                // Store the empty cell sentinel value (0) in the grid
                grid_[row][col] = EMPTY_CELL; // Assign empty cell value
            } // End of empty cell token check
            // Check if token is a valid single-character digit between 1 and 9
            else if (token.length() == 1 && token[0] >= '1' && token[0] <= '9') { // Validate single digit 1-9
                // Convert the ASCII character to its integer value and store in the grid
                grid_[row][col] = token[0] - '0'; // Store numeric clue digit
            } // End of valid digit check
            // Handle any invalid token that is neither an underscore nor a digit 1-9
            else { // Branch executed for malformed tokens
                // Print error message reporting the invalid token
                std::cerr << "Error: Invalid token '" << token // Output invalid token string
                          << "' encountered in file '" << filename // Output filename
                          << "' at cell (" << row << ", " << col << ")\n"; // Output row and column coordinates
                // Close the open input file stream before returning
                inFile.close(); // Close file stream
                // Return false to indicate unsuccessful file loading
                return false; // Exit function returning failure
            } // End of invalid token else-statement
            // Increment the counter of successfully parsed cells
            ++count; // Advance token counter
        } // End of token parsing while-loop
        // Close the input file stream upon reading all tokens
        inFile.close(); // Close file stream
        // Verify whether exactly 81 cells were loaded from the file
        if (count != SIZE * SIZE) { // Check for incomplete cell count
            // Output error message to standard error indicating incomplete file data
            std::cerr << "Error: File '" << filename // Print file name
                      << "' contains incomplete data (expected 81 cells, read " // Print error context
                      << count << ").\n"; // Print count of read cells
            // Return false to notify caller that the puzzle file was incomplete
            return false; // Exit function returning failure
        } // End of incomplete cell count check
        // Return true to indicate successful loading and parsing of the puzzle
        return true; // Exit function returning success
    } // End of loadFromFile member function
    // Member function to print the 9x9 board to a given output stream (defaults to std::cout)
    void print(std::ostream& os = std::cout) const { // Begin print method definition
        // Outer loop iterating over each row index from 0 to 8
        for (int r = 0; r < SIZE; ++r) { // Loop over all board rows
            // Inner loop iterating over each column index from 0 to 8
            for (int c = 0; c < SIZE; ++c) { // Loop over all board columns
                // Check if the current cell contains the empty cell sentinel value
                if (grid_[r][c] == EMPTY_CELL) { // Check if cell is blank
                    // Output an underscore followed by a space for blank cells
                    os << "_ "; // Stream underscore representation
                } // End of empty cell display check
                // Branch executed if the current cell contains a valid digit
                else { // Branch executed for filled cells
                    // Output the numeric value of the cell followed by a space
                    os << grid_[r][c] << " "; // Stream cell numeric value
                } // End of filled cell display check
            } // End of column loop
            // Output a newline character at the end of each completed row
            os << "\n"; // Stream newline for row break
        } // End of row loop
    } // End of print member function
    // Accessor member function to retrieve the integer value at a given row and column
    int get(int row, int col) const { // Begin get accessor definition
        // Return the integer stored at the specified grid position
        return grid_[row][col]; // Return cell value
    } // End of get member function
    // Mutator member function to update the integer value at a given row and column
    void set(int row, int col, int val) { // Begin set mutator definition
        // Assign the new integer value to the specified grid position
        grid_[row][col] = val; // Store new cell value
    } // End of set member function
    // Helper function to find the first unassigned cell in row-major scanning order
    bool findEmptyCell(int& row, int& col) const { // Begin findEmptyCell definition
        // Iterate through all rows starting from row 0
        for (int r = 0; r < SIZE; ++r) { // Scan rows
            // Iterate through all columns starting from column 0
            for (int c = 0; c < SIZE; ++c) { // Scan columns
                // Check if the current cell contains the empty sentinel value
                if (grid_[r][c] == EMPTY_CELL) { // Check for empty cell
                    // Assign current row to the output reference parameter
                    row = r; // Record empty cell row
                    // Assign current column to the output reference parameter
                    col = c; // Record empty cell column
                    // Return true indicating an unassigned cell was located
                    return true; // Exit returning true
                } // End of empty cell detection check
            } // End of column scan loop
        } // End of row scan loop
        // Return false indicating no unassigned cells remain on the board
        return false; // Exit returning false (board is full)
    } // End of findEmptyCell member function
    // Validation function checking whether placing candidate num at (row, col) violates rules
    bool isValidPlacement(int row, int col, int num) const { // Begin isValidPlacement definition
        // Loop through all 9 indices to verify row and column uniqueness
        for (int i = 0; i < SIZE; ++i) { // Check row and column constraints
            // Check if candidate number already exists in the target row
            if (grid_[row][i] == num) { // Check for duplicate in row
                // Return false if a row conflict is found
                return false; // Exit returning placement invalid
            } // End of row conflict check
            // Check if candidate number already exists in the target column
            if (grid_[i][col] == num) { // Check for duplicate in column
                // Return false if a column conflict is found
                return false; // Exit returning placement invalid
            } // End of column conflict check
        } // End of row and column constraint loop
        // Compute the starting row index of the 3x3 subgrid block containing (row, col)
        int startRow = (row / SUBGRID_SIZE) * SUBGRID_SIZE; // Calculate subgrid start row
        // Compute the starting column index of the 3x3 subgrid block containing (row, col)
        int startCol = (col / SUBGRID_SIZE) * SUBGRID_SIZE; // Calculate subgrid start column
        // Loop over the 3 rows of the subgrid block
        for (int r = 0; r < SUBGRID_SIZE; ++r) { // Iterate subgrid rows
            // Loop over the 3 columns of the subgrid block
            for (int c = 0; c < SUBGRID_SIZE; ++c) { // Iterate subgrid columns
                // Check if candidate number already exists inside the 3x3 subgrid block
                if (grid_[startRow + r][startCol + c] == num) { // Check for duplicate in subgrid
                    // Return false if a subgrid conflict is found
                    return false; // Exit returning placement invalid
                } // End of subgrid conflict check
            } // End of subgrid column loop
        } // End of subgrid row loop
        // Return true if candidate satisfies row, column, and subgrid constraints
        return true; // Exit returning placement valid
    } // End of isValidPlacement member function
    // Integrity function checking whether the initial clues given in the file are self-consistent
    bool isInitialBoardValid() const { // Begin isInitialBoardValid definition
        // Iterate through all rows of the board
        for (int r = 0; r < SIZE; ++r) { // Loop over rows
            // Iterate through all columns of the board
            for (int c = 0; c < SIZE; ++c) { // Loop over columns
                // Retrieve the cell value at current coordinates
                int val = grid_[r][c]; // Get cell value
                // Only validate non-empty clue cells
                if (val != EMPTY_CELL) { // Check if cell contains a clue
                    // Loop across all columns in row r to ensure clue uniqueness
                    for (int i = 0; i < SIZE; ++i) { // Scan row for duplicates
                        // Check if another cell in the same row shares the same value
                        if (i != c && grid_[r][i] == val) { // Check conflict with another cell
                            // Return false indicating initial board contains duplicate clue in row
                            return false; // Exit returning initial board invalid
                        } // End of row conflict check
                    } // End of row scan loop
                    // Loop across all rows in column c to ensure clue uniqueness
                    for (int i = 0; i < SIZE; ++i) { // Scan column for duplicates
                        // Check if another cell in the same column shares the same value
                        if (i != r && grid_[i][c] == val) { // Check conflict with another cell in column
                            // Return false indicating initial board contains duplicate clue in col
                            return false; // Exit returning initial board invalid
                        } // End of column conflict check
                    } // End of column scan loop
                    // Calculate starting row for the cell's 3x3 subgrid block
                    int startRow = (r / SUBGRID_SIZE) * SUBGRID_SIZE; // Compute subgrid start row
                    // Calculate starting column for the cell's 3x3 subgrid block
                    int startCol = (c / SUBGRID_SIZE) * SUBGRID_SIZE; // Compute subgrid start col
                    // Iterate through the 3 rows of the subgrid block
                    for (int i = 0; i < SUBGRID_SIZE; ++i) { // Loop over subgrid rows
                        // Iterate through the 3 columns of the subgrid block
                        for (int j = 0; j < SUBGRID_SIZE; ++j) { // Loop over subgrid columns
                            // Compute absolute row coordinate in the 9x9 board
                            int currR = startRow + i; // Compute row coordinate
                            // Compute absolute column coordinate in the 9x9 board
                            int currC = startCol + j; // Compute col coordinate
                            // Check if a different cell in the same subgrid block shares the same value
                            if ((currR != r || currC != c) && grid_[currR][currC] == val) { // Check subgrid duplicate
                                // Return false indicating initial board contains duplicate clue in subgrid
                                return false; // Exit returning initial board invalid
                            } // End of subgrid duplicate check
                        } // End of subgrid column loop
                    } // End of subgrid row loop
                } // End of non-empty cell check
            } // End of column loop
        } // End of row loop
        // Return true if all initial clues are completely valid and conflict-free
        return true; // Exit returning initial board valid
    } // End of isInitialBoardValid member function
    // Accessor returning a const reference to the underlying contiguous 9x9 array
    const GridArray& getGrid() const { // Begin getGrid accessor method
        // Return const reference to the fixed contiguous grid array
        return grid_; // Return internal array
    } // End of getGrid accessor
private: // Private access specifier restricting direct access to internal board state
    // Contiguous 9x9 fixed-size array storing cell integers with zero dynamic heap allocations
    GridArray grid_; // Contiguous fixed 9x9 grid storage
}; // End of SudokuBoard class definition
// ============================================================================
// Class: SudokuSolver
// ============================================================================
// Class encapsulating the recursive Depth-First Search with backtracking algorithm
class SudokuSolver { // Begin definition of SudokuSolver class
public: // Public access specifier for constructor and solving interface methods
    // Parameterized constructor initializing solver with the target initial board
    explicit SudokuSolver(const SudokuBoard& board) // Explicit constructor taking board by const reference
        : initialBoard_(board) { // Initializer list storing a copy of the input board
    } // End of SudokuSolver constructor body
    // Primary method executing exhaustive search to find all solutions
    bool solve() { // Begin solve method definition
        // Clear any previously accumulated solutions in the solutions vector
        solutions_.clear(); // Reset solutions collection
        // Check if the initial board contains contradictory clues before searching
        if (!initialBoard_.isInitialBoardValid()) { // Validate initial puzzle consistency
            // Return false immediately without searching if initial clues are contradictory
            return false; // Exit returning no solution possible
        } // End of initial board check
        // Create a working mutable copy of the initial board to modify during backtracking
        SudokuBoard workingBoard = initialBoard_; // Instantiate working board copy
        // Invoke recursive Depth-First Search backtracking algorithm on the working board
        backtrackDFS(workingBoard); // Start recursive search
        // Return true if at least one solution was discovered, otherwise false
        return !solutions_.empty(); // Return true if solutions found
    } // End of solve member function
    // Accessor method returning a const reference to all discovered solved boards
    const std::vector<SudokuBoard>& getSolutions() const { // Begin getSolutions accessor definition
        // Return const reference to the internal vector of solved boards
        return solutions_; // Return solutions vector
    } // End of getSolutions member function
    // Accessor method returning the integer count of discovered solutions
    int getSolutionCount() const { // Begin getSolutionCount accessor definition
        // Return the number of solutions cast from size_t to int
        return static_cast<int>(solutions_.size()); // Return count of solutions
    } // End of getSolutionCount member function
private: // Private access specifier restricting access to solver data and recursion helper
    // Member variable storing the unmodified original starting board
    SudokuBoard initialBoard_; // Original puzzle board
    // Member vector collecting all valid complete solutions found during search
    std::vector<SudokuBoard> solutions_; // Vector of solved boards
    // Core recursive Depth-First Search backtracking algorithm implementation
    void backtrackDFS(SudokuBoard& currentBoard) { // Begin backtrackDFS method definition
        // Declare variable to receive the row index of an unassigned cell
        int row = -1; // Row coordinate of empty cell
        // Declare variable to receive the column index of an unassigned cell
        int col = -1; // Column coordinate of empty cell
        // Base case: check if there are no remaining empty cells on current board
        if (!currentBoard.findEmptyCell(row, col)) { // Check if board is completely filled
            // Store the successfully solved board into the solutions collection
            solutions_.push_back(currentBoard); // Save discovered valid solution
            // Return from recursive call to backtrack and explore remaining branches
            return; // Backtrack to find any alternative solutions
        } // End of base case check
        // Candidate generation loop testing digits 1 through 9 in the empty cell
        for (int candidate = 1; candidate <= 9; ++candidate) { // Iterate through digits 1 to 9
            // Safety check verifying if candidate satisfies all Sudoku rules
            if (currentBoard.isValidPlacement(row, col, candidate)) { // Check if candidate is legal
                // Choice: tentatively place candidate digit into the empty cell
                currentBoard.set(row, col, candidate); // Tentatively place candidate
                // Explore: recursively search next empty cell with candidate assigned
                backtrackDFS(currentBoard); // Recurse to solve remaining cells
                // Backtrack: reset cell back to empty so alternative digits can be evaluated
                currentBoard.set(row, col, SudokuBoard::EMPTY_CELL); // Undo placement to backtrack
            } // End of candidate validity check
        } // End of candidate testing loop
    } // End of backtrackDFS member function
}; // End of SudokuSolver class definition
// ============================================================================
// Main Execution Function
// ============================================================================
// Program entry point accepting optional command-line puzzle filenames
int main(int argc, char* argv[]) { // Begin main function definition
    // Declare a vector of strings to store all puzzle file paths to process
    std::vector<std::string> puzzleFiles; // List of puzzle filenames
    // Check if custom file arguments were provided via the command line
    if (argc > 1) { // Check for command-line arguments
        // Loop over each argument provided after the executable name
        for (int i = 1; i < argc; ++i) { // Iterate over argv elements
            // Add user-provided puzzle filename to the list of files to process
            puzzleFiles.emplace_back(argv[i]); // Append filename to list
        } // End of argument iteration loop
    } // End of command-line argument check
    // Default branch executed when no command-line file arguments are passed
    else { // Execute default file list branch
        // Initialize the vector with the five standard assignment puzzle files
        puzzleFiles = { // Begin initializer list of default files
            "puzzle1.txt", // File path for puzzle 1
            "puzzle2.txt", // File path for puzzle 2
            "puzzle3.txt", // File path for puzzle 3
            "puzzle4.txt", // File path for puzzle 4
            "puzzle5.txt"  // File path for puzzle 5
        }; // End of initializer list
    } // End of default file list else-statement
    // Loop through each puzzle filename in the list sequentially
    for (const std::string& filename : puzzleFiles) { // Process each puzzle file
        // Output decorative separator line to console
        std::cout << "========================================\n"; // Print top banner
        // Output current filename header to console
        std::cout << "File: " << filename << "\n"; // Print filename header
        // Output decorative separator line to console
        std::cout << "========================================\n"; // Print bottom banner
        // Instantiate a new SudokuBoard object for the current puzzle
        SudokuBoard board; // Instantiate board object
        // Attempt to load the puzzle data from the current file
        if (!board.loadFromFile(filename)) { // Check if file load fails
            // Print error notification when puzzle loading fails
            std::cout << "Failed to load puzzle file: " << filename << "\n\n"; // Print failure message
            // Skip the rest of the loop and continue with the next puzzle file
            continue; // Proceed to next file in loop
        } // End of file load error check
        // Output label before printing original unsolved puzzle
        std::cout << "Original Board:\n"; // Print original board label
        // Display the original unsolved puzzle grid to console
        board.print(); // Print unsolved board grid
        // Output a blank line for visual formatting separation
        std::cout << "\n"; // Print newline separator
        // Instantiate SudokuSolver with the loaded board
        SudokuSolver solver(board); // Instantiate solver object
        // Execute the backtracking solver algorithm
        solver.solve(); // Solve the puzzle
        // Retrieve a const reference to the vector of solutions found
        const auto& solutions = solver.getSolutions(); // Retrieve solutions
        // Check if no solutions were found for this puzzle
        if (solutions.empty()) { // Check for no solutions
            // Print notification that no valid solution exists for this puzzle
            std::cout << "No solution found\n\n"; // Print no solution notification
        } // End of no solution check
        // Branch executed when one or more valid solutions were found
        else { // Branch executed when solutions exist
            // Iterate over all discovered solutions by index
            for (size_t i = 0; i < solutions.size(); ++i) { // Loop over solutions
                // Check if the puzzle has multiple distinct solutions
                if (solutions.size() > 1) { // Check for multiple solutions
                    // Print numbered solution header (e.g. Solution 1:, Solution 2:)
                    std::cout << "Solution " << (i + 1) << ":\n"; // Print numbered header
                } // End of multiple solutions check
                // Branch executed when the puzzle has exactly one unique solution
                else { // Branch executed for single solution
                    // Print standard single solution header
                    std::cout << "Solution:\n"; // Print single solution header
                } // End of single solution check
                // Print the solved board grid to the console
                solutions[i].print(); // Print solved board grid
                // Output a blank line after printing the solved board
                std::cout << "\n"; // Print newline separator
            } // End of solutions iteration loop
            // Print the total number of solutions found formatted as in solution1.txt
            std::cout << "Solutions found: " << solutions.size() << "\n\n"; // Print solutions count
        } // End of solutions display else-statement
    } // End of puzzle file processing loop
    // Return status code 0 indicating successful program execution
    return 0; // Exit program successfully
} // End of main function