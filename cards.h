#ifndef CARDS_H
#define CARDS_H


#include <vector>
using std::vector;

#include <iostream>
using std::cout;
using std::endl;

#include <algorithm>
using std::random_shuffle;
using std::shuffle;

#include "ffbpneuralnet.h"


#define SUIT_HEARTS 0
#define SUIT_SPADES 1
#define SUIT_DIAMONDS 2
#define SUIT_CLUBS 3

#define FACE_2 0
#define FACE_3 1
#define FACE_4 2
#define FACE_5 3
#define FACE_6 4
#define FACE_7 5
#define FACE_8 6
#define FACE_9 7
#define FACE_10 8
#define FACE_J 9
#define FACE_Q 10
#define FACE_K 11
#define FACE_A 12

#define NUM_PLAYERS 5
#define NUM_CARDS_PER_HAND 5
#define NUM_CARDS_PER_DECK 52

#define POSITION_HAND0 0
#define POSITION_HAND1 1
#define POSITION_HAND2 2
#define POSITION_HAND3 3
#define POSITION_HAND4 4
#define POSITION_DISCARD_PILE 5
#define POSITION_TOP_OF_DISCARD_PILE 6
#define POSITION_TOP_OF_PICKUP_PILE 7
#define POSITION_UNCOVERED 8


class card
{
public:
    
    bool operator<(const card &rhs) const;
    void print(void) const;
    
    size_t card_id;
    size_t suit;
    size_t face;
    bool uncovered;
};

class input_output_pair
{
public:
    
    vector<double> input;
    vector<double> output;
};

class blind_poker_table
{
public:

    blind_poker_table(void);
    void reset_table(void);
    void print_table(void);
    void get_card_states(vector<double> &states);
    
    void play_rand(void);
    void play_ANN(vector<input_output_pair> &io, FFBPNeuralNet &NNet);
    
    size_t get_rand_uncovered_index(const size_t player_index);
    void sort_hand(const size_t player_index);
    size_t get_best_rank(void);
    size_t rank_hand(const size_t player_index) const;
    
    void swap_cards(card &a, card &b);
    
    size_t current_player;
    vector< vector < card > > players_hands;
    vector<card> discard_pile;
    vector<card> pickup_pile;
};






#endif
