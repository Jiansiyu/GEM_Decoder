/*
 * GEMEventDecoder.cpp
 *
 *  Created on: Nov 13, 2016
 *      Author: newdriver
 */
#include <stdio.h>

// root
#include "TH1F.h"
#include "TThread.h"
#include "TCanvas.h"
#include "GEMEventDecoder.h"
#include "GEMConfig.h"
using namespace std;

GEMEventDecoder::GEMEventDecoder() {
	// TODO Auto-generated constructor stub

}

GEMEventDecoder::~GEMEventDecoder() {
	// TODO Auto-generated destructor stub
}

GEMEventDecoder::GEMEventDecoder(std::map < int, std::map <int, std::map <int, std::map < int, std::map< int, int > > > > > SingleEvts_Input) {
	SingleEvts=SingleEvts_Input;	//
	//printf("Evts=%d\n",SingleEvts_Input.begin()->first);

};


GEMEventDecoder::GEMEventDecoder(std::map <int, std::map <int, std::map < int, std::map< int, int > > > > SingleEvts_Input) {

};

//      Event ID,      MPDID,         APVID            Tsample    CommonMode
std::map<int, std::map <int, std::map <int, std::map < int,  int > > > > GEMEventDecoder::eDGetCommonMode() {
	// eventID, MPD     APV     Sample Comod
	std::map < int, std::map < int, std::map < int, std::map< int,int > > > > CommonMode_Return;

	std::map< int, std::map<int, std::map<int, std::map<int,int> > > >::iterator iter_mpd=SingleEvts.begin()->second.begin();
	while(iter_mpd != SingleEvts.begin()->second.end()) {
		map<int,map<int,map<int,int> > >::iterator itter_APV=iter_mpd->second.begin();
		while(itter_APV!=iter_mpd->second.end()) {

			map<int,map<int,int> >::iterator ittter_Tsample=itter_APV->second.begin();
			while(ittter_Tsample!=itter_APV->second.end()) {
				map<int,int>::iterator itttter_Nstrps=ittter_Tsample->second.begin();
				TH1F *CommonMode_Histo_temp= new TH1F(Form("Ev%d_MPD%d_APV%d_Ts%d",SingleEvts.begin()->first,iter_mpd->first,itter_APV->first,ittter_Tsample->first),
														Form("Ev%d_MPD%d_APV%d_Ts%d",SingleEvts.begin()->first,iter_mpd->first,itter_APV->first,ittter_Tsample->first),
														4095,
														0,
														4095
														);
				while(itttter_Nstrps!=ittter_Tsample->second.end()) {
					CommonMode_Histo_temp->Fill(itttter_Nstrps->second); // fill all the ADCs in the histo
					itttter_Nstrps++;
				}
				CommonMode_Return[SingleEvts.begin()->first][iter_mpd->first][itter_APV->first][ittter_Tsample->first]=CommonMode_Histo_temp->GetMean();
				delete CommonMode_Histo_temp;
				ittter_Tsample++;
			}

			itter_APV++;
		}
		iter_mpd++;
	}
	return CommonMode_Return;
};

// remove peak first befor get the Common mode
//      Event ID,      MPDID,         APVID            Tsample    CommonMode
std::map<int, std::map <int, std::map <int, std::map < int,  int > > > > GEMEventDecoder::eDGetCommonModeRmPk() {
	// eventID, MPD     APV     Sample Comod
	std::map < int, std::map < int, std::map < int, std::map< int,int > > > > CommonMode_Return;

	std::map< int, std::map<int, std::map<int, std::map<int,int> > > >::iterator iter_mpd=SingleEvts.begin()->second.begin();
	while(iter_mpd != SingleEvts.begin()->second.end()) {
		map<int,map<int,map<int,int> > >::iterator itter_APV=iter_mpd->second.begin();
		while(itter_APV!=iter_mpd->second.end()) {

			map<int,map<int,int> >::iterator ittter_Tsample=itter_APV->second.begin();
			while(ittter_Tsample!=itter_APV->second.end()) {

				eDRemovePeak((++ittter_Tsample)->second);
				/*map<int,int>::iterator itttter_Nstrps=ittter_Tsample->second.begin();
				TH1F *CommonMode_Histo_temp= new TH1F(Form("Ev%d_MPD%d_APV%d_Ts%d",SingleEvts.begin()->first,iter_mpd->first,itter_APV->first,ittter_Tsample->first),
														Form("Ev%d_MPD%d_APV%d_Ts%d",SingleEvts.begin()->first,iter_mpd->first,itter_APV->first,ittter_Tsample->first),
														4095,
														0,
														4095
														);
				eDRemovePeak(ittter_Tsample->second);
				while(itttter_Nstrps!=ittter_Tsample->second.end()) {
					// add cut here to cut the Event Candidate from the back groud
					CommonMode_Histo_temp->Fill(itttter_Nstrps->second); // fill all the ADCs in the histo
					itttter_Nstrps++;
				}
				CommonMode_Return[SingleEvts.begin()->first][iter_mpd->first][itter_APV->first][ittter_Tsample->first]=CommonMode_Histo_temp->GetMean();
				delete CommonMode_Histo_temp;*/

				ittter_Tsample++;
			}

			itter_APV++;
		}
		iter_mpd++;
	}
	return CommonMode_Return;
};

//      Event ID,      MPDID,         APVID             Nstrips   Sigma
std::map<int, std::map <int, std::map <int, std::map < int,  int > > > > GEMEventDecoder::eDGetSigma(std::map<int, std::map <int, std::map <int, std::map < int,  int > > > > CommonMode_Input) {

};


std::map< int, int > GEMEventDecoder::eDRemovePeak(map<int,int> SingleTSample_Input){
	std::map < int, int > SingleTSample_temp=SingleTSample_Input;
	map<int,int>::iterator iter_Nstrps=SingleTSample_temp.begin();
	map<int,int> SingleTSample_CorrectOder;
	while(iter_Nstrps!=SingleTSample_temp.end()) {
		SingleTSample_CorrectOder[ChNb[iter_Nstrps->first]]=iter_Nstrps->second;   // change to the right order
		iter_Nstrps++;
	};
	TCanvas *Canvas_Raw=new TCanvas(Form("Raw_Display_Canvas_Evt"),Form("S"),1000,1000);
	TH1F *Test=new TH1F("","",127,0,127);
	iter_Nstrps=SingleTSample_CorrectOder.begin();
	while (iter_Nstrps != SingleTSample_CorrectOder.end()) {
		printf("Chns=%d, ADC=%d\n",iter_Nstrps->first,iter_Nstrps->second);
		Test->Fill(iter_Nstrps->first,iter_Nstrps->second);
		iter_Nstrps++;
	}
	Canvas_Raw->cd();
	Test->Draw();
	//Canvas_Raw->Update();
	Canvas_Raw->Draw();
	Canvas_Raw->Modified();
	Canvas_Raw->Update();
	getchar();
	delete Canvas_Raw;
	delete Test;
	return SingleTSample_CorrectOder;
}

