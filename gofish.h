#ifndef GOFISH_H
#define GOFISH_H

#include "deck.h"
#include "player.h"


/*
   Function prototypes for the Go Fish game logic.
*/

/*
 * Function: init_game
 * -------------------
 * Initializes the game by shuffling the deck and dealing cards to players.
 */
void init_game();



// const char* check_winner(struct player* user, struct player* computer);
/*
 * Function: display_game_state
 * ------------------------------
 * Displays the current state of the game, including players' hands and books.
 */
void display_game_state();

const char* index_to_rank(int index);

char ask_to_play_again();



#endif // GOFISH_H
