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

// root function used find the peak when calculate the old version pedetal
#include "TMath.h"
#include "TSpectrum.h"
#include "TVirtualFitter.h"
#include "TCanvas.h"
#include "TMath.h"
#include "TH1.h"
#include "TF1.h"
#include "TRandom.h"
#include "TSpectrum.h"
#include "TVirtualFitter.h"


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

//    channels
std::map< int, int > GEMEventDecoder::eDRemovePeak(map<int,int> SingleTSample_Input){

	TCanvas *Canvas_Raw=new TCanvas(Form("Raw_Display_Canvas_Evt"),Form("S"),1000,1000);

	std::map < int, int > SingleTSample_temp=SingleTSample_Input;
	map<int,int>::iterator iter_Nstrps=SingleTSample_temp.begin();
	map<int,int> SingleTSample_CorrectOder;
	TH1F *SingleTSample_Histo=new TH1F("singleEvnt","singleEvnt",128,0,128);

	while(iter_Nstrps!=SingleTSample_temp.end()) {
		SingleTSample_CorrectOder[ChNb[iter_Nstrps->first]]=iter_Nstrps->second;   // change to the right order
		SingleTSample_Histo->Fill(ChNb[iter_Nstrps->first],iter_Nstrps->second);
		iter_Nstrps++;
	};

	// searching for the peaks
	//TSpectrum *s = new TSpectrum(2*30);
	TH1F *PeakSearch_Hito=(TH1F*) SingleTSample_Histo->Clone("h2");
	TSpectrum *SingleTSample_peaksch = new TSpectrum(MAX_PEAKS_PEVNT);
	int NfoundPeak=SingleTSample_peaksch->Search(PeakSearch_Hito,2,"",0.3);
	float *PeakPos= SingleTSample_peaksch->GetPositionX();

	//SingleTSample_Histo->GetMaximumStored();
	//float *PeakHight=SingleTSample_peaksch->Get();
	for(int i =0; i < NfoundPeak; i++){
		printf("position= %d, mean=%f, sigma=%f\n",SingleTSample_Histo->GetXaxis()->FindBin(PeakPos[i]),SingleTSample_Histo->GetMean(),SingleTSample_Histo->GetRMS());//PeakHight[i]);
		iter_Nstrps=SingleTSample_Input.begin();
	}

	Printf("find %d peaks\n",NfoundPeak);
	Canvas_Raw->cd();
	SingleTSample_Histo->Draw();
	Canvas_Raw->Draw();
	Canvas_Raw->Modified();
	Canvas_Raw->Update();
	getchar();
	//sleep(1);
	delete Canvas_Raw;
	delete SingleTSample_Histo;
	return SingleTSample_CorrectOder;
}

