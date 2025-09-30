#include <iostream>
#include <vector>
#include <stack>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <random>

using namespace std;

void write_board_csv(const vector<vector<int>>& board, bool first) {
    ios_base::openmode mode = ios::app;
    if (first) mode = ios::trunc;
    ofstream fout("game_output.csv", mode);
    if (!fout) return;
    for (int r=0;r<4;r++){
        for (int c=0;c<4;c++){
            fout<<board[r][c];
            if (!(r==3 && c==3)) fout<<",";
        }
    }
    fout<<"\n";
}

void print_board(const vector<vector<int>>& board, bool first) {
    for(auto& row: board){
        for(auto& cell: row){
            cout << cell << " ";
        }
        cout << endl;
    }

    write_board_csv(board, first);
}

void spawn_tile(vector<vector<int>>& board) {
    vector<pair<int, int>> empty_cells;
    for (int r=0;r<4;r++){
        for (int c=0;c<4;c++){
            if (board[r][c]==0) { 
                empty_cells.emplace_back(r, c);
            }
        }
    }

    if (empty_cells.empty()) return;

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> cell_dist(0, empty_cells.size() - 1);
    uniform_int_distribution<> val_dist(1, 10);

    auto [r, c] = empty_cells[cell_dist(gen)];
    board[r][c] = (val_dist(gen) <= 9) ? 2 : 4;
}



std::vector<int> compress_row(const std::vector<int>& row) {
    vector<int> compressed;
    copy_if(row.begin(), row.end(), back_inserter(compressed), [](int val) {return val != 0;});
    while(compressed.size() < 4) compressed.push_back(0);
    
    return compressed;
}

std::vector<int> merge_row(std::vector<int> row) {
    for (int i = 0; i < 3; ++i) {
        if (row[i] != 0 && row[i] == row[i + 1]) {
            row[i] *= 2;
            row[i + 1] = 0;
        }
    }
    return compress_row(row);

}

bool move_left(vector<vector<int>>& board){
    bool changed = false;
    for (int i = 0; i < 4; ++i) {
        vector<int> original = board[i];
        vector<int> compressed = compress_row(board[i]);
        vector<int> merged = merge_row(compressed);
        if (merged != original) changed = true;
        board[i] = merged;
    }
    return changed;

}

bool move_right(vector<vector<int>>& board){
    bool changed = false;
    for (int i = 0; i < 4; ++i) {
        vector<int> original = board[i];
        reverse(board[i].begin(), board[i].end());
        board[i] = merge_row(compress_row(board[i]));
        reverse(board[i].begin(), board[i].end());
        if (board[i] != original) changed = true;
    }
    return changed;
}

bool move_up(vector<vector<int>>& board){
    bool changed = false;
    for (int c = 0; c < 4; ++c) {
        vector<int> col;
        for (int r = 0; r < 4; ++r) col.push_back(board[r][c]);
        vector<int> merged = merge_row(compress_row(col));
        for (int r = 0; r < 4; ++r) {
            if (board[r][c] != merged[r]) changed = true;
            board[r][c] = merged[r];
        }
    }
    return changed;

}

bool move_down(vector<vector<int>>& board){
    bool changed = false;
    for (int c = 0; c < 4; ++c) {
        vector<int> col;
        for (int r = 3; r >= 0; --r) col.push_back(board[r][c]);
        vector<int> merged = merge_row(compress_row(col));
        for (int r = 3, i = 0; r >= 0; --r, ++i) {
            if (board[r][c] != merged[i]) changed = true;
            board[r][c] = merged[i];
        }
    }
    return changed;
}

int compute_score(const vector<vector<int>>& board) {
    int total = 0;
    for (auto& row : board)
        for (int v : row)
            total += v;
    return total;
}

int main(){
    srand(time(nullptr));
    vector<vector<int>> board(4, vector<int>(4,0));
    spawn_tile(board);
    spawn_tile(board);

    stack<vector<vector<int>>> history;
    bool first=true;

    while(true){
        print_board(board, first);
        first=false;
        cout << "Score: " << compute_score(board) << "\n";
        cout<<"Move (w=up, a=left, s=down, d=right), u=undo, q=quit: ";
        char cmd;
        if (!(cin>>cmd)) break;
        if (cmd=='q') break;

        if (cmd=='u') {
            if (!history.empty()) {
                board = history.top();
                history.pop();
            }
        }

        vector<vector<int>> prev = board;
        bool moved=false;
        if (cmd=='a') moved=move_left(board);
        else if (cmd=='d') moved=move_right(board);
        else if (cmd=='w') moved=move_up(board);
        else if (cmd=='s') moved=move_down(board);

        if (moved) {
            history.push(prev);
            spawn_tile(board);
        }
    }
    return 0;
}
