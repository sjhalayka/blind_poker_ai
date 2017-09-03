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
                
#ifdef USE_ONE_HOT_INPUT_ENCODING
                
            // 9 neurons per state
            
            case POSITION_HAND0:
            {
                // 0
                states.push_back(1);
                states.push_back(0);
                states.push_back(0);
                states.push_back(0);
                states.push_back(0);
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
                states.push_back(1);
                states.push_back(0);
                states.push_back(0);
                states.push_back(0);
                states.push_back(0);
                states.push_back(0);
                states.push_back(0);
                states.push_back(0);
                break;
            }
            case POSITION_HAND2:
            {
                // 2
                states.push_back(0);
                states.push_back(0);
                states.push_back(1);
                states.push_back(0);
                states.push_back(0);
                states.push_back(0);
                states.push_back(0);
                states.push_back(0);
                states.push_back(0);
                break;
            }
            case POSITION_HAND3:
            {
                // 3
                states.push_back(0);
                states.push_back(0);
                states.push_back(0);
                states.push_back(1);
                states.push_back(0);
                states.push_back(0);
                states.push_back(0);
                states.push_back(0);
                states.push_back(0);
                break;
            }
            case POSITION_HAND4:
            {
                // 4
                states.push_back(0);
                states.push_back(0);
                states.push_back(0);
                states.push_back(0);
                states.push_back(1);
                states.push_back(0);
                states.push_back(0);
                states.push_back(0);
                states.push_back(0);
                break;
            }
            case POSITION_DISCARD_PILE:
            {
                // 5
                states.push_back(0);
                states.push_back(0);
                states.push_back(0);
                states.push_back(0);
                states.push_back(0);
                states.push_back(1);
                states.push_back(0);
                states.push_back(0);
                states.push_back(0);
                break;
            }
            case POSITION_TOP_OF_DISCARD_PILE:
            {
                // 6
                states.push_back(0);
                states.push_back(0);
                states.push_back(0);
                states.push_back(0);
                states.push_back(0);
                states.push_back(0);
                states.push_back(1);
                states.push_back(0);
                states.push_back(0);
                break;
            }
            case POSITION_TOP_OF_PICKUP_PILE:
            {
                // 7
                states.push_back(0);
                states.push_back(0);
                states.push_back(0);
                states.push_back(0);
                states.push_back(0);
                states.push_back(0);
                states.push_back(0);
                states.push_back(1);
                states.push_back(0);
                break;
            }
            case POSITION_UNCOVERED:
            {
                // 8
                states.push_back(0);
                states.push_back(0);
                states.push_back(0);
                states.push_back(0);
                states.push_back(0);
                states.push_back(0);
                states.push_back(0);
                states.push_back(0);
                states.push_back(1);
                break;
            }

#else
            // 4 neurons per state
                
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
                
#endif
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

size_t blind_poker_table::get_rand_uncovered_index(const size_t player_index) const
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

// need to handle †ies
size_t blind_poker_table::get_best_rank(void) const
{
    size_t best_rank = 0;
    size_t best_numeric_rank = 0;
    size_t best_rank_player = 0;
    
    // doesn't deal with ties yet
    for(size_t i = 0; i < NUM_PLAYERS; i++)
    {
        if(rank_finished_hand(i) >= best_rank)
        {
            if(numeric_rank_finished_hand(i) < best_numeric_rank)
                continue;
            
            best_rank = rank_finished_hand(i);
            best_numeric_rank = numeric_rank_finished_hand(i);
            best_rank_player = i;
        }
    }
    
    return best_rank_player;
}

size_t blind_poker_table::rank_finished_hand(const size_t player_index) const
{
    if(player_index >= NUM_PLAYERS)
        return 0;
 
    for(size_t i = 0; i < NUM_CARDS_PER_HAND; i++)
    {
        if(false == players_hands[player_index][i].uncovered)
        {
            cout << "Tried to rank unfinished hand!" << endl;
            return 0;
        }
    }
    
    size_t ret = 0;
    
    if(is_finished_hand_royal_flush(player_index))
        ret = ROYAL_FLUSH;
    else if(is_finished_hand_straight_flush(player_index))
        ret = STRAIGHT_FLUSH;
    else if(is_finished_hand_4_of_a_kind(player_index))
        ret = FOUR_OF_A_KIND;
    else if(is_finished_hand_full_house(player_index))
        ret = FULL_HOUSE;
    else if(is_finished_hand_flush(player_index))
        ret = FLUSH;
    else if(is_finished_hand_straight(player_index))
        ret = STRAIGHT;
    else if(is_finished_hand_3_of_a_kind(player_index))
        ret = THREE_OF_A_KIND;
    else if(is_finished_hand_2_pair(player_index))
        ret = TWO_PAIR;
    else if(is_finished_hand_1_pair(player_index))
        ret = ONE_PAIR;
    else if(is_finished_hand_high_card(player_index))
        ret = HIGH_CARD;

    return ret;
}

size_t blind_poker_table::numeric_rank_finished_hand(const size_t player_index) const
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

bool blind_poker_table::is_finished_hand_royal_flush(const size_t player_index) const
{
    if(player_index >= NUM_PLAYERS)
        return 0;
    
    vector<card> temp_hand = players_hands[player_index];
    sort(temp_hand.begin(), temp_hand.end());
    
    if(false == is_finished_hand_flush(player_index))
        return false;
    
    if(FACE_A != temp_hand[4].face ||
       FACE_K != temp_hand[3].face ||
       FACE_Q != temp_hand[2].face ||
       FACE_J != temp_hand[1].face ||
       FACE_10 != temp_hand[0].face)
        return false;
    
    return true;
}

bool blind_poker_table::is_finished_hand_straight_flush(const size_t player_index) const
{
    if(player_index >= NUM_PLAYERS)
        return 0;
    
    if(false == is_finished_hand_straight(player_index) || false == is_finished_hand_flush(player_index))
        return false;
    
    return true;
}

bool blind_poker_table::is_finished_hand_4_of_a_kind(const size_t player_index) const
{
    if(player_index >= NUM_PLAYERS)
        return 0;
    
    vector<card> temp_hand = players_hands[player_index];
    sort(temp_hand.begin(), temp_hand.end());
    
    map<size_t, size_t> unique_faces;
    
    for(size_t i = 0; i < NUM_CARDS_PER_HAND; i++)
        unique_faces[temp_hand[i].face]++;
    
    bool found_four = false;
    
    for(map<size_t, size_t>::const_iterator ci = unique_faces.begin(); ci != unique_faces.end(); ci++)
    {
        if(4 == ci->second)
            found_four = true;
    }
        
    return found_four;
}

bool blind_poker_table::is_finished_hand_full_house(const size_t player_index) const
{
    if(player_index >= NUM_PLAYERS)
        return 0;
    
    vector<card> temp_hand = players_hands[player_index];
    sort(temp_hand.begin(), temp_hand.end());
    
    map<size_t, size_t> unique_faces;
    
    for(size_t i = 0; i < NUM_CARDS_PER_HAND; i++)
        unique_faces[temp_hand[i].face]++;
    
    bool found_two = false;
    bool found_three = false;
    
    for(map<size_t, size_t>::const_iterator ci = unique_faces.begin(); ci != unique_faces.end(); ci++)
    {
        if(3 == ci->second)
            found_three = true;
        else if(2 == ci->second)
            found_two = true;
    }
    
    if(true == found_three && true == found_two)
        return true;
    
    return false;
}

bool blind_poker_table::is_finished_hand_flush(const size_t player_index) const
{
    if(player_index >= NUM_PLAYERS)
        return 0;
    
    vector<card> temp_hand = players_hands[player_index];
    sort(temp_hand.begin(), temp_hand.end());

    size_t suit = temp_hand[0].suit;
    
    for(size_t i = 1; i < NUM_CARDS_PER_HAND; i++)
        if(temp_hand[i].suit != suit)
            return false;
    
    return true;
}

bool blind_poker_table::is_finished_hand_straight(const size_t player_index) const
{
    if(player_index >= NUM_PLAYERS)
        return 0;
    
    vector<card> temp_hand = players_hands[player_index];
    sort(temp_hand.begin(), temp_hand.end());
    
    if(FACE_A == temp_hand[4].face &&
       FACE_5 == temp_hand[3].face &&
       FACE_4 == temp_hand[2].face &&
       FACE_3 == temp_hand[1].face &&
       FACE_2 == temp_hand[0].face)
        return true;
    
    for(size_t i = 1; i < NUM_CARDS_PER_HAND; i++)
        if(temp_hand[i].face != temp_hand[i - 1].face + 1)
            return false;
    
    return true;
}

bool blind_poker_table::is_finished_hand_3_of_a_kind(const size_t player_index) const
{
    if(player_index >= NUM_PLAYERS)
        return 0;
    
    vector<card> temp_hand = players_hands[player_index];
    sort(temp_hand.begin(), temp_hand.end());
    
    map<size_t, size_t> unique_faces;
    
    for(size_t i = 0; i < NUM_CARDS_PER_HAND; i++)
        unique_faces[temp_hand[i].face]++;
    
    bool found_three = false;
    
    for(map<size_t, size_t>::const_iterator ci = unique_faces.begin(); ci != unique_faces.end(); ci++)
    {
        if(3 == ci->second)
            found_three = true;
    }
    
    return found_three;
}

bool blind_poker_table::is_finished_hand_2_pair(const size_t player_index) const
{
    if(player_index >= NUM_PLAYERS)
        return 0;
    
    vector<card> temp_hand = players_hands[player_index];
    sort(temp_hand.begin(), temp_hand.end());
    
    map<size_t, size_t> unique_faces;
    
    for(size_t i = 0; i < NUM_CARDS_PER_HAND; i++)
        unique_faces[temp_hand[i].face]++;
    
    size_t num_pair_found = 0;
    
    for(map<size_t, size_t>::const_iterator ci = unique_faces.begin(); ci != unique_faces.end(); ci++)
    {
        if(2 == ci->second)
            num_pair_found++;
    }
    
    if(2 == num_pair_found)
        return true;
    
    return false;
}

bool blind_poker_table::is_finished_hand_1_pair(const size_t player_index) const
{
    if(player_index >= NUM_PLAYERS)
        return 0;
    
    vector<card> temp_hand = players_hands[player_index];
    sort(temp_hand.begin(), temp_hand.end());
    
    map<size_t, size_t> unique_faces;
    
    for(size_t i = 0; i < NUM_CARDS_PER_HAND; i++)
        unique_faces[temp_hand[i].face]++;
    
    size_t num_pair_found = 0;
    
    for(map<size_t, size_t>::const_iterator ci = unique_faces.begin(); ci != unique_faces.end(); ci++)
    {
        if(2 == ci->second)
            num_pair_found++;
    }
    
    if(1 == num_pair_found)
        return true;
    
    return false;
}

bool blind_poker_table::is_finished_hand_high_card(const size_t player_index) const
{
    return true;
}
