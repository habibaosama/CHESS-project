#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*****************************************Global Variables**********************************************************/
char captured_player1[16]; int counter_captured_player1 = 0; //arrays to store captured pieces of each player
char captured_player2[16]; int counter_captured_player2 = 0; //variables to store the number of captured pieces

//input stores the input scanned from user
/*
checkedPlayer is a flag that can take 3 values.
It equals -1 when there is no check
It equals 0 when player 1 is in check
It equals 1 when player 2 is in check
*/
char input[4]; int checkedPlayer = -1;
//flags that denote if any of the four rooks have moved. They take 0 if they haven't and 1 if they have moved.
int wr_rook = 0, wl_rook = 0, br_rook = 0, bl_rook = 0;
//flags that denote if any of the two kings have moved. They take 0 if they haven't and 1 if they have moved.
int wk = 0, bk = 0;
/*
Two flags that can take only two values:
It equals 0 when there is no checkmate or stalemate.
It equals 1 when there is checkmate or stalemate.
*/
int isCheckmate = 0, isStalemate = 0, isDeadPosition = 0;
/*
It indicates which player should play.
It equals 0 when it is the first player's turn.
It equals 1 when it is the second player's turn.
*/
int turn = 0;
//2D array that stores the chess board. A change takes place in it after each player's turn.
//Small letters represent white pieces for player 1.
//Capital letters represent black pieces for player 2.
char board[8][8] = {
    {'R','N','B','Q','K','B','N','R'},
    {'P','P','P','P','P','P','P','P'},
    {'0','0','0','0','0','0','0','0'},
    {'0','0','0','0','0','0','0','0'},
    {'0','0','0','0','0','0','0','0'},
    {'0','0','0','0','0','0','0','0'},
    {'p','p','p','p','p','p','p','p'},
    {'r','n','b','q','k','b','n','r'}
};
//a structure to store the position of each piece in the board
typedef struct Piece{
    int x;  //It stores the row number of the piece position.
    int y;  //It stores the column number of the piece position.
} Piece;
Piece King[2];  //An array of pieces that stores the current position of the kings of both players.
/*
Variables for storing the basic data of each move:
history_board: stores the board shape after each move.
counter_history: stores the size of history_board.
n_of_moves: A number that represents the move number the players are in. It increases on undo and decreases on redo.
history_counter_captured_player1: stores the number of captured pieces from player 1 on each move.
history_counter_captured_player2: stores the number of captured pieces from player 2 on each move.
history_captured_player1: stores the captured pieces from player 1 on each move.
history_captured_player2: stores the captured pieces from player 2 on each move.
history_King: stores the position of the 2 kings on each move.
history_checkedPlayer: stores which player is in check on each move. If no check, it takes -1.
*/
int*** history_board; int counter_history = 0, n_of_moves = 0;
int* history_counter_captured_player1; char** history_captured_player1;
int* history_counter_captured_player2; char** history_captured_player2;
Piece** history_King;
int* history_checkedPlayer;
/*****************************************Some Functions Prototypes************************************************/
void player_1();
void player_2();
int process_movement(int, int, int, int, int);
int check();
void edit_history();
/*********************************************Functions Definitions************************************************/
/*
This function initializes:
1. board array with chess pieces in their initial position.
2. King array with the initial position of both kings.
3. history array.
*/
void initialize_board(){
    //allocating history_board[][][].
    history_board = (int***)malloc((counter_history+1)*sizeof(int**));
    history_board[0] = (int**)malloc(8 * sizeof(int*));
    for(int i = 0; i < 8; i++) history_board[0][i] = (int*)calloc(8, sizeof(int));
    //allocating history_counter_captured_player1[] and history_counter_captured_player2[].
    history_counter_captured_player1 = (int*)calloc(counter_history+1, sizeof(int));
    history_counter_captured_player2 = (int*)calloc(counter_history+1, sizeof(int));
    //allocating history_captured_player1[][] and history_captured_player2[][].
    history_captured_player1 = (char**)malloc((counter_history+1)*sizeof(char*));
    history_captured_player1[0] = (char*)malloc(16*sizeof(char));
    history_captured_player2 = (char**)malloc((counter_history+1)*sizeof(char*));
    history_captured_player2[0] = (char*)malloc(16*sizeof(char));
    //allocating history_King[][].
    history_King = (Piece**)malloc((counter_history+1)*sizeof(Piece*));
    history_King[0] = (Piece*)calloc(2, sizeof(Piece));
    //allocating history_checkedPlayer[].
    history_checkedPlayer = (int*)malloc((counter_history+1)*sizeof(int));
    //Initialize board with black and white squares
    for(int x = 2; x < 6; x++){
        for(int y = 0; y < 8; y++){
            if((x+y)%2 == 0) board[x][y] = '.'; //'.' is for black squares
            else board[x][y] = '-'; //'-' is for white squares
        }
    }
    //Initialize the King array with the initial positions of both kings.
    //The zeroth index represents player 1's king.
    King[0].x = 7;
    King[0].y = 4;
    //The first index represents player 2's king.
    King[1].x = 0;
    King[1].y = 4;
    edit_history();
}
//This function prints the chess board.
void print_board(){
    printf("\n");
    printf("  ");
    for(int i = 0; i < 8; i++) printf("%c ", 'A' + i); //display characters from A to H to denote columns above board.
    printf("\n");
    for (int x = 0; x < 8; x++){
        for(int y = 0; y < 8; y++){
            if(!y){
                printf("%d ",8-x); //display numbers from 1 to 8 to denote rows to the left of the board.
            }
            printf("%c ", board[x][y]); //print the current chess board.
            if(y == 7) printf("%d", 8-x); //display numbers from 1 to 8 to denote rows to the right of the board.
        }
        printf("\n");
    }
    printf("  ");
    for(int i = 0; i < 8; i++) printf("%c ", 'A' + i); //display characters from A to H to denote columns below board.
    printf("\n");
    //print the pieces that are currently taken out from both players.
    printf("Pieces taken from player 1: ");
    for(int i = 0; i < counter_captured_player1; i++) printf("%c ", captured_player1[i]);
    printf("\n");
    printf("Pieces taken from player 2: ");
    for(int i = 0; i < counter_captured_player2; i++) printf("%c ", captured_player2[i]);
    printf("\n");
}
//This function moves the chess pieces from the square with position(x1, y1) to the square with position(x2, y2)
/*
parameters:
x1: row number of the first square.
y1: column number of the first square.
x2: row number of the second square.
y2: column number of the second square.
turn: a flag indicates which player is moving. It can either be 0 or 1.
*/
int move(int x1, int y1, int x2, int y2, int turn){
    //Move the piece from (x1, y1) to (x2, y2).
    int temp = board[x2][y2];
    board[x2][y2] = board[x1][y1];
    board[x1][y1] = ((x1+y1) % 2 == 0)? '.' : '-';
    if(board[x2][y2] == 'k' || board[x2][y2] == 'K'){ //If the moved piece is a king.
        //Change the position of this king in King[] array.
        King[checkedPlayer].x = x2;
        King[checkedPlayer].y = y2;
    }
    int flag = check(turn); //Let a variable store if the moving player's king is in check after the move.
    //Return the moved piece from (x2, y2) to (x1, y1).
    board[x1][y1] = board[x2][y2];
    board[x2][y2] = temp;
    if(board[x1][y1] == 'k' || board[x1][y1] == 'K'){ //If the moved piece is a king.
        //Return the position of the king to its first position in the King[] array.
        King[checkedPlayer].x = x1;
        King[checkedPlayer].y = y1;
    }
    //If the moving player's king is in check, return 0 which means it is an illegal move.
    if(flag) return 0;
    counter_history = n_of_moves+1;
    if(board[x2][y2] != '.' && board[x2][y2] != '-'){ //If the second square is not empty (contains a piece).
        if(turn == 0) { //If the moving player is player 1.
            //put the piece in the array of captured pieces of player 2.
            captured_player2[counter_captured_player2++] = board[x2][y2];
        }
        else{ //If the moving player is player 2.
            //put the piece in the array of captured pieces of player 1.
            captured_player1[counter_captured_player1++] = board[x2][y2];
        }
    }
    //If a player tries to move any of the four rooks, switch its flag to 1.
    if(x1 == 0 && y1 == 0) bl_rook = 1;
    if(x1 == 0 && y1 == 7) br_rook = 1;
    if(x1 == 7 && y1 == 0) wl_rook = 1;
    if(x1 == 7 && y1 == 7) wr_rook = 1;
    //Move the piece from (x1, y1) to (x2, y2)
    board[x2][y2] = board[x1][y1];
    board[x1][y1] = ((x1+y1) % 2 == 0)? '.' : '-';
    if(board[x2][y2] == 'k' || board[x2][y2] == 'K'){ //If the moved piece is a king.
        //Switch the flag of this king to 1.
        if(board[x2][y2] == 'k') wk = 1;
        if(board[x2][y2] == 'K') bk = 1;
        //change the position of this king in the King[] array.
        King[turn].x = x2;
        King[turn].y = y2;
    }
    n_of_moves++;
    return 1; //the piece is moved successfully.
}
/***********************The following are functions definitions for chess pieces*************************************/
/*
parameters of these functions:
x1: the row number of the first square.
y1: the column number of the first square.
x2: the row number of the second square.
y2: the column number of the second square.
turn: a flag indicates which player is moving. It can either be 0 or 1.
Return of these functions:
0: If the movement is illegal.
1: If the movement is legal.
*/
//This function checks if the movement is valid for a bishop.
//A bishop moves diagonally and can't jump over pieces.
int bishop(int x1, int y1, int x2, int y2, int turn){
    if((abs(x1 - x2) == abs(y1 - y2)) && x1 != x2){ //if the move is diagonal
        //each loop checks that there are no pieces between the first and the second squares.
        if(x1 > x2 && y1 > y2){
            for(int i = x1-1, j = y1-1; i > x2 && j > y2; j--, i--){
                if(board[i][j] != '.' && board[i][j] != '-') return 0;
            }
        }
        else if(x1 > x2 && y1 < y2){
            for(int i = x1-1, j = y1+1; i > x2 && j < y2; i--, j++){
                if(board[i][j] != '.' && board[i][j] != '-') return 0;
            }
        }
        else if(x1 < x2 && y1 > y2){
            for(int i = x1+1, j = y1-1; i < x2 && j > y2; i++, j--){
                if(board[i][j] != '.' && board[i][j] != '-') return 0;
            }
        }
        else{
            for(int i = x1+1, j = y1+1; i < x2 && j < y2; i++, j++){
                if(board[i][j] != '.' && board[i][j] != '-') return 0;
            }
        }
        if(turn == 0){
            if(board[x2][y2] > 'a' && board[x2][y2] < 'z') return 0;
        }
        else{
            if(board[x2][y2] > 'A' && board[x2][y2] < 'Z') return 0;
        }
        return 1; //A legal move
    }
    else return 0; //An illegal move.
}
//This function checks if the movement is valid for a king.
int king(int x1, int y1, int x2, int y2, int turn){
    if(x2 < 0 || x2 > 7 || y2 < 0 || y2 > 7) return 0; //If one of the squares is out of the board.
    //If the movement is diagonally, forward, backward, left or right.
    if((abs(x1 - x2) == 1 && y1 == y2) || (abs(y1 - y2) == 1 && x1 == x2) || (abs(x1 - x2) == 1 && abs(y1 - y2) == 1)){
        if(turn == 0){ //if the moving player is player 1.
            //If the second square contains a white piece, it is an illegal movement.
            if(board[x2][y2] > 'a' && board[x2][y2] < 'z') return 0;
        }
        else{ //If the moving player is player 2.
            //If the second square contains a black piece, it is an illegal movement.
            if(board[x2][y2] > 'A' && board[x2][y2] < 'Z') return 0;
        }
        //It is a valid movement.
        return 1;
    }
    //Else it is an illegal movement.
    else return 0;
}
//This function checks if the movement is valid for a knight.
//A knight moves in L shape and can jump over pieces.
int knight(int x1, int y1, int x2, int y2, int turn){
    if((abs(y2-y1)==2&&abs(x2-x1)==1)||(abs(y2-y1)==1&&abs(x2-x1)==2)){ //if the move is in L shape.
        if(turn == 0){
            if(board[x2][y2] > 'a' && board[x2][y2] < 'z') return 0;
        }
        else{
           if(board[x2][y2] > 'A' && board[x2][y2] < 'Z') return 0;
        }
        return 1; //A legal move.
    }
    else return 0; //An illegal move.
}
//This function checks if the movement is valid for a pawn.
int pawn(int x1, int y1, int x2, int y2, int turn){
    char x, z;
    if(turn == 0){ //If the moving player is player 1.
        if((x2 == x1 - 1) && (y2 == y1 - 1 || y2 == y1 + 1)){ //if the movement is one square diagonally
            //If the second square is empty or contains a white piece, it is an illegal movement.
            if(board[x2][y2] == '.' || board[x2][y2] == '-') return 0;
            else if(board[x2][y2] > 'a' && board[x2][y2] < 'z') return 0;
        }
        else{
            if(y1 != y2) return 0; //If the y-position is varying, it is an illegal movement.
            //If the second square is not empty, it is an illegal movement.
            if(board[x2][y2] != '.' && board[x2][y2] != '-') return 0;
            //If the pawn moves one square forward, it is a legal movement.
            if(x1 - x2 == 1) return 1;
            //If the pawn moves two squares forward and the first square forward is empty, it is a legal movement.
            if(x1 == 6 && x1 - x2 == 2 && (board[x1-1][y1] == '.' || board[x1-1][y1] == '-')) return 1;
            return 0; //Else it is an illegal movement.
        }
    }
    else if(turn == 1){ //If the moving player is player 2.
        //If the square moves one square diagonally
        if((x2 == x1 + 1 && y2 == y1 - 1) || (x2 == x1 + 1 && y2 == y1 + 1)){
            //If the second square is empty or contains a black piece, it is an illegal movement.
            if(board[x2][y2] == '.' || board[x2][y2] == '-') return 0;
            else if(board[x2][y2] > 'A' && board[x2][y2] < 'Z') return 0;
        }
        else{
            if(y1 != y2) return 0; //If the y-position is varying, it is an illegal movement.
            //If the second square is no empty, it is an illegal movement.
            if(board[x2][y2] != '.' && board[x2][y2] != '-') return 0;
            //If the pawn moves one square forward, it is a legal movement.
            if(x2 - x1 == 1) return 1;
            //If the pawn moves two squares forward and the first square forward is empty, it is a legal movement.
            if(x1 == 1 && x2 - x1 == 2 && (board[x1+1][y1] == '.' || board[x1+1][y1] == '-')) return 1;
            return 0; //Else it is an illegal movement.
        }
    }

    return 1; //Else it is a legal movement.
}
//This function checks if the movement is valid for a rook.
int rook(int x1, int y1, int x2, int y2, int turn){
    //If both the x-position and the y-position are varying or constant, it is an illegal movement.
    if((x1 != x2 && y1 != y2) || (x1 == x2 && y1 == y2)) return 0;
    //If the second square contains a white piece and the moving player is player 1.
    //OR if the second square contains a black piece and the moving player is player 2.
    //This movement is illegal.
    if((board[x2][y2] > 'a' && board[x2][y2] < 'z' && turn == 0) || (board[x2][y2] > 'A' && board[x2][y2] < 'Z' && turn == 1))
       return 0;
    //If the rook moves vertically
    if(x1 == x2){
        int a, b;
        if(y1 < y2){
            a = y1;
            b = y2;
        }
        else{
            a = y2;
            b = y1;
        }
        //This loop checks if all the squares between the first square and the second square are empty.
        for(int i = a+1; i < b; i++){
            if(board[x1][i] != '.' && board[x1][i] != '-'){
                return 0; //If one square is not empty, it is an illegal movement.
            }
        }
    }
    else{ //If the rook moves horizontally
        int a, b;
        if(x1 < x2){
            a = x1;
            b = x2;
        }
        else{
            a = x2;
            b = x1;
        }
        //This loop checks if all the squares between the first and the second square are empty.
        for(int i = a+1; i < b; i++){
            //If one square is not empty, it is an illegal movement.
            if(board[i][y1] != '.' && board[i][y1] != '-') return 0;
        }
    }
    return 1; //else it is a legal movement.
}
//This function checks if the movement is valid for a queen.
//A queen can move like a rook and a bishop, so we call both functions.
int queen(int x1, int y1, int x2, int y2, int turn){
    if(x1 == x2 || y1 == y2){ //If it is a rook movement.
        return rook(x1, y1, x2, y2, turn);
   }
    else if(abs(x2 - x1) == abs(y2 - y1)){ //if it is a bishop movement.
        return bishop(x1, y1, x2, y2, turn);
   }
   else return 0;
}
//This function calls the suitable chess piece function according to the piece the player is trying to move.
/*
Parameters:
x1: row number of the first square.
y1: column number of the first square.
x2: row number of the second square.
y2: column number of the second square.
turn: a flag indicates which player is moving. It can either be 0 or 1.
Return values:
It returns 0 if the movement is illegal.
It returns 1 if the movement is legal.
*/
int process_movement(int x1, int y1, int x2, int y2, int turn){
    int c; //A flag that can either take 0 if the movement is illegal or 1 if the movement is legal.
    if(turn == 0){ //If the moving player is player 1
        switch(board[x1][y1]){ //The chess piece the player is trying to move.
        case 'r': //If it is a white rook.
            c = rook(x1, y1, x2, y2, turn);
            break;
        case 'p': //If it is a white pawn.
            c = pawn(x1, y1, x2, y2, turn);
            break;
        case 'n': //If it is a white knight.
            c = knight(x1, y1, x2, y2, turn);
            break;
        case 'k': //If it is a white king.
            c = king(x1, y1, x2, y2, turn);
            break;
        case 'q': //If it is a white queen.
            c = queen(x1, y1, x2, y2, turn);
            break;
        case 'b': //If it is a white bishop.
            c = bishop(x1, y1, x2, y2, turn);
            break;
        default: //Else, it is an illegal movement.
            c = 0;
        }
    }
    else if(turn == 1){ //If the moving player is player 2.
        switch(board[x1][y1]){ //The chess piece the player is trying to move.
        case 'R': //If the piece is a black rook.
            c = rook(x1, y1, x2, y2, turn);
            break;
        case 'P': //If the piece is a black pawn.
            c = pawn(x1, y1, x2, y2, turn);
            break;
        case 'N': //If the piece is a black knight.
            c = knight(x1, y1, x2, y2, turn);
            break;
        case 'K': //If the piece is a black king.
            c = king(x1, y1, x2, y2, turn);
            break;
        case 'Q': //If the piece is a black queen.
            c = queen(x1, y1, x2, y2, turn);
            break;
        case 'B': //If the piece is a black bishop.
            c = bishop(x1, y1, x2, y2, turn);
            break;
        default: //Else, the movement is illegal.
            c = 0;
        }
    }
    return c; //Return if the movement is valid or not.
}
//promotion is for the pawn when it reaches the last row of opponent's side.
/*
Parameters:
x: the row number of the square the pawn moved to.
y: the column number of the square the pawn moved to.
turn: A flag indicates which player is moving. It can either be 0 or 1.
Return values:
It returns 0 if the Piece the player chose to promote is invalid
It returns 1 if the conditions of promotion are not satisfied or the player chose a valid piece to promote.
*/
int promotion(int x, int y, int turn){
    char z;
    if(turn == 0 && x == 0){ //for white when a white pawn reaches the first row(x = 0).
        //exchange the pawn by a chosen piece.
        printf("choose the promotion('q','n','b',r'): ");
        scanf(" %c",&z);
        switch(z){
            case'q':
                board[x][y]='q';
                break;
            case'n':
                board[x][y]='n';
                break;
            case'b':
                board[x][y]='b';
                break;
            case'r':
                 board[x][y]='r';
                 break;
            default:
                printf("enter valid piece\n");
                return 0; //Invalid Piece.
            }
    }
    else if(turn == 1 && x == 7){ //for black when a black pawn reaches the last row(x = 7).
        //exchange the pawn by a chosen piece.
        printf("choose the promotion('Q','N','B',R'): ");
        scanf(" %c",&z);
        switch(z){
            case'Q':
                board[x][y]='Q';
                break;
            case'N':
                board[x][y]='N';
                break;
            case'B':
                board[x][y]='B';
                break;
            case'R':
                board[x][y]='R';
                break;
            default:
                printf("enter valid piece\n"); //invalid piece.
                return 0;
            }
    }
    return 1;
}
//This function checks if the move of one player results in checking the king of the other player (checked player).
/*
Parameters:
checked_player: The player in check.
Return values:
It returns 0 if the player's king is not in check.
It returns 1 if the player's king is in check.
*/
int check(int checked_player){
    //Loop on all the board squares.
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            //If any chess piece can capture the king
            if(process_movement(i, j, King[checked_player].x, King[checked_player].y, 1-checked_player)){
                return 1; //The player is in check.
            }
        }
    }
    return 0; //else, the player is not in check.
}
//This function checks if the move of one player results in checkmating the king of the other player.
/*
Parameters:
checked_player: The player in check.
Return values:
It returns 0 if the player's king checkmated.
It returns 1 if the player's king is not checkmated.
*/
int checkmate(int checked_player){
    int flag = 1;
    //Loop on all chess board squares.
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            //If the checkedPlayer is player 1 and the piece is white
            //OR the checkedPlayer is player 2 and the piece is black
            if((checked_player == 0&&board[i][j] > 'a'&&board[i][j] < 'z')||(checked_player == 1&&board[i][j] > 'A'&&board[i][j] < 'Z')){
                //Try to move this piece to every square.
                for(int x = 0; x < 8; x++){
                    for(int y = 0; y < 8; y++){
                        if(process_movement(i, j, x, y, checked_player)){ //If the move is valid
                            //Move this Piece.
                            int temp = board[x][y];
                            board[x][y] = board[i][j];
                            board[i][j] = ((i+j) % 2 == 0)? '.' : '-';
                            if(board[x][y] == 'K' || board[x][y] == 'k'){
                                King[checked_player].x = x;
                                King[checked_player].y = y;
                            }
                            flag = check(checked_player); //Check if the player's king is in check and store the result in flag.
                            //Return the moved piece to its position.
                            if(board[x][y] == 'K' || board[x][y] == 'k'){
                                King[checked_player].x = i;
                                King[checked_player].y = j;
                            }
                            board[i][j] = board[x][y];
                            board[x][y] = temp;
                            if(!flag){ //If the king is not in check.
                                //printf("%d%d %d%d", i, j, x, y);
                                return flag; //the player's king is not checkmated.
                            }
                        }
                    }
                }
            }
        }
    }
    //If Each move results in checking the king, the player's king is checkmated.
    return flag;
}
//This function checks the first case of dead position: If no remaining pieces except for the 2 kings.
/*
Return Values:
It returns 0 if the case is not satisfied.
It returns 1 is the case is satisfied
*/
int KingVsKing(){
    //Loop on all board squares
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            //If any square is not empty and contains a piece that is not a king.
            if(board[i][j] != '.' && board[i][j] != '-' && board[i][j] != 'k' && board[i][j] != 'K'){
                return 0; //The case is not satisfied.
            }
        }
    }
    return 1; //If all squares are either empty or contains a king, the case is satisfied.
}
//This function checks the second case of dead position: If no remaining pieces except for the 2 kings and a bishop
//OR no remaining pieces except for the 2 kings and 2 bishops of the same color.
/*
Return Values:
It returns 0 if the case is not satisfied.
It returns 1 if the case is satisfied.
*/
int KingAndBishopCases(){
    int number_bishops = 0; char color[2];
    //Loop on all board squares.
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            //If the square is not empty.
            if(board[i][j] != '.' && board[i][j] != '-'){
                //If any square contains a piece that is not a king or a bishop, the case is not satisfied.
                if(board[i][j] != 'k' && board[i][j] != 'b' && board[i][j] != 'K' && board[i][j] != 'B') return 0;
                else if(board[i][j] == 'b' || board[i][j] == 'B') { //If the square contains a bishop.
                    //If the number of bishops is two or more, the case is not satisfied.
                    if(number_bishops > 1) return 0;
                    //store the color of the bishop.
                    color[number_bishops++] = ((i+j) % 2 == 0)? '.' : '-';
                }
            }
        }
    }
    //If number of bishops is 1 or number of bishops is 2 of the same color, the case is satisfied.
    //Else the case is not satisfied.
    if(number_bishops == 1) return 1;
    else if(number_bishops == 2 && color[0] == color[1]) return 1;
    else return 0;
}
//This function checks the third case of dead position: no remaining pieces except for the 2 kings and a knight.
int KingAndKnightVsKing(){
    int number_knights = 0;
    //Loop on all board squares.
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            //If the square is not empty.
            if(board[i][j] != '.' && board[i][j] != '-'){
                //If the square contains a piece that is not a king or a knight, the case is not satisfied.
                if(board[i][j] != 'k' && board[i][j] != 'n' && board[i][j] != 'K' && board[i][j] != 'N') return 0;
                //If the square contains a knight, increment the number_knights variable.
                else if(board[i][j] == 'n' || board[i][j] == 'N') number_knights++;
            }
        }
    }
    if(number_knights == 1) return 1; //If the number of knights is 1, the case is satisfied.
    else return 0; //Else, the case is not satisfied.
}
//This function checks if the game became in a dead position or not.
/*
Return Values:
It returns 0 if the game is not in dead position.
It returns 1 if the game is in dead position.
*/
int deadposition(){
    //If any dead position case is satisfied, the game is in dead position.
    if(KingVsKing() || KingAndKnightVsKing() || KingAndBishopCases()) return 1;
    else return 0; //Else, the game is not in dead position.
}
//This function saves the game when called.
/*
Parameters:
turn: A flag indicates which player is playing. It can either be 0 or 1.
*/
void save(int turn){
    char folder_name[200],z;
    printf("The file name:\n");
    scanf("%c ", &z);
    gets(folder_name);
    FILE* fp;
    //create a new file.
    fp = fopen(folder_name,"w");
    if(fp == NULL){
        printf("error in saving file");
        return;
    }
    //saving board to file as putc writes a character to file.
     for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            putc(board[i][j], fp);
        }
     }
     //saving the counters of the two arrays of captured pieces.
     putw(counter_captured_player1, fp);
     putw(counter_captured_player2, fp);
     //saving the two arrays of captured pieces.
    for(int i = 0; i < 16; i++){
        putc(captured_player1[i], fp);
    }
    for (int i = 0; i < 16; i++){
        putc(captured_player2[i], fp);
    }
    //saving the player whose turn to play.
    putw(turn, fp);
    //saving the checkedPlayer.
    putw(checkedPlayer, fp);
    //close opened file.
    fclose(fp);
    printf("File is saved successfully!\n");
}
//This function loads a previously saved game when called.
/*
Return Values: It returns the player whose turn to play.
It returns 0 when it is player 1's turn to play.
It returns 1 when it is player 2's turn to play.
*/
int load(){
    char folder_name[200];
    FILE* fp = NULL;
    while(fp == NULL){
        printf("Enter file name:\n");
        scanf("\n");
        gets(folder_name);
        //open an existing file.
        fp = fopen(folder_name,"r");
        if(fp == NULL) printf("Not Found\n");
    }
    //reading the board as getc reads character from a file.
    for(int i=0;i<8;i++){
        for(int j=0;j<8;j++){
            char h = getc(fp);
            board[i][j] = h;
        }
    }
    //reading the counters of the 2 arrays of captured pieces.
    counter_captured_player1 = getw(fp);
    counter_captured_player2 = getw(fp);
    //reading the 2 arrays of captured pieces.
    for(int i=0;i<16;i++){
        char n = getc(fp);
        captured_player1[i] = n;
    }
    for(int i=0;i<16;i++){
        char r = getc(fp);
        captured_player2[i] = r;
    }
    counter_history = 0;
    edit_history();
    //read the player whose turn to play and close the opened file.
    int t = getw(fp);
    //reading the checkedPlayer.
    checkedPlayer = getw(fp);
    //returning the player whose turn to play and close the opened file.
    return t;

}
//This function undoes players' movements if there are previous moves.
void undo(){
    if(n_of_moves == 0){ //If there are no previous moves.
        printf("No previous movements!");
        return;
    }
    turn = 1-turn; //exchange turns on each undo.
    //Restoring the previous board from history.
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            board[i][j] = history_board[n_of_moves-1][i][j];
        }
    }
    //Restoring the previous captured pieces arrays and their counters from history.
    counter_captured_player1 = history_counter_captured_player1[n_of_moves-1];
    counter_captured_player2 = history_counter_captured_player2[n_of_moves-1];
    for(int i = 0; i < counter_captured_player1; i++){
        captured_player1[i] = history_captured_player1[n_of_moves-1][i];
    }
    for(int i = 0; i < counter_captured_player2; i++){
        captured_player2[i] = history_captured_player2[n_of_moves-1][i];
    }
    //Restoring the previous kings positions from history.
    King[0] = history_King[n_of_moves-1][0];
    King[1] = history_King[n_of_moves-1][1];
    //Restoring the player whose king is in check from history.
    checkedPlayer = history_checkedPlayer[n_of_moves-1];
    //Decrement the number of moves.
    n_of_moves--;
}
//This function redoes players' movements if there are following moves.
void redo(){
    if(n_of_moves+1 == counter_history){ //If there is no following moves.
        printf("No following moves !");
        return;
    }
    turn = 1-turn; //exchange turns on each redo.
    //Restore the next board from history.
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            board[i][j] = history_board[n_of_moves+1][i][j];
        }
    }
    //Restore the next captured pieces arrays and their counters from history.
    counter_captured_player1 = history_counter_captured_player1[n_of_moves+1];
    counter_captured_player2 = history_counter_captured_player2[n_of_moves+1];
    for(int i = 0; i < counter_captured_player1; i++){
        captured_player1[i] = history_captured_player1[n_of_moves+1][i];
    }
    for(int i = 0; i < counter_captured_player2; i++){
        captured_player2[i] = history_captured_player2[n_of_moves+1][i];
    }
    //Restore the next kings positions from history.
    King[0] = history_King[n_of_moves+1][0];
    King[1] = history_King[n_of_moves+1][1];
    //Restore the next player whose king is in check from history.
    checkedPlayer = history_checkedPlayer[n_of_moves+1];
    //Increment the number of moves.
    n_of_moves++;
}
/*
This function does a special move: The king and the rook can move at the same time by conditions.
Parameters:
x1: row number of the first square.
y1: column number of the first square.
x2: row number of the second square.
y2: column number of the second square.
Return Value:
It returns 0 when the conditions of castling are not satisfied.
It returns 1 when the conditions of castling are satisfied.
*/
int castling(int x1, int y1, int x2, int y2){
    int flag1, flag2;
    if(checkedPlayer == -1){ //If the player is not in check.
        if(turn==0){
            //king side castling for white
            if(wr_rook==0&& wk==0&& x2==x1&& y2-y1==2){
                //checking that there are no pieces between king and the rook
                if((board[7][5]=='.'||board[7][5]=='-')&&(board[7][6]=='.'||board[7][6]=='-')){
                    King[turn].y = 5;
                    flag1 = check(turn);
                    King[turn].y = 6;
                    flag2 = check(turn);
                    King[turn].y = 4;
                    if(!(flag1 && flag2)){ //If the player is not moving to check or through check.
                        board[x2][y2]='k';
                        board[x2][y2-1]=board[7][7]; //move the rook from H1 to F1
                        board[x1][y1] = ((x1+y1) % 2 == 0)? '.' : '-';
                        board[7][7] = '.';
                        return 1;
                    }
                }
            }
            //queen side castling for white
            else if(wl_rook==0&& wk==0&& x2==x1&& y1-y2==2){
                //checking that there are no pieces between king and the rook
                if((board[7][1]=='.'||board[7][1]=='-')&&(board[7][2]=='.'||board[7][2]=='-')&& (board[7][3]=='.'||board[7][3]=='-')){
                    King[turn].y = 3;
                    flag1 = check(turn);
                    King[turn].y = 2;
                    flag2 = check(turn);
                    King[turn].y = 4;
                    if(!(flag1 && flag2)){ //If the player is not moving to check or through check.
                        board[x2][y2]='k';
                        board[x2][y2+1]=board[7][0]; //move the rook from A1 to D1
                        board[x1][y1] = ((x1+y1) % 2 == 0)? '.' : '-';
                        board[7][0] = '-';
                        return 1;
                    }
                }
            }
        }
        else{ //if turn==1 (black)
            //king side castling for black
            if(br_rook==0&& bk==0&& x2==x1&& y2-y1==2){
                //checking that there are no pieces between king and the rook
                if((board[0][5]=='.'||board[0][5]=='-')&&(board[0][6]=='.'||board[0][6]=='-')){
                    King[turn].y = 5;
                    flag1 = check(turn);
                    King[turn].y = 6;
                    flag2 = check(turn);
                    King[turn].y = 4;
                    if(!(flag1 && flag2)){ //If the player is not moving to check or through check.
                        board[x2][y2]='K';
                        board[x2][y2-1]=board[0][7]; //move the rook from H1 to F1
                        board[x1][y1] = ((x1+y1) % 2 == 0)? '.' : '-';
                        board[0][7] = '-';
                        return 1;
                    }
                }
            }
            //queen side castling for black
            else if(bl_rook==0&& bk==0&& x2==x1&& y1-y2==2){
                //checking that there are no pieces between king and the rook
                if((board[0][1]=='.'||board[0][1]=='-')&&(board[0][2]=='.'||board[0][2]=='-')&& (board[0][3]=='.'||board[0][3]=='-')){
                    King[turn].y = 3;
                    flag1 = check(turn);
                    King[turn].y = 2;
                    flag2 = check(turn);
                    King[turn].y = 4;
                    if(!(flag1 && flag2)){ //If the player is not moving to check or through check.
                        board[x2][y2]='K';
                        board[x2][y2+1]=board[0][0]; //move the rook from A1 to D1
                        board[x1][y1] = ((x1+y1) % 2 == 0)? '.' : '-';
                        board[0][0] = '.';
                        return 1;
                    }
                }
            }
        }
    }
    return 0; //castling is not possible.
}
//This function stores the basic data of each move in history.
void edit_history(){
    //Store the board in history.
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            history_board[counter_history][i][j] = board[i][j];
        }
    }
    //Increase the size of history_board[] by realloc.
    history_board = (int***)realloc(history_board, (counter_history+2)*sizeof(int**));
    history_board[counter_history+1] = (int**)malloc(8 * sizeof(int*));
    for(int i = 0; i < 8; i++) history_board[counter_history+1][i] = (int*)calloc(8, sizeof(int));
    //Store counters of captured pieces in history and increase their history sizes.
    history_counter_captured_player1[counter_history] = counter_captured_player1;
    history_counter_captured_player2[counter_history] = counter_captured_player2;
    history_counter_captured_player1 = (int*)realloc(history_counter_captured_player1, (counter_history+2)*sizeof(int));
    history_counter_captured_player2 = (int*)realloc(history_counter_captured_player2, (counter_history+2)*sizeof(int));
    //Store the captured pieces arrays in history and increase their history sizes.
    for(int i = 0; i < counter_captured_player1; i++){
        history_captured_player1[counter_history][i] = captured_player1[i];
    }
    history_captured_player1 = (char**)realloc(history_captured_player1, (counter_history+2)*sizeof(char*));
    history_captured_player1[counter_history+1] = (char*)malloc(16*sizeof(char));
    for(int i = 0; i < counter_captured_player2; i++){
        history_captured_player2[counter_history][i] = captured_player2[i];
    }
    history_captured_player2 = (char**)realloc(history_captured_player2, (counter_history+2)*sizeof(char*));
    history_captured_player2[counter_history+1] = (char*)malloc(16*sizeof(char));
    //Store the kings positions in history and increase their history sizes.
    history_King[counter_history][0] = King[0];
    history_King[counter_history][1] = King[1];
    history_King = (Piece**)realloc(history_King, (counter_history+2)*sizeof(Piece*));
    history_King[counter_history+1] = (Piece*)malloc(2*sizeof(Piece));
    //Store the checkedPlayer in history_checkedPlayer[] and increase the size of history_checkedPlayer[].
    history_checkedPlayer[counter_history] = checkedPlayer;
    history_checkedPlayer = (int*)realloc(history_checkedPlayer, (counter_history+2)*sizeof(int));
    //Increment the counter of items stored in history.
    counter_history++;
}
//This function calls the main game functions.
/*
Parameters:
x1: row number of the first square.
y1: column number of the first square.
x2: row number of the second square.
y2: column number of the second square.
turn: a flag indicates which player is moving. It can either be 0 or 1.
*/
void play(int x1, int y1, int x2, int y2, int turn){
    if(process_movement(x1, y1, x2, y2, turn)&&move(x1, y1, x2, y2, turn)){ //If the movement is a legal chess movement.
            checkedPlayer = -1;
            if(board[x2][y2] == 'p' || board[x2][y2] == 'P'){ //if the moved piece is a pawn.
                //If promotion is valid, call promotion until the player enters a valid piece to promote.
                if(!promotion(x2, y2, turn)) promotion(x2, y2, turn);
            }
            //if the moved piece checks the other player's king.
            if(process_movement(x2, y2, King[1-turn].x, King[1-turn].y, turn)){
                checkedPlayer = 1-turn; //set checkedPlayer to the player whose king is checked.
                isCheckmate = checkmate(checkedPlayer); //check if this player is checkmated or not and store the result.
            }
            //If the moved piece does not check the other player's king.
            if(!process_movement(x2, y2, King[1-turn].x, King[1-turn].y, turn)){
                //check if every move of the other player leads to check or not and store the result.
                //If yes, it is a stalemate. isStalemate = 1.
                isStalemate = checkmate(1-turn);
            }
            //check if the game became in dead position and store the result.
            isDeadPosition = deadposition();
            //Store the data of this move in history.
            edit_history();
    }
    //else if the piece the player is trying to move is a king and castling conditions are satisfied.
    else if((board[x1][y1] == 'k' || board[x1][y1] == 'K') && castling(x1, y1, x2, y2)){
        n_of_moves++;
        //change the position of this king in the King[] array.
        King[turn].x = x2;
        King[turn].y = y2;
        //If the other player's king is in check.
        if(check(1-turn)){
            checkedPlayer = 1-turn; //set checkedPlayer to the player whose king is checked.
            isCheckmate = checkmate(checkedPlayer); //check if this player is checkmated or not and store the result.
        }
        else{
            //check if every move of the other player leads to check or not and store the result.
            //If yes, it is a stalemate. isStalemate = 1.
            isStalemate = checkmate(1-turn);
        }
        isDeadPosition = deadposition(); //check if the game became in dead position and store the result.
        edit_history(); //Store the data of this move in history.
    }
    //If this movement is not a legal chess movement.
    else{
        printf("Illegal Movement\n");
        //Let this player plays another movement.
        if(turn == 0) player_1();
        else if(turn == 1) player_2();
    }
}
/*
This function is called when it is player 1's turn to move
It scans input from player 1 and processes this input.
*/
void player_1(){
    int c = 0; int x1, x2, y1, y2;
    while(!c){ //loop until player 1 enters a valid movement
        printf("[PLAYER1's turn]: ");
        scanf("%s", input); //scan input from player 1.
        if(strlen(input) == 1){ //if the input is only one character, check is it is one of those commands.
            if(input[0] == 's'){
                save(turn);
                return;
            }
            if(input[0] == 'l'){
                turn = load();
                return;
            }
            if(input[0] == 'u'){
                undo();
                return;
            }
            if(input[0] == 'r'){
                redo();
                return;
            }
        }
        //convert player 1's input from characters to valid board array indices.
        y1 = (input[0] - 'A');
        x1 = 7-(input[1] - '1');
        y2 = (input[2] - 'A');
        x2 = 7-(input[3] - '1');
        c = 1;
        //if the piece player 1 is trying to move is not a white piece.
        if(!(board[x1][y1] > 'a' && board[x1][y1] < 'z')) {
            printf("INVALID MOVEMENT!\n");
            c = 0; //Invalid Movement.
        }
    }
    //Once it is a valid movement, go out of the loop and call play() function.
    play(x1, y1, x2, y2, turn);
    turn = 1; //change the turn to be player 2's turn.
}
/*
This function is called when it is player 2's turn to move.
It scans input from player 2 and processes this input.
*/
void player_2(){
    int c = 0; int x1, x2, y1, y2;
    while(!c){ //loop until player 2 enters a valid movement.
        printf("[PLAYER2's turn]: ");
        scanf("%s", input); //scan input from player 2.
        if(strlen(input) == 1){ //if the input is only one character, check is it is one of those commands.
            if(input[0] == 's'){
                save(turn);
                return;
            }
            if(input[0] == 'l'){
                turn = load();
                return;
            }
            if(input[0] == 'u'){
                undo();
                return;
            }
            if(input[0] == 'r'){
                redo();
                return;
            }
        }
        //convert player 2's input from characters to valid board array indices.
        y1 = (input[0] - 'A');
        x1 = 7-(input[1] - '1');
        y2 = (input[2] - 'A');
        x2 = 7-(input[3] - '1');
        c = 1;
        //if the piece player 2 is trying to move is not a black piece.
        if(!(board[x1][y1] > 'A' && board[x1][y1] < 'Z')) {
            printf("INVALID MOVEMENT!\n");
            c = 0; //Invalid Movement
        }
    }
    //Once it is a valid movement, go out of the loop and call play() function.
    play(x1, y1, x2, y2, turn);
    turn = 0; //change the turn to be player 1's turn.
}
int main(){
    //Game flow.
    initialize_board();
    print_board();
    while(1){ //Loop infinitely until the game ends by either checkmate, stalemate or dead position.
        if(turn == 0){ //if it is player 1's turn.
            player_1();
        }
        else if(turn == 1){ //if it is player 2's turn.
            player_2();
        }
        print_board();
        //If a player's king is in check.
        if(checkedPlayer != -1) printf("CHECK!\n");
        if(isCheckmate){ //if one player is checkmated.
            printf("checkmate\n");
            printf("Player %d is the winner!", 2-turn); //The other player is the winner.
            break; //Break the loop as the game ends.
        }
        else if(isStalemate){ //if the game became in stalemate.
            printf("Stalemate\nDraw"); //It is Draw.
            break; //Break the loop as the game ends.
        }
        else if(isDeadPosition){ //if the game became in dead position.
            printf("DeadPosition\nDraw"); //It is Draw.
            break; //Break the loop as the game ends.
        }
    }
    return 0;
}
