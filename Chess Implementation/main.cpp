#include <bits/stdc++.h>
#include "chess.cpp"
using namespace std;

// main function
int main()
{   
    // cout<<"Chess Game: "<<endl;
    srand(time(0));
    ChessBoard myGame;
    int fR, fC, tR, tC;
    
    string colorStr;
    string assignedColor;
    double myTime, oppTime;

    // Line 1: WHITE or BLACK
    cout<<"Enter BLACK/WHITE";
    if (!(cin >> assignedColor)) return 0;
    for(auto &c : assignedColor) c = toupper(c);

    bool userIsWhite = (assignedColor == "WHITE");
    bool botIsWhite = !userIsWhite;

    // Line 2: Remaining play times
    cout<<"ENTER PLAY TIMES (YourTime OppTime)"<<endl;
    cin >> myTime >> oppTime;

    // Next 8 lines: The Board State
    // for (int i = 0; i < 8; i++) {
    //     string row;
    //     cin >> row;
    //     for (int j = 0; j < 8; j++) {
    //         myGame.board[i][j] = row[j];
    //     }
    // }

    myGame.whiteTurn = true;

    while (true)
    {
        myGame.print_board();

        // 1. Check if the current player has ANY moves left
        bool canMove = myGame.has_legal_moves(myGame.whiteTurn);  

        // --- NEW CHECK WARNING ---
        pair<int, int> kingPos = myGame.find_king(myGame.whiteTurn);
        
        bool inCheck = myGame.is_square_attacked(kingPos.first, kingPos.second, !myGame.whiteTurn);

        if (!canMove)
        {
            if (inCheck)
            {
                cout << "\n************************************" << endl;
                cout << "CHECKMATE! " << (myGame.whiteTurn ? "WHITE" : "BLACK") << " LOST!" << endl;
                cout << (myGame.whiteTurn ? "BLACK" : "WHITE") << " WINS THE GAME!" << endl;
                cout << "************************************" << endl;
            }
            else
            {
                cout << "\nSTALEMATE! The game is a DRAW!" << endl;
            }
            break; // Exit the game loop
        }

        // 2. If they can move but are in check, show a warning
        
        if (inCheck)
        {
            cout << "!!! WARNING: YOU ARE IN CHECK !!!" << endl;
        }

        // BOT MOVE
        // cout << "\nTURN: " << (myGame.whiteTurn ? "WHITE (UPPERCASE)" : "BLACK (lowercase)") << endl;
        // cout << "Enter (fromRow fromCol toRow toCol) or -1 to exit: ";

        // --- DYNAMIC TURN LOGIC ---
        // If the current turn matches the bot's color, the bot plays.
        if (myGame.whiteTurn == botIsWhite) 
        {
            cout << "\nBOT'S TURN (" << (botIsWhite ? "WHITE" : "BLACK") << ")..." << endl;
            
            // Note: If you want the bot to play as White, 
            // you should update play_bot() in chess.cpp to take a color parameter.
            // For now, this calls your existing bot logic.
            myGame.play_bot(botIsWhite); 
            continue; 
        }
        
        cout << "\nYOUR TURN: " << (userIsWhite ? "WHITE" : "BLACK") << endl;
        cout << "Enter (fromRow fromCol toRow toCol) or -1 to exit: ";
        
        cin >> fR;
        if (fR == -1) break;
        cin >> fC >> tR >> tC;
        
        // 1. Basic Boundary Check
        if (!myGame.is_inside(fR, fC) || !myGame.is_inside(tR, tC)) {
            cout << "Out of bounds! Try again." << endl;
            continue;
        }

        // 2. Check: Is it your piece?
        if (!myGame.is_my_piece(fR, fC)) {
            cout << "Sry!! This isn't your piece." << endl;
            continue;
        }

        // 4. THE MASTER VALIDATOR: Does the piece physically move this way?
        if (myGame.is_move_valid(fR, fC, tR, tC))
        {
            // never gonna happen i guess
            // 3. Check: Are you hitting your own teammate at the destination?
            if (myGame.board[tR][tC] != '.' && myGame.is_my_piece(tR, tC))
            {
                cout << "That's your own piece, bruhhh!!" << endl;
                continue;
            }

            if (myGame.is_move_safe(fR, fC, tR, tC))
            {
                myGame.make_move(fR, fC, tR, tC);
            }
            else
            {
                cout << "Illegal Move: You cannot leave your King in check!" << endl;
            }
            // Optional: system("clear"); // Use "cls" on Windows
        }
        else
        {
            cout << "Invalid move for that piece!" << endl;
        }
        
        // Clear the screen for the next turn (optional)
        // system("clear"); // Use "cls" if on Windows
    }

    // checking Rock
    // cb.valid_move(1,0);
    // cb.valid_move(6,0);
    // cb.valid_move(7,0);
    // cb.valid_move(7,7);

    cout << "\nGRACY PATEL" << endl;
    return 0;
}