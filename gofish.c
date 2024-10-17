#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "player.h"
#include "deck.h"
#include "card.h"
#include "gofish.h"



// Main function 
int main(int argc, char* argv[]) {
    char play_again = 'Y';

    while (play_again == 'Y') { 
        // Initialize the game for each round
        init_game(&user, &computer, &deck_instance); // Ensure this initializes the deck and players

        int result;
        int game_over_flag = 0; // Flag to indicate if the game is over
        // Game loop
        while (!game_over_flag) {

            // Player's turn
            do {
                display_game_state();
                result = user_play(&user, &computer, &deck_instance);  
                if (result == -1) {
                    fprintf(stderr, "Error during user's turn.\n");
                    game_over_flag = 1; // Set the flag to end the game due to an error
                    break;
                }

                // printf("Player 1 Books: %lu, Player 2 Books: %lu\n", (unsigned long)user.book_count, (unsigned long)computer.book_count);

                // Check if the game is over
                if (game_over()) {
                    game_over_flag = 1;  // Set flag to exit the game loop
                    break;
                }

            } while (result == 1); // Keep playing if user gets another turn
        if (game_over_flag) break; // Check if the game is over before the computer's turn

        printf("\n");


            // Computer's turn
            do {
                display_game_state();
                result = computer_turn(&computer, &user, &deck_instance);
                if (result == -1) {
                    fprintf(stderr, "Error during computer's turn.\n");
                    game_over_flag = 1;
                    break;
                }

                // printf("Player 1 Books: %lu, Player 2 Books: %lu\n", (unsigned long)user.book_count, (unsigned long)computer.book_count);

                // Check if the game is over
                if (game_over()) {
                    game_over_flag = 1;  // Set flag to exit the game loop
                    break;
                }

            } while (result == 1); // Keep playing if computer gets another turn

        if (game_over_flag) break;
        printf("\n");

        }    

        // Ask if the user wants to play again
        play_again = ask_to_play_again();

        // Reset after each game
        reset_player(&user);      
        reset_player(&computer);  
    }
    printf("Exiting.\n");
    return 0;
}


// Initialize the game
void init_game() {
    // Shuffle the deck and deal cards to both players
    printf("Shuffling deck...\n");
    shuffle();
    // Initialize players
    init_player(&user); // Initialize the user player
    init_player(&computer); // Initialize the computer player

    deal_player_cards(&user);
    deal_player_cards(&computer);
    printf("Game initialized. Players have been dealt cards.\n");

    printf("\n");
}


// Function to check if a rank is valid
int valid_rank(const char* rank) {
    // Check if the rank is one of '2'-'9', '10', 'J', 'Q', 'K', or 'A'
    const char* valid_ranks[] = {"2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "A"};
    for (int i = 0; i < sizeof(valid_ranks) / sizeof(valid_ranks[0]); i++) {
        if (strcmp(rank, valid_ranks[i]) == 0) {
            return 1; // Rank is valid
        }
    }
    return 0; // Rank is invalid
}


void display_game_state() {

      // Display user's hand and book
    printf("Player 1's Hand - ");
    for (size_t i = 0; i < user.hand_size; i++) {
        printf("%s ", card_to_string(user.cards[i]));
    }


    // // Display computer's hand 
    // printf("\nPlayer 2's Hand - ");
    // for (size_t i = 0; i < computer.hand_size; i++) {
    //     printf("%s ", card_to_string(computer.cards[i]));
    // }

    // Printing books
    printf("\nPlayer 1's Book - ");
    for (size_t i = 0; i < user.book_count; i++) {
        printf("%s ", index_to_rank(user.book[i]));
    }
    printf("\n");

    // Display computer's books (Player 2)
    printf("Player 2's Book - ");
    for (size_t i = 0; i < computer.book_count; i++) {
        printf("%s ", index_to_rank(computer.book[i]));
    }
    printf("\n");
    
}




// Function to ask the user if they want to play again
char ask_to_play_again() {
    char response;
    do {
        printf("Do you want to play again [Y/N]: ");
        scanf(" %c", &response); // Add space before %c to consume any leftover newline character
        response = toupper(response); // Convert to uppercase to handle both 'y' and 'n'
    } while (response != 'Y' && response != 'N'); // Ensure valid input
    return response;
}


const char* index_to_rank(int index) {
    // Correct the string returning for ranks 2-9
    switch(index) {
        case 0: return "2";  
        case 1: return "3";  
        case 2: return "4";  
        case 3: return "5";  
        case 4: return "6";  
        case 5: return "7";  
        case 6: return "8";  
        case 7: return "9";  
        case 8: return "10"; 
        case 9: return "J";  
        case 10: return "Q";  
        case 11: return "K";  
        case 12: return "A";  
        default: return " "; // Invalid index
    }
}

