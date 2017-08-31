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
    
    void play_rand(void)
    {
        // make binary choice
        //
        size_t choice0 = rand()%2;
        
        if(0 == choice0) // take top of discard pile
        {
            // get rand uncovered index
            // flip card in player's hand
            // swap discard pile card with hand card
            
            size_t rand_index = get_rand_uncovered_index(current_player);
            players_hands[current_player][rand_index].uncovered = true;
            swap_cards(players_hands[current_player][rand_index], discard_pile[discard_pile.size() - 1]);
        }
        else // flip top of pickup pile
        {
            pickup_pile[pickup_pile.size() - 1].uncovered = true;

            size_t choice1 = rand()%2;

            if(0 == choice1) // discard
            {
                // move top of pickup pile onto top of discard pile
                // get rand uncovered index, flip card
                
                discard_pile.push_back(pickup_pile[pickup_pile.size() - 1]);
                pickup_pile.pop_back();
                
                size_t rand_index = get_rand_uncovered_index(current_player);
                players_hands[current_player][rand_index].uncovered = true;
            }
            else
            {
                // get rand uncovered index
                // move hand card to top of discard pile
                // move pickup pile top card to hand card
                
                size_t rand_index = get_rand_uncovered_index(current_player);
                players_hands[current_player][rand_index].uncovered = true;
                
                discard_pile.push_back(players_hands[current_player][rand_index]);
                players_hands[current_player][rand_index] = pickup_pile[pickup_pile.size() - 1];
                pickup_pile.pop_back();
            }
        }
        
        if(current_player == NUM_PLAYERS - 1)
            current_player = 0;
        else
            current_player++;
    }
    
    void play_AI(vector<input_output_pair> &io, FFBPNeuralNet &NNet)
    {
        vector<double> input, output;
        
        get_card_states(input);
        NNet.FeedForward(input);
        NNet.GetOutputValues(output);
        
        if(0 == floor(output[0] + 0.5))  // take top of discard pile
        {
            input_output_pair iop;
            iop.input = input;
            iop.output = output;
            io.push_back(iop);
            
            // get rand uncovered index
            // flip card in player's hand
            // swap discard pile card with hand card
            size_t rand_index = get_rand_uncovered_index(current_player);
            players_hands[current_player][rand_index].uncovered = true;
            swap_cards(players_hands[current_player][rand_index], discard_pile[discard_pile.size() - 1]);
        }
        else  // flip top of pickup pile
        {
            input_output_pair iop;
            iop.input = input;
            iop.output = output;
            io.push_back(iop);
            
            pickup_pile[pickup_pile.size() - 1].uncovered = true;
            
            // the state of the top of the pickup pile has changed to uncovered
            get_card_states(input);
            NNet.FeedForward(input);
            NNet.GetOutputValues(output);
            
            if(0 == floor(output[0] + 0.5)) // discard
            {
                input_output_pair iop;
                iop.input = input;
                iop.output = output;
                io.push_back(iop);
                
                // move top of pickup pile onto top of discard pile
                // get rand uncovered index, flip card
                
                discard_pile.push_back(pickup_pile[pickup_pile.size() - 1]);
                pickup_pile.pop_back();
                
                size_t rand_index = get_rand_uncovered_index(current_player);
                players_hands[current_player][rand_index].uncovered = true;
            }
            else
            {
                input_output_pair iop;
                iop.input = input;
                iop.output = output;
                io.push_back(iop);
                
                // get rand uncovered index
                // move hand card to top of discard pile
                // move pickup pile top card to hand card
                
                size_t rand_index = get_rand_uncovered_index(current_player);
                players_hands[current_player][rand_index].uncovered = true;
                
                discard_pile.push_back(players_hands[current_player][rand_index]);
                players_hands[current_player][rand_index] = pickup_pile[pickup_pile.size() - 1];
                pickup_pile.pop_back();
            }
        }
        
        if(current_player == NUM_PLAYERS - 1)
            current_player = 0;
        else
            current_player++;
    }
    
    size_t get_rand_uncovered_index(const size_t player_index)
    {
        if(player_index >= NUM_PLAYERS)
            return 0;

        vector<size_t> uncovered_positions;
        
        for(size_t i = 0; i < NUM_CARDS_PER_HAND; i++)
            if(false == players_hands[player_index][i].uncovered)
                uncovered_positions.push_back(i);
        
        if(0 == uncovered_positions.size())
            return 0;
        
        return uncovered_positions[rand() % uncovered_positions.size()];
    }
    
    void sort_hand(const size_t player_index)
    {
        if(player_index >= NUM_PLAYERS)
            return;
        
        sort(players_hands[player_index].begin(), players_hands[player_index].end());
    }
    
    size_t get_best_rank(void)
    {
        size_t best_rank = 0;
        size_t best_rank_player = 0;
        
        for(size_t i = 0; i < NUM_PLAYERS; i++)
        {
            if(rank_hand(i) > best_rank)
            {
                best_rank = rank_hand(i);
                best_rank_player = i;
            }
        }
        
        return best_rank_player;
    }
    
    size_t rank_hand(const size_t player_index) const
    {
        if(player_index >= NUM_PLAYERS)
            return 0;
        
        size_t ret = 0;
        
        vector<card> temp_hand = players_hands[player_index];
        
        sort(temp_hand.begin(), temp_hand.end());

        // Note: FACE_A is defined to be 12
        size_t offset = FACE_A + 1;
        
        for(size_t i = 0; i < NUM_CARDS_PER_HAND; i++)
        {
            ret += temp_hand[i].face * offset;
            offset *= FACE_A + 1;
        }
        
        return ret;
    }
    
    void swap_cards(card &a, card &b)
    {
        card temp_card = a;
        a = b;
        b = temp_card;
    }
    
    size_t current_player;
    vector< vector < card > > players_hands;
    vector<card> discard_pile;
    vector<card> pickup_pile;
};






#endif
