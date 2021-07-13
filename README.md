# CHESS-project
 Chess game with Features : check, checkmate, promotion, Salemate, Dead position, castling, save and load, undo and redo 
 
 
 
*User Manual that describes how to deal with your application*:

-The user to input move like: A1D1 (capital letters).

- If user input wrong move or not in the right way “INVALID MOVE” will
appear and user will be able to write it again.

-To save the game when it is player’s turn input : ‘s’(small letter).

-To load the game input ‘l’ and write the file name.

-To undo the game input ‘u’ and will return one step.

-For promotion input first the move: eg A7A8 then after display message
choose promotion input the piece chosen: ‘q’

-If the king is in check “check” will be displayed.

-If checkmate occurs will display checkmate and end the game.

-If stalemate occurs will display “stalemate””draw” and end the game.

-If there is dead position will display “Dead position” “draw” and end the
game



Some Features

● PROMOTION
It is a feature especially for the pawn piece if it reaches the end of
the opponent’s side. The player will be able to exchange the pawn
with ( Queen or knight or bishop or rook).
 The way user enters it :
when player_1 enters A7A8 message of promotion
appears”choose the promotion('q','n','b',r'): “
then player_1 enter piece: q
If a player enters other than those mentioned, “enter a valid piece”
message will be printed and he will be asked to enter a piece again.

● CHECK
if any piece of a player is attacking the opponent’s king, the program
will print “CHECK!”. At this point, the checked player will only be
able to move a piece that eliminates the threat on the king. If he
moves any other piece, “Illegal Movement” message will be printed
and this player will be asked to enter another movement.● CHECKMATE
if the king is in check and there is no piece able to block the attacker
or to capture the attacker or the king could move from its place. The
program prints “checkmate” and ends the game.

● STALEMATE
If the king of the player whose turn to play is not in check and every
move of this player leads to checking his own king i.e No valid move
for the player and no check, the game is ended as “Draw by
Stalemate”.

● DEAD POSITION
If checkmate became impossible, i.e No player could checkmate the
other player by any number of legal chess movements, the game is
ended as “Draw by dead position”.
It arises when:
1. Only the two kings are remaining.
2. Only the 2 kings and one knight are remaining.
3. Only the 2 kings and a bishop are remaining.
4. Only the 2 kings and two bishops of the same colour are
remaining.

● CASTLING
This feature is special for the king and the rook where the user is
able to move the king 2 squares and the program moves the rook
directly to the other side of the king.
This occurs if the following conditions are satisfied:
1. The king is not in check and not moving to check or through
check.
2. Both the king and the rook didn’t move before.
3. there are no pieces between king and the rook.
There are two types of castling which is king side castling (when
king moves to right) and queen side castling(when king moves to
the left).

● SAVE AND LOAD
The user is able to save the game at any time and load it again
later.
How the user enters it:
when it is the player turn: if he writes ‘s’ the program will ask the
user to enter the file name in which the game will be saved.
When the game is saved, “File is saved successfully!” message will
be printed.

● UNDO
A player is able to undo movements any number of times till the first
move played.
How the user enters it:
Whenever he writes ‘u’, the last movement will be undone.
If a player writes the undo command and there are no previous
moves, A “No Previous Movements!” message will be printed.

● REDO
A player is able to redo movements any number of times till the last
move played.
How the user enters it:
Whenever he writes ‘r’, the following movement will be redone.
If a player writes the redo command and there are no following
moves, A “No Following Movements!” message will be printed
