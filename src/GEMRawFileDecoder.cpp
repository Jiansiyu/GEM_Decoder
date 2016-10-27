/*
 * GEMRawFileDecoder.cpp
 *
 *  Created on: Oct 25, 2016
 *      Author: newdriver
 */

#include <stdio.h>
#include <stdint.h>
// ROOT functions
#include <TString.h>
#include <vector>
// user defined functions
#include "GEMRawFileDecoder.h"
#include "GEMInfor.h"           // GEM infor class

using namespace std;

GEMRawFileDecoder::GEMRawFileDecoder() {
	// TODO Auto-generated constructor stub

};

GEMRawFileDecoder::GEMRawFileDecoder(TString Raw_File){
	GEMRawFileDecoder_Raw_File = Raw_File;
	printf("[Test Infor]:: %s\n",Raw_File.Data());
};

GEMRawFileDecoder::~GEMRawFileDecoder() {
	// TODO Auto-generated destructor stub
}

//
vector<GEMInfor> GEMRawFileDecoder::GEMRawFileDecoder_Run( vector<GEMInfor> GEMInfor_Buffer_Input) {

	vector<GEMInfor> GEMInfor_Buffer_temp;       // used for buffer the data temporary

	FILE *Input_File_temp;
	Input_File_temp = fopen(GEMRawFileDecoder_Raw_File.Data(), "rb");
	if(Input_File_temp == 0) {
		printf("[ERROR]:: %s Error in loading the raw data file, maybe the raw data \"%s\"file does's exist\n",__FUNCTION__, GEMRawFileDecoder_Raw_File.Data());
	    exit(-1);
	 }
	else {
		printf("[RUN INFOR]:: %s loading file %s \n", __FUNCTION__, GEMRawFileDecoder_Raw_File.Data());
	};

	uint32_t fileID_temp;
	uint32_t fileVersion_temp;
	uint32_t fNumberSample_temp;
	uint32_t fNumberAPV_temp;
	fread(&fileID_temp, sizeof(uint32_t),1,Input_File_temp); // read the fileID
	if(fileID_temp == BINARYFILE_ID){      // mactch the file ID

		fread(&fileVersion_temp,sizeof(uint32_t),1,Input_File_temp);   // load the file version
		fread(&fNumberSample_temp,sizeof(uint32_t),1,Input_File_temp);
		fread(&fNumberAPV_temp,sizeof(uint32_t),1,Input_File_temp);

		printf("[Test Variables]:: %s fileVersion=%x, fNumberSample=%d, InpHand_fNumberAPV = %d \n",__FUNCTION__, fileVersion_temp,fNumberSample_temp,fNumberAPV_temp);


		if(fileVersion_temp == 5) {
			printf("[RUN INFOR]:: %s Decoding the headers\n",__FUNCTION__);
			GEMInfor_Buffer_temp=GEMRawFileDecoder_ingestFileHeader(Input_File_temp, (int)fileVersion_temp, GEMInfor_Buffer_temp); // decoder the file header
			printf("[RUN INFOR]:: %s Finish decoding the headers  %d MPD found, which match the setting\n",__FUNCTION__,GEMInfor_Buffer_temp.size());
			GEMRawFileDecoder_ingestEventV5(Input_File_temp, GEMRawFileDecoder_Raw_File ,GEMInfor_Buffer_temp); // decoder the data
		}
		else {
			printf("[ERROR]:: _%s Unsupported file Version\n",__FUNCTION__);
			exit(-1);
		};


	}
	else {
		printf("[ERROR]:: %s Unsupport fileID",__FUNCTION__);
		exit(-1);
	}

return GEMInfor_Buffer_temp;
};

// file header decoder
vector<GEMInfor> GEMRawFileDecoder::GEMRawFileDecoder_ingestFileHeader(FILE *file_input, int  fileVersion_input, vector<GEMInfor> GEMInfor_Buffer_Input) {
	printf("[RUN INFOR]:: %s Start decoding the file header \n", __FUNCTION__);


	if(GEMInfor_Buffer_Input.size()!=0){
		printf("[ERROR]:: %s The input vector is not empty\n", __FUNCTION__ );
	}

	//double check the file version, used for the future multi-decoder
	if(fileVersion_input!=5){
		printf("[ERROR]:: %s File Version error\n",__FUNCTION__);
		exit(-1);
	};

	uint32_t fDaqMode_temp;
	fread(&fDaqMode_temp,sizeof(uint32_t),1,file_input);

	uint32_t DATA_nMPD;       // number of MPDs
	fread(&DATA_nMPD, sizeof(uint32_t),1,file_input);   // read the number of MPDs, in final, will double check with the size of the vector to returned.

	printf("[RUN INFOR]:: %s Number of MPD detected  = %d\n",__FUNCTION__,DATA_nMPD);

	// loop all the MPDs. read all the configuration for the MPDs
	for(int MPD_Loop_counter=0; MPD_Loop_counter < (int)DATA_nMPD ; MPD_Loop_counter++) {
		printf("[TEST INFOR]:: %s Decoding MPD No.%d\n",__FUNCTION__,MPD_Loop_counter);
		GEMInfor *GEMInfor_Buffer_temp= new GEMInfor();   // used for buffer one MPD data

		uint32_t  DATA_MPDConfiguration;
		fread(&DATA_MPDConfiguration, sizeof(uint32_t),1,file_input);
		GEMInfor_Buffer_temp->GEMInfor_fApvCount    = (int)(DATA_MPDConfiguration&0xf);
		GEMInfor_Buffer_temp->GEMInfor_fSlot        = (int)((DATA_MPDConfiguration&0xff0)>>4);
		GEMInfor_Buffer_temp->GEMInfor_fNumTrigger  = (int)((DATA_MPDConfiguration&0xff000)>>12);
		GEMInfor_Buffer_temp->GEMInfor_fCalibLatency= (int)((DATA_MPDConfiguration&0xff00000)>>20);

		printf("[Test Infor]::  %s Number of APV= %d, SLOT=%d, number of Triger=%d, lantency=%d\n",
								__FUNCTION__,
								GEMInfor_Buffer_temp->GEMInfor_fApvCount,
								GEMInfor_Buffer_temp->GEMInfor_fSlot,
								GEMInfor_Buffer_temp->GEMInfor_fNumTrigger,
								GEMInfor_Buffer_temp->GEMInfor_fCalibLatency
									);

		uint32_t DATA_EtcConfiguration;
		fread(&DATA_EtcConfiguration,sizeof(uint32_t),1,file_input);
		GEMInfor_Buffer_temp->GEMInfor_fClockPhase[0]   = DATA_EtcConfiguration&0xff;
		GEMInfor_Buffer_temp->GEMInfor_fClockPhase[1]   = (DATA_EtcConfiguration>>8)&0xff;
		GEMInfor_Buffer_temp->GEMInfor_fCommonBaseline  = (DATA_EtcConfiguration>>16)&0xfff;
		GEMInfor_Buffer_temp->GEMInfor_fCommonMode      = (DATA_EtcConfiguration>>28)&1;
		GEMInfor_Buffer_temp->GEMInfor_fEventBuilder    = (DATA_EtcConfiguration>>29)&1;

		uint32_t DATA_LevelConfiguration;
		fread(&DATA_LevelConfiguration, sizeof(uint32_t),1,file_input);
		GEMInfor_Buffer_temp->GEMInfor_fLevelZero      = DATA_LevelConfiguration&0xfff;
		GEMInfor_Buffer_temp->GEMInfor_fLevelOne       = (DATA_LevelConfiguration>>16)&0xfff;

		for(unsigned int ADCchip_index=0; ADCchip_index<2; ADCchip_index++) {
			printf("[RUN INFOR]:: %s Loading ADC gain\n",__FUNCTION__);
			uint32_t DATA_ADCconfiguration;
			fread(&DATA_ADCconfiguration,sizeof(uint32_t),1,file_input);
			for(unsigned int ADC_channel_index_loop=0; ADC_channel_index_loop< 8 ; ADC_channel_index_loop++){
				GEMInfor_Buffer_temp->GEMInfor_fAdcGain.insert(make_pair(ADCchip_index*8+ADC_channel_index_loop, (DATA_ADCconfiguration>>(ADC_channel_index_loop*4))&0xf));
			}
		}

		// test functions+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		map<int,int>::iterator iter_temp=GEMInfor_Buffer_temp->GEMInfor_fAdcGain.begin();
		printf("[RUN INFOR]:: ADC Gain ");
		unsigned int counter_temp=0;
		while(iter_temp!= GEMInfor_Buffer_temp->GEMInfor_fAdcGain.end()) {

			printf("Channel(%d)=: %d ",counter_temp++, iter_temp->second);
			iter_temp++;
		}
		printf("\n");
		// test functions+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

		for (unsigned int APV_index_temp = 0; APV_index_temp < GEMInfor_Buffer_temp->GEMInfor_fApvCount; ++APV_index_temp) {

			printf("[Test INFOR]:: Decoding APV %d/%d infor\n",APV_index_temp,GEMInfor_Buffer_temp->GEMInfor_fApvCount);

			GEMAPVinfor *GEMAPV_Buffer_temp= new GEMAPVinfor();      // temp buffer for APVinfor class

			uint32_t DATA_APVConfiguration;
			fread(&DATA_APVConfiguration, sizeof(uint32_t),1,file_input);

			GEMAPV_Buffer_temp->GEMAPVinfor_fADC		= DATA_APVConfiguration&0xf;
			GEMAPV_Buffer_temp->GEMAPVinfor_fI2C		= (DATA_APVConfiguration>>4)&0x1f;
			GEMAPV_Buffer_temp->GEMAPVinfor_fNumSample  = (DATA_APVConfiguration>>9)&0x7f;
			GEMAPV_Buffer_temp->GEMAPVinfor_fLatency	= (DATA_APVConfiguration>>16)&0xff;
			GEMAPV_Buffer_temp->GEMAPVinfor_fMode		= (DATA_APVConfiguration>>24)&0x3f;

			GEMInfor_Buffer_temp->GEMInfor_fAPVIndex.insert(make_pair(GEMAPV_Buffer_temp->GEMAPVinfor_fI2C,GEMAPV_Buffer_temp->GEMAPVinfor_fADC));

			(GEMInfor_Buffer_temp->GEM_APV_Attached).push_back(*GEMAPV_Buffer_temp);
			delete GEMAPV_Buffer_temp;
		 }
		/*map<int,int>::iterator iter_test=(GEMInfor_Buffer_temp->GEMInfor_fAPVIndex).begin();
		while(iter_test!=(GEMInfor_Buffer_temp->GEMInfor_fAPVIndex).end()) {
			printf("[Test Variables]:: I2C %d ADC %d\n",iter_test->first,iter_test->second);
			iter_test++;
		}*/


		vector<GEMAPVinfor>::iterator test_temp = (GEMInfor_Buffer_temp->GEM_APV_Attached).begin();
		for(test_temp;test_temp<(GEMInfor_Buffer_temp->GEM_APV_Attached).end();test_temp++){
			//
			printf("[Test Variables]:: fADC=%d, I2C=%d, fNumberSample=%d, fLatency=%d, fMode=%d\n",
					test_temp->GEMAPVinfor_fADC,
					test_temp->GEMAPVinfor_fI2C,
					test_temp->GEMAPVinfor_fNumSample,
					test_temp->GEMAPVinfor_fLatency,
					test_temp->GEMAPVinfor_fMode
						);
		}
		//GEMInfor_Buffer_Input.push_back(*GEMInfor_Buffer_temp);
		GEMInfor_Buffer_Input.push_back(*GEMInfor_Buffer_temp);
	}    // end of for loop, loop all the MPDs
	if(GEMInfor_Buffer_Input.size()!=DATA_nMPD){
		printf("[ERROR]:: %s The MPD number doesnot match the setting\n",__FUNCTION__);
		exit(-1);
	}
	return GEMInfor_Buffer_Input;
};

vector<GEMInfor> GEMRawFileDecoder::GEMRawFileDecoder_ingestEventV5(FILE *file_input, TString ifile, vector<GEMInfor> GEMInfor_Buffer_Input) {
	printf("\n\n********DECODE RAW DATA********\n\n");
	printf("[RUN INFOR]:: %s Prepare decoding the raw data, %d MPD fund in total\n",__FUNCTION__,GEMInfor_Buffer_Input.size());
	vector<GEMInfor>::iterator Iter_GEMMPD=GEMInfor_Buffer_Input.begin();
	for(Iter_GEMMPD; Iter_GEMMPD < GEMInfor_Buffer_Input.end();Iter_GEMMPD++) {

	};
    uint32_t ievt=0;
	while(feof(file_input)==0)   {    // while the file did not reach the end

		uint32_t Data_temp;
		fread(&Data_temp,sizeof(uint32_t),1,file_input);
		printf("0x%x\n",Data_temp);

		// End of Evnts
		if((Data_temp&0xF0000000)== 0xE0000000) {
			ievt = Data_temp&0xFFFFFFF;

		};

		// vme Event header
		if((Data_temp&0xF0000000)== 0x10000000) {
			uint32_t Data_eventsID_temp= Data_temp&0xFFFFFFF;
			continue;
		}

		// User infor, if this is user block

		if((Data_temp&0xF0000000)== 0xD0000000) {
			uint32_t Data_eventsID_temp= Data_temp&0xFFFFFFF;
			uint32_t Data_UserWordCount, *Data_UserData;
			fread(&Data_UserWordCount,sizeof(uint32_t),1,file_input);   // read how large is the user data is
			Data_UserData= new uint32_t[Data_UserWordCount];
			fread(&Data_UserData,sizeof(uint32_t),Data_UserWordCount,file_input);   // read the user infor block
			delete Data_UserData;
			continue;
		}
    exit(0); // for test usage
	}

};

// test functions
void GEMRawFileDecoder::GEMRawFileDecoder_TestFunction(){
	vector<GEMInfor> MPD_infor_test;
	GEMRawFileDecoder_Run(MPD_infor_test);
};
