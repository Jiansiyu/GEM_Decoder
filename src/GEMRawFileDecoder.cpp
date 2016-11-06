/*
 * GEMRawFileDecoder.cpp
 *
 *  Created on: Oct 25, 2016
 *      Author: newdriver
 *
 *
 */

#include <stdio.h>
#include <stdint.h>
#include <fstream>
#include <vector>

// ROOT functions
#include <TString.h>
#include <TTree.h>

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

//
GEMRawFileDecoder::GEMRawFileDecoder(TString Raw_File, TTree &APVTree) {
	GEMRawFileDecoder_Raw_File = Raw_File;
	GEMAPV_tree = &APVTree;

	GEMAPV_tree->Branch("detID", &GEMR_ApvStrData.detID, "detID/I");
	GEMAPV_tree->Branch("MPDID", &GEMR_ApvStrData.MPDID, "MPDID/I");
	GEMAPV_tree->Branch("APVID", &GEMR_ApvStrData.APVID, "APVID/I");
	//GEMAPV_tree->Branch("SampleID", &GEMR_ApvStrData.SampeID, "SampeID/I");
	GEMAPV_tree->Branch("StripADC", *GEMR_ApvStrData.StripADC, "StripADC[KMAX_NSAMPLE][KNSTRIPS]/I");

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
		for(;test_temp<(GEMInfor_Buffer_temp->GEM_APV_Attached).end();test_temp++){
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

// file version 5 decoder, to b improved
vector<GEMInfor> GEMRawFileDecoder::GEMRawFileDecoder_ingestEventV5(FILE *file_input, TString ifile, vector<GEMInfor> GEMInfor_Buffer_Input) {

	vector<GEMInfor> GEMInfor_Buffer_return=GEMInfor_Buffer_Input;

	// globle control variables
	int Run_Ctrl_stateflg=0;



	// no  sure what is this used for
	uint16_t Data_ADCFifo_index=0;
	int Data_CommonBase = 0;
	int Data_DCount=0;

	printf("\n\n********DECODE RAW DATA********\n\n");
	printf("[RUN INFOR]:: %s Prepare decoding the raw data, %d MPD fund in total copy %d MPD \n",__FUNCTION__,GEMInfor_Buffer_Input.size(),GEMInfor_Buffer_return.size());
	vector<GEMInfor>::iterator Iter_GEMMPD=GEMInfor_Buffer_Input.begin();
	for(; Iter_GEMMPD < GEMInfor_Buffer_Input.end();Iter_GEMMPD++) {

	};


	uint32_t Data_eventsID_temp=0;
    uint32_t Data_PrevEventsID_temp=0;   // last events ID that just finished

	while(feof(file_input)==0)   {       // while the file did not reach the end, in this loop, it will loop over all the events

		uint32_t Data_temp;
		fread(&Data_temp,sizeof(uint32_t),1,file_input);
		//printf("0x%x\n",Data_temp);

		// End of Evnts
		if((Data_temp&0xF0000000)== 0xE0000000) {

			Data_eventsID_temp = Data_temp&0xFFFFFFF;
			Data_PrevEventsID_temp= Data_eventsID_temp;      // The end of one Events, this is the Evnts ID that Just finished
			printf("[Test Variables]:: End of Events=%d\n", Data_eventsID_temp);

		};

		// vme Event header
		if((Data_temp&0xF0000000)== 0x10000000) {
			printf("*******\n\n\n\nVME HRADER\n");
			Data_eventsID_temp= Data_temp&0xFFFFFFF;
			continue;
		}

		// User infor, if this is user block
		if((Data_temp&0xF0000000)== 0xD0000000) {

			printf("********\n\n\n\nUSER BLOCK\n");

			Data_eventsID_temp= Data_temp&0xFFFFFFF;
			uint32_t Data_UserWordCount, *Data_UserData;
			fread(&Data_UserWordCount,sizeof(uint32_t),1,file_input);   // read how large is the user data is
			Data_UserData= new uint32_t[Data_UserWordCount];
			fread(&Data_UserData,sizeof(uint32_t),Data_UserWordCount,file_input);   // read the user infor block
			delete Data_UserData;
			continue;
		}

		// trigger Event Header
		if((Data_temp&0xF0000000)== 0x20000000) {
			Data_eventsID_temp=Data_temp&0xFFFFFFF;
			printf("**********\n\n\n\nTRIGGER BLOCK\n");
			uint32_t Data_triggerpatttern;
			if(feof(file_input)==0) fread(&Data_triggerpatttern,sizeof(uint32_t),1,file_input);

			printf("[RUN INFOR]:: eventID=%d, trigger pattern =%d \n",Data_eventsID_temp, (int)Data_triggerpatttern);
			continue;
		}

		// APV Events header,
		if((Data_temp&0xF0000000)== 0xA0000000) {
			Data_eventsID_temp=Data_temp&0xFFFFFFF;
			printf("***********  Start of Evnts ***********\n");
			printf("**********APV EVENT BLOCK**********\n");
			printf("[RUN INFOR]:: eventID=%d\n",Data_eventsID_temp);

			Run_Ctrl_stateflg=0;  // reset when comes to the next events


			// in this do loop, it will loop all "6" time samples and all the APV cards, there will be 6*Number of Cards*128 strips events
			int Run_Ctrl_EndApvEndOfBlock=0;
			int Run_Ctrl_Current_APVID=-1;
			int Run_Ctrl_Current_MPDID=-1;
			int Run_Ctrl_Current_EvntID=-1;
			int Run_Ctrl_TimeSample_Index=-1;
			//variables that used for buffer one time sample & one cards events, set to -1 when initilize

			uint32_t Data_MPDIndex_temp=-1;      //MPD index informations
			uint32_t Data_APVADC_index_temp=-1;  //APV index informations
			//uint32_t Data_EventID_index_temp=-1; //Events ID tracking information
			map<int,int> Data_APV_StrADC_temp;
			map<int,map<int,int>> Data_APV_TimeSp_StrADC_temp;
			do {								// in this loop, it will loop over all the "6" time samples all the APVs
				uint32_t Data_temp;
				fread(&Data_temp,sizeof(uint32_t),1,file_input);
				int File_HeaderID_temp=(Data_temp>>19)&0x3;
				switch(File_HeaderID_temp) {
				// APVsampleHeader APVID, this is the begining of each 128 strips data,
				case 0x0: {
					if(Run_Ctrl_stateflg!=0) {
						printf("[ERROR]:: %s wrong control flag, it should be 0, but here the control flag is %d\n",__FUNCTION__, Run_Ctrl_stateflg);
					}
					Run_Ctrl_EndApvEndOfBlock = Data_temp & 0x40000; // end of APV block
					if(Run_Ctrl_EndApvEndOfBlock){      // if reach the end of APV events block
						break;
					}

					//double check the buffer variable that used for buffer the data, can be deleted
					if((Data_MPDIndex_temp!=-1)||(Data_APVADC_index_temp!=-1)||(Data_APV_StrADC_temp.size()!=0)) {  // check the buffer data, whether the data is initialized successfully, to avoid conflict
						printf("[ERROR]:: %s The Raw data is not initialized \n");
						Data_MPDIndex_temp=-1;
						Data_APVADC_index_temp=-1;
						Data_APV_StrADC_temp.clear();
					};

					Run_Ctrl_stateflg=1;
					uint32_t Data_FrameHeader=(Data_temp>>4)& 0xFFF;   // varify the frame header
					if(((Data_FrameHeader&0xE00)!= 0xE00)||((Data_FrameHeader&0x1)==0)) {    // check the header
						printf("[ERROR]::  %s  Frame header Error\n",__FUNCTION__);
					}
					Data_ADCFifo_index = Data_temp&0xf;

					// save the APV adc index to the save buffer
					Data_APVADC_index_temp= Data_ADCFifo_index;

					Data_CommonBase    = (Data_temp >> 6) & 0x800;
					//printf("******** %d ***************\n",Data_temp&0xF);
					printf("[Test Variables]::* First * %s APV infors=> EventsID=%d, IAPV=%d \n",__FUNCTION__,Data_eventsID_temp, Data_ADCFifo_index);
					Data_DCount=0;
					break;
				}

				// APV sample Data
				case 0x1: {
					if((Run_Ctrl_stateflg<1)||(Run_Ctrl_stateflg>2)){
						printf("[ERROR]:: %s wrong control flag, it should be 1/2, but here the control flag is %d\n",__FUNCTION__, Run_Ctrl_stateflg);
					}
					Run_Ctrl_stateflg=2;
					Data_APV_StrADC_temp.insert(make_pair((Data_temp>>12)&0x7f,Data_temp& 0xFFF));
					//printf("[Test Variables]::* First <-> Second *\n");
					//printf("[Test Variables]::EventsID=%d, Count=%d, trips=%d, ADC= %d \n",Data_eventsID_temp, Data_DCount++,(Data_temp>>12)&0x7f, Data_temp& 0xFFF);
					break;
				}

				// APVSampleTrailer
				case 0x2: {
					if((Run_Ctrl_stateflg<1)||(Run_Ctrl_stateflg>2)){
						printf("[ERROR]:: %s wrong control flag, it should be 1/2, but here the control flag is %d\n",__FUNCTION__, Run_Ctrl_stateflg);
					}
					//Time_samples_test=0;
					Run_Ctrl_stateflg = 3;
					uint32_t Data_MPD_slot_index=(Data_temp >> 12)& 0x1F;    //MPD-id

					//  save the MPD slot index to the saving buffer
					Data_MPDIndex_temp = Data_MPD_slot_index;

					uint32_t Data_Frame_Trailer=Data_temp&& 0xfff;
					printf("[Test Variables]:: * Second * MPD slot_index= %d Frame_trailer (sample index)=%d \n", Data_MPD_slot_index,Data_Frame_Trailer);
					//printf("Steps two APVSample Traile\n");
					break;
				}

				//APV  Sample End Block, save and statistic all the data here
				case 0x3: {

					if(Run_Ctrl_stateflg!=3) {
						printf("[ERROR]:: %s wrong control flag, it should be 0, but here the control flag is %d\n",__FUNCTION__, Run_Ctrl_stateflg);
					 }
					printf("[Test Variables]::  Steps three sample end block\n"
							"");
					Run_Ctrl_stateflg=0;

					//check the buffer data and send the data to the data saving function, and initilize the variables, get ready for next data
					//printf("[Test Variables]:: MPDIndex=%d, APV ADC Index =%d, Number of Strips =%d\n\n\n\n",Data_MPDIndex_temp,Data_APVADC_index_temp,Data_APV_StrADC_temp.size());
					if(((Run_Ctrl_Current_APVID==-1)||(Run_Ctrl_Current_EvntID==-1)||(Run_Ctrl_Current_MPDID==-1))&&(Run_Ctrl_TimeSample_Index==-1)) {  //initial status, new EventsID
						Run_Ctrl_Current_APVID  = Data_APVADC_index_temp;
						Run_Ctrl_Current_EvntID = Data_eventsID_temp;
						Run_Ctrl_Current_MPDID  = Data_MPDIndex_temp;
						Run_Ctrl_TimeSample_Index=1;
						Data_APV_TimeSp_StrADC_temp.insert(make_pair(Run_Ctrl_TimeSample_Index,Data_APV_StrADC_temp));   // buffer the initial time sample
						Run_Ctrl_TimeSample_Index++; // next time sample
					}
					else
					{    // old Event ID but New APVID or New MPD_ID
						if((Run_Ctrl_Current_APVID  != Data_APVADC_index_temp)||(Run_Ctrl_Current_EvntID != Data_eventsID_temp)||(Run_Ctrl_Current_MPDID  != Data_MPDIndex_temp)) {   // new round data

							// for Test Usage
							if(Run_Ctrl_Current_EvntID != Data_eventsID_temp) {
								printf("[ERROR]:: %s DATA is not what we expected, maybe there is something wrong, contact the author if you are not sure\n",__FUNCTION__);
							}
							if(Run_Ctrl_Current_MPDID  != Data_MPDIndex_temp) {
								printf("[WORNING]:: %s Are you using MULTI-MPD during the process?? Please have a double check, if not, there is a error in the data format\n",__FUNCTION__);
							}

							//printf("\n\n\n\n[Test Variables]::  *** Run_Ctrl_TimeSample_Index= %d, size=%d *****\n\n\n\n ", Run_Ctrl_TimeSample_Index-1, Data_APV_TimeSp_StrADC_temp.size());

							GEMRawFileDecoder_TreeSave(Run_Ctrl_Current_EvntID,Run_Ctrl_Current_MPDID,Run_Ctrl_Current_APVID,Data_APV_TimeSp_StrADC_temp);
							Data_APV_TimeSp_StrADC_temp.clear();				  // finish all the time sample for one APV

							Run_Ctrl_Current_APVID  = Data_APVADC_index_temp;     //
							//Run_Ctrl_Current_EvntID = Data_eventsID_temp;       // The prpgram will jump out this APV event block, so this variable will and only will change during the first time sample
							Run_Ctrl_Current_MPDID  = Data_MPDIndex_temp;		  // Multi-MPD usage
							Run_Ctrl_TimeSample_Index=1;
							Data_APV_TimeSp_StrADC_temp.insert(make_pair(Run_Ctrl_TimeSample_Index,Data_APV_StrADC_temp));   // buffer the initial time sample
							Run_Ctrl_TimeSample_Index++; // next time sample
						}
						else{
							Data_APV_TimeSp_StrADC_temp.insert(make_pair(Run_Ctrl_TimeSample_Index,Data_APV_StrADC_temp));   // buffer the initial time sample
							Run_Ctrl_TimeSample_Index++; // next time sample
						}

					}

					Data_MPDIndex_temp=-1;			//  this will renew after finish this step( new time sample or new APV or new Events)
					Data_APVADC_index_temp=-1;      //  this will renew after finish this step( new time sample or new APV or new Events)
					Data_APV_StrADC_temp.clear();

					break;
				}
				default:
					printf("[ERROR]:: Error in decoding raw data\n");

				 }

			}
			while((feof(file_input)==0)&&(Run_Ctrl_EndApvEndOfBlock==0));
			Data_APV_StrADC_temp.clear();
			Data_APV_TimeSp_StrADC_temp.clear();

		};   //APV block Ended all

		// MAROC block
		if((Data_temp & 0xF0000000) == 0xB0000000) {
			printf("[Run Infor]:: MAROC Block decoder\n");
			Data_eventsID_temp= Data_temp & 0xFFFFFFF;
			uint32_t Data_MaRocWordCount=0;
			fread(&Data_MaRocWordCount,sizeof(uint32_t),1,file_input);
			int Data_WordCount=Data_MaRocWordCount & 0xFFFF;
			int Data_DeviceID=(Data_MaRocWordCount>>16)&0xFFF;
			printf("[Run Infor]:: MaRoc Block of CB %d, word count=%d\n", Data_DeviceID,Data_WordCount);
			unsigned char *Data_Maroc;
			Data_Maroc=(unsigned char *)malloc(4*Data_WordCount*sizeof(char));
			fread(&Data_Maroc,sizeof(char),4*Data_MaRocWordCount,file_input);
			continue;
		}

    //exit(0); // for test usage
	}
//GEMRawFileDecoder_DataCheck(GEMInfor_Buffer_return);
return GEMInfor_Buffer_return;
};


// save the individual data informations to the data buffer
vector<GEMInfor> GEMRawFileDecoder::GEMRawFileDecoder_BufferSave(std::vector<GEMInfor> GEMInfor_Buffer_Input,int EventID_index_Input,uint32_t MPD_Index_Input, uint32_t APVADC_Index_Input,map<int,map<int, int> > Tsample_StrADC_Input) {

	vector<GEMInfor> GEMInfor_Buffer_return;    // return data
	GEMInfor_Buffer_return.clear();
    //printf("\n**************** %d ***********************\n",GEMInfor_Buffer_Input.size());
	//GEMInfor *GEMInfor_Indiv_buffer;            // single MPD infor buffer

	vector<GEMInfor>::iterator Iter_MPD=GEMInfor_Buffer_Input.begin();        /// loop all the MPDs and find MPD that the data belongs to
	for(;Iter_MPD<GEMInfor_Buffer_Input.end();Iter_MPD++){    // loop the MPD, Find the MPD

		//printf("slot in vector =%d   MPD slot =%d \n",(*Iter_MPD).GEMInfor_fSlot,MPD_Index_Input );
		if((*Iter_MPD).GEMInfor_fSlot == MPD_Index_Input){    //

			GEMInfor * GEMInfor_Buffer_temp=new GEMInfor();
			(*Iter_MPD).GEMInfor_clone(*GEMInfor_Buffer_temp);

			// for test usage
			//printf("********   size=%d  ***********\n",(GEMInfor_Buffer_temp->GEM_APV_Attached).size());
			int size1=(*Iter_MPD).GEMInfor_fAPVIndex.size();
			int size2=GEMInfor_Buffer_temp->GEMInfor_fAPVIndex.size();
			if(!((size1==size2)&&(size2!=0))){
				printf("[ERROR]:: %s Error in copying data vector\n",__FUNCTION__);
				}
			else
			{
				vector<GEMAPVinfor>::iterator Iter_APV=(GEMInfor_Buffer_temp->GEM_APV_Attached).begin();
				vector<GEMAPVinfor> GEM_APV_Attached_buffer_temp;
				for(;Iter_APV<(GEMInfor_Buffer_temp->GEM_APV_Attached).end();Iter_APV++){
					if((*Iter_APV).GEMAPVinfor_fADC==APVADC_Index_Input) {      // find the APV that the data belongs to

						GEMAPVinfor *GEMAPVInfor_Buffer_temp= new GEMAPVinfor();
						(*Iter_APV).GEMAPVinfor_clone(*GEMAPVInfor_Buffer_temp); //clone the vector
						(GEMAPVInfor_Buffer_temp->GEMAPVinfor_RawData).insert(make_pair(EventID_index_Input,Tsample_StrADC_Input));
						//printf("**************8 size=%d\n",(GEMAPVInfor_Buffer_temp->GEMAPVinfor_RawData).size());
						GEM_APV_Attached_buffer_temp.push_back(*GEMAPVInfor_Buffer_temp);

					}
					else {														//
						GEM_APV_Attached_buffer_temp.push_back(*Iter_APV);
					}
				}
				GEMInfor_Buffer_temp->GEM_APV_Attached.clear();
				GEMInfor_Buffer_temp->GEM_APV_Attached=GEM_APV_Attached_buffer_temp;
				GEMInfor_Buffer_return.push_back(*GEMInfor_Buffer_temp);
			 }

		  }
		 else {    // if this is not the MPD that the data belongs to, just buffer the data to return vector
			GEMInfor_Buffer_return.push_back(*Iter_MPD);
		}
	}
	return GEMInfor_Buffer_return;
}

//
void GEMRawFileDecoder::GEMRawFileDecoder_TreeSave(int EventID_index_temp,uint32_t MPD_Index_Input, uint32_t APVADC_Index_Input,map<int,map<int, int> > Tsample_StrADC_Input) {
	printf("\n\n\n");
	printf("save function\n");
	GEMR_ApvStrData.EventID=EventID_index_temp;
	GEMR_ApvStrData.MPDID=MPD_Index_Input;
	GEMR_ApvStrData.APVID=APVADC_Index_Input;

	map<int,map<int,int>>::iterator Iter_Tsamples=Tsample_StrADC_Input.begin();
	while (Iter_Tsamples!=Tsample_StrADC_Input.end()){

		map<int,int>::iterator Iter_NStrips = (Iter_Tsamples->second).begin();
		while (Iter_NStrips != (Iter_Tsamples->second).end()) {

			//printf("EID=%d,  MPDID=%d,  APVID=%d, Sample=%d, Strps=%d,   adc = %d\n",EventID_index_temp,MPD_Index_Input, APVADC_Index_Input, Iter_Tsamples->first,
			//		Iter_NStrips->first, Iter_NStrips->second);

			//GEMR_ApvStrData.StripADC[Iter_Tsamples->first][Iter_NStrips->first]=Iter_NStrips->second;
			Iter_NStrips++;
		}
		Iter_Tsamples++;
	}
	//GEMAPV_tree->Fill();
   //return GEMAPV_tree;
	printf("\n\n\n");
}

// file format check function, mainly used for test
int GEMRawFileDecoder::GEMRawFileDecoder_DataCheck(vector<GEMInfor> GEMInfor_Buffer_Input) {

	// error flag
	int ERROR_FLAG_ID=-10;

	vector<GEMInfor>::iterator Iter_MPD=GEMInfor_Buffer_Input.begin();
	for(;Iter_MPD < GEMInfor_Buffer_Input.end();Iter_MPD++){
		// check the the matching of the setting
		map<int,int>::iterator I2C_check_iter=(*Iter_MPD).GEMInfor_fAPVIndex.begin();
		while(I2C_check_iter != (*Iter_MPD).GEMInfor_fAPVIndex.end()) {
			vector<GEMAPVinfor>::iterator Iter_APV=(*Iter_MPD).GEM_APV_Attached.begin();
			for(;Iter_APV < (*Iter_MPD).GEM_APV_Attached.end();Iter_APV++)
			{
				if(((*I2C_check_iter).first==(*Iter_APV).GEMAPVinfor_fI2C)&&((*I2C_check_iter).second==(*Iter_APV).GEMAPVinfor_fADC)){
					printf("[RUN INFOR check]:: %s ADC=%d, I2C=%d, Events=%d\n",__FUNCTION__, (*Iter_APV).GEMAPVinfor_fADC, (*Iter_APV).GEMAPVinfor_fI2C, (*Iter_APV).GEMAPVinfor_RawData.size());
				}
			}
			I2C_check_iter++;
		}

	}

return 1;
}

// test functions
void GEMRawFileDecoder::GEMRawFileDecoder_TestFunction(){
	vector<GEMInfor> MPD_infor_test;
	GEMRawFileDecoder_Run(MPD_infor_test);
};
