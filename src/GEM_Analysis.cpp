//============================================================================
// Name        : GEM_Analysis.cpp
// Author      : Siyu Jian
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <getopt.h>
#include <time.h>    // used for calculate the process time
#include <map>
//ROOT related libs
#include <TApplication.h>
#include "TH1F.h"
#include "TCanvas.h"

// user defined functions
#include "GEMInputHandler.h"
#include "GEMRawFileDecoder.h"
using namespace std;

// help usage
void Usage()
{

	}
int main(int argc, char *argv[])
{
		TApplication theApp("App", NULL, NULL);   //Root application framework
		GEMInputHandler *GEMDecoderIuputHandler = new GEMInputHandler();
		GEMDecoderIuputHandler->GEMInputHandler_TestFunctions();

		//test functions
		TTree test_tree;
		GEMRawFileDecoder *GEMRawFileDecoder_TEST=new GEMRawFileDecoder("/home/newdriver/Research/Eclipse_workspace/GEM_Analysis/Debug/test_2385.dat", &test_tree);
		GEMRawFileDecoder_TEST->GEMRawFileDecoder_TestFunction();

		return 0;
}
