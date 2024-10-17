#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "deck.h"
#include "player.h"
#include "card.h"
#include "gofish.h"


/*
 * Variable: deck_instance
 * ----------------------- 
 *  
 * Go Fish uses a single deck
 */
struct deck deck_instance;

// Function to initialize and shuffle the deck
int shuffle() {
    // Initialize the deck
    char suits[] = {'C', 'D', 'H', 'S'};
    char ranks[][3] = {"2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "A"};
    
    deck_instance.top_card = 0; // Reset top card

    // Fill the deck with cards
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 13; j++) {
            deck_instance.list[i * 13 + j].suit = suits[i];
            snprintf(deck_instance.list[i * 13 + j].rank, sizeof(deck_instance.list[i * 13 + j].rank), "%s", ranks[j]);
        }
    }

    // Shuffle the deck using Fisher-Yates algorithm
    srand(time(NULL)); // Seed the random number generator
    for (int i = 51; i > 0; i--) {
        int j = rand() % (i + 1);
        struct card temp = deck_instance.list[i];
        deck_instance.list[i] = deck_instance.list[j];
        deck_instance.list[j] = temp;
    }

    return 0; // Success
}

// Function to get the next card from the deck
struct card* next_card() {
    if (deck_instance.top_card < 52) { // Ensure there are cards left in the deck
        return &deck_instance.list[deck_instance.top_card++]; // Return the current card and increment the top card
    }
    return NULL; // No more cards available
}

// Function to deal 7 random cards to the specified player
int deal_player_cards(struct player* target) {
    // Check for invalid player or card list
    if (target == NULL || target->cards == NULL) {
        fprintf(stderr, "Invalid player or card list.\n");
        return -1; // Handle invalid player or card list
    }

    // Check if there are enough cards left in the deck
    if (deck_size() < 7) {
        fprintf(stderr, "Not enough cards left in the deck.\n");
        return -1; // Not enough cards left in the deck
    }

    target->hand_size = 0; // Reset hand size for the new cards

    for (int i = 0; i < 7; i++) {
        struct card* card_ptr = next_card(); // Get the next card from the deck
        if (card_ptr == NULL) {
            fprintf(stderr, "No more cards available to deal.\n");
            return -1; // Handle case where no more cards are available
        }
        target->cards[target->hand_size] = *card_ptr; // Copy the card value to the player's hand
        target->hand_size++; // Increment hand size
    }

    return 0; // Success
}

// Function to get the number of cards left in the deck
size_t deck_size() {
    return 52 - deck_instance.top_card; // Return the number of cards left in the deck
}


