package tendong.hexchess;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.HashSet;

public final class GlinskiChess {

    private static final class HexCell { // typedef Cell (unsigned) byte;
        private static final byte BLACK  = 1; // ON: White, OFF: Black

        private static final byte PAWN   = 1 << 1;
        private static final byte KNIGHT = 1 << 2;
        private static final byte BISHOP = 1 << 3;
        private static final byte ROOK   = 1 << 4;
        private static final byte QUEEN  = BISHOP | ROOK;
        private static final byte KING   = 1 << 5;

        private static final byte EMPTY  = -1 << 7;

        private HexCell() {}
    }

    private static final class Move {
        private static final String[] PRETTY_TEMPLATE = {
                "                __",
                "             __/  \\__",
                "          __/  \\__/  \\__",
                "       __/  \\__/  \\__/  \\__",
                "    __/  \\__/  \\__/  \\__/  \\__",
                " __/  \\__/  \\__/  \\__/  \\__/  \\__",
                "/  \\__/  \\__/  \\__/  \\__/  \\__/  \\",
                "\\__/  \\__/  \\__/  \\__/  \\__/  \\__/",
                "/  \\__/  \\__/  \\__/  \\__/  \\__/  \\",
                "\\__/  \\__/  \\__/  \\__/  \\__/  \\__/",
                "/  \\__/  \\__/  \\__/  \\__/  \\__/  \\",
                "\\__/  \\__/  \\__/  \\__/  \\__/  \\__/",
                "/  \\__/  \\__/  \\__/  \\__/  \\__/  \\",
                "\\__/  \\__/  \\__/  \\__/  \\__/  \\__/",
                "/  \\__/  \\__/  \\__/  \\__/  \\__/  \\",
                "\\__/  \\__/  \\__/  \\__/  \\__/  \\__/",
                "/  \\__/  \\__/  \\__/  \\__/  \\__/  \\",
                "\\__/  \\__/  \\__/  \\__/  \\__/  \\__/",
                "   \\__/  \\__/  \\__/  \\__/  \\__/",
                "      \\__/  \\__/  \\__/  \\__/",
                "         \\__/  \\__/  \\__/",
                "            \\__/  \\__/",
                "               \\__/"
        };

        private final byte[][] board = new byte[11][];

        private Move(byte[][] board) {
            for (int i = 0; i < board.length; i++)
                this.board[i] = board[i].clone();
        }

        public void prettyPrint() {
            System.out.println(PRETTY_TEMPLATE[0]);
            StringBuilder boardSB;
            for (int i = 1; i <= 21; i++) {
                boardSB = new StringBuilder(PRETTY_TEMPLATE[i]);
                for (int j = Math.max(0, i-11); j <= Math.min(i-1, 10); j++) {
                    byte cell = board[i-1-j][j];
                    if (cell <= 0) continue;
                    int ix = 19 - 3*i + 6*j;
                    boardSB.setCharAt(ix, (cell & HexCell.BLACK) == 0 ? 'w' : 'b');
                    boardSB.setCharAt(ix+1, cell2Char(cell));
                }
                System.out.println(boardSB);
            }
            System.out.println(PRETTY_TEMPLATE[22]);
            System.out.println();

            System.out.println("full-move count: " + (128 * (board[8][1] & 0x7f) + (board[8][0] & 0x7f)));
            System.out.println("half-move clock: " + (board[10][1] & 0x7f));
            System.out.printf("e.p. coordinate: (%d, %d)\n", board[9][1] & 0xf, board[9][0] & 0xf);
            System.out.println(((board[10][0] & HexCell.BLACK) == 0 ? "WHITE" : "BLACK") + "'s turn");
        }

        private char cell2Char(byte cell) {
            return switch(cell & 0x3e) {
                case HexCell.PAWN   -> 'P';
                case HexCell.ROOK   -> 'R';
                case HexCell.KNIGHT -> 'N';
                case HexCell.BISHOP -> 'B';
                case HexCell.QUEEN  -> 'Q';
                case HexCell.KING   -> 'K';
                default             -> ' ';
            };
        }
    }

    private static final byte E = HexCell.EMPTY;

    private static final byte WK = HexCell.KING;
    private static final byte WQ = HexCell.QUEEN;
    private static final byte WR = HexCell.ROOK;
    private static final byte WB = HexCell.BISHOP;
    private static final byte WN = HexCell.KNIGHT;
    private static final byte WP = HexCell.PAWN;

    private static final byte BK = HexCell.BLACK | HexCell.KING;
    private static final byte BQ = HexCell.BLACK | HexCell.QUEEN;
    private static final byte BR = HexCell.BLACK | HexCell.ROOK;
    private static final byte BB = HexCell.BLACK | HexCell.BISHOP;
    private static final byte BN = HexCell.BLACK | HexCell.KNIGHT;
    private static final byte BP = HexCell.BLACK | HexCell.PAWN;

    private static final byte[][] INIT_BOARD = {
            {BB, BK, BN, BR, BP,  0,  E,  E,  E, -1, -1}, // last move fromPos y / x
            {BQ, BB,  0,  0, BP,  0,  0,  E,  E, -1, -1}, // last move toPos y / x
            {BN,  0, BB,  0, BP,  0,  0,  0,  E,  E,  E}, // last piece captured (including empty cell)
            {BR,  0,  0,  0, BP,  0,  0,  0,  0,  E,  E}, // was the last move en passant?
            {BP, BP, BP, BP, BP,  0,  0,  0,  0,  0,  E},
            { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
            { E,  0,  0,  0,  0,  0, WP, WP, WP, WP, WP},
            { E,  E,  0,  0,  0,  0, WP,  0,  0,  0, WR},
            { E,  E,  E,  0,  0,  0, WP,  0, WB,  0, WN}, // full-move count in 128-ary numerics
            {-1, -1,  E,  E,  0,  0, WP,  0,  0, WB, WK}, // en passant coord y / x
            { E,  E,  E,  E,  E,  0, WP, WR, WN, WQ, WB}  // active turn, half-move clock, e.p. availability
    };

    private static final byte[][] BISHOP_RAY = {
            {-1, 1}, {-2, -1}, {-1, -2}, {1, -1}, {2, 1}, {1, 2}
    };
    private static final byte[][] ROOK_RAY = {
            {0, 1}, {-1, 0}, {-1, -1}, {0, -1}, {1, 0}, {1, 1}
    };
    private static final byte[][] KNIGHT_JUMP = {
            {-1, 2}, {-2, 1}, {-3, -1}, {-3, -2}, {-2, -3}, {-1, -3},
            {1, -2}, {2, -1}, {3, 1}, {3, 2}, {2, 3}, {1, 3}
    };

    private static final long[] BISHOP_BIT = { // bit 0 ~ 5
            1, 1 << 1, 1 << 2, 1 << 3, 1 << 4, 1 << 5
    };
    private static final long[] ROOK_BIT = { // bit 8 ~ 13
            1 << 8, 1 << 9, 1 << 10, 1 << 11, 1 << 12, 1 << 13
    };
    private static final long[] KNIGHT_BIT = { // bit 16 ~ 27
            1 << 16, 1 << 17, 1 << 18, 1 << 19, 1 << 20, 1 << 21,
            1 << 22, 1 << 23, 1 << 24, 1 << 25, 1 << 26, 1 << 27,
    };
    private static final long[] BISHOP_RAY_BIT = { // bit 32 ~ 37
            (long)1 << 32, (long)1 << 33, (long)1 << 34, (long)1 << 35, (long)1 << 36, (long)1 << 37
    };
    private static final long[] ROOK_RAY_BIT = { // bit 40 ~ 45
            (long)1 << 40, (long)1 << 41, (long)1 << 42, (long)1 << 43, (long)1 << 44, (long)1 << 45
    };

    private final byte[][] board = new byte[11][11];

    private final byte[] kingCell = new byte[2];
    private final HashSet<Byte> whiteOccCell = new HashSet<>();
    private final HashSet<Byte> blackOccCell = new HashSet<>();
    private final long[][][] attackingCell = new long[2][11][11]; // typedef CellAttackInfo long
    /* 0b******** ******** ******** ********  ******** ******** ******** ********
     *                     ________                                               : rook ray
     *                              ________                                      : bishop ray
     *                                        ________ ________                   : knight jump
     *                                                          ________          : rook jump
     *                                                                   ________ : bishop jump
     */

    private final HashMap<Byte, Byte> whiteRookPinnedCell = new HashMap<>();
    private final HashMap<Byte, Byte> blackRookPinnedCell = new HashMap<>();
    private final HashMap<Byte, Byte> whiteBishopPinnedCell = new HashMap<>();
    private final HashMap<Byte, Byte> blackBishopPinnedCell = new HashMap<>();

    // ------------------ current state ------------------

    private byte color;
    private HashSet<Byte> occCell;
    private HashMap<Byte, Byte> rookPinnedCell, bishopPinnedCell;
    private HashMap<Byte, HashSet<Byte>> moveMap;

    private ArrayList<Move> moveList;

    // ------------------ init func ------------------

    public GlinskiChess() { init(); }

    private void init() {
        init(INIT_BOARD);
    }

    private void init(byte[][] initBoard) { // fixme for general init board
        assert initBoard.length == 11 && initBoard[0].length == 11;
        for (int sgm = 0; sgm < 11; sgm++)
            board[sgm] = initBoard[sgm].clone();

        kingCell[0] = kingCell[1] = -1;

        whiteOccCell.clear();
        blackOccCell.clear();
        for (int y = 0; y < board.length; y++) {
            for (int x = Math.max(0, y-5); x <= Math.min(10, 5+y); x++) {
                if (!inRange(x, y) || board[y][x] <= 0) continue;
                byte pos = coord2Byte(x, y);
                if ((board[y][x] & HexCell.BLACK) == 0) {
                    whiteOccCell.add(pos);
                    flipCellAttackInfo(pos, false);
                    if (board[y][x] == WK)
                        kingCell[0] = coord2Byte(x, y);
                } else {
                    blackOccCell.add(pos);
                    flipCellAttackInfo(pos, true);
                    if (board[y][x] == BK)
                        kingCell[1] = coord2Byte(x, y);
                }
            }
        }

        color = (byte) (board[10][0] & 1);
        if (color == 0) {
            occCell = whiteOccCell;
            rookPinnedCell = whiteRookPinnedCell;
            bishopPinnedCell = whiteBishopPinnedCell;
        } else {
            occCell = blackOccCell;
            rookPinnedCell = blackRookPinnedCell;
            bishopPinnedCell = blackBishopPinnedCell;
        }

        board[8][0] += 1; // full-move count

        moveMap = legalMoveMap();
        moveList = new ArrayList<>();
        moveList.add(new Move(board));
    }

    private void flipCellAttackInfo(byte pos, boolean isBlack) {
        int x = pos & 0xf, y = pos >> 4 & 0xf;
        flipCellAttackInfo(x, y, board[y][x], isBlack);
    }

    private void flipCellAttackInfo(int x, int y, byte cell, boolean isBlack) {
        if (cell < 0) return;
        switch (cell & 0x3e) {
            case HexCell.PAWN ->
                    flipPawnAttackInfo(x, y, isBlack);
            case HexCell.KNIGHT ->
                    flipCellAttackInfo(x, y, isBlack, KNIGHT_JUMP, KNIGHT_BIT, false);
            case HexCell.KING -> {
                flipCellAttackInfo(x, y, isBlack, ROOK_RAY, ROOK_BIT, false);
                flipCellAttackInfo(x, y, isBlack, BISHOP_RAY, BISHOP_BIT, false);
            }
            default -> {
                if ((cell & HexCell.ROOK) != 0)
                    flipCellAttackInfo(x, y, isBlack, ROOK_RAY, ROOK_RAY_BIT, true);
                if ((cell & HexCell.BISHOP) != 0)
                    flipCellAttackInfo(x, y, isBlack, BISHOP_RAY, BISHOP_RAY_BIT, true);
            }
        }
    }

    private void flipPawnAttackInfo(int x, int y, boolean isBlack) {
        if (isBlack) {
            int color = 1;
            if (inRange(x, y+1)) // right-diag attack
                attackingCell[color][y+1][x] ^= ROOK_BIT[4];
            if (inRange(x+1, y))// left-diag attack
                attackingCell[color][y][x+1] ^= ROOK_BIT[0];
        } else {
            int color = 0;
            if (inRange(x, y-1))// right-diag attack
                attackingCell[color][y-1][x] ^= ROOK_BIT[1];
            if (inRange(x-1, y)) // left-diag attack
                attackingCell[color][y][x-1] ^= ROOK_BIT[3];
        }
    }

    private void flipCellAttackInfo(int x, int y, boolean isBlack, byte[][] vector, long[] bitArr, boolean forRay) {
        int color = isBlack ? 1 : 0;
        if (forRay) { // for ray
            for (int i = 0; i < vector.length; i++) {
                byte[] ray = vector[i];
                int xv = x + ray[0], yv = y + ray[1];
                while (inRange(xv, yv)) {
                    attackingCell[color][yv][xv] ^= bitArr[i];
                    if (board[yv][xv] != 0) break;
                    xv += ray[0]; yv += ray[1];
                }
            }
        } else { // for jump
            for (int i = 0; i < vector.length; i++) {
                byte[] jump = vector[i];
                int xv = x + jump[0], yv = y + jump[1];
                if (inRange(xv, yv))
                    attackingCell[color][yv][xv] ^= bitArr[i];
            }
        }
    }

    // ------------------ move gen func ------------------

    private HashMap<Byte, HashSet<Byte>> legalMoveMap() {
        System.out.println("WHITE OCC CELL : " + whiteOccCell);
        System.out.println("BLACK OCC CELL : " + blackOccCell);
        moveMap = new HashMap<>();
        int opCol = color ^ 1;
        for (byte pos : occCell)
            moveMap.put(pos, new HashSet<>());
        int kingX = kingCell[color] & 0xf, kingY = kingCell[color] >> 4 & 0xf;
        assert board[kingY][kingX] == (color == 0 ? WK : BK);
        int dirCnt = 0;
        for (long b = 1; b > 0; b <<= 1) // search where the king is attacked from
            if ((attackingCell[opCol][kingY][kingX] & b) != 0)
                dirCnt++;
        { // add king's legal moves
            System.out.printf("adding King Moves : (%d, %d) -> ", kingX, kingY);
            HashSet<Byte> kingMove = moveMap.get(kingCell[color]);
            for (byte[] jump : ROOK_RAY) {
                int x = kingX + jump[0], y = kingY + jump[1];
                if (inRange(x, y) && (board[y][x] == 0 || (board[y][x] & HexCell.BLACK) != color)
                        && attackingCell[opCol][y][x] == 0) {
                    kingMove.add(coord2Byte(x, y));
                    System.out.printf("(%d, %d), ", x, y);
                }
            }
            for (byte[] jump : BISHOP_RAY) {
                int x = kingX + jump[0], y = kingY + jump[1];
                if (inRange(x, y) && (board[y][x] == 0 || (board[y][x] & HexCell.BLACK) != color)
                        && attackingCell[opCol][y][x] == 0) {
                    kingMove.add(coord2Byte(x, y));
                    System.out.printf("(%d, %d), ", x, y);
                }
            }
            System.out.println();
        }
        if (dirCnt >= 2) return moveMap; // double check
        { // init & find pinned pieces
            findPinnedCells(kingX, kingY, ROOK_RAY, ROOK_RAY_BIT, rookPinnedCell);
            findPinnedCells(kingX, kingY, BISHOP_RAY, BISHOP_RAY_BIT, bishopPinnedCell);
        }
        if (dirCnt == 1) { // non-double check
            byte[] atkVec = attackVector(attackingCell[opCol][kingY][kingX]);
            boolean forRay = attackingCell[opCol][kingY][kingX] >= ((long) 1 << 32);
            int x = kingX - atkVec[0], y = kingY - atkVec[1];
            if (forRay) {
                while (inRange(x, y)) {
                    addBlockingPieceMoves(x, y);
                    addBlockingPawnMoves(x, y);
                    if (board[y][x] != 0) break;
                    x -= atkVec[0];
                    y -= atkVec[1];
                }
            } else {
                addBlockingPieceMoves(x, y);
                addBlockingPawnMoves(x, y);
            }
        } else { // not a check
            for (byte pos : occCell) {
                if (pos == kingCell[color])
                    continue;
                addPieceMoves(pos);
            } addEnPassant();
        }
        System.out.println(Arrays.deepToString(board));
        return moveMap;
    }

    private void findPinnedCells (int kingX, int kingY, byte[][] vector, long[] bitArr, HashMap<Byte, Byte> pinnedCell) {
        int opCol = color ^ 1;
        pinnedCell.clear();
        for (byte i = 0; i < vector.length; i++) {
            if ((attackingCell[opCol][kingY][kingX] & bitArr[i]) != 0)
                continue;
            int dx = vector[i][0], dy = vector[i][1];
            int x = kingX + dx, y = kingY + dy;
            while (inRange(x, y)) {
                if (board[y][x] != 0 && (board[y][x] & HexCell.BLACK) == color) {
                    if ((attackingCell[opCol][y][x] & bitArr[i]) != 0)
                        pinnedCell.put(coord2Byte(x, y), i);
                    break;
                } x += dx; y += dy;
            }
        }
    }

    private static byte[] attackVector(long bit) {
        assert bit != 0;
        if ((bit & 0xff) != 0) { // bishop jump
            for (int i = 0; i < BISHOP_RAY.length; i++)
                if ((BISHOP_BIT[i] & bit) != 0)
                    return BISHOP_RAY[i];
        } else if ((bit & 0xff00) != 0) { // rook jump
            for (int i = 0; i < ROOK_RAY.length; i++)
                if ((ROOK_BIT[i] & bit) != 0)
                    return ROOK_RAY[i];
        } else if ((bit & (long)0xffff << 16) != 0) { // knight jump
            for (int i = 0; i < KNIGHT_JUMP.length; i++)
                if ((KNIGHT_BIT[i] & bit) != 0)
                    return KNIGHT_JUMP[i];
        } else if ((bit & (long)0xff << 32) != 0) { // bishop ray
            for (int i = 0; i < BISHOP_RAY.length; i++)
                if ((BISHOP_RAY_BIT[i] & bit) != 0)
                    return BISHOP_RAY[i];
        } else if ((bit & (long)0xff << 40) != 0) { // rook ray
            for (int i = 0; i < ROOK_RAY.length; i++)
                if ((ROOK_RAY_BIT[i] & bit) != 0)
                    return ROOK_RAY[i];
        } return new byte[]{16, 16};
    }

    private void addBlockingPieceMoves(int x, int y) {
        System.out.printf("adding Blocking Piece Moves : (%d, %d) <- ", x, y);
        long bits = attackingCell[color][y][x];
        byte target = coord2Byte(x, y);
        if ((bits & (long)0xffff << 16) != 0) { // knight
            for (int i = 0; i < KNIGHT_JUMP.length; i++) {
                if ((KNIGHT_BIT[i] & bits) != 0) {
                    byte pos = coord2Byte(x - KNIGHT_JUMP[i][0], y - KNIGHT_JUMP[i][1]);
                    if (freeFromPin(pos)) {
                        moveMap.get(pos).add(target);
                        System.out.printf("(%d, %d), ", x - KNIGHT_JUMP[i][0], y - KNIGHT_JUMP[i][1]);
                    }
                }
            }
        }
        if ((bits & (long)0xff << 32) != 0) { // bishop | queen
            System.out.printf("\n\tadding Bishop-blocking moves : (%d, %d) <- ", x, y);
            addBlockingPieceMoves(x, y, bits, target, BISHOP_RAY, BISHOP_RAY_BIT);
        }
        if ((bits & (long)0xff << 40) != 0) { // rook | queen
            System.out.printf("\n\tadding Rook-blocking moves : (%d, %d) <- ", x, y);
            addBlockingPieceMoves(x, y, bits, target, ROOK_RAY, ROOK_RAY_BIT);
        }
    }

    private void addBlockingPieceMoves(int x, int y, long bits, byte target, byte[][] vector, long[] bitArr) {
        for (int i = 0; i < vector.length; i++) {
            if ((bitArr[i] & bits) != 0) {
                byte dx = vector[i][0], dy = vector[i][1];
                int xa = x - dx, ya = y - dy;
                while (board[ya][xa] == 0) {
                    xa -= dx; ya -= dy;
                }
                byte pos = coord2Byte(xa, ya);
                if (freeFromPin(pos)) {
                    moveMap.get(pos).add(target);
                    System.out.printf("(%d, %d), ", xa, ya);
                }
            }
        }
        System.out.println();
    }

    private void addBlockingPawnMoves(int x, int y) {
        // important! en passant info must be included
        System.out.printf("adding Blocking Pawn Moves : (%d, %d) <- ", x, y);
        int dir = color != 0 ? 1 : -1;
        byte target = coord2Byte(x, y);
        if (board[y][x] != 0) { // capturing move
            if (inRange(x-dir, y) && board[y][x-dir] == (HexCell.PAWN | color)) {
                byte pos = coord2Byte(x-dir, y);
                if (freeFromPin(pos)) {
                    moveMap.get(pos).add(target);
                    System.out.printf("(%d, %d), ", x-dir, y);
                }
            }
            if (inRange(x, y-dir) && board[y-dir][x] == (HexCell.PAWN | color)) {
                byte pos = coord2Byte(x, y-dir);
                if (freeFromPin(pos)) {
                    moveMap.get(pos).add(target);
                    System.out.printf("(%d, %d), ", x, y-dir);
                }
            }
            if ((board[9][0] & 0xf) == y+dir || (board[9][1] & 0xf) == x+dir) { // en passant available
                System.out.println();
                System.out.print("\tadding Blocking En Passant Moves : ");
                byte xRayInd, yRayInd;
                byte kingId;
                if (color == 0) { // white
                    xRayInd = 3;
                    yRayInd = 1;
                    kingId = WK;
                } else { // black
                    xRayInd = 0;
                    yRayInd = 4;
                    kingId = BK;
                }
                if (inRange(x+dir, y) && board[y][x+dir] == kingId) {
                    if ((attackingCell[color^1][y][x+dir] & ROOK_RAY_BIT[xRayInd]) == 0
                            || (kingCell[color] & 0xf) == x) {
                        moveMap.get(coord2Byte(x + dir, y)).add(coord2Byte(x + dir, y + dir));
                        System.out.printf("(%d, %d) -> (%d, %d)", x+dir, y, x+dir, y+dir);
                    }
                }
                if (inRange(x, y+dir) && board[y+dir][x] == kingId) {
                    if ((attackingCell[color^1][y+dir][x] & ROOK_RAY_BIT[yRayInd]) == 0
                            || (kingCell[color] & 0xf) == x-dir) {
                        moveMap.get(coord2Byte(x, y + dir)).add(coord2Byte(x + dir, y + dir));
                        System.out.printf("(%d, %d) -> (%d, %d)", x, y+dir, x+dir, y+dir);
                    }
                }
            }
        } else { // non-capturing move; ray piece blocking
            byte pawnId = color != 0 ? BP : WP;
            int xv = x-dir, yv = y-dir;
            if (inRange(xv, yv)) {
                if (board[yv][xv] != 0) {
                    if (board[yv][xv] == pawnId && freeFromPin(coord2Byte(xv, yv))) {
                        moveMap.get(coord2Byte(xv, yv)).add(coord2Byte(x, y));
                        System.out.printf("(%d, %d), ", xv, yv);
                    }
                } else if (inRange((xv -= dir), (yv -= dir)) && inCamp(xv, yv)
                        && board[yv][xv] == pawnId && freeFromPin(coord2Byte(xv, yv))) {
                    moveMap.get(coord2Byte(xv, yv)).add(coord2Byte(x, y));
                    System.out.printf("(%d, %d), ", xv, yv);
                }
            }
        }
        System.out.println();
    }

    private void addPieceMoves(byte pos) {
        assert occCell.contains(pos);
        HashSet<Byte> moves = moveMap.get(pos);
        int x = pos & 0xf, y = pos >> 4 & 0xf;
        System.out.printf("adding Piece Moves : (%d, %d) -> ", x, y);
        if (bishopPinnedCell.containsKey(pos)) {
            byte rayInd = bishopPinnedCell.get(pos);
            if ((board[y][x] & HexCell.BISHOP) != 0) { // movable through the pinning ray; i.e. bishop | queen
                System.out.println();
                System.out.print("\t");
                addPinnedRayPieceMoves(x, y, BISHOP_RAY[rayInd][0], BISHOP_RAY[rayInd][1], moves);
            }
        } else if (rookPinnedCell.containsKey(pos)) {
            byte rayInd = rookPinnedCell.get(pos);
            if ((board[y][x] & HexCell.ROOK) != 0) { // movable through the pinning ray; i.e. rook | queen
                System.out.println();
                System.out.print("\t");
                addPinnedRayPieceMoves(x, y, ROOK_RAY[rayInd][0], ROOK_RAY[rayInd][1], moves);
            } else if ((board[y][x] & HexCell.PAWN) != 0) { // pawn is pinned
                int dir = color != 0 ? 1 : -1;
                switch (rayInd) {
                    case 1, 4:
                        if (board[y+dir][x] != 0) {
                            moves.add(coord2Byte(x, y + dir));
                            System.out.printf("(%d, %d), ", x, y + dir);
                        }
                        break;
                    case 3, 0:
                        if (board[y][x+dir] != 0) {
                            moves.add(coord2Byte(x + dir, y));
                            System.out.printf("(%d, %d), ", x + dir, y);
                        }
                        break;
                    default: // case 2, 5:
                        if (board[(y += dir)][(x += dir)] == 0) {
                            moves.add(coord2Byte(x, y));
                            System.out.printf("(%d, %d), ", x, y);
                            if (inCamp((x += dir), (y += dir)) && board[y][x] == 0) {
                                moves.add(coord2Byte(x, y));
                                System.out.printf("(%d, %d), ", x, y);
                            }
                        } break;
                }
            }
        } else { // not pinned; still en passant exception must be considered
            switch (board[y][x] & 0x3e) {
                case HexCell.PAWN ->
                        addPawnMoves(x, y, moves);
                case HexCell.KNIGHT ->
                        addPieceMoves(x, y, moves, KNIGHT_JUMP, false);
                default -> {
                    if ((board[y][x] & HexCell.ROOK) != 0)
                        addPieceMoves(x, y, moves, ROOK_RAY, true);
                    if ((board[y][x] & HexCell.BISHOP) != 0)
                        addPieceMoves(x, y, moves, BISHOP_RAY, true);
                }
            }
        }
    }

    private void addPinnedRayPieceMoves(int x, int y, byte dx, byte dy, HashSet<Byte> moves) {
        System.out.printf("adding Pinned Ray Piece Moves : (%d, %d) -> ", x, y);
        int xv = x + dx, yv = y + dy;
        while (board[yv][xv] == 0) { // moves away from the king
            moves.add(coord2Byte(xv, yv));
            System.out.printf("(%d, %d), ", xv, yv);
            xv += dx;
            yv += dy;
        }
        if (board[yv][xv] > 0 && (board[yv][xv] & HexCell.BLACK) != color) {
            moves.add(coord2Byte(xv, yv)); // capturing move
            System.out.printf("(%d, %d), ", xv, yv);
        }
        xv = x - dx;
        yv = y - dy;
        while (board[yv][xv] == 0) { // moves towards the king
            moves.add(coord2Byte(xv, yv));
            System.out.printf("(%d, %d), ", xv, yv);
            xv -= dx;
            yv -= dy;
        }
        System.out.println();
    }

    private void addPawnMoves(int x, int y, HashSet<Byte> moves) {
        System.out.printf("adding Pawn Moves : (%d, %d) -> ", x, y);
        int dir = color != 0 ? 1 : -1;
        int xv = x+dir, yv = y+dir;
        if (inRange(xv, yv) && board[yv][xv] == 0) {
            moves.add(coord2Byte(xv, yv));
            System.out.printf("(%d, %d), ", xv, yv);
            if (inCamp(x, y)) {
                xv += dir; yv += dir;
                if (board[yv][xv] == 0) {
                    moves.add(coord2Byte(xv, yv));
                    System.out.printf("(%d, %d), ", xv, yv);
                }
            }
        }
        xv = x; yv = y+dir;
        if (inRange(xv, yv) && board[yv][xv] != 0
                && (board[yv][xv] & HexCell.BLACK) != color) {
            moves.add(coord2Byte(xv, yv));
            System.out.printf("(%d, %d), ", xv, yv);
        }
        xv = x+dir; yv = y;
        if (inRange(xv, yv) && board[yv][xv] != 0
                && (board[yv][xv] & HexCell.BLACK) != color) {
            moves.add(coord2Byte(xv, yv));
            System.out.printf("(%d, %d), ", xv, yv);
        }
        System.out.println();
    }

    private void addPieceMoves(int x, int y, HashSet<Byte> moves, byte[][] vector, boolean forRay) {
        System.out.printf("adding Piece Moves : (%d, %d) -> ", x, y);
        if (forRay) { // for ray
            for (byte[] ray : vector) {
                int xv = x + ray[0], yv = y + ray[1];
                while (inRange(xv, yv)) {
                    if (board[yv][xv] != 0) {
                        if ((board[yv][xv] & HexCell.BLACK) != color) {
                            moves.add(coord2Byte(xv, yv));
                            System.out.printf("(%d, %d), ", xv, yv);
                        }
                        break;
                    }
                    moves.add(coord2Byte(xv, yv));
                    System.out.printf("(%d, %d), ", xv, yv);
                    xv += ray[0];
                    yv += ray[1];
                }
            }
        } else { // for jump
            for (byte[] jump : vector) {
                int xv = x + jump[0], yv = y + jump[1];
                if (inRange(xv, yv)
                        && (board[yv][xv] == 0 || (board[yv][xv] & HexCell.BLACK) != color)) {
                    moves.add(coord2Byte(xv, yv));
                    System.out.printf("(%d, %d), ", xv, yv);
                }
            }
        }
        System.out.println();
    }

    private void addEnPassant() {
        System.out.print("adding En Passant Moves : ");
        int epx = board[9][1] & 0xf, epy = board[9][0] & 0xf;
        if (!inRange(epx, epy)) return; // previous move was not a pawn's two-cell maneuver
        int dir = color != 0 ? 1 : -1;
        byte rookBit = (byte) (HexCell.ROOK | color^1), kingBit = (byte) (HexCell.KING | color);
        int xv = epx - dir, yv = epy;
        if (board[yv][xv] == (HexCell.PAWN | color)) { // inRange() check not needed
            boolean epPinned = false;
            byte otherPiece = 0;
            yv = epy + dir + dir;
            while (inRange(xv, yv)) {
                if (board[yv][xv] == 0)
                    yv += dir;
                else {
                    if ((board[yv][xv] & rookBit) == rookBit)
                        otherPiece = kingBit;
                    else if ((board[yv][xv] & kingBit) == kingBit)
                        otherPiece = rookBit;
                    break;
                }
            }
            if (otherPiece != 0) {
                yv = epy - dir;
                while (inRange(xv, yv)) {
                    if (board[yv][xv] == 0)
                        yv -= dir;
                    else {
                        epPinned = board[yv][xv] == otherPiece;
                        break;
                    }
                }
            }
            if (!epPinned) {
                moveMap.get(coord2Byte(epx - dir, epy)).add(coord2Byte(epx, epy));
                System.out.printf("(%d, %d) -> (%d, %d), ", epx - dir, epy, epx, epy);
                board[10][2] = E | 1;
            }
        }
        xv = epx; yv = epy - dir;
        if (board[yv][xv] == (HexCell.PAWN | color)) { // inRange() check not needed
            boolean epPinned = false;
            byte otherPiece = 0;
            xv = epx + dir + dir;
            while (inRange(xv, yv)) {
                if (board[yv][xv] == 0)
                    xv += dir;
                else {
                    if ((board[yv][xv] & rookBit) == rookBit)
                        otherPiece = kingBit;
                    else if ((board[yv][xv] & kingBit) == kingBit)
                        otherPiece = rookBit;
                    break;
                }
            }
            if (otherPiece != 0) {
                xv = epx - dir;
                while (inRange(xv, yv)) {
                    if (board[yv][xv] == 0)
                        xv -= dir;
                    else {
                        epPinned = board[yv][xv] == otherPiece;
                        break;
                    }
                }
            }
            if (!epPinned) {
                moveMap.get(coord2Byte(epx, epy - dir)).add(coord2Byte(epx, epy));
                System.out.printf("(%d, %d) -> (%d, %d), ", epx, epy - dir, epx, epy);
                board[10][2] = E | 1;
            }
        }
        System.out.println();
    }

    private boolean freeFromPin(byte pos) {
        return !rookPinnedCell.containsKey(pos) && !bishopPinnedCell.containsKey(pos);
    }

    // ------------------ actual move func ------------------

    public int moveResult(byte fromPos, byte toPos) { // for any normal moves
        return moveResult(fromPos, toPos, '\0');
    }

    public int moveResult(byte fromPos, byte toPos, char promotingPieceChar) { // for promoting moves
        if (!moveMap.containsKey(fromPos) || !moveMap.get(fromPos).contains(toPos)) {
            System.out.printf("Illegal move! %s -> %s\n", byte2CoordString(fromPos), byte2CoordString(toPos));
            return -2;
        }
        int fromX = fromPos & 0xf, fromY = fromPos >> 4 & 0xf;
        int toX = toPos & 0xf, toY = toPos >> 4 & 0xf;

        byte capturedPiece = board[toY][toX];
        boolean irreversible = capturedPiece != 0;

        byte movingPiece = board[fromY][fromX];
        flipCellAttackInfo(fromX, fromY, movingPiece, color != 0); // remove
        if (irreversible) // i.e. captured
            flipCellAttackInfo(toX, toY, capturedPiece, color == 0); // remove
        board[fromY][fromX] = 0;
        changeAttackingRayEndpoint(fromX, fromY);

        byte epPos = -1;
        int d = color != 0 ? 1 : -1;
        byte promotingPieceKind = 0;
        if ((movingPiece & HexCell.PAWN) != 0) {
            irreversible = true;
            if (toY == (board[9][0] & 0xf) && toX == (board[9][1] & 0xf)) { // en passant
                epPos = coord2Byte(toX-d, toY-d);
                flipPawnAttackInfo(toX-d, toY-d, color == 0);
                changeAttackingRayEndpoint(toX-d, toY-d);
            } else if (Math.abs(toX - fromX) == 2) {
                board[9][0] = (byte) (E | toY-d);
                board[9][1] = (byte) (E | toX-d);
            } else {
                board[9][0] = board[9][1] = -1;
                if (onEndLine(toX, toY)) { // promotion
                    assert promotingPieceChar != '\0';
                    promotingPieceKind = (byte) (switch (promotingPieceChar) {
                        case 'r' -> HexCell.ROOK;
                        case 'b' -> HexCell.BISHOP;
                        case 'n' -> HexCell.KNIGHT;
                        default /*case 'q'*/ -> HexCell.QUEEN;
                    } | color);
                }
            }
        } else {
            board[9][0] = board[9][1] = -1;
            if ((movingPiece & HexCell.KING) != 0)
                kingCell[color] = toPos;
        }

        board[toY][toX] = promotingPieceKind != 0 ? promotingPieceKind : movingPiece;
        if (epPos != -1)
            board[toY-d][toX-d] = 0;
        flipCellAttackInfo(toX, toY,
                           promotingPieceKind != 0 ? promotingPieceKind : movingPiece,
                           color != 0); // add
        if (capturedPiece == 0) // non-capture move || en passant
            changeAttackingRayEndpoint(toX, toY);

        occCell.remove(fromPos);
        occCell.add(toPos);
        color ^= 1; // flip active color
        if (color == 0) {
            occCell = whiteOccCell;
            rookPinnedCell = whiteRookPinnedCell;
            bishopPinnedCell = whiteBishopPinnedCell;
        } else {
            occCell = blackOccCell;
            rookPinnedCell = blackRookPinnedCell;
            bishopPinnedCell = blackBishopPinnedCell;
        }
        occCell.remove(toPos);
        if (epPos != -1)
            occCell.remove(epPos);

        // board side-info manip
        board[0][ 9] = (byte) (E | fromY);
        board[0][10] = (byte) (E | fromX);
        board[1][ 9] = (byte) (E | toY);
        board[1][10] = (byte) (E | toX);
        board[2][10] = (byte) (E | capturedPiece);
        board[3][10] = (byte) (E | ((epPos != 1) ? 1 : 0));
        board[10][0] ^= HexCell.BLACK;
        if (color == 0)
            plusOneMove();
        if (irreversible)
            board[10][1] = E;
        else
            board[10][1]++;

        // check draw
        if (isDraw())
            return 2;
        moveMap = legalMoveMap();
        moveList.add(new Move(board));
        // check win/lose
        return mateOrContinueFlag();
    }

    private void changeAttackingRayEndpoint(int x, int y) {
        for (int clr = 0; clr <= 1; clr++) {
            changeAttackingRayEndpoint(x, y, clr, BISHOP_RAY, BISHOP_RAY_BIT);
            changeAttackingRayEndpoint(x, y, clr, ROOK_RAY, ROOK_RAY_BIT);
        }
    }

    private void changeAttackingRayEndpoint(int x, int y, int clr, byte[][] vector, long[] bitArr) {
        for (int i = 0; i < vector.length; i++) {
            if ((attackingCell[clr][y][x] & bitArr[i]) == 0)
                continue;
            byte dx = vector[i][0], dy = vector[i][1];
            int xv = x + dx, yv = y + dy;
            while (inRange(xv, yv)) {
                attackingCell[clr][yv][xv] ^= bitArr[i];
                if (board[yv][xv] != 0)
                    break;
                xv += dx; yv += dy;
            }
        }
    }

    // ------------------ result func ------------------

    private boolean isDraw() {
        // 50-move rule
        if ((board[10][1] & 0x7f) >= 100)
            return true;

        { // 3-fold repetition
            int foldCnt = 1;
            for (int i = moveList.size() - 1; i >= 0; i--) {
                if (boardRightsEqual(moveList.get(i).board))
                    foldCnt++;
                if (moveList.get(i).board[10][1] == 0)
                    break;
            }
            if (foldCnt >= 3)
                return true;
        }
        { // add piece deficiency condition
            int wc = whiteOccCell.size();
            int bc = blackOccCell.size();
            if (wc == 1) {
                if (bc == 1) return true;
                else if (bc > 2) return false;
                for (byte pos : blackOccCell) {
                    if (pos == kingCell[1]) continue;
                    byte piece = board[pos >> 4 & 0xf][pos & 0xf];
                    if (piece == BB || piece == BN) return true;
                }
            } else if (bc == 1) {
                if (wc > 2) return false;
                for (byte pos : whiteOccCell) {
                    if (pos == kingCell[0]) continue;
                    byte piece = board[pos >> 4 & 0xf][pos & 0xf];
                    if (piece == WB || piece == WN) return true;
                }
            }
        }
        return false;
    }

    private boolean boardRightsEqual(byte[][] histBoard) {
        if (board[10][0] != histBoard[10][0]) // active color is different
            return false;
        for (int y = 0; y < board.length; y++)
            for (int x = Math.max(0, y-5); x <= Math.min(10, 5+y); x++)
                if (board[y][x] != histBoard[y][x])
                    return false;
        return board[10][2] == histBoard[10][2]; // e.p. availability check
    }

    private int mateOrContinueFlag() {
        // stalemate & checkmate
        // white 4-3-2-1-0 black
        for (byte pos : moveMap.keySet())
            if (!moveMap.get(pos).isEmpty())
                return -1;
        return color == 0
                ? (attackingCell[1][kingCell[0] >> 4 & 0xf][kingCell[0] & 0xf] == 0
                    ? 1 : 0)
                : (attackingCell[0][kingCell[1] >> 4 & 0xf][kingCell[1] & 0xf] == 0
                    ? 3 : 4);
    }

    // ------------------ util func ------------------

    private static boolean inRange(int x, int y) {
        return x >= 0 && x < 11
            && y >= 0 && y < 11
            && Math.abs(x-y) <= 5;
    }

    private boolean inCamp(int x, int y) {
        return color != 0
             ? x < 5 && y < 5 // black camp
             : x > 5 && y > 5;// white camp
    }

    private boolean onEndLine(int x, int y) {
        return color != 0
             ? x == 10 || y == 10
             : x == 0 || y == 0;
    }

    private static byte coord2Byte(int x, int y) {
        return (byte) (y << 4 | x & 0xf);
    }

    private void plusOneMove() {
        // board[8][0] and board[8][1] are each 1st & 2nd digit of 128-ary full-move counter
        if (++board[8][0] >= 0) {
            board[8][1]++;
            board[8][0] = E;
        }
    }

    public void prettyPrintStatus() {
        for (int clr = 0; clr < 2; clr++) {
            System.out.println((clr == 0 ? "WHITE" : "BLACK") + "'s attacking cell status :");
            for (int y = 0; y < board.length; y++) {
                for (int x = 0; x < Math.max(0, y - 5); x++)
                    System.out.print("                ");
                for (int x = Math.max(0, y - 5); x <= Math.min(10, 5 + y); x++)
                    System.out.printf("    %12x", attackingCell[clr][y][x]);
                System.out.println();
            }
        }
        System.out.println();
        moveList.get(moveList.size()-1).prettyPrint();
        System.out.println("Available moves:");
        for (byte fromPos : moveMap.keySet()) {
            System.out.print(byte2CoordString(fromPos));
            System.out.print(" -> [");
            for (byte toPos : moveMap.get(fromPos))
                System.out.print(byte2CoordString(toPos) + ", ");
            System.out.println("]");
        }
    }

    private static String byte2CoordString(byte b) {
        int x = b & 0xf, y = b >> 4 & 0xf;
        return String.format("%c%d", 'f' + x - y, 11 - Math.max(x, y));
    }
}
