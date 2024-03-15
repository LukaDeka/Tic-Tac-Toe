# Tic-Tac-Toe
Small Tic-Tac-Toe game with an option to play *singleplayer* against a computer,
or *two-player* against a friend.

**Type "make main" to compile**

Input/Output is handled through the terminal.

The computer algorithm is very basic, there is no searching and evaluation.
The logic works as described, step by step:

1. Since the computer always plays second, it always plays in the middle of the grid,
if the player has played in any of the corners. If this move isn't played, white can always win
with perfect play.

2. If the computer has a winning move, it will play it.

3. If the player has at least one winning move on the next turn, the computer will block at least one.

4. Otherwise, it plays a random valid move.

*Two-player* mode is self-explanatory.
