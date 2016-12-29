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
#include <fstream>
#include <string>
#include <getopt.h>
#include <algorithm>
#include <cstdlib>
#include <cstdio>
#include <cassert>
//ROOT related libs
#include <TApplication.h>
#include "TH1F.h"
#include "TCanvas.h"
#include <TROOT.h>
#include <TError.h>
#include <TChain.h>
#include <TFile.h>
#include <TTree.h>
/// GEM analysis part
#include "../analysis/Config.h"
#include "../analysis/GEMTree.h"
#include "../analysis/GEMTracking.h"
#include "../analysis/GEMFittingShape.h"

// user defined functions
//#include "GEMInputHandler.h"
#include "GEMRawFileDecoder.h"
using namespace std;

// help usage
void Usage()
{
	ifstream README("README.md");
	string lines;
	if(README){
		while(getline(README,lines)) printf("%s\n",lines.c_str());
	}else{
		ifstream README("../README.md");
		string lines;
		if(README){
				while(getline(README,lines)) printf("%s\n",lines.c_str());
			}
	}
  }

struct globalViabArgs_t {
	const char *Run_mode;
	const char *input_rawfilenames;
	const char *input_pedestal_name;
	const char *output_filename;
	const char *pedestal_filename;
	const char *batch_mode_filename;
};

static const struct option long_options[]={
		{"help", no_argument, 0, 'h'},
		{"mode", required_argument, 0, 'm'},
        {"output",required_argument,0,'o'},
        {"pedestal",required_argument,0,'p'},
        {"input",required_argument,0,'i'},
        {"version", no_argument, 0, 'v'},
        {0, 0, 0, 0}
};

int main(int argc, char **argv)
{
		//
		TApplication theApp("App", NULL, NULL);   //Root application framework
		int ui_input=0;
		int option_index = 0;
		if(argc<=1) {
			printf("./GEM_Analysis -[option] [file or command]\n");
			exit(0);
		}
		globalViabArgs_t globalViabArgs_temp;
		ui_input=getopt_long(argc, argv, "vm:o:i:p:h", long_options, &option_index);
		while(ui_input!=-1) {
			switch (ui_input) {
				case 'h':
					Usage();
					exit(0);
				case 'v':
					printf("GEM Decoder v1.01\n");
					exit(0);
				case 'o':
					globalViabArgs_temp.output_filename=optarg;
					printf("[%s(line%d)] Output file \"%s\"\n",__FUNCTION__,__LINE__,optarg);
					break;
				case 'i':
					globalViabArgs_temp.input_rawfilenames=optarg;
					break;
				case 'p':
					globalViabArgs_temp.input_pedestal_name=optarg;
					break;
				case 'm':
					{
					std::string RunMode(optarg);
					std::transform(RunMode.begin(),RunMode.end(),RunMode.begin(),::tolower);
					globalViabArgs_temp.Run_mode=RunMode.c_str();
					break;
					}
				default:
					printf("[%s(line%d)] unrecognized option \n",__FUNCTION__,__LINE__);
					break;
			};
			ui_input=getopt_long(argc,argv,"vm:o:i:p:h", long_options, &option_index);
		};

		if(globalViabArgs_temp.Run_mode==NULL){
			printf("[%s(line%d)] Can not resolve \"RUN MODE\"\n",__FUNCTION__,__LINE__);
			exit(-1);
		}else{
			if(globalViabArgs_temp.input_rawfilenames==NULL){
				printf("[%s(line%d)] no input file detected\n",__FUNCTION__,__LINE__);
			}
			string inputfilename_str(globalViabArgs_temp.input_rawfilenames);
			string runmode_str(globalViabArgs_temp.Run_mode);
			transform(runmode_str.begin(),runmode_str.end(),runmode_str.begin(),::tolower);
			if(runmode_str=="r"){
				cout<<"runmode = "<<runmode_str.c_str()<<endl;
			}else if(runmode_str=="p") {

			}else if(runmode_str=="h") {

			}else if(runmode_str=="z") {

			}else if(runmode_str=="a") {
				if(inputfilename_str.substr(inputfilename_str.find_last_of(".")+1)=="root"){
					TChain *fChain = new TChain("GEMHit", "");
					TFile *ff=new TFile(inputfilename_str.c_str());
					assert(ff->IsOpen());
					TTree *theTree = (TTree*) ff->Get("GEMHit");
					if (theTree->IsZombie()) {
		 	               cout << "Tree is not found" << endl;
		 	               exit(1);
					}
					delete ff;
					fChain->AddFile(inputfilename_str.c_str());

					GEMTracking *pGEMTrack = new GEMTracking(fChain);
					pGEMTrack->Run(-1,globalViabArgs_temp.output_filename);
					delete pGEMTrack;

				}else{
					printf("[%s(line%d)] .root file is needed is \"A\" mode\n",__FUNCTION__,__LINE__);
				}
			}else{
				printf("[%s(line%d)] Can not resolve \"RUN MODE\"\n",__FUNCTION__,__LINE__);
				exit(0);
			}

		}
		/*
		for (int i =2515; i <2568;i++){
			TTree test_tree;
			GEMRawFileDecoder *GEMRawFileDecoder_run=new GEMRawFileDecoder(Form("/media/newdriver/data/DATA\ back_up/APV_data_weizhengli/IHEP_ciae_image_x_tube/test_%d.dat",i), &test_tree);
			GEMRawFileDecoder_run->GEMRawFileDecoder_PedestalDecoder(Form("Pedestal_%d.root",i),1,-1);
			GEMRawFileDecoder_run->GEMRawFileDecoder_HistoDecoder(Form("Pedestal_%d.root",i),Form("/home/newdriver/Research/GEM_Analsys_CIAE/Result/ciae/decoder/test_result_%d.root",i),-1);
			delete GEMRawFileDecoder_run;
		}*/

		return 0;
}
