#include <bits/stdc++.h>
using namespace std;

// constants
const int BOARD_SIZE = 8;

struct Move
{
    int fR, fC, tR, tC;
};

class ChessBoard
{
    public:
        char board[BOARD_SIZE][BOARD_SIZE];
        bool whiteTurn;

        // ctor
        ChessBoard();

        // game start and print function
        void initialize_board();
        void print_board();

        // helper function
        bool is_inside(int row, int col);
        bool is_white(char p);
        bool is_black(char p);
        bool is_enemy(char current_piece, char target);
        bool is_my_piece(int row,int col);

        // making move
        vector<pair<int, int>> move_helper(int row, int col);
        void make_move(int fR, int fC, int tR, int tC);
        bool is_move_valid(int fR, int fC, int tR, int tC);

        // moves
        vector<std::pair<int, int>> move_pawn(int row,int col);
        vector<std::pair<int, int>> move_rock(int row, int col);
        vector<std::pair<int, int>> move_knight(int row, int col);
        vector<std::pair<int, int>> move_bishop(int row, int col);
        vector<std::pair<int, int>> move_queen(int row, int col);
        vector<std::pair<int, int>> move_king(int row, int col);

        // special moves
        void promote_pawn(int r, int c, bool isWhite);

        // check moves
        pair<int,int> find_king(bool isWhite);
        bool is_square_attacked(int r, int c, bool whiteAttacker);
        bool is_move_safe(int fR, int fC, int tR, int tC);

        // final win - loss
        bool has_legal_moves(bool isWhite);
        vector<Move> get_all_possible_moves(bool isWhite);
        
        // mini-max game play
        void play_bot(bool botIsWhite);
        int evaluate_board();
        // int minimax(int depth, bool isMaximizing);
        int minimax(int depth, int alpha, int beta, bool isMaximizing);
};


ChessBoard :: ChessBoard()
{
    initialize_board();
}

void ChessBoard :: initialize_board()
{
    char initial[BOARD_SIZE][BOARD_SIZE] = {
    {'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r'}, 
    {'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p'},
    {'.', '.', '.', '.', '.', '.', '.', '.'},
    {'.', '.', '.', '.', '.', '.', '.', '.'},
    {'.', '.', '.', '.', '.', '.', '.', '.'},
    {'.', '.', '.', '.', '.', '.', '.', '.'},
    {'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P'},
    {'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R'}
};

    
    for(int i=0;i<BOARD_SIZE;i++)
    {
        for(int j=0;j<BOARD_SIZE;j++)
        {
            board[i][j] = initial[i][j];
        }
    } 

    whiteTurn = true;
}

// prinnt current position
void ChessBoard :: print_board()
{
    for(int i=0;i<BOARD_SIZE;i++)
    {
        for(int j=0;j<BOARD_SIZE;j++)
        {
            cout<<board[i][j]<<" ";
        }
        cout<<endl;
    }
}

// basic check functions
bool ChessBoard :: is_inside(int row, int col) { return row >= 0 && row < BOARD_SIZE && col >= 0 && col < BOARD_SIZE; }

bool ChessBoard :: is_white(char p) { return p >= 'A' && p <= 'Z'; }
bool ChessBoard :: is_black(char p) { return p >= 'a' && p <= 'z'; }
bool ChessBoard :: is_enemy(char current_piece, char target) { return ((is_white(current_piece) && is_black(target)) || (is_black(current_piece) && is_white(target))); }

// valid move
vector<pair<int, int>> ChessBoard :: move_helper(int row, int col)
{
    char peice = toupper(board[row][col]);
    // cout<<endl;
    // cout<<peice<<" - ";

    switch (peice)
    {
        case 'R':
        {
            // cout<<"ROCK"<<endl;
            return move_rock(row,col);
            break;
        }
        case 'N':
        {
            // cout<<"KNIGHT"<<endl;
            return move_knight(row,col);
            break;
        }
        case 'B':
        {
            // cout<<"BISHOP"<<endl;
            return move_bishop(row,col);
            break;
        }
        case 'Q':
        {
            // cout<<"QUEEN"<<endl;
            return move_queen(row,col);
            break;
        }
        case 'K':
        {
            // cout<<"KING"<<endl;
            return move_king(row,col);
            break;
        }
        case 'P':
        {
            // cout<<"PAWN"<<endl;
            return move_pawn(row,col);
            break;
        }
        case '.':
        {
            // cout<<"EMPTY PLACE"<<endl;
            break;
        }
        default:
            break;
    }
    return {};
}

void ChessBoard::make_move(int fR, int fC, int tR, int tC)
{
    // 1. Get the piece that is moving
    char piece = board[fR][fC];

    // 2. Move it to the destination (this overwrites whatever was there)
    board[tR][tC] = piece;

    // 3. Leave the old square empty
    board[fR][fC] = '.';

    // 2. Check for Promotion
    if (toupper(piece) == 'P') {
        // White reaches top or Black reaches bottom
        if ((is_white(piece) && tR == 0) || (is_black(piece) && tR == 7)) {
            promote_pawn(tR, tC, is_white(piece));
        }
    }

    // 4. Flip the turn to the other player
    whiteTurn = !whiteTurn;
    
    // cout << "Move executed!" << endl;
}

bool ChessBoard::is_my_piece(int row, int col)
{
    char piece = board[row][col];

    if(piece == '.')
        return false;
    
        if(whiteTurn)
            return is_white(piece);
        else
            return is_black(piece);
}

bool ChessBoard::is_move_valid(int fR, int fC, int tR, int tC) {
    std::vector<std::pair<int, int>> legal_squares;
    char type = toupper(board[fR][fC]);

    // Route to the correct piece-logic
    if (type == 'R') legal_squares = move_rock(fR, fC);
    else if (type == 'N') legal_squares = move_knight(fR, fC);
    else if (type == 'B') legal_squares = move_bishop(fR, fC);
    else if (type == 'Q') legal_squares = move_queen(fR, fC);
    else if (type == 'K') legal_squares = move_king(fR, fC);
    else if (type == 'P') legal_squares = move_pawn(fR, fC);

    // 2. Search the list: Is the player's 'To' square in the list?
    for (auto const& pos : legal_squares) {
        if (pos.first == tR && pos.second == tC) {
            return true; // The move exists in the piece's rulebook!
        }
    }

    return false; // Not a valid move for this piece
}

// move rock
vector<std::pair<int, int>> ChessBoard::move_rock(int row, int col) {
    
    vector<std::pair<int, int>> possible_moves;

    int dr[] = {-1, 1, 0, 0}; // Up, Down, Left, Right
    int dc[] = {0, 0, -1, 1};
    char current_piece = board[row][col];
    bool found_any = false;

    for (int i = 0; i < 4; i++) {
        int next_r = row + dr[i];
        int next_c = col + dc[i];

        // This loop must continue until it hits a boundary or a piece
        while (is_inside(next_r, next_c)) {
            char target = board[next_r][next_c];

            if (target == '.') {
                // cout << "  Move to: (" << next_r << "," << next_c << ")" << endl;
                possible_moves.push_back({next_r, next_c});
                found_any = true;
                // DO NOT BREAK HERE. We need to see the next square in this direction!
            } 
            else {
                // We hit a piece. Check if we can capture it.
                if (is_enemy(current_piece, target)) {
                    // cout << "  Capture at: (" << next_r << "," << next_c << ")" << endl;
                    possible_moves.push_back({next_r, next_c});
                    found_any = true;
                }
                // WE MUST BREAK HERE. A Rook cannot jump over pieces.
                break; 
            }

            // CRITICAL: Move to the next square in the SAME direction
            next_r += dr[i];
            next_c += dc[i];
        }
    }

    // if (!found_any)
    //     cout << "  No moves possible!" << endl;

    return possible_moves;
}

// move 
vector<std::pair<int, int>> ChessBoard::move_bishop(int row, int col) {

    vector<std::pair<int, int>> possible_moves;

    int dr[] = {-1, -1, 1, 1}; // Up, Down, Left, Right
    int dc[] = {-1, 1, -1, 1};
    char current_piece = board[row][col];
    bool found_any = false;

    for (int i = 0; i < 4; i++) {
        int next_r = row + dr[i];
        int next_c = col + dc[i];

        // This loop must continue until it hits a boundary or a piece
        while (is_inside(next_r, next_c)) {
            char target = board[next_r][next_c];

            if (target == '.') {
                // cout << "  Move to: (" << next_r << "," << next_c << ")" << endl;
                possible_moves.push_back({next_r, next_c});
                found_any = true;
                // DO NOT BREAK HERE. We need to see the next square in this direction!
            } 
            else {
                // We hit a piece. Check if we can capture it.
                if (is_enemy(current_piece, target)) {
                    // cout << "  Capture at: (" << next_r << "," << next_c << ")" << endl;
                    possible_moves.push_back({next_r, next_c});
                    found_any = true;
                }
                // WE MUST BREAK HERE. A Rook cannot jump over pieces.
                break; 
            }

            // CRITICAL: Move to the next square in the SAME direction
            next_r += dr[i];
            next_c += dc[i];
        }
    }

    // if (!found_any) cout << "  No moves possible!" << endl;
    return possible_moves;
}

// move 
vector<std::pair<int, int>> ChessBoard::move_queen(int row, int col) {
    
    vector<std::pair<int, int>> possible_moves;

    // All 8 directions: 4 straight (Rook) + 4 diagonal (Bishop)
    int dr[] = {-1, 1, 0, 0, -1, -1, 1, 1};
    int dc[] = {0, 0, -1, 1, -1, 1, -1, 1};
    char current_piece = board[row][col];
    bool found_any = false;

    for (int i = 0; i < BOARD_SIZE; i++) {
        int next_r = row + dr[i];
        int next_c = col + dc[i];

        // This loop must continue until it hits a boundary or a piece
        while (is_inside(next_r, next_c)) {
            char target = board[next_r][next_c];

            if (target == '.') {
                // cout << "  Move to: (" << next_r << "," << next_c << ")" << endl;
                possible_moves.push_back({next_r, next_c});
                found_any = true;
                // DO NOT BREAK HERE. We need to see the next square in this direction!
            } 
            else {
                // We hit a piece. Check if we can capture it.
                if (is_enemy(current_piece, target)) {
                    // cout << "  Capture at: (" << next_r << "," << next_c << ")" << endl;
                    possible_moves.push_back({next_r, next_c});
                    found_any = true;
                }
                // WE MUST BREAK HERE. A Rook cannot jump over pieces.
                break; 
            }

            // CRITICAL: Move to the next square in the SAME direction
            next_r += dr[i];
            next_c += dc[i];
        }
    }

    // if (!found_any) cout << "  No moves possible!" << endl;
    return possible_moves;
}

// move 
vector<std::pair<int, int>> ChessBoard::move_king(int row, int col) {

    vector<std::pair<int, int>> possible_moves;

    // only one move but in every direction (8 dir)
    int dr[] = {-1, -1, -1,  0, 0,  1, 1, 1};
    int dc[] = {-1,  0,  1, -1, 1, -1, 0, 1};
    char current_piece = board[row][col];
    bool found_any = false;

    for (int i = 0; i < BOARD_SIZE; i++) {
        int next_r = row + dr[i];
        int next_c = col + dc[i];

        // This loop must continue until it hits a boundary or a piece
        if (is_inside(next_r, next_c)) {
            char target = board[next_r][next_c];

            if (target == '.') {
                // cout << "  Move to: (" << next_r << "," << next_c << ")" << endl;
                possible_moves.push_back({next_r, next_c});
                found_any = true;
            } 
            else if (is_enemy(current_piece, target)) {
                    // cout << "  Capture at: (" << next_r << "," << next_c << ")" << endl;
                    possible_moves.push_back({next_r, next_c});
                    found_any = true;
                }
        }
    }

    // if (!found_any) cout << "  No moves possible!" << endl;
    return possible_moves;
}

// move 
vector<std::pair<int, int>> ChessBoard::move_knight(int row, int col) {

    vector<std::pair<int, int>> possible_moves;

    // only one move but in every direction (8 dir)
    int dr[] = {-2, -2, -1, -1,  1, 1,  2, 2};
    int dc[] = {-1,  1, -2,  2, -2, 2, -1, 1};
    char current_piece = board[row][col];
    bool found_any = false;

    for (int i = 0; i < BOARD_SIZE; i++) {
        int next_r = row + dr[i];
        int next_c = col + dc[i];

        // This loop must continue until it hits a boundary or a piece
        if (is_inside(next_r, next_c)) {
            char target = board[next_r][next_c];

            if (target == '.') {
                // cout << "  Move to: (" << next_r << "," << next_c << ")" << endl;
                possible_moves.push_back({next_r, next_c});
                found_any = true;
            } 
            else if (is_enemy(current_piece, target)) {
                    // cout << "  Capture at: (" << next_r << "," << next_c << ")" << endl;
                    possible_moves.push_back({next_r, next_c});
                    found_any = true;
                }
        }
    }

    // if (!found_any) cout << "  No moves possible!" << endl;
    return possible_moves;
}

// move 
vector<std::pair<int, int>> ChessBoard::move_pawn(int row, int col) {

    vector<std::pair<int, int>> possible_moves;

    char current_piece = board[row][col];
    int direction = is_white(current_piece) ? -1: 1;
    int start_row = is_white(current_piece) ? 6 : 1;
    bool found_any = false;

    // normal move forward
    int next_r = row + direction;
    if (is_inside(next_r, col) && board[next_r][col] == '.') {
        // cout << "  Advance to (" << next_r << "," << col << ")" << endl;
        possible_moves.push_back({next_r, col});
        found_any = true;

        // 2. Initial Double Step (Only if the 1st step was empty)
        int double_r = row + (2 * direction);
        if (row == start_row && is_inside(double_r, col) && board[double_r][col] == '.') {
            // cout << "  Double Advance to (" << double_r << "," << col << ")" << endl;
            possible_moves.push_back({double_r, col});
            found_any = true;
        }
    }
    // diagonal capture move

    // 3. Diagonal Captures
    int diag_cols[] = {col - 1, col + 1};
    for (int next_c : diag_cols) {
        if (is_inside(next_r, next_c)) {
            char target = board[next_r][next_c];
            if (target != '.' && (is_enemy(current_piece, target)))
            {
                // cout << "  Capture at (" << next_r << "," << next_c << ")" << endl;
                possible_moves.push_back({next_r, next_c});
                found_any = true;
            }
        }
    }

    // if (!found_any) cout <<"  No moves possible!" << endl;
    return possible_moves;
}

void ChessBoard::promote_pawn(int r, int c, bool isWhite) {
    char choice;
    cout << "\n*** PAWN PROMOTION! ***" << endl;
    cout << "Choose your piece (Q = Queen, R = Rook, B = Bishop, N = Knight): ";
    
    while (true) {
        cin >> choice;
        choice = toupper(choice);
        
        if (choice == 'Q' || choice == 'R' || choice == 'B' || choice == 'N') {
            break;
        }
        cout << "Invalid choice. Please enter Q, R, B, or N: ";
    }

    // If it's black's turn, convert the choice to lowercase
    if (!isWhite) {
        choice = tolower(choice); // Use a helper or choice + 32
    }

    board[r][c] = choice;
    cout << "Pawn promoted to " << choice << "!" << endl;
}

pair<int,int> ChessBoard:: find_king(bool isWhite)
{
    char target = isWhite ? 'K' : 'k';
    for(int r=0; r<BOARD_SIZE;r++)
    {
        for(int c=0; c<BOARD_SIZE;c++)
        {
            if(board[r][c] == target)
                return {r,c};
        }
    }

    // never applies
    // king always there
    return {-1,-1};
}

bool ChessBoard::is_square_attacked(int r, int c, bool whiteAttacker)
{
    for(int i=0;i<BOARD_SIZE;i++)
    {
        for(int j=0;j<BOARD_SIZE;j++)
        {
            char piece = board[i][j];
            if(piece!='.' && is_white(piece) == whiteAttacker)
            {
                vector<pair<int,int>> moves = move_helper(i,j);

                // If any move lands on (r, c), the square is attacked!
                for (auto m : moves) {
                    if (m.first == r && m.second == c) return true;
                }
            }
        }
    }
    return false;
}


bool ChessBoard::is_move_safe(int fR, int fC, int tR, int tC)
{
    // 1. Save current state
    char movingPiece = board[fR][fC];
    char targetPiece = board[tR][tC];
    bool isWhite = is_white(movingPiece);

    // 2. Pretend to move
    board[tR][tC] = movingPiece;
    board[fR][fC] = '.';

    // 3. Find King and see if he is attacked
    pair<int, int> kingPos = find_king(isWhite);
    bool inCheck = is_square_attacked(kingPos.first, kingPos.second, !isWhite);

    // 4. UNDO the move (CRITICAL!)
    board[fR][fC] = movingPiece;
    board[tR][tC] = targetPiece;

    return !inCheck; // Move is safe if NOT in check
}


// FINAL CHECK - WIN OR LOSS
bool ChessBoard::has_legal_moves(bool isWhite)
{
    for (int r = 0; r < BOARD_SIZE; r++)
    {
        for (int c = 0; c < BOARD_SIZE; c++)
        {
            char piece = board[r][c];
            if (piece != '.' && is_white(piece) == isWhite)
            {
                vector<pair<int, int>> moves = move_helper(r, c);
                
                // Check if any of these moves are "Safe"
                for (auto const& m : moves)
                {
                    if (is_move_safe(r, c, m.first, m.second))
                    {
                        return true; // Found at least one way to play!
                    }
                }
            }
        }
    }
    return false; // No moves found. It's either Checkmate or Stalemate.
}

vector<Move> ChessBoard::get_all_possible_moves(bool isWhite)
{
    vector<Move> allMoves;

    for(int r=0;r<BOARD_SIZE;r++)
    {
        for(int c=0;c<BOARD_SIZE;c++)
        {
            if(board[r][c]!='.' && is_white(board[r][c])== isWhite)
            {
                vector<pair<int,int>> targets = move_helper(r,c);

                for (auto const& t : targets) {

                    // Only add if it doesn't put the King in check
                    if (is_move_safe(r, c, t.first, t.second)) {
                        allMoves.push_back({r, c, t.first, t.second});
                    }
                }
            }
        }
    }

    return allMoves;
}  

// void ChessBoard::play_bot()
// {
//     vector<Move> legalMoves = get_all_possible_moves(whiteTurn);

//     if(legalMoves.empty())
//     {
//         cout << "Bot has no moves! Game Over." << endl;
//         return;
//     }

//     // Pick a random move
//     int randomIndex = rand() % legalMoves.size();
//     Move selected = legalMoves[randomIndex];

//     cout << "Bot chooses move: (" << selected.fR << "," << selected.fC 
//          << ") to (" << selected.tR << "," << selected.tC << ")" << endl;

//     make_move(selected.fR, selected.fC, selected.tR, selected.tC);
// }


// mini-max wala
// void ChessBoard::play_bot() {
//     vector<Move> moves = get_all_possible_moves(false); // Bot is Black
//     int bestScore = 10000; // Since Black wants to minimize the score
//     Move bestMove = {-1, -1, -1, -1};

//     for (auto& m : moves) {
//         // Simulate
//         char tempTo = board[m.tR][m.tC];
//         board[m.tR][m.tC] = board[m.fR][m.fC];
//         board[m.fR][m.fC] = '.';

//         int score = minimax(2, true); // Depth 2 is a good start

//         // Undo
//         board[m.fR][m.fC] = board[m.tR][m.tC];
//         board[m.tR][m.tC] = tempTo;

//         if (score < bestScore) {
//             bestScore = score;
//             bestMove = m;
//         }
//     }

//     if (bestMove.fR != -1) {
//         make_move(bestMove.fR, bestMove.fC, bestMove.tR, bestMove.tC);
//     }
// }

// void ChessBoard::play_bot() {
//     vector<Move> moves = get_all_possible_moves(false); // Black bot
//     int bestScore = 30000; 
//     Move bestMove = {-1, -1, -1, -1};

//     for (auto& m : moves) {
//         char tempTo = board[m.tR][m.tC];
//         board[m.tR][m.tC] = board[m.fR][m.fC];
//         board[m.fR][m.fC] = '.';

//         // Depth 4 is usually the "sweet spot" for a fast C++ engine
//         int score = minimax(4, -30000, 30000, true); 

//         board[m.fR][m.fC] = board[m.tR][m.tC];
//         board[m.tR][m.tC] = tempTo;

//         if (score < bestScore) {
//             bestScore = score;
//             bestMove = m;
//         }
//     }

//     if (bestMove.fR != -1) {
//         make_move(bestMove.fR, bestMove.fC, bestMove.tR, bestMove.tC);
//     }
// }

void ChessBoard::play_bot(bool botIsWhite) {
    // The bot now gets moves for whichever color it was assigned
    vector<Move> moves = get_all_possible_moves(botIsWhite); 
    
    // If bot is White, it wants to MAXIMIZE score. If Black, MINIMIZE.
    int bestScore = botIsWhite ? -30000 : 30000; 
    Move bestMove = {-1, -1, -1, -1};

    for (auto& m : moves) {
        char tempTo = board[m.tR][m.tC];
        board[m.tR][m.tC] = board[m.fR][m.fC];
        board[m.fR][m.fC] = '.';

        // If bot is White (maximizing), the next turn is Black (minimizing)
        int score = minimax(4, -30000, 30000, !botIsWhite); 

        board[m.fR][m.fC] = board[m.tR][m.tC];
        board[m.tR][m.tC] = tempTo;

        if (botIsWhite) {
            if (score > bestScore) {
                bestScore = score;
                bestMove = m;
            }
        } else {
            if (score < bestScore) {
                bestScore = score;
                bestMove = m;
            }
        }
    }

    if (bestMove.fR != -1) {
        cout << "Bot moves from " << bestMove.fR << "," << bestMove.fC << " to " << bestMove.tR << "," << bestMove.tC << endl;
        make_move(bestMove.fR, bestMove.fC, bestMove.tR, bestMove.tC);
    }
}

int ChessBoard::evaluate_board() {
    int score = 0;
    for (int r = 0; r < BOARD_SIZE; r++) {
        for (int c = 0; c < BOARD_SIZE; c++) {
            char p = board[r][c];
            if (p == '.') continue;

            int val = 0;
            char type = toupper(p);
            if (type == 'P') val = 10;
            else if (type == 'N' || type == 'B') val = 30;
            else if (type == 'R') val = 50;
            else if (type == 'Q') val = 90;
            else if (type == 'K') val = 900;

            if (is_white(p)) score += val;
            else score -= val;
        }
    }
    return score;
}

// int ChessBoard::minimax(int depth, bool isMaximizing) {
//     // Base case: if we've looked far enough or game is over
//     if (depth == 0) return evaluate_board();

//     vector<Move> moves = get_all_possible_moves(isMaximizing);
    
//     if (moves.empty()) return isMaximizing ? -9999 : 9999; // Checkmate/Stalemate

//     if (isMaximizing) {
//         int bestScore = -10000;
//         for (auto& m : moves) {
//             // 1. Simulate Move
//             char tempTo = board[m.tR][m.tC];
//             board[m.tR][m.tC] = board[m.fR][m.fC];
//             board[m.fR][m.fC] = '.';
            
//             // 2. Recurse
//             bestScore = max(bestScore, minimax(depth - 1, false));

//             // 3. Undo Move
//             board[m.fR][m.fC] = board[m.tR][m.tC];
//             board[m.tR][m.tC] = tempTo;
//         }
//         return bestScore;
//     } else {
//         int bestScore = 10000;
//         for (auto& m : moves) {
//             char tempTo = board[m.tR][m.tC];
//             board[m.tR][m.tC] = board[m.fR][m.fC];
//             board[m.fR][m.fC] = '.';

//             bestScore = min(bestScore, minimax(depth - 1, true));

//             board[m.fR][m.fC] = board[m.tR][m.tC];
//             board[m.tR][m.tC] = tempTo;
//         }
//         return bestScore;
//     }
// }

int ChessBoard::minimax(int depth, int alpha, int beta, bool isMaximizing)
{
    // 1. Base Case
    if (depth == 0) return evaluate_board();

    vector<Move> moves = get_all_possible_moves(isMaximizing);
    
    // 2. Terminal Case (Checkmate/Stalemate)
    if (moves.empty()) {
        pair<int, int> kPos = find_king(isMaximizing);
        if (is_square_attacked(kPos.first, kPos.second, !isMaximizing))
            return isMaximizing ? -20000 : 20000; // Checkmate
        return 0; // Stalemate
    }

    if (isMaximizing) {
        int maxEval = -30000;
        for (auto& m : moves) {
            // Simulate
            char tempTo = board[m.tR][m.tC];
            board[m.tR][m.tC] = board[m.fR][m.fC];
            board[m.fR][m.fC] = '.';

            int eval = minimax(depth - 1, alpha, beta, false);

            // Undo
            board[m.fR][m.fC] = board[m.tR][m.tC];
            board[m.tR][m.tC] = tempTo;

            maxEval = max(maxEval, eval);
            alpha = max(alpha, eval);
            if (beta <= alpha) break; // ALPHA-BETA PRUNING
        }
        return maxEval;
    } else {
        int minEval = 30000;
        for (auto& m : moves) {
            // Simulate
            char tempTo = board[m.tR][m.tC];
            board[m.tR][m.tC] = board[m.fR][m.fC];
            board[m.fR][m.fC] = '.';

            int eval = minimax(depth - 1, alpha, beta, true);

            // Undo
            board[m.fR][m.fC] = board[m.tR][m.tC];
            board[m.tR][m.tC] = tempTo;

            minEval = min(minEval, eval);
            beta = min(beta, eval);
            if (beta <= alpha) break; // ALPHA-BETA PRUNING
        }
        return minEval;
    }
}