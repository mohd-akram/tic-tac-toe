#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include "tictactoe.h"

#define SIZE 3

typedef struct index2d {
	int i;
	int j;
} Index2d;

static
void print_grid(char grid[SIZE][SIZE])
{
	/* Heading */
	for (int i = 0; i < SIZE; i++)
		printf("   %c", 'a'+i);
	puts("");

	for (int i = 0; i < SIZE; i++) {
		/* Row number */
		printf("%d ", i+1);

		/* Columns */
		for (int j = 0; j < SIZE; j++) {
			printf(" %c ", grid[i][j]);
			if (j < SIZE-1)
				putchar('|');
		}
		printf("\n  ");

		/* Horizontal border */
		if (i < SIZE-1)
			for (int j = 0; j < SIZE*3 + SIZE-1; j++)
				putchar('-');

		puts("");
	}
}

#ifdef _MSC_VER
#define inline __inline
#endif

static inline
bool in_range(int x, int lo, int hi)
{
	return x >= lo && x < hi;
}

#undef max

static inline
int max(int a, int b)
{
	return a > b ? a : b;
}

static
int horizontal(char grid[SIZE][SIZE], Index2d pos, char player)
{
	int hor = 0;
	for (int j = 0; j < SIZE; j++)
		if (grid[pos.i][j] == player)
			++hor;

	return hor;
}

static
int vertical(char grid[SIZE][SIZE], Index2d pos, char player)
{
	int ver = 0;
	for (int i = 0; i < SIZE; i++)
		if (grid[i][pos.j] == player)
			++ver;

	return ver;
}

static
int diagonal(char grid[SIZE][SIZE], Index2d pos, char player)
{
	int diag = 0;
	if (pos.i == pos.j)
		for (int k = 0; k < SIZE; k++)
			if (grid[k][k] == player)
				++diag;

	return diag;
}

static
int anti_diagonal(char grid[SIZE][SIZE], Index2d pos, char player)
{
	int diag2 = 0;
	if (pos.j == (SIZE-1) - pos.i)
		for (int i = 0, j = SIZE-1; i < SIZE && j >= 0; i++, j--)
			if (grid[i][j] == player)
				++diag2;

	return diag2;
}

static
int evaluate(char grid[SIZE][SIZE], Index2d pos, char player)
{
	int rank = 0;

	rank = max(rank, horizontal(grid, pos, player));
	rank = max(rank, vertical(grid, pos, player));
	rank = max(rank, diagonal(grid, pos, player));
	rank = max(rank, anti_diagonal(grid, pos, player));

	return rank;
}

static inline
bool won(char grid[SIZE][SIZE], Index2d pos, char player)
{
	return evaluate(grid, pos, player) == SIZE;
}

static
Index2d ai_turn(char grid[SIZE][SIZE], char player, char ai_player)
{
	Index2d temp, pos;

	int count = 0, rank = 0;

	for (int i = 0; i < SIZE; i++) {
		for (int j = 0; j < SIZE; j++) {
			if (grid[i][j] != ' ')
				continue;

			temp.i = i, temp.j = j;

			/* Check if AI can win immediately */
			if (evaluate(grid, temp, ai_player) == SIZE-1) {
				grid[temp.i][temp.j] = ai_player;
				return temp;
			}

			/* Get a random best position */
			int result = evaluate(grid, temp, player);
			if (result > rank)
				count = 0, rank = result;

			if (result == rank)
				if ((rand() % ++count) == 0)
					pos = temp;
		}
	}

	grid[pos.i][pos.j] = ai_player;

	return pos;
}

static
bool prompt_turn(Index2d *pos)
{
	char col;
	int row;

	printf("Select a position: ");

	col = getchar();
	if (col == '\n')
		return false;

	int res = scanf_s("%d", &row);
	while (getchar() != '\n')
		;

	if (res != 1 || col < 'a' || col >= 'a'+SIZE || row < 1 || row > SIZE)
		return false;

	pos->i = row-1, pos->j = col-'a';
	return true;
}

static
Index2d player_turn(char grid[SIZE][SIZE], char player)
{
	Index2d pos;

	for (;;) {
		if (!prompt_turn(&pos))
			puts("Invalid move");

		else if (grid[pos.i][pos.j] != ' ')
			puts("Already played there");

		else
			break;
	}

	grid[pos.i][pos.j] = player;

	return pos;
}

bool prompt_bool(const char *question, bool default_)
{
	char yes = 'y', no = 'n';
	if (default_)
		yes = toupper(yes);
	else
		no = toupper(no);

	printf("%s [%c/%c]: ", question, yes, no);
	char choice = tolower(getchar());
	if (choice != '\n')
		while (getchar() != '\n')
			;

	return choice == 'y' ? true : (choice == 'n' ? false : default_);
}

bool play(bool with_ai)
{
	char grid[SIZE][SIZE];
	Index2d pos;

	int moves = 0;
	int max_moves = SIZE * SIZE;

	static int player1_score = 0, player2_score = 0;

	srand((unsigned)time(NULL));
	rand();

	/* Initialize grid */
	memset(grid, ' ', SIZE * SIZE);

	char player1 = rand() % 2 ? 'x' : 'o';
	char player2 = (player1 == 'x' ? 'o' : 'x');

	if (with_ai) {
		printf("You are %c\n", player1);
		if (player1 == 'x')
			print_grid(grid);
	}
	else {
		printf("Player 1 is %c. Player 2 is %c\n", player1, player2);
		print_grid(grid);
	}

	if (player1 == 'o') {
		if (with_ai)
			ai_turn(grid, player1, player2);
		else {
			puts("Player 2's turn");
			player_turn(grid, player2);
		}
		++moves;
		print_grid(grid);
	}

	for (;;) {
		/* Player 1 */
		if (moves < max_moves) {
			if (!with_ai)
				puts("Player 1's turn");
			pos = player_turn(grid, player1);
			++moves;

			if (won(grid, pos, player1)) {
				if (with_ai)
					puts("You won");
				else
					puts("Player 1 won");
				++player1_score;
				break;
			}

			/* Don't print the grid before the AI's turn */
			if (!with_ai)
				print_grid(grid);
		}

		/* Player 2 / AI */
		if (moves < max_moves) {
			if (with_ai)
				pos = ai_turn(grid, player1, player2);
			else {
				puts("Player 2's turn");
				pos = player_turn(grid, player2);
			}
			++moves;

			if (won(grid, pos, player2)) {
				if (with_ai)
					puts("You lost");
				else
					puts("Player 2 won");
				++player2_score;
				break;
			}

			print_grid(grid);
		}

		if (moves == max_moves) {
			puts("Tie");
			break;
		}
	}

	print_grid(grid);
	printf("Player 1  Player 2\n"
		"--------  --------\n"
		"%8d%10d\n", player1_score, player2_score);

	return prompt_bool("Play again?", true);
}
