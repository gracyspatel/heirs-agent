import os
import time
import sys

# High recursion limit for deep search trees
sys.setrecursionlimit(200000)

# Column mapping skips 'i' and lowercase 'L' 
COLS = "abcdefghjkmn"
INV_COL_MAP = {i: char for i, char in enumerate(COLS)}

# Piece Values [cite: 144, 278]
PIECE_VALS = {
    'P': 100000, 'X': 900, 'S': 500, 'G': 300, 
    'T': 300, 'N': 250, 'Y': 200, 'B': 100
}

class HeirsAgent:
    def __init__(self, color, initial_time):
        self.color = color.upper()
        self.is_white = (self.color == "WHITE")
        self.start_time = time.time()
        # Adaptive time budget: 4% of total remaining time [cite: 163, 265]
        self.time_limit = min(12.0, max(0.05, initial_time * 0.04))
        
        # PST: Strategic values for center control [cite: 151, 278]
        self.PST = [[(min(r, 11-r) + min(c, 11-c)) for c in range(12)] for r in range(12)]

    def in_bounds(self, r, c):
        return 0 <= r < 12 and 0 <= c < 12

    def is_enemy(self, p, color):
        if p == '.': return False
        return p.isupper() != (color == "WHITE")

    def is_friend(self, p, color):
        if p == '.': return False
        return p.isupper() == (color == "WHITE")

    def get_moves(self, board, white_turn):
        color_str = "WHITE" if white_turn else "BLACK"
        fwd = 1 if white_turn else -1 # White moves 1->12 (up), Black 12->1 (down) [cite: 100]
        moves = []

        for r in range(12):
            for c in range(12):
                p = board[r][c]
                if p == '.' or p.isupper() != white_turn: continue
                
                pt = p.upper()
                raw_dst = []

                if pt == 'B': # Baby: 1-2 fwd, captures forward [cite: 103, 107]
                    nr = r + fwd
                    if self.in_bounds(nr, c):
                        target = board[nr][c]
                        if target == '.' or self.is_enemy(target, color_str):
                            raw_dst.append((nr, c))
                        if target == '.' and self.in_bounds(r + 2*fwd, c):
                            target2 = board[r+2*fwd][c]
                            if target2 == '.' or self.is_enemy(target2, color_str):
                                raw_dst.append((r+2*fwd, c))

                elif pt == 'P': # Prince: 1 sq any direction [cite: 111]
                    for dr in [-1,0,1]:
                        for dc in [-1,0,1]:
                            if dr == 0 and dc == 0: continue
                            nr, nc = r+dr, c+dc
                            if self.in_bounds(nr, nc) and not self.is_friend(board[nr][nc], color_str):
                                raw_dst.append((nr, nc))

                elif pt == 'X': # Princess: 1-3 steps, no jump [cite: 118, 119]
                    for dr, dc in [(0,1),(0,-1),(1,0),(-1,0),(1,1),(1,-1),(-1,1),(-1,-1)]:
                        for s in range(1, 4):
                            nr, nc = r+dr*s, c+dc*s
                            if not self.in_bounds(nr, nc): break
                            t = board[nr][nc]
                            if t == '.': raw_dst.append((nr, nc))
                            elif self.is_enemy(t, color_str):
                                raw_dst.append((nr, nc)); break
                            else: break

                elif pt == 'S': # Scout: 1-3 fwd + 1 shift, CAN JUMP [cite: 132, 133]
                    for s in range(1, 4):
                        nr = r + fwd * s
                        if not self.in_bounds(nr, 0): break
                        for dc in [-1, 0, 1]:
                            nc = c + dc
                            if self.in_bounds(nr, nc) and not self.is_friend(board[nr][nc], color_str):
                                raw_dst.append((nr, nc))

                elif pt in ('G', 'T'): # Guard/Tutor: 1-2 steps, no jump [cite: 125, 128]
                    dirs = [(0,1),(0,-1),(1,0),(-1,0)] if pt == 'G' else [(1,1),(1,-1),(-1,1),(-1,-1)]
                    for dr, dc in dirs:
                        for s in range(1, 3):
                            nr, nc = r+dr*s, c+dc*s
                            if not self.in_bounds(nr, nc): break
                            t = board[nr][nc]
                            if t == '.': raw_dst.append((nr, nc))
                            elif self.is_enemy(t, color_str):
                                raw_dst.append((nr, nc)); break
                            else: break

                elif pt == 'Y': # Pony: 1 diagonal [cite: 122]
                    for dr, dc in [(1,1),(1,-1),(-1,1),(-1,-1)]:
                        nr, nc = r+dr, c+dc
                        if self.in_bounds(nr, nc) and not self.is_friend(board[nr][nc], color_str):
                            raw_dst.append((nr, nc))

                elif pt == 'N': # Sibling: Adjacency requirement [cite: 137]
                    for dr in [-1,0,1]:
                        for dc in [-1,0,1]:
                            if dr == 0 and dc == 0: continue
                            nr, nc = r+dr, c+dc
                            if self.in_bounds(nr, nc) and not self.is_friend(board[nr][nc], color_str):
                                if any(self.is_friend(board[nr+ar][nc+ac], color_str)
                                       for ar in [-1,0,1] for ac in [-1,0,1]
                                       if (ar!=0 or ac!=0) and self.in_bounds(nr+ar, nc+ac) and (nr+ar, nc+ac) != (r,c)):
                                    raw_dst.append((nr, nc))

                for dst in raw_dst:
                    moves.append(((r, c), dst))
        return moves

    def evaluate(self, board):
        score = 0
        for r in range(12):
            for c in range(12):
                p = board[r][c]
                if p == '.': continue
                val = PIECE_VALS.get(p.upper(), 0) + self.PST[r][c]
                if p.isupper() == self.is_white: score += val
                else: score -= val
        return score

    def minimax(self, board, depth, alpha, beta, maximizing, white_turn):
        if time.time() - self.start_time > self.time_limit:
            return self.evaluate(board), None
        
        if depth == 0: return self.evaluate(board), None

        moves = self.get_moves(board, white_turn)
        if not moves: return self.evaluate(board), None
        
        # MVV Move Ordering
        moves.sort(key=lambda m: PIECE_VALS.get(board[m[1][0]][m[1][1]].upper(), 0), reverse=True)

        best_m = moves[0]
        if maximizing:
            val = float('-inf')
            for m in moves:
                target = board[m[1][0]][m[1][1]]
                if target.upper() == 'P': return 1000000, m
                board[m[1][0]][m[1][1]], board[m[0][0]][m[0][1]] = board[m[0][0]][m[0][1]], '.'
                ev, _ = self.minimax(board, depth-1, alpha, beta, False, not white_turn)
                board[m[0][0]][m[0][1]], board[m[1][0]][m[1][1]] = board[m[1][0]][m[1][1]], target
                if ev > val: val, best_m = ev, m
                alpha = max(alpha, val)
                if beta <= alpha: break
            return val, best_m
        else:
            val = float('inf')
            for m in moves:
                target = board[m[1][0]][m[1][1]]
                if target.upper() == 'P': return -1000000, m
                board[m[1][0]][m[1][1]], board[m[0][0]][m[0][1]] = board[m[0][0]][m[0][1]], '.'
                ev, _ = self.minimax(board, depth-1, alpha, beta, True, not white_turn)
                board[m[0][0]][m[0][1]], board[m[1][0]][m[1][1]] = board[m[1][0]][m[1][1]], target
                if ev < val: val, best_m = ev, m
                beta = min(beta, val)
                if beta <= alpha: break
            return val, best_m

def main():
    if not os.path.exists("input.txt"): return
    with open("input.txt", "r") as f:
        data = f.read().splitlines()
    
    player, time_left = data[0].strip(), float(data[1].split()[0])
    board = [list(row.strip()) for row in data[2:14]]
    
    agent = HeirsAgent(player, time_left)
    best_move = None
    
    # Iterative Deepening [cite: 266]
    for d in range(1, 10):
        if time.time() - agent.start_time > agent.time_limit * 0.8: break
        _, m = agent.minimax(board, d, float('-inf'), float('inf'), True, agent.is_white)
        if m: best_move = m

    if best_move:
        (sr, sc), (dr, dc) = best_move
        # Formatting for Row 1 at bottom [cite: 52-63, 203]
        output = f"{INV_COL_MAP[sc]}{12-sr} {INV_COL_MAP[dc]}{12-dr}"
        with open("output.txt", "w") as f:
            f.write(output); f.flush(); os.fsync(f.fileno())

if __name__ == "__main__":
    main()