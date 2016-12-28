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
//#include "GEMInputHandler.h"
#include "GEMRawFileDecoder.h"
using namespace std;

// help usage
void Usage()
{

  }


int main(int argc, char **argv)
{
		TApplication theApp("App", &argc, argv);   //Root application framework

		// mapping reading function
		TTree test_tree;
		for(int i=2260; i<2266; i ++){
			//GEMRawFileDecoder *GEMRawFileDecoder_TEST=new GEMRawFileDecoder(Form("/home/newdriver/Research/GEM_Analsys_CIAE/Data/test_%d.dat",i), &test_tree);
			GEMRawFileDecoder *GEMRawFileDecoder_TEST=new GEMRawFileDecoder(Form("/media/newdriver/data/DATA\ back_up/APV_data/ihep_spatial_resolution/resolution_x_data/test_%d.dat",i), &test_tree);
			printf("/home/newdriver/Research/GEM_Analsys_CIAE/Data/test_%d.dat\n",i);
			GEMRawFileDecoder_TEST->GEMRawFileDecoder_PedestalDecoder(Form("pedestal_%4d.root",i),-1);
			//GEMRawFileDecoder_TEST->GEMRawFileDecoder_ZeroSubtractionDisplay(Form("pedestal_%4d.root",i),Form("/home/newdriver/Research/GEM_Analsys_CIAE/Result/spacial/Decoder/test_result_%d.root",i),-1);
			GEMRawFileDecoder_TEST->GEMRawFileDecoder_HistoDecoder(Form("pedestal_%4d.root",i),Form("/home/newdriver/Research/GEM_Analsys_CIAE/Result/spacial/Decoder/test_result_%d.root",i),-1);

			//GEMRawFileDecoder_TEST->GEMRawFileDecoder_TestFunction();
			delete GEMRawFileDecoder_TEST;

		}
		//theApp.Run(kTRUE);
		return 0;
}
