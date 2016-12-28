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
	vector<const char *>input_filenames;
	//vector<string> input_filename_str;
	const char *RunFile_name;
	Bool_t File_run_mode;
	const char *output_filename;
	const char *pedestal_filename;
	const char *batch_mode_filename;
}globalViabArgs;

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
		if(argc<=1) {
			printf("./GEM_Analysis -[option] [file or command]\n");
			exit(0);
		}
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
							printf("[%s(line%d)] Input root file \"%s\"\n",__FUNCTION__,__LINE__,input_file_temp.c_str());
						}else if(input_file_temp.substr(input_file_temp.find_last_of(".")+1)=="dat") {
							ifstream test_file(input_file_temp.c_str());
							if(test_file){
								globalViabArgs.input_filenames.push_back(input_file_temp.c_str());
								//globalViabArgs.input_filename_str.push_back(input_file_temp);
								printf("[%s(line%d)] Input dat file \"%s\"\n",__FUNCTION__,__LINE__,input_file_temp.c_str());
							}else{
								printf("[%s(line%d)] Input dat file \"%s\" is not fund, omitted\n",__FUNCTION__,__LINE__,input_file_temp.c_str());
								}
						}
						else if(input_file_temp.substr(input_file_temp.find_last_of(".")+1)=="txt") {
							globalViabArgs.batch_mode_filename=input_file_temp.c_str();
							printf("[%s(line%d)] Input txt file \"%s\", batch mode enabled, decoding the script\n",__FUNCTION__,__LINE__,input_file_temp.c_str());
							printf("[%s(line%d)] Input txt file \"%s\", batch mode enabled currently is not supported\n",__FUNCTION__,__LINE__,input_file_temp.c_str());
						}else {
								printf("[%s(line%d)] Input file \"%s\", Format unrecognized\n",__FUNCTION__,__LINE__,input_file_temp.c_str());
							}
					};
					break;
				case 'm':
					{
						string RunMode(optarg);
						transform(RunMode.begin(),RunMode.end(),RunMode.begin(),::toupper);
						globalViabArgs.Run_mode=RunMode.c_str();
						printf("[%s(line%d)] Run mode \"%s\"\n",__FUNCTION__,__LINE__,globalViabArgs.Run_mode);
					}
					break;
				default:
					break;
			};
			ui_input=getopt_long(argc,argv,"vm:o:i:h", long_options, &option_index);
		}

		// in case there is left out input, consider it to be the data file
		vector<string>fname;
		if(optind<argc) {
			for (Int_t ii = optind; ii < argc; ii++) {
				string input_file_temp(argv[ii]);
				if(input_file_temp.substr(input_file_temp.find_last_of(".")+1)=="dat") {

				}else if(input_file_temp.substr(input_file_temp.find_last_of(".")+1)=="root") {
					ifstream test_file(input_file_temp.c_str());
					if(test_file){
						globalViabArgs.pedestal_filename=input_file_temp.c_str();
					}else {
						globalViabArgs.output_filename=input_file_temp.c_str();
					}
					test_file.close();
				}else if(input_file_temp.substr(input_file_temp.find_last_of(".")+1)=="txt") {
					globalViabArgs.batch_mode_filename=input_file_temp.c_str();
					printf("[%s(line%d)] Input txt file \"%s\", batch mode enabled, decoding the script\n",__FUNCTION__,__LINE__,input_file_temp.c_str());
					printf("[%s(line%d)] Input txt file \"%s\", batch mode enabled currently is not supported\n",__FUNCTION__,__LINE__,input_file_temp.c_str());
				}
			}
		}
		// command control report
		// finish the user interface diaglog and common control
		if(globalViabArgs.Run_mode==NULL){
			printf("Command option is not detected\n");
		}else

		if(!strcmp(globalViabArgs.Run_mode,"Z")){
			if(globalViabArgs.input_filenames.size()!=0){
				if(globalViabArgs.pedestal_filename!=NULL){
					TTree test_tree;
					GEMRawFileDecoder *GEMRawFileDecoder_run=new GEMRawFileDecoder(globalViabArgs.input_filenames[0], &test_tree);
					GEMRawFileDecoder_run->GEMRawFileDecoder_ZeroSubtractionDisplay(globalViabArgs.pedestal_filename,-1);
					delete GEMRawFileDecoder_run;
				}else
				{
					printf("[%s(line%d)] Run Mode \"Z\", Not pedestal file detected\n",__FUNCTION__,__LINE__);
				}
			}
			else{
				printf("[%s(line%d)] Run Mode \"Z\", Not input dat file detected\n",__FUNCTION__,__LINE__);
			}
		}else
		if(!strcmp(globalViabArgs.Run_mode,"H")){
			if(globalViabArgs.input_filenames.size()!=0){
				vector<const char*>::iterator input_index=globalViabArgs.input_filenames.begin();
				for(;input_index!=globalViabArgs.input_filenames.end();input_index++){
					string output_filename_temp;
					if(globalViabArgs.pedestal_filename!=NULL){
						if((globalViabArgs.output_filename==NULL)||(globalViabArgs.input_filenames.size()>=2)){
							string output(*(input_index));
							output.erase(output.find_last_of("."),string::npos);
							output_filename_temp=output+".root";
							printf("[%s(line%d)] No output filename detected, trying to generate, the output data will save as \" %s \"\n",__FUNCTION__,__LINE__,output_filename_temp);
						}else
						{
							string temp(globalViabArgs.output_filename);
							output_filename_temp=temp;
						}
						TTree test_tree;
						GEMRawFileDecoder *GEMRawFileDecoder_run=new GEMRawFileDecoder((*input_index), &test_tree);
						GEMRawFileDecoder_run->GEMRawFileDecoder_HistoDecoder(globalViabArgs.pedestal_filename,output_filename_temp,-1);//GEMRawFileDecoder_RawDisplay(-1);
						delete GEMRawFileDecoder_run;
					}
					else{
						printf("[%s(line%d)] Run Mode \"H\", Not Pedestal(root) file detected, are you want to run \"PH\" mode?(pedestal file is not needed is PH mode)\n",__FUNCTION__,__LINE__);
					}
				}
			}
			else{
				printf("[%s(line%d)] Run Mode \"H\", Not input dat file detected\n",__FUNCTION__,__LINE__);
			}
		}else
		if(!strcmp(globalViabArgs.Run_mode,"P")){
			if(globalViabArgs.input_filenames.size()!=0) {
				vector<const char*>::iterator input_index=globalViabArgs.input_filenames.begin();
				for(;input_index!=globalViabArgs.input_filenames.end();input_index){
					string output_filename_temp;
					if((globalViabArgs.output_filename==NULL)||(globalViabArgs.input_filenames.size()>=2)){
						string output(*(input_index));
						output.erase(output.find_last_of("."),string::npos);
						output_filename_temp=output+"Pedestal.root";
					}else{
						string temp(globalViabArgs.output_filename);
						output_filename_temp=temp;
					}
					TTree test_tree;
					GEMRawFileDecoder *GEMRawFileDecoder_run=new GEMRawFileDecoder((*input_index), &test_tree);
					GEMRawFileDecoder_run->GEMRawFileDecoder_PedestalDecoder(output_filename_temp,1,-1);
					delete GEMRawFileDecoder_run;
				}
			}else{
				printf("[%s(line%d)] Run Mode \"P\", Not input dat file detected\n",__FUNCTION__,__LINE__);
			}
			//if()
		}else
		if(!strcmp(globalViabArgs.Run_mode,"R")){
			if(globalViabArgs.input_filenames.size()!=0){
				TTree test_tree;
				GEMRawFileDecoder *GEMRawFileDecoder_run=new GEMRawFileDecoder(globalViabArgs.input_filenames[0], &test_tree);
				GEMRawFileDecoder_run->GEMRawFileDecoder_RawDisplay(-1);
				delete GEMRawFileDecoder_run;
			}else{
				printf("[%s(line%d)] Run Mode \"R\", Not input dat file detected\n",__FUNCTION__,__LINE__);
			}
			//printf("r\n");
		}else
		if(!strcmp(globalViabArgs.Run_mode,"A")){
			printf("Analysis mode not supported at this moment\n");
		}else
		if(!strcmp(globalViabArgs.Run_mode,"PH")){
			if(globalViabArgs.input_filenames.size()!=0) {
				vector<const char*>::iterator input_index=globalViabArgs.input_filenames.begin();
				for(;input_index!=globalViabArgs.input_filenames.end();input_index){
					string Pedestal_output_filename_temp;
					string Decodered_output_filename_temp;
					if((globalViabArgs.output_filename==NULL)||(globalViabArgs.input_filenames.size()>=2)){
						string output(*(input_index));
						output.erase(output.find_last_of("."),string::npos);
						Pedestal_output_filename_temp=output+"_Pedestal.root";
						Decodered_output_filename_temp=output+"_decode.root";
					}else{
						string temp(globalViabArgs.output_filename);
						Pedestal_output_filename_temp=temp;
						temp.erase(temp.find_last_of("."),string::npos);
						Decodered_output_filename_temp=temp+"_decode.root";
					}
					TTree test_tree;
					GEMRawFileDecoder *GEMRawFileDecoder_run=new GEMRawFileDecoder((*input_index), &test_tree);
					GEMRawFileDecoder_run->GEMRawFileDecoder_PedestalDecoder(Pedestal_output_filename_temp,1,-1);
					GEMRawFileDecoder_run->GEMRawFileDecoder_HistoDecoder(Pedestal_output_filename_temp,Decodered_output_filename_temp,-1);
					delete GEMRawFileDecoder_run;
				}
			}else{
				printf("[%s(line%d)] Run Mode \"P\", Not input dat file detected\n",__FUNCTION__,__LINE__);
			}
			//printf("ph\n");
		}else{
			printf("unrecgnize command\n");
		}

		return 0;
}
