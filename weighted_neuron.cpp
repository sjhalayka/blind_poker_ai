#include "weighted_neuron.h"


#include <stdexcept>
using std::out_of_range;


WeightedNeuron::WeightedNeuron(const size_t &src_num_inputs)
{
	if(src_num_inputs == 0)
        throw out_of_range("Invalid number of inputs.");

	weights.resize(src_num_inputs);
	previous_weight_adjustments.resize(src_num_inputs, 0.0);

	RandomizeWeights();

	bias = 1.0;
	value = 0.0;
}

size_t WeightedNeuron::GetNumInputs(void) const
{
	return weights.size();
}

void WeightedNeuron::ResetNumInputs(const size_t &src_num_inputs)
{
	if(src_num_inputs == 0)
        throw out_of_range("Invalid number of inputs.");

	size_t temp_weights_size = weights.size();

	if(src_num_inputs < temp_weights_size)
	{
		for(size_t i = 0; i < temp_weights_size - src_num_inputs; i++)
		{
			weights.pop_back();
			previous_weight_adjustments.pop_back();
		}
	}
	else if(src_num_inputs > temp_weights_size)
	{
		for(size_t i = 0; i < src_num_inputs - temp_weights_size; i++)
		{
			weights.push_back(GetRandWeight());
			previous_weight_adjustments.push_back(0.0);
		}
	}
}

void WeightedNeuron::SetInputValues(const vector<double> &src_inputs)
{
	if(src_inputs.size() != weights.size())
		throw out_of_range("Invalid input vector size.");
	
	value = bias * bias_weight;

	for(size_t i = 0; i < src_inputs.size(); i++)
		value += src_inputs[i] * weights[i];

	value = WeightedNeuron::ActivationFunction(value);
}

void WeightedNeuron::SetWeight(const size_t &index, const double &src_weight)
{
	if(index >= weights.size())
		throw out_of_range("Invalid weight index.");

	weights[index] = src_weight;
}

double WeightedNeuron::GetWeight(const size_t &index) const
{
	if(index >= weights.size())
		throw out_of_range("Invalid weight index.");

	return weights[index];
}

void WeightedNeuron::SetPreviousWeightAdjustment(const size_t &index, const double &src_weight_adjustment)
{
	if(index >= previous_weight_adjustments.size())
		throw out_of_range("Invalid previous weight adjustment index.");

	previous_weight_adjustments[index] = src_weight_adjustment;
}

double WeightedNeuron::GetPreviousWeightAdjustment(const size_t &index) const
{
	if(index >= previous_weight_adjustments.size())
		throw out_of_range("Invalid previous weight adjustment index.");

	return previous_weight_adjustments[index];
}

void WeightedNeuron::SetBiasWeight(const double &src_bias_weight)
{
	bias_weight = src_bias_weight;
}

double WeightedNeuron::GetBiasWeight(void) const
{
	return bias_weight;
}

double WeightedNeuron::GetValue(void) const
{
	return value;
}

void WeightedNeuron::SetBias(const double &src_bias)
{
	bias = src_bias;
}

double WeightedNeuron::GetBias(void) const
{
	return bias;
}

void WeightedNeuron::RandomizeWeights(void)
{
	for(vector<double>::iterator i = weights.begin(); i != weights.end(); i++)
		*i = GetRandWeight();  // from -1.0 to 1.0

	bias_weight = GetRandWeight();
}

void WeightedNeuron::PerturbWeights(const double scale)
{
    for(vector<double>::iterator i = weights.begin(); i != weights.end(); i++)
        *i += GetRandWeight()*scale;

    bias_weight += GetRandWeight()*scale;
}




