// CusumARL.cpp : Defines the entry point for the console application.

#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <string>
#include <vector>
#include <time.h>  
#include <chrono>
#include "CusumARL.h"
#include "stdafx.h"
#include <tclap/CmdLine.h>
#include "RangeConstraint.h"
using namespace std;

static void show_usage(std::string name)
{
	std::cerr << " Usage: " << name << "\n<option(s)> SOURCES\n\n"
		<< "Required options (caps indicates minimum to be typed):\n"
		<< "  -H\t\tThreshold\n"
		<< "  -P0\t\tPredicted probability\n"
		<< "Optional:\n"
		<< "  -P1\t\tRandomly generated probability (binomial distributed).\n\t\tIf not provided, will be calculated from oddsratio and p0\n"
		<< "  -Iterations\ttotal number of times to create a new observaion.\n\t\tDefault 10 000\n"
		<< "  -Oddsratio\tthe odds ratio used in the calculation of the log likelihood.\n\t\tDefault 2.0\n"
		<< "  -HEAdstart\tFraction of h to reset to (FIR CUSUM).\n\t\tIf omitted calculates standard CUSUM\n"
		<< "  -Seed\t\tRandom number seed.\n"
		//<< "  -Prf\t\tProbability ratio formula. .\n\t\tDefault ."  to account for inclusion of log OR or not - currently not implemented
		<< std::endl;
}

int main(int argc,      // Number of strings in array argv
	char *argv[],   // Array of command-line argument strings
	char *envp[])  // Array of environment variable strings
{
	using namespace CusumARL;

	double h, p0, p1, headstart, odds;
	unsigned long it;
	unsigned int seed;
	int full;

	try {

		TCLAP::CmdLine cmd("Run monte carlo simulations on SPRT CUSUMS to determine Average Run Length (ARL) before threshold exceeded.\nkey:\tR-real numbers, Z-integers, N-natural numbers(+ve integer)\n\t(a,b)-range excluding bounds a<x<b\n\t[a,b]-range inclusive of a & b", ' ', "0.1");

		//while probabilities can have values of 0 or 1, it makes no sense to test such probabilities
		RangeConstraint<double> probConstraint = RangeConstraint<double>(0.0, 1.0, RangeBounds::excludeBounds);
		RangeConstraint<double> gt0 = RangeConstraint<double>::GT(0.0);
		RangeConstraint<double> headstartConstraint = RangeConstraint<double>(0.0, 1.0, RangeBounds::includeMin);

		TCLAP::ValueArg<double> p0Arg("p", "p0", "Predicted probability", false, 0.0, &probConstraint);
		TCLAP::ValueArg<double> hArg("t", "threshold", "Decision threshold (h)", true, 0.0, &gt0);
		TCLAP::ValueArg<double> p1Arg("g", "gen", "Randomly generated probability", false, 0.0, &probConstraint);

		TCLAP::ValueArg<unsigned long> itArg("i", "iterations", "No. of times to create a random observation", false, 10000, "N");
		TCLAP::ValueArg<unsigned int> seedArg("s", "seed", "Random number seed", false, 0, "N");
		TCLAP::ValueArg<double> orArg("o", "or", "Odds Ratio to calculate SPRT", false, 2.0, &gt0);
		TCLAP::ValueArg<double> headstartArg("r", "reset", "Head start (as fraction of decision threshold) to reset to (FIR CUSUM)", false, 0.0, &headstartConstraint);

		TCLAP::SwitchArg fullArg("f","f", "calculate ARL0 and ARL1");
		
		// Add the argument nameArg to the CmdLine object. The CmdLine object
		// uses this Arg to parse the command line.
		cmd.add(p0Arg);
		cmd.add(p1Arg);

		cmd.add(hArg); 
		cmd.add(itArg);
		cmd.add(seedArg); 
		cmd.add(orArg); 
		cmd.add(headstartArg);

		cmd.add(fullArg);

		// Parse the argv array.
		cmd.parse(argc, argv);

		// Get the value parsed by each arg. 
		h = hArg.getValue();
		p0 = p0Arg.getValue();
		p1 = p1Arg.getValue();
		headstart = headstartArg.getValue();
		odds = orArg.getValue();
		it=itArg.getValue();
		full = fullArg.getValue();

		seed = seedArg.getValue();
		if (seed == 0) { seed = (unsigned int)time(NULL); }
		srand(seed);

		if (p0 == 0.0 && p1 == 0.0) { //should add orAdd to tclap
			TCLAP::CmdLineOutput * o = cmd.getOutput();
			std::cerr << "PARSE ERROR\n\tRequired argument missing: either " << p0Arg.getName() << " or " << p1Arg.getName();
			o->usage(cmd);
			exit(0);
		}

	}
	catch (TCLAP::ArgException &e)  // catch any exceptions
	{
		std::cerr << "\n\nerror:\t" << e.error() << "\n\t" << e.argId() << std::endl;
		
#ifdef _DEBUG
		std::cout << "\n\nPress any key to close.";
		std::cin.get();
#endif
		exit(0);
	}

	ProbabilityRatioFormula formula(ProbabilityRatioFormula::LogLikelihoodRatio);
	
	W deflect = Deflections(h, p0, p1, odds, formula);

	std::cout << "threshold (h)\t" << h
		<< "\npredicted (p0)\t" << p0
		<< "\nsimulated (p1)\t" << p1
		<< "\niterations\t" << it
		<< "\nodds ratio\t" << odds
		<< "\nhead start frac\t" << headstart
		<< "\nseed\t\t" << seed 
		<< "\n\ndeflection if 0\t" << deflect.If0 
		<< "\ndeflection if 1\t " << deflect.If1 << "\n\ncommencing...\n";

	do
	{
		ARL_params retval = MonteCarlo(h, p1, deflect,it,headstart);
		std::cout << std::string(40,'-') << "\nthreshold exceeded: " << retval.Runs 
			<< " times\nlast exceeded at iteration number: " << retval.LastThreshold 
			<< "\nminimum run length: " << retval.MinRun
			<< "\nmaximum run length: " << retval.MaxRun;
		if (retval.Runs > 0) {
			double arl = (double)retval.LastThreshold / retval.Runs;
			std::cout << "\n~failures to signal (arl*p1): " << arl*p1
				<< "\n\nARL:" << arl << "\n";
		}
		std::cout << "\nSimulation took: " << retval.Seconds << " Seconds" << std::endl;

		if (full > 0) {
			p1 = p0;
			std::cout << "\np1 = p0 = " << p0 << "\n";
		}
		
	} while (--full >= 0);
	

	
#ifdef _DEBUG
	std::cout << "\n\nPress any key to close.";
	std::cin.get();
#endif
	return 0;
}

namespace CusumARL
{

	inline W Deflections(double h, double &p0, double &p1, double oddsRatio, ProbabilityRatioFormula formula)
	{
		if (p1 == 0.0 && p0 == 0.0) {
			throw(invalid_argument("p0 and p1 cannot both be 0"));
		}
		if (p1 == 0.0) {
			p1 = oddsRatio*p0 / (oddsRatio*p0 + 1 - p0);
		}
		else if (p0 == 0.0) {
			p0 = p1 / (oddsRatio + p1 - oddsRatio*p1);
		}

		W returnVal = W();

		switch (formula) {

		default:
			returnVal.If0 = log(1.0 / (oddsRatio*p0 + 1 - p0));
			returnVal.If1 = log(oddsRatio / (oddsRatio*p0 + 1 - p0));
			break;
		}

		return returnVal;
	}

	ARL_params MonteCarlo(double h, double p, W w, long iterations, double headstart)
	{
		using namespace std::chrono;
		ARL_params returnVal = ARL_params();
		
		long cutpt = (long)(RAND_MAX*p + 0.5);

		double resetTo = headstart*h;
		double ti = resetTo; 

		long runLength;

		high_resolution_clock::time_point t1 = high_resolution_clock::now();
		for (long i = 1; i <= iterations; i++) {
			ti += (rand() < cutpt ? w.If1:w.If0);
			if (ti < 0) {
				ti = 0;
			}
			else if (ti >= h) {
				++returnVal.Runs;
				runLength = i - returnVal.LastThreshold;
				if (runLength > returnVal.MaxRun) {
					returnVal.MaxRun = runLength;
				}
				else if (runLength < returnVal.MinRun) {
					returnVal.MinRun = runLength;
				}
				returnVal.LastThreshold = i;
				ti = resetTo;
			}
		}
		high_resolution_clock::time_point t2 = high_resolution_clock::now();

		returnVal.Seconds = duration_cast<duration<double>>(t2 - t1).count();
		if (returnVal.MinRun == LONG_MAX) { returnVal.MinRun = 0; }
		return returnVal;
	}

}

