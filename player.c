#include "player.h"
#include "deck.h"
#include "card.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "gofish.h"


/*
 * Instance Variables: user, computer   
 * ----------------------------------   
 *  
 *  We only support 2 users: a human and a computer
 */
struct player user;
struct player computer;



void init_player(struct player *player) {
    player->hand_size = 0; // Start with no cards in hand
    player->max_hand_size = 7; // Set the initial maximum hand size

    // Allocate memory for the cards array
    player->cards = malloc(player->max_hand_size * sizeof(struct card));
    if (player->cards == NULL) {
        // Handle memory allocation failure
        fprintf(stderr, "Memory allocation failed for player cards.\n");
        exit(EXIT_FAILURE);
    }

    // Initialize the book array to 0
    for (size_t i = 0; i < 7; i++) {
        player->book[i] = 0; // Start with no books
    }

    // Initialize the cards
    for (size_t i = 0; i < player->max_hand_size; i++) {
        player->cards[i].rank[0] = '\0'; // Set rank to empty
        player->cards[i].suit = '\0';     // Set suit to empty
    }
}




// Add a new card to the player's hand
int add_card(struct player *target, struct card *new_card) {
    // Check if there's enough space in the current allocation
    if (target->hand_size >= target->max_hand_size) {
        // Increase the size of the cards array (e.g., double the size)
        target->max_hand_size *= 2;
        struct card* new_cards = realloc(target->cards, target->max_hand_size * sizeof(struct card));
        
        if (new_cards == NULL) {
            // Handle memory allocation failure
            fprintf(stderr, "Memory reallocation failed for player cards.\n");
            return -1; // Error
        }
        target->cards = new_cards; // Update the pointer to the resized array
    }

    // Add the new card
    target->cards[target->hand_size] = *new_card; // Add the new card
    target->hand_size++; // Update hand size
    return 0; // Success
}



// Remove a card from the player's hand
int remove_card(struct player* target, struct card* old_card) {
    for (size_t i = 0; i < target->hand_size; i++) {
        if (strcmp(target->cards[i].rank, old_card->rank) == 0 &&
            target->cards[i].suit == old_card->suit) {
            // Shift cards down to fill the gap
            for (size_t j = i; j < target->hand_size - 1; j++) {
                target->cards[j] = target->cards[j + 1];
            }
            target->hand_size--;
            return 0; // Success
        }
    }
    return -1; // Error: card not found
}


// Check if a player has all 4 cards of the same rank
char check_add_book(struct player* target) {
    int count[13] = {0}; // Assuming rank is 0-12 for A, 2, ..., K
    char added_rank = -1; // To store the rank of added book if any

    // Count the number of each rank in the player's hand
    for (size_t i = 0; i < target->hand_size; i++) {
        int rank_index = rank_to_index(target->cards[i].rank);
        count[rank_index]++;
    }

    // Check for books and update the player's book
    for (int i = 0; i < 13; i++) {
        if (count[i] == 4) {
            target->book[target->book_count] = i; // Add rank to the book
            target->book_count++; // Increment the book count

            // Remove cards from hand
            for (size_t j = 0; j < target->hand_size; j++) {
                int rank_index = rank_to_index(target->cards[j].rank);
                if (rank_index == i) {
                    remove_card(target, &target->cards[j]);
                    j--; // Adjust index after removal
                }
            }
            added_rank = i; // Store the rank of the book found
        }
    }
    return added_rank; // Return the rank index of the book found or -1 if none found
}



int search(struct player* target, const char* rank) {
    if (target == NULL) {
        printf("Error: target player is NULL.\n");
        return 0; // Invalid target
    }

    for (size_t i = 0; i < target->hand_size; i++) {
        // Use strcmp to compare strings
        if (strcmp(target->cards[i].rank, rank) == 0) {
            return 1; // Found the rank
            printf("Player 1 has cards of rank %s\n", rank); // Use %s for strings
        }
    }
    return 0; // Rank not found
}




// Transfer cards of a given rank from src to dest
int transfer_cards(struct player* src, struct player* dest, const char* rank) {
    int count = 0;

    // Loop through the source player's hand
    for (size_t i = 0; i < src->hand_size; i++) {
        // Compare the first character of src->hand[i].rank with the passed rank
        if (strcmp(src->cards[i].rank, rank) == 0) {
            // Add the card to the destination player's hand
            add_card(dest, &src->cards[i]);

            // Remove the card from the source player's hand
            remove_card(src, &src->cards[i]);
            count++;
            i--; // Decrement i to account for the shifted cards
        }
    }
    return count;
}

// Check if a player has 7 books and if it's over yet
int game_over() {
    if (user.book_count >= 7) {
        display_game_state();  // Display the current game state
        printf("Player 1 Wins! %lu-%lu\n", (unsigned long)user.book_count, (unsigned long)computer.book_count);
        return 1;  // Game over, Player 1 wins
    }
    
    if (computer.book_count >= 7) {
        display_game_state();  // Display the current game state
        printf("Player 2 Wins! %lu-%lu\n", (unsigned long)computer.book_count, (unsigned long)user.book_count);
        return 1;  // Game over, Player 2 wins
    }

    return 0;  // Game is not over yet
}




// Reset player for a new game
int reset_player(struct player* target) {
    // Free any dynamically allocated memory for the hand
    if (target->cards != NULL) {
        free(target->cards);
        target->cards = NULL; // Set to NULL after freeing
    }

    target->hand_size = 0; // Reset the hand size
    target->max_hand_size = 0; // Optionally reset max_hand_size if needed

    // Reset the books
    for (size_t i = 0; i < target->book_count; i++) {
        target->book[i] = 0; // Reset books
    }

    target->book_count = 0; // Reset the book count

    return 0; // Success
}



// Function to randomly select a rank from the computer's hand
const char* computer_play(struct player* computer) {
    // Store the unique ranks present in the computer's hand
    char available_ranks[52][3];  
    size_t unique_ranks_count = 0;

    // Iterate through the computer's hand to find unique ranks
    for (size_t i = 0; i < computer->hand_size; i++) {
        char rank[3];
        strcpy(rank, computer->cards[i].rank);  // Correctly copy the rank

        // Check if this rank is already in available_ranks
        int already_present = 0;
        for (size_t j = 0; j < unique_ranks_count; j++) {
            if (strcmp(available_ranks[j], rank) == 0) {
                already_present = 1;
                break;
            }
        }

        // If the rank is not already in available_ranks, add it
        if (!already_present) {
            strcpy(available_ranks[unique_ranks_count], rank);
            unique_ranks_count++;
        }
    }

    // Randomly select one of the available ranks if there are any
    if (unique_ranks_count > 0) {
        int selected_index = rand() % unique_ranks_count;
        char* selected_rank = strdup(available_ranks[selected_index]);  // Dynamically allocate memory for the rank
        return selected_rank;
    }

    // If no unique ranks are available, return NULL
    return NULL;  
}

// Function to handle the computer's turn
int computer_turn(struct player* computer, struct player* user, struct deck* deck) {
    // Computer selects a random rank from its hand
    const char* rank = computer_play(computer); 

    // If no rank could be selected (computer has no cards), return immediately
    if (!rank) {
        printf("Player 2 has no cards left to play.\n");
        return 0;
    }

    // Print computer's selected rank
    printf("Player 2's turn, enter a Rank: %2s\n", rank);

    // Show Player 2's requested rank and suit
    printf("    - Player 2 has: ");
    int firstUser = 1; // To format output

    for (size_t i = 0; i < user->hand_size; i++) { // Iterate through Player 2's hand
        if (strcmp(computer->cards[i].rank, rank) == 0) { // Check if the card matches the requested rank
            if (!firstUser) {
                printf(", "); // Add comma before subsequent cards
            }
            printf("%s%c", computer->cards[i].rank, computer->cards[i].suit); // Print card's rank and suit
            firstUser = 0; // Set first to false after the first output
        }
    }
    printf("\n"); // New line after printing all cards



    // Check if the player (user) has cards of the requested rank
    if (search(user, rank)) {
        // Player 1 (user) has cards of the requested rank
        printf("    - Player 1 has: ");

        // Print out the cards of the requested rank
        int first = 1; // To format output
        for (size_t i = 0; i < user->hand_size; i++) {
            if (strcmp(user->cards[i].rank, rank) == 0) {
                if (!first) {
                    printf(", ");
                }
                printf("%s%c", user->cards[i].rank, user->cards[i].suit);
                first = 0;
            }
        }
        printf("\n");

        // Transfer cards from the user to the computer
        transfer_cards(user, computer, rank);

        char added_rank = check_add_book(computer);
        if (added_rank != -1) {
            printf("    - Player 2 books %s\n", index_to_rank(added_rank));
            printf("    - Player 2 gets another turn\n");
            printf("\n");
            return 1;  // Computer gets another turn
        }

    } else {
        // Player 1 does not have the requested rank
        printf("    - Player 1 has no %s's\n", rank);

        // Computer goes fishing from the deck
        struct card* drawn_card = next_card(deck);
        if (drawn_card) {
            printf("    - Go Fish, Player 2 draws a card\n");
            add_card(computer, drawn_card);

            // First, check if the drawn card completes a book
            char added_rank = check_add_book(computer);
            if (added_rank != -1) {
                printf("    - Player 2 books %s\n", index_to_rank(added_rank));
            }

            // Then, check if the drawn card matches the requested rank
            if (strcmp(drawn_card->rank, rank) == 0) { // Use strcmp for string comparison
                printf("    - Player 2 drew the card asked for: %s%c!\n", drawn_card->rank, drawn_card->suit);

                // If both match the rank and form a book, player gets another turn
                if (added_rank != -1) {
                    printf("    - Player 2 gets another turn for both matching the rank and forming a book!\n");
                } else {
                    printf("    - Player 2 gets another turn for matching the rank!\n");
                }
                printf("\n");
                return 1;  // Player gets another turn
            }

            // If the drawn card does not match the rank but forms a book, the player still gets another turn
            if (added_rank != -1) {
                printf("    - Player 2 gets another turn for forming a book!\n");
                printf("\n");
                return 1;  // Player gets another turn
            }

        } else {
            printf("No more cards left in the deck.\n");
        }
    }

    // Indicate it's the user's turn
    printf("    - Player 1's turn\n");
    return 0;  // Computer's turn is over
}




// Handle a player's turn
int user_play(struct player* target, struct player* computer, struct deck* deck) {
    // Check if Player 1 has no cards left
    if (target == &user && target->hand_size == 0) {
        printf("Player 1 has no cards left to play.\n");
        return 0; // Return 0 to indicate no valid move
    }

    if (target == &user) {
        char rank[3];

        // Prompt for input
        printf("Player 1's turn, enter a Rank: ");
        scanf("%2s", rank);  // Capture up to 2 characters for rank input

        // Validate the rank input
        if (rank_to_index(rank) == -1) {
            printf("Invalid rank. Please enter a valid rank (2-10, J, Q, K, A).\n");
            printf("\n");
            return 1; 
        }

        // Check if the user has at least one card of the requested rank
        if (!search(target, rank)) {
            printf("Error - must have at least one card from rank %s to play.\n", rank);
            printf("\n");
            return 1; // Reprompt
        }

        // Show Player 1's requested rank and suit
        printf("    - Player 1 has: ");
        int firstUser = 1; // To format output

        for (size_t i = 0; i < target->hand_size; i++) { // Iterate through Player 1's hand
            if (strcmp(target->cards[i].rank, rank) == 0) { // Check if the card matches the requested rank
                if (!firstUser) {
                    printf(", "); // Add comma before subsequent cards
                }
                printf("%s%c", target->cards[i].rank, target->cards[i].suit); // Print card's rank and suit
                firstUser = 0; // Set first to false after the first output
            }
        }
        printf("\n"); // New line after printing all cards

        if (search(computer, rank)) {
            // Player 2 has cards of the requested rank
            printf("    - Player 2 has ");

            // Print out the cards of the requested rank
            int first = 1; // To format output

            for (size_t i = 0; i < computer->hand_size; i++) { // Iterate through Player 2's hand
                if (strcmp(computer->cards[i].rank, rank) == 0) { // Check if the card matches the requested rank
                    if (!first) {
                        printf(", "); // Add comma before subsequent cards
                    }
                    printf("%s%c", computer->cards[i].rank, computer->cards[i].suit); // Print card's rank and suit
                    first = 0; // Set first to false after the first output
                }
            }
            printf("\n"); // New line after printing all cards
            // Transfer cards from Player 2 to Player 1
            transfer_cards(computer, target, rank); // Pass computer and target (user) directly

            // // Check if User can book the rank
            char added_rank = check_add_book(target);
            if (added_rank != -1) {
                printf("    - Player 1 books %s\n", index_to_rank(added_rank));
                printf("    - Player 1 gets another turn\n");
                printf("\n");
                return 1;  // user gets another turn
            }

        } else {
            // Player 2 does not have the requested rank
            printf("    - Player 2 has no %s's\n", rank);
            struct card* drawn_card = next_card(deck);
            if (drawn_card) {
                printf("    - Go Fish, Player 1 draws %s%c\n", drawn_card->rank, drawn_card->suit);
                add_card(target, drawn_card);

                // First, check if the drawn card completes a book
                char added_rank = check_add_book(target);
                if (added_rank != -1) {
                    printf("    - Player 1 books %s\n", index_to_rank(added_rank));
                }

                // Then, check if the drawn card matches the requested rank
                if (strcmp(drawn_card->rank, rank) == 0) { // Use strcmp for string comparison
                    printf("    - Player 1 drew the card asked for: %s%c!\n", drawn_card->rank, drawn_card->suit);

                    // If both match the rank and form a book, player gets another turn
                    if (added_rank != -1) {
                        printf("    - Player 1 gets another turn for both matching the rank and forming a book!\n");
                    } else {
                        printf("    - Player 1 gets another turn for matching the rank!\n");
                    }
                    printf("\n");
                    return 1;  // Player gets another turn
                }

                // If the drawn card does not match the rank but forms a book, the player still gets another turn
                if (added_rank != -1) {
                    printf("    - Player 1 gets another turn for forming a book!\n");
                    printf("\n");
                    return 1;  // Player gets another turn
                }
            
            } else {
                printf("No more cards left in the deck.\n");
            }
        }
        // Indicate Player 2's turn
        printf("    - Player 2's turn\n");
    }

    return 0; // Return default value for non-user turns
}




int rank_to_index(const char* rank) {
    if (strcmp(rank, "2") == 0) return 0;
    if (strcmp(rank, "3") == 0) return 1;
    if (strcmp(rank, "4") == 0) return 2;
    if (strcmp(rank, "5") == 0) return 3;
    if (strcmp(rank, "6") == 0) return 4;
    if (strcmp(rank, "7") == 0) return 5;
    if (strcmp(rank, "8") == 0) return 6;
    if (strcmp(rank, "9") == 0) return 7;
    if (strcmp(rank, "10") == 0) return 8;
    if (strcmp(rank, "J") == 0) return 9;
    if (strcmp(rank, "Q") == 0) return 10;
    if (strcmp(rank, "K") == 0) return 11;
    if (strcmp(rank, "A") == 0) return 12;
    return -1; // Invalid rank
}

const char* card_to_string(struct card c) {
    static char buffer[10]; // Adjust size as needed
    snprintf(buffer, sizeof(buffer), "%s%c", c.rank, c.suit); // Format the rank (as a string) and suit
    return buffer;
}
