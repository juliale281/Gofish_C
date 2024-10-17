#ifndef CARD_H
#define CARD_H

#include <stdio.h> // Include this for snprintf



/*
  Valid suits: C, D, H, and S
  Valid ranks: 2, 3, 4, 5, 6, 7, 8, 9, 10, J, Q, K, A
*/
struct card
{
  char suit;
  char rank[3];
};




#endif
