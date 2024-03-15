#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef enum Bool {
	false,
	true,
} Bool;

typedef enum Block {
	BlockX, // X written on the grid
	BlockO, // O written on the grid
	BlockE  // grid block empty
} Block;

// forces player to input a valid grid position (1-9), block must be empty
// returns array index, where player wants to play
int read_grid_position(Block *grid, Block whose_turn) {
	int input, result;
	do {
		printf("Enter the position to write %s: ",
		((whose_turn == BlockX) ? "\033[0;31mX\033[0m" : "\033[0;32mO\033[0m"));
		result = scanf("%d", &input);
		input--; // 1-9 user input format to 0-8 (array) conversion

		while (getchar() != '\n');

		if (result != 1 || (input < 0 || input > 8)) { // if input is not 1-9
			printf("\n\033[0;31mInvalid input, try again.\033[0m\n");
		} else if (grid[input] != BlockE) { // if block isn't empty
			printf("\n\033[0;31mBlock is taken, try again.\033[0m\n");
		}
  	} while (result != 1 || (input < 0 || input > 8) || grid[input] != BlockE);

	return input;
}

// forces player to answer correctly (y/Y/n/N)
// returns true, if player wants to play against computer
Bool read_playing_vs_computer() {
  	char choice;

  	do {
		printf("\nWould you like to play against the program? \033[0;32m(y/n)\033[0m: ");
		scanf(" %c", &choice);

		while (getchar() != '\n');

		if (choice != 'y' && choice != 'Y' && choice != 'n' && choice != 'N') {
	  		printf("\n\033[0;31mInvalid input, please enter 'y' or 'n'.\033[0m");
		}
  	} while (choice != 'y' && choice != 'Y' && choice != 'n' && choice != 'N');

  	return (choice == 'y' || choice == 'Y') ? true : false;
}

// initializes grid to empty grid squares
void initialize_grid(Block *grid) {
	for (int i = 0; i < 9; i++) {
		grid[i] = BlockE;
  	}
}

// prints a single grid character (with color)
void print_block(Block grid, int i) {
  	switch (grid) {
  	case (BlockE):
		printf("\033[0;30m");
		printf("(%d)", i);
		printf("\033[0m");
	break;
  	case (BlockX):
		printf("\033[0;31m");
		printf(" X ");
		printf("\033[0m");
	break;
  	case (BlockO):
		printf("\033[0;32m");
		printf(" O ");
		printf("\033[0m");
	break;
  	}
}

// prints the grid, calls print_line
void print_grid(Block *grid) {

  	printf("\033[0;32m================================\033[0;30m\n\n");

  	for (int i = 0; i <= 6; i += 3) {

		print_block(grid[i], i + 1);
		printf("|");
		print_block(grid[i + 1], i + 2);
		printf("|");
		print_block(grid[i + 2], i + 3);
		printf("\n");

		if (i == 6) break;
		printf("--- --- ---\n");
  	}

  	printf("\n");
}

// returns true if there's a winning match on the board
Bool check_victory(Block *grid) {

  	// check horizontally and vertically
  	for (int i = 0; i < 3; i++) {
		if (grid[i * 3] != BlockE && grid[i * 3] == grid[i * 3 + 1] && grid[i * 3] == grid[i * 3 + 2]) {
	  		return true;
		}
		if (grid[i] != BlockE && grid[i] == grid[i + 3] && grid[i] == grid[i + 6]) {
	  		return true;
		}
 	}

  	// check diagonals
  	if (grid[0] != BlockE && grid[0] == grid[4] && grid[0] == grid[8]) {
		return true;
	}
  	if (grid[2] != BlockE && grid[2] == grid[4] && grid[2] == grid[6]){
		return true;
	}

  	return false;
}

// writes X/O (based on "whose_turn") to the grid
// returns true if there's a winning match
// assumes input is valid
Bool make_move(Block *grid, int input, Block whose_turn) {
  	grid[input] = whose_turn;
  	print_grid(grid);
  	return check_victory(grid);
}

// two-player-mode
// same return value as main()
int two_player_mode(Block *grid) {
  	Bool game_in_progress = true;
  	Block whose_turn = BlockX; // X always starts

  	print_grid(grid);
  	while (game_in_progress) {
		int input = read_grid_position(grid, whose_turn); // get grid position from player

		// write X/O to the grid, check if it was a winning move and print the grid
		// go into if statement, if the move was a winning move
		if (make_move(grid, input, whose_turn)) {
	  		printf("%s won! Congratulations!\n",
			(whose_turn == BlockX) ? "Player \033[0;31mX\033[0m" : "Player \033[0;32mO\033[0m");
			return 0;
		}

		whose_turn = (whose_turn != BlockX) ? BlockX : BlockO; // flip "whose_turn", other player's turn

		game_in_progress = false;
		for (int i = 0; i < 9; i++) { // if all the blocks are taken and no one has won, it's a draw
	  		if (grid[i] == BlockE) game_in_progress = true;
		}
	}
  	printf("Game over! Draw!\n");

  	return 0;
}

// function used in counting Xs and Os per row/columbn/diagonal (line)
// must be called only in "computer_turn()"
void counter_operation(Block *grid, int pos, int *pos_e, int *x_counter, int *o_counter) {
  	if (grid[pos] == BlockX) {
		*x_counter = *x_counter + 1;
		*o_counter = *o_counter - 1; // if there's an O on the line, it can't be won with Xs
  	}

  	if (grid[pos] == BlockO) {
		*o_counter = *o_counter + 1;
		*x_counter = *x_counter - 1; // if there's an X on the line, it can't be won with Os
  	}

  	if (grid[pos] == BlockE) {
		*pos_e = pos; // save index of an empty block
  	}
}

Bool computer_turn(Block *grid, Bool is_first_move) {
	// if it's the first move and player chooses a corner,
	// pick middle square (otherwise loss with perfect play from white)
  	if (is_first_move) {
		if (grid[0] == BlockX || grid[2] == BlockX || grid[6] == BlockX || grid[8] == BlockX) {
	  		is_first_move = false;
	  		return make_move(grid, 4, BlockO);
		}
		is_first_move = false;
  	}

  	int x_counter = 0, o_counter = 0, pos_e;
  	int enemy_win_index = -1;	// saves index of the grid, where player can win on his next turn
		  						// made to prioritize winning the game, instead of defending,
		  						// defends only, if there is no winning move available


  	// if two other blocks on the same horizontal line are the same, either win,
  	// or save "enemy_win_index"
  	for (int i = 0; i < 3; i++) {   // vertical lines
		for (int j = 0; j < 3; j++) { // horizontal lines
			counter_operation(grid, j + 3 * i, &pos_e, &x_counter, &o_counter);
		}
		if (o_counter == 2) { // comparison against 2, logic explained in "counter_operation()"
			return make_move(grid, pos_e, BlockO);
		}
		if (x_counter == 2) enemy_win_index = pos_e;

		x_counter = 0;
		o_counter = 0;
  	}

  	// same for loop logic, but for vertical lines
  	for (int i = 0; i < 3; i++) {   // horizontal lines
		for (int j = 0; j < 3; j++) { // vertical lines
	  		counter_operation(grid, i + 3 * j, &pos_e, &x_counter, &o_counter);
		}
		if (o_counter == 2) {
	  		return make_move(grid, pos_e, BlockO);
		}
		if (x_counter == 2) enemy_win_index = pos_e;

		x_counter = 0;
		o_counter = 0;
  	}

  	// 1-9 diagonal test
  	counter_operation(grid, 0, &pos_e, &x_counter, &o_counter);
  	counter_operation(grid, 4, &pos_e, &x_counter, &o_counter);
  	counter_operation(grid, 8, &pos_e, &x_counter, &o_counter);
  	if (o_counter == 2) {
		return make_move(grid, pos_e, BlockO);
  	}
  	if (x_counter == 2) enemy_win_index = pos_e;

  	x_counter = 0;
  	o_counter = 0;

  	// 3-7 diagonal test
  	counter_operation(grid, 2, &pos_e, &x_counter, &o_counter);
  	counter_operation(grid, 4, &pos_e, &x_counter, &o_counter);
  	counter_operation(grid, 6, &pos_e, &x_counter, &o_counter);
  	if (o_counter == 2) {
		return make_move(grid, pos_e, BlockO);
  	}
  	if (x_counter == 2) enemy_win_index = pos_e;

  	x_counter = 0;
  	o_counter = 0;

  	// no wins available for computer, check if there are wins for player
  	if (enemy_win_index != -1) { // if enemy has at least one winning move, prevent it
		return make_move(grid, enemy_win_index, BlockO);
  	}

  	// otherwise, play a random move
  	srand(time(NULL)); // randomize the seed
  	int random_move_index = rand() % 9;
  	while (grid[random_move_index] != BlockE) { // repeat until random index is a playable move
		random_move_index = rand() % 9;
  	}
  	return make_move(grid, random_move_index, BlockO); // write random block
}

int single_player_mode(Block *grid) {
 	Bool is_first_move = true;
 	Bool game_in_progress = true;
  	print_grid(grid);

  	while (game_in_progress) {
		int input = read_grid_position(grid, BlockX); // get grid position from player

		// player's move
		if (make_move(grid, input, BlockX)) {
			printf("The player is victorious! Well done!\n");
	  		return 0;
		}

		game_in_progress = false; // check if there are no more moves after player's move (X, aka player, moves last)
		for (int i = 0; i < 9; i++) {
	  		if (grid[i] == BlockE) {
				game_in_progress = true; // if there's an empty block, game continues
		  	}
		}
		if (!game_in_progress) { // otherwise, draw
		  	printf("Draw! Well that was a close one!\n");
		  	return 0;
		}

		// computer's move
		if (computer_turn(grid, is_first_move)) {
			printf("You lose! I win! Humans will never beat machines!\n");
			return 0;
		}

		is_first_move = false; // first turn is over
  	}
  	return 0;
}

int main() {
  	Bool computer_playing = read_playing_vs_computer(); // ask user about mode selection
  	Block grid[9];
  	initialize_grid(grid); // initialisation of grid

  	// call either single or two player mode, return 0 if no errors
  	return computer_playing ? single_player_mode(grid) : two_player_mode(grid);
}
