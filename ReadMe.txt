Run monte carlo simulations on Sequential Probability Ratio test (SPRT) CUSUMS to determine the average run length (ARL) before exceeding given threshold.

USAGE:

   cusumarl  [-r <R[0,1)>] [-o <R(0,)>] [-g <R(0,1)>] [-s <N>] [-i <N>] -t <R(0,)> -p <R(0,1)> [--] [--version] [-h]

   key: R-real numbers, Z-integers, N-natural numbers(+ve integer)
        (a,b)-range excluding bounds a<x<b
        [a,b]-range inclusive of a & b

Required:
   -t <R(0,)>,  --threshold <R(0,)> Decision threshold. Often denoted (h) on the y axis of the CUSUM graph.

   -p <R(0,1)>,  --p0 <R(0,1)> Predicted probability.
   
Optional:
   -r <R[0,1)>,  --reset <R[0,1)> Fraction of decision threshold (h) to reset to after threshold is exceeded (for Fast Initial Response or FIR CUSUM).
   Default is 0 (no FIR)

   -o <R(0,)>,  --or <R(0,)> Odds Ratio used to calculate the log likelihood ratio. That is, the ratio of observed to predicted outcomes under the alternative hypothesis.
   Default is 2.0

   -g <R(0,1)>,  --gen <R(0,1)> Randomly generated probability. The probability of a failure in the randomly generated sequence. 
   Default is the probability which would result in the odds ratio specified by --or -i.e. calculate ARL1. To calculate ARL0, assign the same value as --p0

   -s <N>,  --seed <N> Random number seed.
   Default is to use the system clock

   -i <N>,  --iterations <N> No. of times to create a random observation. 
   Default is 10 000

   --,  --ignore_rest Ignores the rest of the labelled arguments following this flag.

   --version Displays version information and exits.

   -h,  --help Displays usage information and exits.

Calculation:
   the log likelihood score for a given iteration is calculated as:
	log(1.0       / (oddsRatio*p0 + 1 - p0)) if outcome=0
	log(oddsRatio / (oddsRatio*p0 + 1 - p0)) if outcome=1

Examples:

		C:\Documents>cusumarl -p 0.1 -t 2

		threshold (h)   2
		predicted (p0)  0.1
		simulated (p1)  0.181818
		iterations      10000
		odds ratio      2
		head start frac 0
		seed            1449026759

		deflection if 0 -0.0953102
		deflection if 1  0.597837

		commencing...
		----------------------------------------
		threshold exceeded: 224 times
		last exceeded at iteration number: 9946
		minimum run length: 6
		maximum run length: 148
		~failures to signal (arl*p1): 8.07305

		ARL:44.4018

		Simulation took: 0.000926327 Seconds

	given simulation took around 1 millisecond to perform 10 thousand iterations, it is reasonable to run 100 million iterations, knowing that the console window will freeze for up to 10 seconds

		C:\Documents>cusumarl -p 0.1 -t 2 -i 100000000
		...
		threshold exceeded: 2251172 times
		last exceeded at iteration number: 99999985
		~failures to signal (arl*p1): 8.0766

		ARL:44.4213

		Simulation took: 4.4853 Seconds
		
		C:\Documents>cusumarl -p 0.1 -g 0.1  -t 2 -i 100000000

		threshold (h)   2
		predicted (p0)  0.1
		simulated (p1)  0.1
		iterations      100000000
		odds ratio      2
		head start frac 0
		seed            1449025393
		...
		threshold exceeded: 416819 times
		last exceeded at iteration number: 99999383
		minimum run length: 4
		maximum run length: 2774
		~failures to signal (arl*p1): 23.9911

		ARL:239.911

		Simulation took: 4.36145 Seconds
	 
	 That is, an ARL0 of 239.9 and ARL1 of 44.4. 
	 
	 To see the effect of a 50% head start:
		C:\Documents>cusumarl -p 0.1 -t 2 -r 0.5 -i 100000000
		...
		~failures to signal (arl*p1): 5.68874
		ARL:31.288

		C:\Documents>cusumarl -p 0.1 -g 0.1 -t 2 -r 0.5 -i 100000000
		...
		~failures to signal (arl*p1): 20.8856
		ARL:208.856

Notes on application:
	This program is incredibly simple, and is designed for performance. It is written in C++ and currently compiled in Windows for 32 and 64 bit operating systems, but should be easily portable to Mac and Linux environments. 

	It is licensed under the MIT licence.

Notes on using a console application within a windows environment for beginners:
	-Save the .exe (binary) file - select the 32 bit or 64 bit, and then rename the file CusumARL.exe
	-Note the folder the .exe file is saved in. It is easiest to copy the path from the 'address bar'.
	-Open the Command Prompt. Start Prompt -> All programs -> accessories
	-type (without square brackets):
		cd [folder path containing the .exe file]
	 *note that to paste the path after cd, you will have to use right mouse rather than Ctrl + v
	-type commands as per the examples above
	-To copy the output from the console window to your clipboard, right click the mouse, choose 'Mark' from the menu, select the required text with your mouse and then either hit the enter key, or click on the title bar of the console window (not within the window) and select 'Copy'.
