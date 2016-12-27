/*
 * GEMRawFileDecoder.cpp
 *
 *  Created on: Oct 25, 2016
 *      Author: Siyu Jian
 *  	email : jiansiyu@gmail.com
 *
 *
 */

#include <stdio.h>
#include <stdint.h>

#include <stdint.h>

#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>   // used for remove_if
#include <vector>

// ROOT functions
#include "TObject.h"
#include <TString.h>
#include <TTree.h>
#include "TH1.h"
#include "TCanvas.h"
#include "TThread.h"
#include "TFile.h"
// user defined functions
#include "GEMRawFileDecoder.h"   // declaration
#include "GEMInfor.h"           // GEM infor class
#include "GEMEventDecoder.h"	// Low level calculation method

using namespace std;

GEMRawFileDecoder::GEMRawFileDecoder() {
	// TODO Auto-generated constructor stub

};

GEMRawFileDecoder::GEMRawFileDecoder(TString Raw_File){
	GEMRawFileDecoder_Raw_File = Raw_File;
	printf("[Test Infor]:: %s\n",Raw_File.Data());
};

//return the TTree files which buffers all the data
GEMRawFileDecoder::GEMRawFileDecoder(TString Raw_File, TTree *APVTree) {
	GEMRawFileDecoder_Raw_File = Raw_File;
	//&GEMAPV_tree= APVTree;
	GEMAPV_tree.Branch("EventID", &GEMR_ApvStrData.EventID, "EventID/I");
	GEMAPV_tree.Branch("MPDID", &GEMR_ApvStrData.MPDID, "MPDID/I");
	GEMAPV_tree.Branch("APVID", &GEMR_ApvStrData.APVID, "APVID/I");
	for(unsigned i =0; i<KMAX_NSAMPLE; i++){
		GEMAPV_tree.Branch(Form("StripADC[%d]",i), GEMR_ApvStrData.StripADC[i], Form("StripADC[%d]/I",i));
	};
	printf("[Test Infor]:: %s\n",Raw_File.Data());
};

GEMRawFileDecoder::~GEMRawFileDecoder() {
	// TODO Auto-generated destructor stub
}

// used for display the raw data
void GEMRawFileDecoder::GEMRawFileDecoder_RawDisplay(int Entries_input) {

	TCanvas *Canvas_Raw=new TCanvas(Form("Raw_Display_Canvas_Evt"),Form("Raw display"),1000,1000);
	Canvas_Raw->Divide(2,(int)4/2);

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
		//printf("[Test Variables]:: %s fileVersion=%x, fNumberSample=%d, InpHand_fNumberAPV = %d \n",__FUNCTION__, fileVersion_temp,fNumberSample_temp,fNumberAPV_temp);

		if(fileVersion_temp == 5) {
			//printf("[RUN INFOR]:: %s Decoding the headers\n",__FUNCTION__);
			GEMInfor_Buffer_temp=GEMRawFileDecoder_ingestFileHeader(Input_File_temp, (int)fileVersion_temp, GEMInfor_Buffer_temp); // decoder the file header
			//printf("[RUN INFOR]:: %s Finish decoding the headers  %d MPD found, which match the setting\n",__FUNCTION__,GEMInfor_Buffer_temp.size());

			map <int, map < int, map < int, map<int, map < int, int > > > > > SingleEvent_temp;
			while(feof(Input_File_temp)==0) {
				if(Entries_input<0){
					SingleEvent_temp=GEMRawFileDecoder_SingleingestEventV5(Input_File_temp, GEMRawFileDecoder_Raw_File ,GEMInfor_Buffer_temp); // decoder the data
				}
				else{

					do{
						SingleEvent_temp=GEMRawFileDecoder_SingleingestEventV5(Input_File_temp, GEMRawFileDecoder_Raw_File ,GEMInfor_Buffer_temp); // decoder the data
					}while((SingleEvent_temp.begin()->first) <Entries_input);
				}
			map < int, map < int, map<int, map < int, int > > > >::iterator itter_mpd=SingleEvent_temp.begin()->second.begin();
			int NumberofAPV=0;
			while(itter_mpd!=SingleEvent_temp.begin()->second.end())
			{
			 NumberofAPV+=itter_mpd->second.size();
			 itter_mpd++;
			 }
			printf("Entries=%d APV=%d\n",SingleEvent_temp.begin()->first,NumberofAPV);


			itter_mpd=SingleEvent_temp.begin()->second.begin();
			int Histo_counter=0;

			TH1F *sEvent_histo[NumberofAPV];
			while(itter_mpd!=SingleEvent_temp.begin()->second.end()){   // loop on MPD
				map < int, map<int, map < int, int > > > ::iterator ittter_apv=itter_mpd->second.begin();
				while(ittter_apv!=itter_mpd->second.end()){                                               // loop on APV
					TThread::Lock();
					sEvent_histo[Histo_counter]= new TH1F(Form("Ev%d_MPD%d_APV%d",SingleEvent_temp.begin()->first,itter_mpd->first,ittter_apv->first),
														Form("Ev%d_MPD%d_APV%d",SingleEvent_temp.begin()->first,itter_mpd->first,ittter_apv->first),
														(SingleEvent_temp.begin()->second.begin()->second.begin()->second.size())*(128+2)-1,
														0,
														(SingleEvent_temp.begin()->second.begin()->second.begin()->second.size())*(128+2));
					TThread::UnLock();

					map<int, map < int, int > >::iterator itttter_Tsample=ittter_apv->second.begin();
					while(itttter_Tsample!=ittter_apv->second.end()){
						map < int, int >::iterator ittttter_Nstrips=itttter_Tsample->second.begin();
						while(ittttter_Nstrips!=itttter_Tsample->second.end()){
							TThread::Lock();
							sEvent_histo[Histo_counter]->Fill((itttter_Tsample->first)*(128+2)+ChNb[ittttter_Nstrips->first],ittttter_Nstrips->second);
							TThread::UnLock();

							ittttter_Nstrips++;
						  }
						  itttter_Tsample++;
					  }
					ittter_apv++;
					Histo_counter++;
				}
				itter_mpd++;
			}

			for(int i=0; i<NumberofAPV; i++)
			{
                Canvas_Raw->cd(i+1);
                sEvent_histo[i]->Draw();
                Canvas_Raw->Update();
		     }
			Canvas_Raw->Modified();
			Canvas_Raw->Update();
			string a ;
			getline(cin,a);

			for(int i =0; i < NumberofAPV; i++)
				delete sEvent_histo[i];
			}
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
	delete Canvas_Raw;
};
void GEMRawFileDecoder::GEMRawFileDecoder_PedestalDecoder(string Pedestal_filename,int Entries_input){
	// reading mapping file test
	ifstream filestream("Mapping/temp_Mapping.cfg");
	string  line;
	int Mapping_mpdId,Mapping_ADCId,Mapping_I2C,Mapping_GEMId,Mapping_Xis,Mapping_Pos,Mapping_Invert;
	map<int,map<int,vector<int> > > mMapping;

	if(!filestream){
		printf("[ERROR]::  %s error when try to open the mapping file\n",__FUNCTION__);
		exit(-1);
	}else {
		printf("[INFOR]::  %s reading the mapping file\n",__FUNCTION__);
	}
	cout<<"MPDID"<<"	GEMID"<<"	Xis"<<"	ADCID"<<"	I2C"<<"	Pos"<<"	Invert"<<endl;
	while(getline(filestream,line)){   // reading the mapping line by line
		//cout<< line.c_str()<<endl;
		line.erase(std::remove_if(line.begin(),line.end(), ::isspace),line.end());   // remove the space in the string
		if(line.find("#")==0)continue;
		char *tokens=strtok((char *)line.data(),",");// token one string before ,
		if(tokens!=NULL) {
			cout<<tokens<<"	";Mapping_mpdId=atoi(tokens);
			tokens = strtok(NULL, " ,");cout<<tokens<<"	";Mapping_GEMId=atoi(tokens);
			tokens = strtok(NULL, " ,");cout<<tokens<<"	";Mapping_Xis=atoi(tokens);
			tokens = strtok(NULL, " ,");cout<<tokens<<"	";Mapping_ADCId=atoi(tokens);
			tokens = strtok(NULL, " ,");cout<<tokens<<"	";Mapping_I2C=atoi(tokens);
			tokens = strtok(NULL, " ,");cout<<tokens<<"	";Mapping_Pos=atoi(tokens);
			tokens = strtok(NULL, " ,");cout<<tokens<<"	";Mapping_Invert=atoi(tokens);
			mMapping[Mapping_mpdId][Mapping_I2C].push_back(Mapping_GEMId);//0
			mMapping[Mapping_mpdId][Mapping_I2C].push_back(Mapping_Xis);//1
			mMapping[Mapping_mpdId][Mapping_I2C].push_back(Mapping_Pos);//2
			mMapping[Mapping_mpdId][Mapping_I2C].push_back(Mapping_Invert);//3
		}
	  cout<<endl;
	}
	filestream.close();
	// finish loading the mapping file
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
    // ev    MPD       APV   nstrips Sigma
    map<int,map<int,map<int,map<int,int> > > > Sigma_all;

    fread(&fileID_temp, sizeof(uint32_t),1,Input_File_temp); // read the fileID
    if(fileID_temp == BINARYFILE_ID){      // mactch the file ID
        fread(&fileVersion_temp,sizeof(uint32_t),1,Input_File_temp);   // load the file version
        fread(&fNumberSample_temp,sizeof(uint32_t),1,Input_File_temp);
        fread(&fNumberAPV_temp,sizeof(uint32_t),1,Input_File_temp);
        if(fileVersion_temp == 5) {
            GEMInfor_Buffer_temp=GEMRawFileDecoder_ingestFileHeader(Input_File_temp, (int)fileVersion_temp, GEMInfor_Buffer_temp); // decoder the file header
            while(feof(Input_File_temp)==0) {
            	map <int, map < int, map < int, map<int, map < int, int > > > > > SingleEvent_temp; //single events
                SingleEvent_temp=GEMRawFileDecoder_SingleingestEventV5(Input_File_temp, GEMRawFileDecoder_Raw_File ,GEMInfor_Buffer_temp); // decoder the data
                if(SingleEvent_temp.size()==1){
                	GEMEventDecoder *GEMSgEvntsDecode=new GEMEventDecoder(SingleEvent_temp);
                	// EvntID  MPD   APV    nstrips  Sigma
                	map<int,map<int,map<int,map<int,int>>>> SingleEvent_sigma_temp=GEMSgEvntsDecode->eDGetSigma();
                	Sigma_all.insert(make_pair(SingleEvent_sigma_temp.begin()->first,SingleEvent_sigma_temp.begin()->second));
                	delete GEMSgEvntsDecode;
                }
                else{
                printf("[%s(line%d)] unrecognized data package detected, ignore",__FUNCTION__, __LINE__);
                continue;
                }
            }
            printf("This is the end of the file\n");
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
   // finish decoder all the data, start caclculate the pedestals for the program
    TThread::Lock();
    // final result
    map<int, map <int, TH1F* > > GEMEvts_PedestalMean;  //MPDID, APVID,TH1F(channel-128, Mean)
    map<int, map <int, TH1F* > > GEMEvts_PedestalSigma; //MPDID, APVID,TH1F(channel-128, Sigma)
    // used to get the final result
    // MPDID   APVID     chennals
    map<int, map<int, map<int, TH1F* > > > GEMEvts_PedestalHisto; //MPD, APVID, Channel, sigma of six sample
    map<int,map<int,vector<int> > >::iterator iter_mapping_mpd=mMapping.begin();
    while(iter_mapping_mpd!=mMapping.end()) {
    	map<int,vector<int> > ::iterator itter_mapping_apv=iter_mapping_mpd->second.begin();
    	while(itter_mapping_apv!=iter_mapping_mpd->second.end()){
    		//printf("[INFOR]:: initialize the result histogram  MPD%d APV%d\n",iter_mapping_mpd->first,itter_mapping_apv->first);
    		GEMEvts_PedestalMean[iter_mapping_mpd->first][itter_mapping_apv->first]=new TH1F(Form("PedestalMean(offset)_mpd_%d_ch_%d",iter_mapping_mpd->first,itter_mapping_apv->first),Form("PedestalMean(offset)_mpd_%d_ch_%d",iter_mapping_mpd->first,itter_mapping_apv->first),128,0,128);
    		GEMEvts_PedestalSigma[iter_mapping_mpd->first][itter_mapping_apv->first]=new TH1F(Form("PedestalRMS_mpd_%d_ch_%d",iter_mapping_mpd->first,itter_mapping_apv->first),Form("PedestalRMS_mpd_%d_ch_%d",iter_mapping_mpd->first,itter_mapping_apv->first),128,0,128);
    		for(int i=0;i<128;i++){
    			//printf("[INFOR]:: initialize the result histogram  MPD%d APV%d nstrips%d\n",iter_mapping_mpd->first,itter_mapping_apv->first,i);
    			GEMEvts_PedestalHisto[iter_mapping_mpd->first][itter_mapping_apv->first][i]=new TH1F(Form("MPD%d_APV%d_CH%d\n",iter_mapping_mpd->first,itter_mapping_apv->first,i),Form("MPD%d_APV%d_CH%d\n",iter_mapping_mpd->first,itter_mapping_apv->first,i),8000,-2000,2000);
    		}
    		itter_mapping_apv++;
    	}
    	iter_mapping_mpd++;
    }
    TThread::UnLock();
    printf("[%s(line%d)]  start calculate pedestals\n",__FUNCTION__,__LINE__);
    iter_mapping_mpd=mMapping.begin(); //mapping
    while(iter_mapping_mpd!=mMapping.end()){
    	map<int,vector<int>>::iterator itter_mapping_apv=iter_mapping_mpd->second.begin();
    	while(itter_mapping_apv!=iter_mapping_mpd->second.end()) {
    		// write the sigmain the histogram
    		map<int,map<int,map<int,map<int,int> > > >::iterator iter_sigma_events=Sigma_all.begin();
    		while(iter_sigma_events!=Sigma_all.end()){ // loop on all the events
    			for(int nstrips_cpunter=0; nstrips_cpunter<128;nstrips_cpunter++){
    				if((iter_sigma_events)->second[iter_mapping_mpd->first][itter_mapping_apv->first][nstrips_cpunter]!=NULL){
    					GEMEvts_PedestalHisto[iter_mapping_mpd->first][itter_mapping_apv->first][nstrips_cpunter]->Fill((iter_sigma_events)->second[iter_mapping_mpd->first][itter_mapping_apv->first][nstrips_cpunter]);
    				}
    			}
    			iter_sigma_events++;
    		}
    		itter_mapping_apv++;
    	}
    	iter_mapping_mpd++;
    }
    map<int, map<int, map<int, TH1F* > > >::iterator iter_histo_mpd=GEMEvts_PedestalHisto.begin();
    while(iter_histo_mpd!=GEMEvts_PedestalHisto.end()){
    	map<int, map<int, TH1F* > >::iterator itter_histo_apv=iter_histo_mpd->second.begin();
    	while(itter_histo_apv!=iter_histo_mpd->second.end()){
    		map<int, TH1F* >::iterator ittter_histo_nstrips=itter_histo_apv->second.begin();
    		while(ittter_histo_nstrips!=itter_histo_apv->second.end()){   // loop on all the  strips
    			GEMEvts_PedestalMean[iter_histo_mpd->first][itter_histo_apv->first]->Fill(ittter_histo_nstrips->first,ittter_histo_nstrips->second->GetMean());
    			GEMEvts_PedestalSigma[iter_histo_mpd->first][itter_histo_apv->first]->Fill(ittter_histo_nstrips->first,ittter_histo_nstrips->second->GetRMS());
    			ittter_histo_nstrips++;
    		}
    		itter_histo_apv++;
    	}
    	iter_histo_mpd++;
    }
    // write the data in root file
    printf("[%s(%d)] create root file\n",__FUNCTION__, __LINE__);
    TFile *rootfile=new TFile(Pedestal_filename.c_str(),"RECREATE");
    map<int, map<int, TH1F* >>::iterator iter_report_mpd=GEMEvts_PedestalMean.begin();
    while(iter_report_mpd!=GEMEvts_PedestalMean.end()){
    	map<int, TH1F* >::iterator itter_report_apv=iter_report_mpd->second.begin();
    	while(itter_report_apv!=iter_report_mpd->second.end()){
    		//itter_report_apv->second->Write();
    		GEMEvts_PedestalMean[iter_report_mpd->first][itter_report_apv->first]->Write();
    		GEMEvts_PedestalSigma[iter_report_mpd->first][itter_report_apv->first]->Write();
    		itter_report_apv++;
    	}
    	iter_report_mpd++;
    }
    rootfile->Close();
}


void GEMRawFileDecoder::GEMRawFileDecoder_HistoDecoder(string pedestal_file,string output_file,int Entries_input) {

	// reading mapping file test
	ifstream filestream("Mapping/temp_Mapping.cfg");
	string  line;
	int Mapping_mpdId,Mapping_ADCId,Mapping_I2C,Mapping_GEMId,Mapping_Xis,Mapping_Pos,Mapping_Invert;
	map<int,map<int,vector<int> > > mMapping;

	if(!filestream){
		printf("[ERROR]::  %s error when try to open the mapping file\n",__FUNCTION__);
		exit(-1);
	   }else {
	        printf("[INFOR]::  %s reading the mapping file\n",__FUNCTION__);
	}
	cout<<"MPDID"<<"	GEMID"<<"	Xis"<<"	ADCID"<<"	I2C"<<"	Pos"<<"	Invert"<<endl;
	while(getline(filestream,line)){   // reading the mapping line by line
		//cout<< line.c_str()<<endl;
		line.erase(std::remove_if(line.begin(),line.end(), ::isspace),line.end());   // remove the space in the string
		if(line.find("#")==0)continue;
		char *tokens=strtok((char *)line.data(),",");// token one string before ,
		if(tokens!=NULL) {
			cout<<tokens<<"	";Mapping_mpdId=atoi(tokens);
			tokens = strtok(NULL, " ,");cout<<tokens<<"	";Mapping_GEMId=atoi(tokens);
			tokens = strtok(NULL, " ,");cout<<tokens<<"	";Mapping_Xis=atoi(tokens);
			tokens = strtok(NULL, " ,");cout<<tokens<<"	";Mapping_ADCId=atoi(tokens);
			tokens = strtok(NULL, " ,");cout<<tokens<<"	";Mapping_I2C=atoi(tokens);
			tokens = strtok(NULL, " ,");cout<<tokens<<"	";Mapping_Pos=atoi(tokens);
			tokens = strtok(NULL, " ,");cout<<tokens<<"	";Mapping_Invert=atoi(tokens);
			mMapping[Mapping_mpdId][Mapping_I2C].push_back(Mapping_GEMId);//0
			mMapping[Mapping_mpdId][Mapping_I2C].push_back(Mapping_Xis);//1
			mMapping[Mapping_mpdId][Mapping_I2C].push_back(Mapping_Pos);//2
			mMapping[Mapping_mpdId][Mapping_I2C].push_back(Mapping_Invert);//3
		}
		cout<<endl;
	}
	filestream.close();
	// finish loading the mapping

	// generate the root file, ready to save the histo data
	Int_t EvtID,nch,*Vstrip,*VdetID,*VplaneID,*adc0,*adc1,*adc2,*adc3,*adc4,*adc5;
	Vstrip	=new Int_t[2000];
	VdetID	=new Int_t[2000];
	VplaneID=new Int_t[2000];
	adc0	=new Int_t[2000];
	adc1	=new Int_t[2000];
	adc2	=new Int_t[2000];
	adc3	=new Int_t[2000];
	adc4	=new Int_t[2000];
	adc5	=new Int_t[2000];

	TFile *Hit_rootfile=new TFile(output_file.c_str(),"RECREATE");
	TTree *Hit=new TTree("GEMHit","Hit list");
	Hit->Branch("evtID",&EvtID,"evtID/I");	     // event ID, start from 1
	Hit->Branch("nch",&nch,"nch/I");                   // how many channels been fired at one time
	Hit->Branch("strip",Vstrip,"strip[nch]/I");	     // number of strips that been fired
	Hit->Branch("detID",VdetID,"detID[nch]/I");        // detector ID ? used for multi-detector case ?  always 0 in this case
	Hit->Branch("planeID",VplaneID,"planeID[nch]/I");  // plan ID

	Hit->Branch("adc0",adc0,"adc0[nch]/I");            // six samples
	Hit->Branch("adc1",adc1,"adc1[nch]/I");
	Hit->Branch("adc2",adc2,"adc2[nch]/I");
	Hit->Branch("adc3",adc3,"adc3[nch]/I");
	Hit->Branch("adc4",adc4,"adc4[nch]/I");
	Hit->Branch("adc5",adc5,"adc5[nch]/I");
	//end of initialize root tree to store hits

	// start decode the raw file
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
    // ev    MPD       APV   nstrips Sigma
    map<int,map<int,map<int,map<int,int> > > > Sigma_all;

    fread(&fileID_temp, sizeof(uint32_t),1,Input_File_temp); // read the fileID
    if(fileID_temp == BINARYFILE_ID){      // mactch the file ID
        fread(&fileVersion_temp,sizeof(uint32_t),1,Input_File_temp);   // load the file version
        fread(&fNumberSample_temp,sizeof(uint32_t),1,Input_File_temp);
        fread(&fNumberAPV_temp,sizeof(uint32_t),1,Input_File_temp);
        if(fileVersion_temp == 5) {
            GEMInfor_Buffer_temp=GEMRawFileDecoder_ingestFileHeader(Input_File_temp, (int)fileVersion_temp, GEMInfor_Buffer_temp); // decoder the file header
            while(feof(Input_File_temp)==0) {
            	map <int, map < int, map < int, map<int, map < int, int > > > > > SingleEvent_temp; //single events
                SingleEvent_temp=GEMRawFileDecoder_SingleingestEventV5(Input_File_temp, GEMRawFileDecoder_Raw_File ,GEMInfor_Buffer_temp); // decoder the data
                if(SingleEvent_temp.size()==1){

                }
                else{
                printf("[%s(line%d)] unrecognized data package detected, ignore",__FUNCTION__, __LINE__);
                continue;
                }
            }
            printf("This is the end of the file\n");
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

}
vector<GEMInfor> GEMRawFileDecoder::GEMRawFileDecoder_Run( vector<GEMInfor> GEMInfor_Buffer_Input) {

	vector<GEMInfor> GEMInfor_Buffer_temp;       // used for buffer the data temporary

	FILE *Input_File_temp;
	Input_File_temp = fopen(GEMRawFileDecoder_Raw_File.Data(), "r");
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
	//printf("[RUN INFOR]:: %s Start decoding the file header \n", __FUNCTION__);

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
	// loop all the MPDs. read all the configuration for the MPDs
	for(int MPD_Loop_counter=0; MPD_Loop_counter < (int)DATA_nMPD ; MPD_Loop_counter++) {
		//printf("[TEST INFOR]:: %s Decoding MPD No.%d\n",__FUNCTION__,MPD_Loop_counter);
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
			//printf("[RUN INFOR]:: %s Loading ADC gain\n",__FUNCTION__);
			uint32_t DATA_ADCconfiguration;
			fread(&DATA_ADCconfiguration,sizeof(uint32_t),1,file_input);
			for(unsigned int ADC_channel_index_loop=0; ADC_channel_index_loop< 8 ; ADC_channel_index_loop++){
				GEMInfor_Buffer_temp->GEMInfor_fAdcGain.insert(make_pair(ADCchip_index*8+ADC_channel_index_loop, (DATA_ADCconfiguration>>(ADC_channel_index_loop*4))&0xf));
			}
		}

		for (unsigned int APV_index_temp = 0; APV_index_temp < GEMInfor_Buffer_temp->GEMInfor_fApvCount; ++APV_index_temp) {

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
	const int GlNSample= (*(*GEMInfor_Buffer_Input.begin()).GEM_APV_Attached.begin()).GEMAPVinfor_fNumSample;
	// globle control variables
	int Run_Ctrl_stateflg=0;
	/// loop all the data find the apv data
	map< int , map < int , map < int, map< int,int > > > >  rdSingleEvent;   //MPDID, APVID, TimesampleID, StripsID, ADC value
	uint32_t Data_eventsID_temp=0;
	while(feof(file_input)==0)   {       // while the file did not reach the end, in this loop, it will loop over all the events

		uint32_t Data_temp;
		fread(&Data_temp,sizeof(uint32_t),1,file_input);

		// vme Event header
		if((Data_temp&0xF0000000)== 0x10000000) {
			Data_eventsID_temp= Data_temp&0xFFFFFFF;

			continue;
		}

		// User infor, if this is user block
		if((Data_temp&0xF0000000)== 0xD0000000) {
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
			//printf("**********\n\n\n\nTRIGGER BLOCK\n");
			uint32_t Data_triggerpatttern;
			if(feof(file_input)==0) fread(&Data_triggerpatttern,sizeof(uint32_t),1,file_input);
			//printf("[RUN INFOR]:: eventID=%d, trigger pattern =%d \n",Data_eventsID_temp, (int)Data_triggerpatttern);
			continue;
		}

		// APV Events header, one event data
		if((Data_temp&0xF0000000)== 0xA0000000) {
			Data_eventsID_temp=Data_temp&0xFFFFFFF;
			//printf("***********  Start of Evnts ***********\n");
			//printf("**********APV EVENT BLOCK**********\n");
			//printf("[RUN INFOR]:: eventID=%d\n",Data_eventsID_temp);

			Run_Ctrl_stateflg=0;  // reset when comes to the next events


			// in this do loop, it will loop all "6" time samples and all the APV cards, there will be 6*Number of Cards*128 strips events
			int Run_Ctrl_EndApvEndOfBlock=0;  // check whether reach the end of the data block
			uint16_t Data_APV_index=0;    // buffer the APV address
			//int Data_DCount=0;				  // original variable, maybe used for check the detected Nstrips, useless for map variable, which can be check by itself

			// data saving control variables
			int Run_Ctrl_Current_APVID=-1;
			int Run_Ctrl_Current_MPDID=-1;
			int Run_Ctrl_TimeSample_Index=-1;


			map < int,int > Data_APV_SingleTimeSp_StrADC_temp;				//
			map < int, map < int,int > > Data_APV_TimeSp_StrADC_temp;		//Time SampleID. <Number of Strips, ADC value>
			map < int, map< int, map<int,int > > > Data_AllAPV_temp;        // APVID, Time Sample ID, <Number of strips, ADC value>

			do {								//in this loop, it will loop over all the "6" time samples all the APVs

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
					Run_Ctrl_stateflg=1; // change the process control buffer, ready for the second step decode process
					uint32_t Data_FrameHeader=(Data_temp>>4)& 0xFFF;   // varify the frame header
					if(((Data_FrameHeader&0xE00)!= 0xE00)||((Data_FrameHeader&0x1)==0)) {    // check the header
						printf("[ERROR]::  %s  Frame header Error\n",__FUNCTION__);
					}
					Data_APV_index = Data_temp&0xf;

					Data_APV_SingleTimeSp_StrADC_temp.clear();
					Run_Ctrl_TimeSample_Index++;
					break;
				}

				// APV sample Data
				case 0x1: {
					if((Run_Ctrl_stateflg<1)||(Run_Ctrl_stateflg>2)){
						printf("[ERROR]:: %s wrong control flag, it should be 1/2, but here the control flag is %d\n",__FUNCTION__, Run_Ctrl_stateflg);
					}
					Run_Ctrl_stateflg=2;

					// save the data
					Data_APV_SingleTimeSp_StrADC_temp.insert(make_pair((Data_temp>>12)&0x7f,Data_temp& 0xFFF));
					//printf("[Test Variables]::* First <-> Second *EventsID=%d, Count=%d, trips=%d, ADC= %d \n",Data_eventsID_temp, ++Data_DCount,(Data_temp>>12)&0x7f, Data_temp& 0xFFF);
					break;
				}
				// APVSampleTrailer, can be used to check the data here, this is the end of the one time sample, all the 128 channel data should be already buffered in the buffer
				case 0x2: {
					if((Run_Ctrl_stateflg<1)||(Run_Ctrl_stateflg>2)){
						printf("[ERROR]:: %s wrong control flag, it should be 1/2, but here the control flag is %d\n",__FUNCTION__, Run_Ctrl_stateflg);
					}
					//Time_samples_test=0;
					Run_Ctrl_stateflg = 3;
					uint32_t Data_MPD_slot_index=(Data_temp >> 12)& 0x1F;    //MPD-ID
					uint32_t Data_Frame_Trailer=Data_temp&& 0xfff;
					Run_Ctrl_Current_MPDID=Data_MPD_slot_index;
					//printf("[Test Variables]:: * Second      *  MPD slot_index= %d Frame_trailer (sample index)=%d \n", Data_MPD_slot_index,Data_Frame_Trailer);
					break;
				}

				//APV  Sample End Block, save and statistic all the data here
				case 0x3: {

					if(Run_Ctrl_stateflg!=3) {
						printf("[ERROR]:: %s wrong control flag, it should be 0, but here the control flag is %d\n",__FUNCTION__, Run_Ctrl_stateflg);
					 }
					//printf("[Test Variables]::  * third    *  Steps three sample end block\n");
					Run_Ctrl_stateflg=0;    // clean the flag, ready for the next time sample save
					//data saving
					if(Data_APV_SingleTimeSp_StrADC_temp.size()==128){
						if(Data_APV_TimeSp_StrADC_temp.size()<GlNSample){

							Data_APV_TimeSp_StrADC_temp.insert(make_pair(Run_Ctrl_TimeSample_Index,Data_APV_SingleTimeSp_StrADC_temp));
							Data_APV_SingleTimeSp_StrADC_temp.clear();
							if(Data_APV_TimeSp_StrADC_temp.size()==GlNSample){   // all the simple samples for the apv
								Run_Ctrl_TimeSample_Index=-1;

								if(Data_AllAPV_temp.size()<(*GEMInfor_Buffer_Input.begin()).GEM_APV_Attached.size()){   // check the number of APV reach the maximum the number of APV
									Data_AllAPV_temp.insert(make_pair(Data_APV_index,Data_APV_TimeSp_StrADC_temp));
								    Data_APV_TimeSp_StrADC_temp.clear();
								    if(Data_AllAPV_temp.size()==(*GEMInfor_Buffer_Input.begin()).GEM_APV_Attached.size()){ // if all the apvs finish decode
								    	if(rdSingleEvent.size() < GEMInfor_Buffer_Input.size()){
								    		rdSingleEvent.insert(make_pair(Run_Ctrl_Current_MPDID,Data_AllAPV_temp));
								    		Data_AllAPV_temp.clear();
								    	}
								     }
									}
							}
						}
					 }
					else {
						printf("[ERROR]:: %s data number error",__FUNCTION__);
					    }
					// finish data saving

				}

				default:
					printf("[ERROR]:: Error in decoding raw data\n");
				 }

			}
			while((feof(file_input)==0)&&(Run_Ctrl_EndApvEndOfBlock==0));     // the end of one event

		};   //APV block Ended all

		// End of Evnts
		if((Data_temp&0xF0000000)== 0xE0000000) {

			Data_eventsID_temp = Data_temp&0xFFFFFFF;
			GEMRawFileDecoder_TreeSave(Data_eventsID_temp,rdSingleEvent);
			rdSingleEvent.clear();
			//++++++++++++++++++++++++++++++++++++++++++++++++++++++
		};
		// MAROC block
		if((Data_temp & 0xF0000000) == 0xB0000000) {
			//printf("[Run Infor]:: MAROC Block decoder\n");
			Data_eventsID_temp= Data_temp & 0xFFFFFFF;
			uint32_t Data_MaRocWordCount=0;
			fread(&Data_MaRocWordCount,sizeof(uint32_t),1,file_input);
			int Data_WordCount=Data_MaRocWordCount & 0xFFFF;
			int Data_DeviceID=(Data_MaRocWordCount>>16)&0xFFF;
			//printf("[Run Infor]:: MaRoc Block of CB %d, word count=%d\n", Data_DeviceID,Data_WordCount);
			unsigned char *Data_Maroc;
			Data_Maroc=(unsigned char *)malloc(4*Data_WordCount*sizeof(char));
			fread(&Data_Maroc,sizeof(char),4*Data_MaRocWordCount,file_input);
			continue;
		}
	}
	GEMAPV_tree.Print();
return GEMInfor_Buffer_return;
};


// file version 5 decoder, used for decoder just one event, when using this function, need to check whether the file has reach the end
map <int, map < int, map < int, map<int, map < int, int > > > > > GEMRawFileDecoder::GEMRawFileDecoder_SingleingestEventV5(FILE *file_input, TString ifile, vector<GEMInfor> GEMInfor_Buffer_Input) {

	int Run_Ctrl_stateflg=0;        // globle control variables
	map< int , map < int , map < int, map< int,int > > > >  rdSingleEvent;   //MPDID, APVID, TimesampleID, StripsID, ADC value
	uint32_t Data_eventsID_temp=0;
	while(feof(file_input)==0)   {       // while the file did not reach the end, in this loop, it will loop over all the events
		uint32_t Data_temp;
		fread(&Data_temp,sizeof(uint32_t),1,file_input);
		// vme Event header
		if((Data_temp&0xF0000000)== 0x10000000) {
			Data_eventsID_temp= Data_temp&0xFFFFFFF;
			continue;
		}
		// User infor, if this is user block
		if((Data_temp&0xF0000000)== 0xD0000000) {
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
			//printf("**********\n\n\n\nTRIGGER BLOCK\n");
			uint32_t Data_triggerpatttern;
			if(feof(file_input)==0) fread(&Data_triggerpatttern,sizeof(uint32_t),1,file_input);
			//printf("[RUN INFOR]:: eventID=%d, trigger pattern =%d \n",Data_eventsID_temp, (int)Data_triggerpatttern);
			continue;
		}
		// APV Events header, one event data
		if((Data_temp&0xF0000000)== 0xA0000000) {
			Data_eventsID_temp=Data_temp&0xFFFFFFF;
			Run_Ctrl_stateflg=0;  // reset when comes to the next events

			// in this do loop, it will loop all "6" time samples and all the APV cards, there will be 6*Number of Cards*128 strips events
			int Run_Ctrl_EndApvEndOfBlock=0;  // check whether reach the end of the data block
			uint16_t Data_APV_index=0;    // buffer the APV address
			// data saving control variables
			int Run_Ctrl_Current_APVID=-1;
			int Run_Ctrl_Current_MPDID=-1;
			int Run_Ctrl_TimeSample_Index=-1;

			map < int,int > Data_APV_SingleTimeSp_StrADC_temp;				//
			map < int, map < int,int > > Data_APV_TimeSp_StrADC_temp;		//Time SampleID. <Number of Strips, ADC value>
			map < int, map< int, map<int,int > > > Data_AllAPV_temp;        // APVID, Time Sample ID, <Number of strips, ADC value>

			do {								//in this loop, it will loop over all the "6" time samples all the APVs
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
					Run_Ctrl_stateflg=1; // change the process control buffer, ready for the second step decode process
					uint32_t Data_FrameHeader=(Data_temp>>4)& 0xFFF;   // varify the frame header
					if(((Data_FrameHeader&0xE00)!= 0xE00)||((Data_FrameHeader&0x1)==0)) {    // check the header
						printf("[ERROR]::  %s  Frame header Error\n",__FUNCTION__);
					}
					Data_APV_index = Data_temp&0xf;
					//Data_CommonBase    = (Data_temp >> 6) & 0x800;
					//printf("[Test Variables]::* First * %s APV infors=> EventsID=%d, IAPV=%d \n",__FUNCTION__,Data_eventsID_temp, Data_APV_index);
					//Data_DCount=0;
					Data_APV_SingleTimeSp_StrADC_temp.clear();
					Run_Ctrl_TimeSample_Index++;
					break;
				}

				// APV sample Data
				case 0x1: {
					if((Run_Ctrl_stateflg<1)||(Run_Ctrl_stateflg>2)){
						printf("[ERROR]:: %s wrong control flag, it should be 1/2, but here the control flag is %d\n",__FUNCTION__, Run_Ctrl_stateflg);
					}
					Run_Ctrl_stateflg=2;
					// save the data
					Data_APV_SingleTimeSp_StrADC_temp.insert(make_pair((Data_temp>>12)&0x7f,Data_temp& 0xFFF));
					//printf("[Test Variables]::* First <-> Second *EventsID=%d, Count=%d, trips=%d, ADC= %d \n",Data_eventsID_temp, ++Data_DCount,(Data_temp>>12)&0x7f, Data_temp& 0xFFF);
					break;
				}
				// APVSampleTrailer, can be used to check the data here, this is the end of the one time sample, all the 128 channel data should be already buffered in the buffer
				case 0x2: {
					if((Run_Ctrl_stateflg<1)||(Run_Ctrl_stateflg>2)){
						printf("[ERROR]:: %s wrong control flag, it should be 1/2, but here the control flag is %d\n",__FUNCTION__, Run_Ctrl_stateflg);
					}
					//Time_samples_test=0;
					Run_Ctrl_stateflg = 3;
					uint32_t Data_MPD_slot_index=(Data_temp >> 12)& 0x1F;    //MPD-ID
					uint32_t Data_Frame_Trailer=Data_temp&& 0xfff;
					Run_Ctrl_Current_MPDID=Data_MPD_slot_index;
					//printf("[Test Variables]:: * Second      *  MPD slot_index= %d Frame_trailer (sample index)=%d \n", Data_MPD_slot_index,Data_Frame_Trailer);
					break;
				}

				//APV  Sample End Block, save and statistic all the data here
				case 0x3: {

					if(Run_Ctrl_stateflg!=3) {
						printf("[ERROR]:: %s wrong control flag, it should be 0, but here the control flag is %d\n",__FUNCTION__, Run_Ctrl_stateflg);
					 }
					Run_Ctrl_stateflg=0;    // clean the flag, ready for the next time sample save
					//data saving
					if(Data_APV_SingleTimeSp_StrADC_temp.size()==128){
						if(Data_APV_TimeSp_StrADC_temp.size()<(*(*GEMInfor_Buffer_Input.begin()).GEM_APV_Attached.begin()).GEMAPVinfor_fNumSample){

							Data_APV_TimeSp_StrADC_temp.insert(make_pair(Run_Ctrl_TimeSample_Index,Data_APV_SingleTimeSp_StrADC_temp));
							Data_APV_SingleTimeSp_StrADC_temp.clear();
							if(Data_APV_TimeSp_StrADC_temp.size()==(*(*GEMInfor_Buffer_Input.begin()).GEM_APV_Attached.begin()).GEMAPVinfor_fNumSample){   // all the simple samples for the apv
								Run_Ctrl_TimeSample_Index=-1;

								if(Data_AllAPV_temp.size()<(*GEMInfor_Buffer_Input.begin()).GEM_APV_Attached.size()){   // check the number of APV reach the maximum the number of APV
									Data_AllAPV_temp.insert(make_pair(Data_APV_index,Data_APV_TimeSp_StrADC_temp));
								    Data_APV_TimeSp_StrADC_temp.clear();
								    if(Data_AllAPV_temp.size()==(*GEMInfor_Buffer_Input.begin()).GEM_APV_Attached.size()){ // if all the apvs finish decode
								    	if(rdSingleEvent.size() < GEMInfor_Buffer_Input.size()){
								    		rdSingleEvent.insert(make_pair(Run_Ctrl_Current_MPDID,Data_AllAPV_temp));
								    		Data_AllAPV_temp.clear();
								    	}
								     }
									}
							}
						}
					 }
					else {
						printf("[ERROR]:: %s data number error",__FUNCTION__);
					    }
					// finish data saving

					break;
				}

				default:
					printf("[ERROR]:: Error in decoding raw data\n");
				 }

			}
			while((feof(file_input)==0)&&(Run_Ctrl_EndApvEndOfBlock==0));     // the end of one event

		};   //APV block Ended all

		// End of Evnts
		if((Data_temp&0xF0000000)== 0xE0000000) {

			Data_eventsID_temp = Data_temp&0xFFFFFFF;
			//this is the right place to save the single event data
			//eventID       MPDID,    APVID, TimesampleID, StripsID, ADC value
			map <int, map < int, map < int, map<int, map < int, int > > > > > Single_Evnts_Return;
			Single_Evnts_Return.clear();
			//printf("[%s-line%d] eID=%d\n", __FUNCTION__,__LINE__,Data_eventsID_temp);
			Single_Evnts_Return.insert(make_pair(Data_eventsID_temp,rdSingleEvent));
			rdSingleEvent.clear();
			return Single_Evnts_Return;
		}
		// MAROC block
		if((Data_temp & 0xF0000000) == 0xB0000000) {
			Data_eventsID_temp= Data_temp & 0xFFFFFFF;
			uint32_t Data_MaRocWordCount=0;
			fread(&Data_MaRocWordCount,sizeof(uint32_t),1,file_input);
			int Data_WordCount=Data_MaRocWordCount & 0xFFFF;
			int Data_DeviceID=(Data_MaRocWordCount>>16)&0xFFF;
			unsigned char *Data_Maroc;
			Data_Maroc=(unsigned char *)malloc(4*Data_WordCount*sizeof(char));
			fread(&Data_Maroc,sizeof(char),4*Data_MaRocWordCount,file_input);
			continue;
		}
	}
	// if the file is file is not completw it will return black array
	map <int, map < int, map < int, map<int, map < int, int > > > > > Single_Evnts_Return;
	return Single_Evnts_Return;
	//printf("[%s-line%d] end of the file, maybe this is cash happen\n", __FUNCTION__,__LINE__);
};

////                                                                           MPDID, APVID, TimesampleID, StripsID, ADC value
void GEMRawFileDecoder::GEMRawFileDecoder_TreeSave(int EventID_index_temp, map< int , map < int , map < int, map< int,int > > > >  rdSingleEvent_Input) {
	GEMR_ApvStrData.EventID=EventID_index_temp;
	map< int , map < int , map < int, map< int,int > > > >::iterator iter_mpd=rdSingleEvent_Input.begin();
	while(iter_mpd!=rdSingleEvent_Input.end()){    // loop on the MPD
		GEMR_ApvStrData.MPDID=iter_mpd->first;

		map < int , map < int, map< int,int > > > ::iterator itter_apv=iter_mpd->second.begin();   // loop on all the APV that attched on this mpd
		while(itter_apv!=iter_mpd->second.end()){
			GEMR_ApvStrData.APVID=itter_apv->first;
			map < int, map< int,int > >::iterator ittter_Tsample=itter_apv->second.begin();
			while(ittter_Tsample!=itter_apv->second.end()){

				map<int,int>::iterator itttt_Nstrip=ittter_Tsample->second.begin();
				while(itttt_Nstrip!=ittter_Tsample->second.end()){
					GEMR_ApvStrData.StripADC[ittter_Tsample->first][itttt_Nstrip->first]=itttt_Nstrip->second;
					itttt_Nstrip++;
				}
				ittter_Tsample++;
			}
			GEMAPV_tree.Fill();
			itter_apv++;
		}

		iter_mpd++;
	}
}

// test functions
void GEMRawFileDecoder::GEMRawFileDecoder_TestFunction(){
	vector<GEMInfor> MPD_infor_test;
	GEMRawFileDecoder_HistoDecoder("/home/newdriver/Research/Eclipse_workspace/GEM_Analysis/test.root","test_out.root",-1);
	//GEMRawFileDecoder_PedestalDecoder("test.root",-1);
	//GEMRawFileDecoder_RawDisplay();
	//GEMRawFileDecoder_Run(MPD_infor_test);
};
