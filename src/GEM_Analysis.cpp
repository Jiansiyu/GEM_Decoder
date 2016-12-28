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

//ROOT related libs
#include <TApplication.h>
#include "TH1F.h"
#include "TCanvas.h"

// user defined functions
//#include "GEMInputHandler.h"
#include "GEMRawFileDecoder.h"
using namespace std;

//std::string fn = "filename.conf";
//fn.substr(fn.find_last_of(".") + 1) == "conf"

struct globalViabArgs_t {
	const char *Run_mode;
	vector<const char *>input_filenames;
	const char *RunFile_name;
	Bool_t File_run_mode;
	const char *output_filename;
	const char *pedestal_filename;
}globalViabArgs;

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

static const struct option long_options[]={
		{"help", no_argument, 0, 'h'},
		{"mode", required_argument, 0, 'm'},
        {"output",required_argument,0,'o'},
        {"input",required_argument,0,'i'},
        {"version", no_argument, 0, 'v'},
        {0, 0, 0, 0}
};

int main(int argc, char **argv)
{
		//TApplication theApp("App", &argc, argv);   //Root application framework
		int ui_input=0;
		int option_index = 0;
		if(argc<=1) Usage();
		ui_input=getopt_long(argc, argv, "vm:o:i:h", long_options, &option_index);
		while(ui_input!=-1) {
			switch (ui_input) {
				case 'h':
					Usage();
					exit(0);
				case 'v':
					printf("GEM Decoder v1.01\n");
					exit(0);
				case 'o':
					globalViabArgs.output_filename=optarg;
					printf("[%s(line%d)] Output file \"%s\"\n",__FUNCTION__,__LINE__,globalViabArgs.output_filename);
					break;
				case 'i':
					{
						string input_file_temp(optarg);
						if(input_file_temp.substr(input_file_temp.find_last_of(".")+1)=="root") {
							globalViabArgs.pedestal_filename=input_file_temp.c_str();
							printf("[%s(line%d)] Input root file \"%s\"\n",__FUNCTION__,__LINE__,optarg);
						}else if(input_file_temp.substr(input_file_temp.find_last_of(".")+1)=="dat") {
								ifstream test_file(optarg);
								if(test_file){
									globalViabArgs.input_filenames.push_back(optarg);
									printf("[%s(line%d)] Input dat file \"%s\"\n",__FUNCTION__,__LINE__,optarg);
								}else{
									printf("[%s(line%d)] Input dat file \"%s\" is not fund, omitted\n",__FUNCTION__,__LINE__,optarg);
								}
							}
							else if(input_file_temp.substr(input_file_temp.find_last_of(".")+1)=="dat") {
									printf("[%s(line%d)] Input txt file \"%s\", batch mode enabled, decoding the script\n",__FUNCTION__,__LINE__,optarg);
									printf("[%s(line%d)] Input txt file \"%s\", batch mode enabled currently is not supported\n",__FUNCTION__,__LINE__,optarg);
								}else {
									printf("[%s(line%d)] Input file \"%s\", Format unrecognized\n",__FUNCTION__,__LINE__,optarg);
								}

						break;
					};
				case 'm':
					globalViabArgs.Run_mode=optarg;
					printf("[%s(line%d)] Run mode \"%s\"\n",__FUNCTION__,__LINE__,globalViabArgs.Run_mode);
					break;
				default:
					Usage();
					break;
			};
			ui_input=getopt_long(argc,argv,"vm:o:i:h", long_options, &option_index);
		}
		// incase there is left out input, consider it to be the data file
		vector<string>fname;
		if(optind<argc) {
			for (Int_t ii = optind; ii < argc; ii++) {
				ifstream test_file(argv[ii]);
				if(test_file){
				globalViabArgs.input_filenames.push_back(argv[ii]);
				}
				test_file.close();
			}
		}




		/*
		// mapping reading function
		TTree test_tree;
		for(int i=2562; i<2569; i ++){
			//GEMRawFileDecoder *GEMRawFileDecoder_TEST=new GEMRawFileDecoder(Form("/home/newdriver/Research/GEM_Analsys_CIAE/Data/test_%d.dat",i), &test_tree);
			GEMRawFileDecoder *GEMRawFileDecoder_TEST=new GEMRawFileDecoder(Form("/media/newdriver/data/DATA\ back_up/APV_data_weizhengli/IHEP_ciae_image_x_tube/test_%d.dat",i), &test_tree);
			printf("test_%d.dat\n",i);
			GEMRawFileDecoder_TEST->GEMRawFileDecoder_PedestalDecoder(Form("pedestal_%4d.root",i),1,-1);
			//GEMRawFileDecoder_TEST->GEMRawFileDecoder_ZeroSubtractionDisplay(Form("pedestal_%4d.root",i),Form("/home/newdriver/Research/GEM_Analsys_CIAE/Result/spacial/Decoder/test_result_%d.root",i),-1);
			//GEMRawFileDecoder_TEST->GEMRawFileDecoder_HistoDecoder(Form("pedestal_%4d.root",i),Form("/home/newdriver/Research/GEM_Analsys_CIAE/Result/ciae/decoder/test_result_%d.root",i),-1);
			GEMRawFileDecoder_TEST->GEMRawFileDecoder_ZeroSubtractionDisplay(Form("pedestal_%4d.root",i),Form("/home/newdriver/Research/GEM_Analsys_CIAE/Result/ciae/decoder/test_result_%d.root",i),-1);

			//GEMRawFileDecoder_TEST->GEMRawFileDecoder_TestFunction();
			delete GEMRawFileDecoder_TEST;

		}*/
		//theApp.Run(kTRUE);
		return 0;
}
