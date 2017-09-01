#include "cards.h"

bool card::operator<(const card &rhs) const
{
    if(card_id < rhs.card_id)
        return true;
    
    return false;
}

void card::print(void) const
{
    switch(face)
    {
        case FACE_2: { cout << "2"; break; }
        case FACE_3: { cout << "3"; break; }
        case FACE_4: { cout << "4"; break; }
        case FACE_5: { cout << "5"; break; }
        case FACE_6: { cout << "6"; break; }
        case FACE_7: { cout << "7"; break; }
        case FACE_8: { cout << "8"; break; }
        case FACE_9: { cout << "9"; break; }
        case FACE_10: { cout << "10"; break; }
        case FACE_J: { cout << "J"; break; }
        case FACE_Q: { cout << "Q"; break; }
        case FACE_K: { cout << "K"; break; }
        case FACE_A: { cout << "A"; break; }
    }
    
    switch(suit)
    {
        case SUIT_HEARTS: { cout << "H"; break; }
        case SUIT_SPADES: { cout << "S"; break; }
        case SUIT_DIAMONDS: { cout << "D"; break; }
        case SUIT_CLUBS: { cout << "C"; break; }
    }
}

blind_poker_table::blind_poker_table(void)
{
    reset_table();
    current_player = 0;
}

void blind_poker_table::reset_table(void)
{
    players_hands.clear();
    discard_pile.clear();
    pickup_pile.clear();
    
    // initialize the deck
    size_t id = 0;

    for(size_t i = FACE_2; i <= FACE_A; i++)
    {
        for(size_t j = SUIT_HEARTS; j <= SUIT_CLUBS; j++)
        {
            card c;
            
            c.card_id = id++;
            c.suit = j;
            c.face = i;
            c.uncovered = false;
            
            pickup_pile.push_back(c);
        }
    }
    
    // shuffle the deck
    for(size_t i = 0; i < 100000; i++)
    {
        size_t first_pos = rand() % NUM_CARDS_PER_DECK;
        size_t second_pos = rand() % NUM_CARDS_PER_DECK;
        
        card temp_card = pickup_pile[first_pos];
        pickup_pile[first_pos] = pickup_pile[second_pos];
        pickup_pile[second_pos] = temp_card;
    }
    
    // deal cards to each player
    players_hands.resize(NUM_PLAYERS);
    
    for(size_t i = 0; i < NUM_CARDS_PER_HAND; i++)
    {
        for(size_t j = 0; j < NUM_PLAYERS; j++)
        {
            players_hands[j].push_back(pickup_pile[pickup_pile.size() - 1]);
            pickup_pile.pop_back();
        }
    }
    
    // flip a card off of the pickup pile onto the discard pile
    discard_pile.push_back(pickup_pile[pickup_pile.size() - 1]);
    discard_pile[0].uncovered = true;
    pickup_pile.pop_back();
}

void blind_poker_table::print_table(void)
{
    for(size_t i = 0; i < NUM_PLAYERS; i++)
    {
        cout << "Player " << i + 1 << ": ";
        
        // Check all cards in hand
        for(size_t j = 0; j < NUM_CARDS_PER_HAND; j++)
        {
            players_hands[i][j].print();
            
            if(false == players_hands[i][j].uncovered)
                cout << "* ";
            else
                cout << ' ';
        }
        
        cout << endl;
    }
    
    cout << "Discard pile: ";
    
    for(size_t i = 0; i < discard_pile.size(); i++)
    {
        discard_pile[i].print();
        cout << ' ';
    }
    
    cout << endl;
    
    cout << "Top of pickup pile: ";
    
    pickup_pile[pickup_pile.size() - 1].print();
    
    if(false == pickup_pile[pickup_pile.size() - 1].uncovered)
        cout << '*';
    
    cout << endl << endl;
}

void blind_poker_table::get_card_states(vector<double> &states)
{
    states.clear();
    
    for(size_t card_id = 0; card_id < NUM_CARDS_PER_DECK; card_id++)
    {
        bool found_card = false;
        size_t position = POSITION_HAND0;
        
        // Check players' hands
        for(size_t i = 0; i < NUM_PLAYERS; i++)
        {
            if(true == found_card)
                break;
            
            // Check all cards in hand
            for(size_t j = 0; j < NUM_CARDS_PER_HAND; j++)
            {
                if(true == found_card)
                    break;
                
                if(card_id == players_hands[i][j].card_id)
                {
                    if(true == players_hands[i][j].uncovered)
                    {
                        //cout << "POSITION_HAND0 + " << i << endl;
                        position = POSITION_HAND0 + i;
                    }
                    else
                    {
                        //cout << "POSITION_UNCOVERED" << endl;
                        position = POSITION_UNCOVERED;
                    }
                    
                    found_card = true;
                }
            }
        }
        
        // Check top of discard pile
        if(false == found_card && card_id == discard_pile[discard_pile.size() - 1].card_id)
        {
            //cout << "POSITION_TOP_OF_DISCARD_PILE" << endl;
            position = POSITION_TOP_OF_DISCARD_PILE;
            found_card = true;
        }
        
        // Check all except for top discard pile
        for(size_t i = 0; i < discard_pile.size() - 1; i++)
        {
            if(true == found_card)
                break;
            
            if(card_id == discard_pile[i].card_id)
            {
                //cout << "POSITION_DISCARD_PILE" << endl;
                position = POSITION_DISCARD_PILE;
                found_card = true;
            }
        }
        
        
        // check top of pickup pile, if it's uncovered that is
        if(false == found_card && true == pickup_pile[pickup_pile.size() - 1].uncovered && card_id == pickup_pile[pickup_pile.size() - 1].card_id)
        {
            //cout << "POSITION_TOP_OF_PICKUP_PILE" << endl;
            position = POSITION_TOP_OF_PICKUP_PILE;
            found_card = true;
        }
        
        // if not found by now, it's uncovered in the pickup pile
        if(false == found_card)
        {
            //cout << "POSITION_UNCOVERED 2" << endl;
            position = POSITION_UNCOVERED;
        }
        
        switch(position)
        {
            case POSITION_HAND0:
            {
                // 0
                states.push_back(0);
                states.push_back(0);
                states.push_back(0);
                states.push_back(0);
                break;
            }
            case POSITION_HAND1:
            {
                // 1
                states.push_back(0);
                states.push_back(0);
                states.push_back(0);
                states.push_back(1);
                break;
            }
            case POSITION_HAND2:
            {
                // 2
                states.push_back(0);
                states.push_back(0);
                states.push_back(1);
                states.push_back(0);
                break;
            }
            case POSITION_HAND3:
            {
                // 3
                states.push_back(0);
                states.push_back(0);
                states.push_back(1);
                states.push_back(1);
                break;
            }
            case POSITION_HAND4:
            {
                // 4
                states.push_back(0);
                states.push_back(1);
                states.push_back(0);
                states.push_back(0);
                break;
            }
            case POSITION_DISCARD_PILE:
            {
                // 5
                states.push_back(0);
                states.push_back(1);
                states.push_back(0);
                states.push_back(1);
                break;
            }
            case POSITION_TOP_OF_DISCARD_PILE:
            {
                // 6
                states.push_back(0);
                states.push_back(1);
                states.push_back(1);
                states.push_back(0);
                break;
            }
            case POSITION_TOP_OF_PICKUP_PILE:
            {
                // 7
                states.push_back(0);
                states.push_back(1);
                states.push_back(1);
                states.push_back(1);
                break;
            }
            case POSITION_UNCOVERED:
            {
                // 8
                states.push_back(1);
                states.push_back(0);
                states.push_back(0);
                states.push_back(0);
                break;
            }
        }
    }
}

void blind_poker_table::play_rand(void)
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

void blind_poker_table::play_ANN(vector<input_output_pair> &io, FFBPNeuralNet &NNet)
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

size_t blind_poker_table::get_rand_uncovered_index(const size_t player_index)
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

void blind_poker_table::sort_hand(const size_t player_index)
{
    if(player_index >= NUM_PLAYERS)
        return;
    
    sort(players_hands[player_index].begin(), players_hands[player_index].end());
}

size_t blind_poker_table::get_best_rank(void)
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

size_t blind_poker_table::rank_hand(const size_t player_index) const
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

void blind_poker_table::swap_cards(card &a, card &b)
{
    card temp_card = a;
    a = b;
    b = temp_card;
}



