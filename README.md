//============================================================================
// Name        : GEM_Analysis.cpp
// Author      : Siyu Jian, Danning Di
// EMAIL       : jiansiyu@gmail.com   sj9va@virginia.edu
// Version     : V1.01
// Copyright   : @copyright
// Description : Can be used to decode MPD data INFN Version 5, other versions include 
		  new type of JLab formate has not been added, will added latter.

//============================================================================

Compile Requirement:
	
	CERN ROOT 5.34.36 or higher(multithread supported)
	gcc 5.4.0 or higher
	[evio version4.3 will be needed for Jlab data format, but at this moment, those part of code has not been added]
	all the test based on Ubuntu 16.04.4

Installation instruction:
	cd xx/GEM_Analysis/
	make clean 
	make

Run instruction:
	
./GEM_decoder
 
	-h or --help	: print this instructions 
	-m or --mode    : run mode input. 
			  Currently it support the following input mode (capital or lower)
				>  Z	zero subtraction mode, used for display the result after zero subtraction, this is only used for check the result
				>  H	nomal run mode. This will generate the Hit root file used for next step analysis 
				>  P	pedestal mode. This will generate the pedestal root file
				>  R	Raw display mode. This mode will display the raw data, used for intial check when taken data
				>  A    analysis mode, this will analysis 
				> PH    pedestal and Hit mode, this is used for the old data. when analysis the data, it will try to extract the pedestals from 
					   current data file, and used this pedestal when analysis the data. (Not recommended)
				> HA    histo Analysis mode. run histo and analysis.

	-o or --ouput	: specify the out put file name( if neccessery), for pedestal or hit mode usage 
				Attention: In Hit mode, it multi raw files are calculate at the same time. please donot specify the output filename
				In the following case, if there is not output name specified, it will trying to generate the outfilename itself
					> Multi file mode
					> batch mode
					> pedestal mode (P)
					> hit mode (H)
					> pedestal-hit mode (PH)
 

	-i or --input   : specify the input file name, support format: 
				.dat  will be consider as the raw data files, 
				      Multi-file can be input at the same time. In this case, donot specify the ouput filename. It will generate by itself
				      it will check the files, if the file does not exist, it will be omit by the program
				      In Pedestal and Zero sutraction mode, although multi input can be accept, it will only calculate the first file. 
				.root will be consider as the pedestal file
				.txt  batch mode, it will read the filenames from the file, it has not been added at this moment, it wil be added latter

The files can also be added with out specify the [-option], in this case, .dat file will be consider as the raw data file, and .root file will be consider as pedestal file 
	

NOTE: 
	This demo code currently can only be used for INFN version 5 formate data. Other versions including the newest JLab evio formate are not supported at this moment, but will be added in the future.
	If there is any bug or any improvement about the code, please contect me via email or upload the change to github.
	The last version(if there is) of this code can be fund at https://github.com/Jiansiyu/GEM_Decoder
