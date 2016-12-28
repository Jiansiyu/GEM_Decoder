//============================================================================
// Name        : GEM_Analysis.cpp
// Author      : Siyu Jian, Danning Di
// EMAIL       : jiansiyu@gmail.com   sj9va@virginia.edu
// Version     : V1.01
// Copyright   : Your copyright notice
// Description : Can be used to decode MPD data INFN Version 5, other versions include new type of JLab formate has not been added, will added latter.

//============================================================================

Compile Requirement:
	
	CERN ROOT 5.34.36 or higher(multithread supported)
	gcc 5.4.0 or higher

	all the test based on Ubuntu 16.04.4

Installation instruction:
	cd xx/GEM_Analysis/
	make clean 
	make

Run instruction:
	
./GEM_decoder 
	-h or --help	: print this instructions 
	-m or --mode    : run mode input. 
			  Currently it support the following input mode
				>  Z	zero subtraction mode, used for display the result after zero subtraction, this is only used for check the result
				>  H	nomal run mode. This will generate the Hit root file used for next step analysis 
				>  P	pedestal mode. This will generate the pedestal root file
				>  R	Raw display mode. This mode will display the raw data, used for intial check when taken data
				>  A    analysis mode, this will analysis 
				> PH    pedestal and Hit mode, this is used for the old data. when analysis the data, it will try to extract the pedestals from current data file, and used this pedestal when analysis the data. (Not recommended)

	-o or --ouput	: specify the out put file name( if neccessery)
	


NOTE: 
	This demo code currently can only be used for INFN version 5 formate data. Other versions including the newest JLab evio formate are not supported at this moment, but will be added in the future.
	If there is any bug or any improvement about the code, please contect me via email or upload the change to github.
	The last version(if there is) of this code can be fund at https://github.com/Jiansiyu/GEM_Decoder
