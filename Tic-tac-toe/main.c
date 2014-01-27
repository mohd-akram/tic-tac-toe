#include "tictactoe.h"

int main(void)
{
	while(play(!prompt_bool("2 player?", false)))
		;

	return 0;
}
