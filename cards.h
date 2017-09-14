#ifndef CARDS_H
#define CARDS_H


#include <vector>
using std::vector;

#include <map>
using std::map;

#include <iostream>
using std::cout;
using std::endl;

#include <algorithm>
using std::random_shuffle;
using std::shuffle;

#include "ffbpneuralnet.h"


#define USE_ONE_HOT_INPUT_ENCODING


#define SUIT_HEARTS 0
#define SUIT_SPADES 1
#define SUIT_DIAMONDS 2
#define SUIT_CLUBS 3

#define FACE_A_LOW 0
#define FACE_2 1
#define FACE_3 2
#define FACE_4 3
#define FACE_5 4
#define FACE_6 5
#define FACE_7 6
#define FACE_8 7
#define FACE_9 8
#define FACE_10 9
#define FACE_J 10
#define FACE_Q 11
#define FACE_K 12
#define FACE_A 13

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
#define POSITION_NOT_SHOWN 8

#define HIGH_CARD 0
#define ONE_PAIR 1
#define TWO_PAIR 2
#define THREE_OF_A_KIND 3
#define STRAIGHT 4
#define FLUSH 5
#define FULL_HOUSE 6
#define FOUR_OF_A_KIND 7
#define STRAIGHT_FLUSH 8
#define ROYAL_FLUSH 9

#define MAX_EXTENT_SPREAD_FOR_STRAIGHT 4



class card
{
public:
    
    bool operator<(const card &rhs) const;
    void print(void) const;
    
    size_t card_id;
    size_t suit;
    size_t face;
    bool shown;
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
    void print_table(void) const;
    void print_sorted_hand(const size_t player_index) const;
    void get_card_states(vector<double> &states) const;
    
    size_t get_best_rank_finished(void) const;
    void print_finished_rank(const size_t player_index) const;
    size_t rank_finished_hand(const size_t player_index) const;
    size_t numeric_rank_finished_hand(const size_t player_index) const;
    
    void play_rand(void);
    void play_ANN(vector<input_output_pair> &io, FFBPNeuralNet &NNet);

    
protected:
    
    bool is_card_not_shown(const size_t card_id) const;
    size_t get_card_id(const size_t face, const size_t suit) const;
    
    void swap_cards(card &a, card &b);
    
    size_t get_rand_not_shown_index(const size_t player_index) const;
    
    bool is_finished_hand_royal_flush(const vector<card> &hand) const;
    bool is_finished_hand_straight_flush(const vector<card> &hand) const;
    bool is_finished_hand_4_of_a_kind(const vector<card> &hand) const;
    bool is_finished_hand_full_house(const vector<card> &hand) const;
    bool is_finished_hand_flush(const vector<card> &hand) const;
    bool is_finished_hand_straight(const vector<card> &hand) const;
    bool is_finished_hand_3_of_a_kind(const vector<card> &hand) const;
    bool is_finished_hand_2_pair(const vector<card> &hand) const;
    bool is_finished_hand_1_pair(const vector<card> &hand) const;
    bool is_finished_hand_high_card(const vector<card> &hand) const;
    
    size_t hand_num_shown(const vector<card> &hand) const;
    bool does_hand_contain_multiple_suits(const vector<card> &hand) const;
    bool does_hand_contain_face_multiples(const vector<card> &hand) const;
    bool hand_cards_less_than_or_equal_to_face(const size_t face, const vector<card> &hand) const;
    bool hand_cards_greater_than_or_equal_to_face(const size_t face, const vector<card> &hand) const;
    size_t hand_get_extent_spread(const vector<card> &hand) const;
    size_t hand_get_highest_face(const vector<card> &hand) const;
    size_t hand_get_second_highest_face(const vector<card> &hand) const;
    size_t hand_get_lowest_face(const vector<card> &hand) const;
    
    
    size_t current_player;
    vector< vector < card > > players_hands;
    vector<card> discard_pile;
    vector<card> pickup_pile;
    
    vector<card> card_id_lookup_helper;
};






#endif
