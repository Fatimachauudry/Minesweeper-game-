// Console Minesweeper in C++
// Compile: g++ -std=c++17 -o minesweeper minesweeper.cpp
// Commands: r <row> <col>  = reveal a cell
//           f <row> <col>  = flag / unflag a cell
//           q              = quit

#include <iostream>
#include <vector>
#include <random>
#include <queue>
#include <string>
using namespace std;

struct Cell {
    bool mine = false;
    bool revealed = false;
    bool flagged = false;
    int adjacent = 0;
};

class Minesweeper {
public:
    Minesweeper(int rows, int cols, int mines)
        : rows(rows), cols(cols), mineCount(mines),
          grid(rows, vector<Cell>(cols)) {}

    void play() {
        cout << "=== MINESWEEPER ===\n";
        cout << "Commands: r row col (reveal), f row col (flag), q (quit)\n";

        while (true) {
            print();
            cout << "> ";
            string cmd;
            if (!(cin >> cmd)) return;

            if (cmd == "q") {
                cout << "Bye!\n";
                return;
            }

            int r, c;
            if ((cmd != "r" && cmd != "f") || !(cin >> r >> c)) {
                cin.clear();
                cin.ignore(10000, '\n');
                cout << "Invalid command.\n";
                continue;
            }
            r--; c--; // user enters 1-based coordinates
            if (!inBounds(r, c)) {
                cout << "Out of range.\n";
                continue;
            }

            if (cmd == "f") {
                if (!grid[r][c].revealed)
                    grid[r][c].flagged = !grid[r][c].flagged;
                continue;
            }

            // reveal
            if (grid[r][c].flagged) {
                cout << "Unflag that cell first.\n";
                continue;
            }
            if (!minesPlaced) placeMines(r, c); // first click is always safe

            if (grid[r][c].mine) {
                revealAllMines();
                print();
                cout << "BOOM! You hit a mine. Game over.\n";
                return;
            }

            reveal(r, c);
            if (hasWon()) {
                revealAllMines();
                print();
                cout << "Congratulations, you won!\n";
                return;
            }
        }
    }

private:
    int rows, cols, mineCount;
    bool minesPlaced = false;
    vector<vector<Cell>> grid;

    bool inBounds(int r, int c) const {
        return r >= 0 && r < rows && c >= 0 && c < cols;
    }

    void placeMines(int safeR, int safeC) {
        mt19937 rng(random_device{}());
        uniform_int_distribution<int> distR(0, rows - 1), distC(0, cols - 1);

        int placed = 0;
        while (placed < mineCount) {
            int r = distR(rng), c = distC(rng);
            // keep the first clicked cell and its neighbours mine-free
            if (abs(r - safeR) <= 1 && abs(c - safeC) <= 1) continue;
            if (grid[r][c].mine) continue;
            grid[r][c].mine = true;
            placed++;
        }

        for (int r = 0; r < rows; r++)
            for (int c = 0; c < cols; c++) {
                int count = 0;
                for (int dr = -1; dr <= 1; dr++)
                    for (int dc = -1; dc <= 1; dc++) {
                        int nr = r + dr, nc = c + dc;
                        if (inBounds(nr, nc) && grid[nr][nc].mine) count++;
                    }
                grid[r][c].adjacent = count;
            }
        minesPlaced = true;
    }

    // Flood-fill reveal: opens empty regions automatically
    void reveal(int sr, int sc) {
        queue<pair<int, int>> q;
        q.push({sr, sc});
        while (!q.empty()) {
            auto [r, c] = q.front();
            q.pop();
            Cell &cell = grid[r][c];
            if (cell.revealed || cell.flagged) continue;
            cell.revealed = true;
            if (cell.adjacent == 0) {
                for (int dr = -1; dr <= 1; dr++)
                    for (int dc = -1; dc <= 1; dc++) {
                        int nr = r + dr, nc = c + dc;
                        if (inBounds(nr, nc) && !grid[nr][nc].revealed)
                            q.push({nr, nc});
                    }
            }
        }
    }

    bool hasWon() const {
        for (auto &row : grid)
            for (auto &cell : row)
                if (!cell.mine && !cell.revealed) return false;
        return true;
    }

    void revealAllMines() {
        for (auto &row : grid)
            for (auto &cell : row)
                if (cell.mine) cell.revealed = true;
    }

    int flagsUsed() const {
        int n = 0;
        for (auto &row : grid)
            for (auto &cell : row)
                if (cell.flagged) n++;
        return n;
    }

    void print() const {
        cout << "\nMines left: " << mineCount - flagsUsed() << "\n\n    ";
        for (int c = 1; c <= cols; c++) cout << c << (c < 10 ? "  " : " ");
        cout << "\n";
        for (int r = 0; r < rows; r++) {
            cout << (r + 1 < 10 ? "  " : " ") << r + 1 << " ";
            for (int c = 0; c < cols; c++) {
                const Cell &cell = grid[r][c];
                char ch;
                if (cell.flagged) ch = 'F';
                else if (!cell.revealed) ch = '#';
                else if (cell.mine) ch = '*';
                else if (cell.adjacent == 0) ch = '.';
                else ch = '0' + cell.adjacent;
                cout << ch << "  ";
            }
            cout << "\n";
        }
        cout << "\n";
    }
};

int main() {
    int choice = 0;
    cout << "Choose difficulty:\n 1) Easy   (9x9,  10 mines)\n"
            " 2) Medium (16x16, 40 mines)\n 3) Hard   (16x30, 99 mines)\n> ";
    cin >> choice;

    if (choice == 2) Minesweeper(16, 16, 40).play();
    else if (choice == 3) Minesweeper(16, 30, 99).play();
    else Minesweeper(9, 9, 10).play();

    return 0;
}
