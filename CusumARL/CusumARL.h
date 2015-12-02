#pragma once
#ifndef __cusum_arl_included
#define __cusum_arl_included
#include <stdlib.h>
namespace CusumARL
{

	struct ARL_params
	{
		long Runs;
		long LastThreshold;
		double Seconds;
	};
	enum ProbabilityRatioFormula { LogLikelihoodRatio };
	struct W
	{
		double If0;
		double If1;
	};
	/*
	* @param h - threshold
	* @param p0 - predicted probability
	* @param p1 - randomly generated probability (binomial distributed). If set at 0, will calculate from oddsratio and p0
	* @param iterations - total number of times to create a new observaion
	* @param oddsratio - the odds ratio used in the calculation of the wald log likelihood.
	* @param headstart - for FIR CUSUMS - leave default of 0 to reset back to 0 (standard CUSUM)
	*/
	ARL_params MonteCarlo(double h, double p, W w, long iterations, double headstart);
	inline W Deflections(double h, double p0, double &p1, double oddsRatio, ProbabilityRatioFormula formula);

}
#endif