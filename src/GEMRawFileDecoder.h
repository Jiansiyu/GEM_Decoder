/*
 * GEMRawFileDecoder.h
 *
 *  Created on: Oct 25, 2016
 *      Author: newdriver
 */

#ifndef GEMRAWFILEDECODER_H_
#define GEMRAWFILEDECODER_H_

#include <stdio.h>   // file operations
#include <stdint.h>  //uint16_t...
#include <vector>

//CERN ROOT functions
#include <TString.h>
#include <TTree.h>
//#include <TMath.h>
//#include <TROOT.h>

//user defined functions
#include "GEMInfor.h"       // GEM infor class

// global configuration files
#include "GEMConfig.h"

using namespace std;


class GEMRawFileDecoder {
public:
	GEMRawFileDecoder();
	GEMRawFileDecoder(TString Raw_File);   // load the raw files
	GEMRawFileDecoder(TString Raw_File, TTree &APVTree);

	virtual ~GEMRawFileDecoder();

public://
	vector<GEMInfor> GEMRawFileDecoder_Run(std::vector<GEMInfor> GEMInfor_Buffer_Input);    // main decoder function
    vector<GEMInfor> GEMRawFileDecoder_ingestFileHeader(FILE *file_input, int  fileVersion_input, vector<GEMInfor> GEMInfor_Buffer_Input);
    vector<GEMInfor> GEMRawFileDecoder_ingestEventV5(FILE *file_input, TString ifile, vector<GEMInfor> GEMInfor_Buffer_Input);

//report functions
public:
    TTree GEMRawFileDecoder_TreeSave(int EventID_index_temp,uint32_t MPD_Index_Input, uint32_t APVADC_Index_Input,map<int,map<int, int> > Tsample_StrADC_Input);    // generate the tree files


// test functions
public:
    void GEMRawFileDecoder_TestFunction();

// private variables
private:
	TString  GEMRawFileDecoder_Raw_File;
	int    GEMRawFileDecoder_EventsIndex=0; // buffer the current Evnts ID, used for the concurrent process

	// used for return the value

	TTree *GEMAPV_tree;					// used for buffer the data
	APV_DataStruct GEMR_ApvStrData;     // used for save the data in the tree file
// private functions
private:
	vector<GEMInfor> GEMRawFileDecoder_BufferSave(std::vector<GEMInfor> GEMInfor_Buffer_Input,int EventID_index_temp,uint32_t MPD_Index_Input, uint32_t APVADC_Index_Input,map<int,map<int, int> > Tsample_StrADC_Input); // save the individual data informations to the
    int GEMRawFileDecoder_BufferSave(int EventID_index_temp,uint32_t MPD_Index_Input, uint32_t APVADC_Index_Input,map<int,map<int, int> > Tsample_StrADC_Input);
	int GEMRawFileDecoder_DataCheck(vector<GEMInfor> GEMInfor_Buffer_Input);                // check the number of Datas, varify it is matching the configuration

// files related IDs
private:
	const uint32_t BINARYFILE_ID = ((uint32_t) 0xE5C190C0);
	const uint16_t BINARYAPV_ID  = ((uint16_t) 0xE59F);
	const uint16_t BINARYVME_ID  = ((uint16_t) 0xE590);
	const uint16_t BINARYTRG_ID  = ((uint16_t) 0xE591);

};

#endif /* GEMRAWFILEDECODER_H_ */
