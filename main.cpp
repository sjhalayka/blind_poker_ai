#include "ffbpneuralnet.h"
#include "cards.h"

#include <iostream>
using std::cout;
using std::endl;

#include <sstream>
using std::ostringstream;

#include <ctime>




int main(void)
{
	srand(static_cast<unsigned int>(time(0)));
	//srand(123);


    double max_error_rate = 0.00001;
    double error_rate = 0;
    
    size_t max_training_sessions = 10000;
	size_t num_training_sessions = 0;
    
    vector<FFBPNeuralNet> NNets;
    
    for(size_t i = 0; i < NUM_PLAYERS - 1; i++)
    {
        // create a network of 208 input neurons, one hidden layer of 14 neurons, and 1 output neuron
        vector<size_t> HiddenLayers;
        HiddenLayers.push_back(14);
        FFBPNeuralNet NNet(208, HiddenLayers, 1);
        NNet.SetLearningRate(1.0);
        NNet.SetMomentum(1.0);
        
        NNets.push_back(NNet);
    }
    
    do
    {
        // keep track of card states / binary choices
        
        // have a different copy of the AI for 2 players, 3 players, 4 players, 5 players
        
        // have a different copy of the AI for 1st player, 2nd, 3rd, etc.
        
        // 2 players: HUMAN (rand) vs ANN1
        // 3 players: HUMAN (rand) vs ANN1 ANN2
        // 4 players: HUMAN (rand) vs ANN1 ANN2 ANN3
        // 5 players: HUMAN (rand) vs ANN1 ANN2 ANN3 ANN4
        //
        // total 1 + 2 + 3 + 4 = 10 ANNs
        
        if(num_training_sessions % 10 == 0)
            cout << num_training_sessions << endl;
        
        // play game
        blind_poker_table bpt;
        
        vector< vector<input_output_pair> > nnet_io;
        nnet_io.resize(NUM_PLAYERS - 1);
        
        for(size_t i = 0; i < NUM_CARDS_PER_HAND; i++)
        {
            bpt.play_rand();
            
            for(size_t j = 1; j < NUM_PLAYERS; j++)
                bpt.play_ANN(nnet_io[j - 1], NNets[j - 1]);
        }

        // Determine the winner
        size_t index = bpt.get_best_rank();

        error_rate = 0;

        // for each ANN
        for(size_t i = 1; i < NUM_PLAYERS; i++)
        {
            // if winner, do nothing
            if(index == i)
                continue;
            
            // if loser, switch ~0 for 1 and ~1 for 0
            for(size_t j = 0; j < nnet_io[i - 1].size(); j++)
            {
                if(0 == floor(nnet_io[i - 1][j].output[0] + 0.5))
                    nnet_io[i - 1][j].output[0] = 1;
                else
                    nnet_io[i - 1][j].output[0] = 0;
            }
            
            // train network using nnet_io
            for(size_t j = 0; j < nnet_io[i - 1].size(); j++)
            {
                NNets[i - 1].FeedForward(nnet_io[i - 1][j].input);
                error_rate += NNets[i - 1].BackPropagate(nnet_io[i - 1][j].output);
            }

            if(0 != nnet_io[i - 1].size())
                error_rate /= nnet_io[i - 1].size();
            
            cout << "Error rate = " << error_rate << endl;
        }
  
        
        
        
        
     /*
        // An AI won
        if(index > 0)
        {
            for(size_t i = 0; i < nnet_io[index - 1].size(); i++)
            {
                // this does nothing to the weights, it's a waste of time and space
                NNets[index - 1].FeedForward(nnet_io[index - 1][i].input);
                error_rate += NNets[index - 1].BackPropagate(nnet_io[index - 1][i].output);
            }
            
            cout << error_rate / nnet_io[index - 1].size() << endl;
        }
       */
                    
                    
        num_training_sessions++;
    }
    while(/*error_rate >= max_error_rate &&*/ num_training_sessions < max_training_sessions);
    

    
    
    for(size_t i = 0; i < NNets.size(); i++)
    {
        ostringstream oss;
        
        oss << NUM_PLAYERS << "_players_" << "player_" << (i + 2) << ".bin";
        
        NNets[i].SaveToFile(oss.str().c_str());
        cout << oss.str() << endl;
        
    }


    
    
    
    
    return 0;
}
