#ifndef WEIGHTED_NEURON_H
#define WEIGHTED_NEURON_H


#include <vector>
using std::vector;

#include <cmath>
#include <cstdlib>

class WeightedNeuron
{
public:
	WeightedNeuron(const size_t &src_num_inputs);
	size_t GetNumInputs(void) const;
	void ResetNumInputs(const size_t &src_num_inputs);
	void SetInputValues(const vector<double> &src_inputs);
	void SetWeight(const size_t &index, const double &src_weight);
	double GetWeight(const size_t &index) const;
	void SetPreviousWeightAdjustment(const size_t &index, const double &src_weight_adjustment);
	double GetPreviousWeightAdjustment(const size_t &index) const;
	void SetBiasWeight(const double &src_bias_weight);
	double GetBiasWeight(void) const;
	double GetValue(void) const;
	void SetBias(const double &src_bias);
	double GetBias(void) const;
	void RandomizeWeights(void);
    void PerturbWeights(const double scale);
    
	// logistic function
	static inline double ActivationFunction(const double &x)
	{
		return 1.0 / (1.0 + exp(-x));
	}
    
    // the input for this function is not x, but f(x)
	// this is because the logistic function f(x)'s derivative is f(x) * (1 - f(x))
	// and f(x) is generally handy during error calculation, while x is not
	// if you use a different activation function, you may have to manually
	// recalculate x from f(x) and then get the derivative based on x, not f(x)
	static inline double DerivativeOfActivationFunction(const double &f_x)
	{
		return f_x * (1.0 - f_x);
    }
    
	inline double GetRandWeight(void) const
	{
		// get value from -1.0 to 1.0
		return (static_cast<double>(rand()%2001) / 1000.0) - 1.0;
	}
    
protected:
	double bias_weight, bias;
	vector<double> weights;
	vector<double> previous_weight_adjustments;
	double value;
};


#endif
