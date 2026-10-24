#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <chrono>
using namespace std;
using Clock = chrono::steady_clock;

const int N = 12;
const string LABELS = "abcdefghjkmn";

struct MoveT { int fr, fc, tr, tc; };

char board_[N][N];
bool botWhite;

Clock::time_point START;
double TIME_BUDGET;
long long nodeCount = 0;
bool aborted = false;

inline bool inside(int r, int c) { return r >= 0 && r < N && c >= 0 && c < N; }
inline bool isWhitePiece(char p) { return p >= 'A' && p <= 'Z'; }
inline bool isEnemy(char cur, char tgt) { return tgt != '.' && isWhitePiece(cur) != isWhitePiece(tgt); }

bool timeUp() {
    if ((++nodeCount & 1023) != 0) return false;
    return chrono::duration<double>(Clock::now() - START).count() >= TIME_BUDGET;
}

int pieceVal(char p) {
    switch (toupper(p)) {
        case 'P': return 10000000;
        case 'X': return 950;
        case 'G': return 520;
        case 'T': return 500;
        case 'S': return 480;
        case 'Y': return 330;
        case 'N': return 260;
        case 'B': return 100;
        default:  return 0;
    }
}

inline int centerBonus(int r, int c) {
    return min(r, N - 1 - r) + min(c, N - 1 - c); // 0..10
}

int evaluate() {
    int score = 0;
    for (int r = 0; r < N; r++) {
        for (int c = 0; c < N; c++) {
            char p = board_[r][c];
            if (p == '.') continue;
            bool white = isWhitePiece(p);
            char up = toupper(p);
            int total = pieceVal(p);
            if (up != 'P') total += centerBonus(r, c) * 3;
            if (up == 'B') {
                int advance = white ? (N - 1 - r) : r;
                total += advance * 5;
            }
            score += white ? total : -total;
        }
    }
    return botWhite ? score : -score;
}

void genPieceMoves(int r, int c, vector<MoveT>& caps, vector<MoveT>& quiets) {
    char cur = board_[r][c];
    char p = toupper(cur);
    bool white = isWhitePiece(cur);

    auto addSlide = [&](const int dr[], const int dc[], int n, int lim) {
        for (int i = 0; i < n; i++) {
            for (int d = 1; d <= lim; d++) {
                int nr = r + dr[i] * d, nc = c + dc[i] * d;
                if (!inside(nr, nc)) break;
                char t = board_[nr][nc];
                if (t == '.') quiets.push_back({r, c, nr, nc});
                else { if (isEnemy(cur, t)) caps.push_back({r, c, nr, nc}); break; }
            }
        }
    };

    if (p == 'G') { int dr[] = {-1,1,0,0}, dc[] = {0,0,-1,1}; addSlide(dr, dc, 4, 2); }
    else if (p == 'T') { int dr[] = {-1,-1,1,1}, dc[] = {-1,1,-1,1}; addSlide(dr, dc, 4, 2); }
    else if (p == 'X') { int dr[] = {-1,1,0,0,-1,-1,1,1}, dc[] = {0,0,-1,1,-1,1,-1,1}; addSlide(dr, dc, 8, 3); }
    else if (p == 'P') {
        int dr[] = {-1,-1,-1,0,0,1,1,1}, dc[] = {-1,0,1,-1,1,-1,0,1};
        for (int i = 0; i < 8; i++) {
            int nr = r + dr[i], nc = c + dc[i];
            if (!inside(nr, nc)) continue;
            char t = board_[nr][nc];
            if (t == '.') quiets.push_back({r, c, nr, nc});
            else if (isEnemy(cur, t)) caps.push_back({r, c, nr, nc});
        }
    }
    else if (p == 'Y') {
        int dr[] = {-1,-1,1,1}, dc[] = {-1,1,-1,1};
        for (int i = 0; i < 4; i++) {
            int nr = r + dr[i], nc = c + dc[i];
            if (!inside(nr, nc)) continue;
            char t = board_[nr][nc];
            if (t == '.') quiets.push_back({r, c, nr, nc});
            else if (isEnemy(cur, t)) caps.push_back({r, c, nr, nc});
        }
    }
    else if (p == 'B') {
        int dir = white ? -1 : 1;
        int nr1 = r + dir;
        if (inside(nr1, c)) {
            char t1 = board_[nr1][c];
            if (t1 == '.') {
                quiets.push_back({r, c, nr1, c});
                int nr2 = r + 2 * dir;
                if (inside(nr2, c)) {
                    char t2 = board_[nr2][c];
                    if (t2 == '.') quiets.push_back({r, c, nr2, c});
                    else if (isEnemy(cur, t2)) caps.push_back({r, c, nr2, c});
                }
            } else if (isEnemy(cur, t1)) caps.push_back({r, c, nr1, c});
        }
    }
    else if (p == 'S') {
        int dir = white ? -1 : 1;
        for (int d = 1; d <= 3; d++) {
            for (int dc2 = -1; dc2 <= 1; dc2++) {
                int nr = r + dir * d, nc = c + dc2;
                if (!inside(nr, nc)) continue;
                char t = board_[nr][nc];
                if (t == '.') quiets.push_back({r, c, nr, nc});
                else if (isEnemy(cur, t)) caps.push_back({r, c, nr, nc});
            }
        }
    }
    else if (p == 'N') {
        int dr[] = {-1,-1,-1,0,0,1,1,1}, dc[] = {-1,0,1,-1,1,-1,0,1};
        for (int i = 0; i < 8; i++) {
            int nr = r + dr[i], nc = c + dc[i];
            if (!inside(nr, nc)) continue;
            char t = board_[nr][nc];
            if (t != '.' && !isEnemy(cur, t)) continue;
            bool friendNearby = false;
            for (int j = 0; j < 8; j++) {
                int fr = nr + dr[j], fc = nc + dc[j];
                if (inside(fr, fc) && !(fr == r && fc == c) && board_[fr][fc] != '.' &&
                    isWhitePiece(board_[fr][fc]) == white) { friendNearby = true; break; }
            }
            if (!friendNearby) continue;
            if (t == '.') quiets.push_back({r, c, nr, nc});
            else caps.push_back({r, c, nr, nc});
        }
    }
}

vector<MoveT> genAllMoves(bool white) {
    vector<MoveT> caps, quiets;
    for (int r = 0; r < N; r++)
        for (int c = 0; c < N; c++) {
            char p = board_[r][c];
            if (p == '.' || isWhitePiece(p) != white) continue;
            genPieceMoves(r, c, caps, quiets);
        }
    sort(caps.begin(), caps.end(), [](const MoveT& a, const MoveT& b) {
        return pieceVal(board_[a.tr][a.tc]) > pieceVal(board_[b.tr][b.tc]);
    });
    vector<MoveT> all;
    all.reserve(caps.size() + quiets.size());
    all.insert(all.end(), caps.begin(), caps.end());
    all.insert(all.end(), quiets.begin(), quiets.end());
    return all;
}

int alphaBeta(int depth, int alpha, int beta, bool maximizing) {
    if (timeUp()) { aborted = true; return 0; }
    if (depth == 0) return evaluate();

    bool sideWhite = (maximizing == botWhite);
    vector<MoveT> moves = genAllMoves(sideWhite);
    if (moves.empty()) return evaluate();

    int best = maximizing ? -2000000000 : 2000000000;
    for (auto& m : moves) {
        char cap = board_[m.tr][m.tc];
        board_[m.tr][m.tc] = board_[m.fr][m.fc];
        board_[m.fr][m.fc] = '.';
        int val = alphaBeta(depth - 1, alpha, beta, !maximizing);
        board_[m.fr][m.fc] = board_[m.tr][m.tc];
        board_[m.tr][m.tc] = cap;
        if (aborted) return 0;

        if (maximizing) { if (val > best) best = val; if (best > alpha) alpha = best; }
        else            { if (val < best) best = val; if (best < beta)  beta  = best; }
        if (beta <= alpha) break;
    }
    return best;
}

MoveT bestMoveOverall;
bool haveMove = false;

void search() {
    vector<MoveT> rootMoves = genAllMoves(botWhite);
    if (rootMoves.empty()) return;
    bestMoveOverall = rootMoves[0];
    haveMove = true;

    for (int depth = 1; depth <= 40; depth++) {
        aborted = false;
        int alpha = -2000000000, beta = 2000000000;
        MoveT depthBest = rootMoves[0];
        int depthBestScore = -2000000001;

        for (auto& m : rootMoves) {
            char cap = board_[m.tr][m.tc];
            board_[m.tr][m.tc] = board_[m.fr][m.fc];
            board_[m.fr][m.fc] = '.';
            int val = alphaBeta(depth - 1, alpha, beta, false);
            board_[m.fr][m.fc] = board_[m.tr][m.tc];
            board_[m.tr][m.tc] = cap;
            if (aborted) break;

            if (val > depthBestScore) { depthBestScore = val; depthBest = m; }
            if (val > alpha) alpha = val;
        }
        if (aborted) break;

        bestMoveOverall = depthBest;
        auto it = find_if(rootMoves.begin(), rootMoves.end(), [&](const MoveT& mm) {
            return mm.fr == depthBest.fr && mm.fc == depthBest.fc &&
                   mm.tr == depthBest.tr && mm.tc == depthBest.tc;
        });
        if (it != rootMoves.begin() && it != rootMoves.end()) rotate(rootMoves.begin(), it, it + 1);

        if (chrono::duration<double>(Clock::now() - START).count() >= TIME_BUDGET) break;
    }
}

int main() {
    if (freopen("input.txt", "r", stdin) == NULL) return 0;
    if (freopen("output.txt", "w", stdout) == NULL) return 0;

    START = Clock::now();

    string color;
    if (!(cin >> color)) return 0;
    botWhite = (color == "WHITE");
    double myTime, oppTime;
    cin >> myTime >> oppTime;
    for (int r = 0; r < N; r++) {
        string row; cin >> row;
        for (int c = 0; c < N; c++) board_[r][c] = row[c];
    }

    if (myTime < 0.6) {
        vector<MoveT> moves = genAllMoves(botWhite);
        if (!moves.empty()) {
            MoveT m = moves[0];
            cout << LABELS[m.fc] << (N - m.fr) << " " << LABELS[m.tc] << (N - m.tr) << "\n";
        }
        return 0;
    }

    double reserve = 0.25;
    double usable = myTime - reserve;
    if (usable < 0.01) usable = 0.01;
    TIME_BUDGET = min(usable * 0.06, 10.0);
    if (TIME_BUDGET < 0.05) TIME_BUDGET = min(0.05, usable);

    search();

    if (haveMove) {
        cout << LABELS[bestMoveOverall.fc] << (N - bestMoveOverall.fr) << " "
             << LABELS[bestMoveOverall.tc] << (N - bestMoveOverall.tr) << "\n";
    }
    return 0;
}
