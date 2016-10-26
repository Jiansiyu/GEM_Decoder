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
		GEMRawFileDecoder *GEMRawFileDecoder_TEST=new GEMRawFileDecoder("/home/newdriver/Research/Eclipse_workspace/GEM_Analysis/Debug/test_0888.dat");
		GEMRawFileDecoder_TEST->GEMRawFileDecoder_TestFunction();

		/*
		map<int,int> map_test;
		for(int i=0; i<100;i++)
		{
			map_test.insert(make_pair(i,i));
	 	 }
		map<int,int>::iterator map_iter=map_test.begin();
		while(map_iter != map_test.end())
		{
			cout<<"first="<<map_iter->first<<"  second="<<map_iter->second<<endl;
			map_iter++;
		}*/



		return 0;
}
