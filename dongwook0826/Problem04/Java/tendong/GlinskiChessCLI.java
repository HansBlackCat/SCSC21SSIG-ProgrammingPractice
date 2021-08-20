package tendong;

import tendong.hexchess.GlinskiChess;

import java.util.Scanner;

public class GlinskiChessCLI {
    public static void main(String[] args) {
        System.out.println("   ****************************   ");
        System.out.println("  *                            *  ");
        System.out.println(" *       Glinski HexChess       * ");
        System.out.println("*                                *");
        System.out.println(" *       MADE BY. TenDong       * ");
        System.out.println("  *                            *  ");
        System.out.println("   ****************************   ");

        Scanner sc = new Scanner(System.in);
        if (choosingYes(sc, "Continue to the game? [Y/N] : ")) {
            do {
                GlinskiChess hexChessGame = new GlinskiChess();
                hexChessGame.prettyPrintStatus();
                int flag;
                do {
                    System.out.print("What is your move? : ");
                    flag = moveFlag(sc.nextLine(), hexChessGame);
                    if (flag <= -2) {
                        System.out.print("Invalid move; try again : ");
                        continue;
                    }
                    hexChessGame.prettyPrintStatus();
                } while (flag < 0);
                System.out.println(switch (flag) {
                    case 0 -> "BLACK's WIN!";
                    case 1 -> "BLACK gave a STALEMATE";
                    case 2 -> "DRAW";
                    case 3 -> "WHITE gave a STALEMATE";
                    case 4 -> "WHITE's WIN!";
                    default -> "Unexpected result";
                });
            } while (choosingYes(sc, "Replay? [Y/N] : "));
        }
    }

    private static boolean choosingYes(Scanner sc, String explanation) {
        System.out.print(explanation);
        while (true) {
            String choice = sc.nextLine();
            if (choice.isEmpty()) {
                System.out.print("No input; try again : ");
                continue;
            }
            if (choice.toUpperCase().charAt(0) == 'Y') {
                return true;
            } else if (choice.toUpperCase().charAt(0) == 'N') {
                return false;
            } else {
                System.out.print("Invalid choice; try again : ");
            }
        }
    }

    private static int moveFlag(String notation, GlinskiChess game) { // fixme add quit, draw, resign option
        if (notation.matches("^([a-k]((10)|(11)|[1-9])){2}[qrbn]?$")) {
            String[] colProSplit = notation.split("(10)|(11)|[1-9]");
            String[] rowSplit = notation.split("[a-z]");
            byte fromPos = notation2Byte(colProSplit[0], rowSplit[1]);
            byte toPos = notation2Byte(colProSplit[1], rowSplit[2]);
            return colProSplit.length <= 2
                    ? game.moveResult(fromPos, toPos)
                    : game.moveResult(fromPos, toPos, colProSplit[2].charAt(0));
        } else return -2;
    }

    private static byte notation2Byte(String col, String row) {
        int c = 'f' - col.charAt(0);
        assert -5 <= c && c <= 5;
        int r = Integer.parseInt(row);
        assert 1 <= r && r <= 11;
        if (r >= 7 && Math.abs(c) + r > 11) // invalid notation
            return -1;

        int x, y;
        if (c < 0) {
            x = 11 - r;
            y = x + c;
        } else {
            y = 11 - r;
            x = y - c;
        }
        return (byte) (y << 4 | x & 0xf);
    }
}
