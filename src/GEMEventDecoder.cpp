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
//#include "TCanvas.h"

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

std::map <int, std::map < int, std::map < int, std::map<int, std::map < int, int > > > > > GEMEventDecoder::eDCommonModeSubtr() {
	std::map <int, std::map < int, std::map < int, std::map<int, std::map < int, int > > > > > sEvent_Return;

	//cout<<SingleEvts.begin()->first<<endl;
	std::map< int, std::map<int, std::map<int, std::map<int,int> > > >::iterator iter_mpd=SingleEvts.begin()->second.begin();
	std::map< int, std::map<int, std::map<int, std::map<int,int> > > > mpd_buffer;
	while(iter_mpd!= SingleEvts.begin()->second.end()){
		std::map<int, std::map<int, std::map<int,int> > >::iterator itter_apvs=iter_mpd->second.begin();
		std::map<int, std::map<int, std::map<int,int> > > APVs_buffer;
		while(itter_apvs!= iter_mpd->second.end()){
			std::map<int, std::map<int,int> > ::iterator ittter_tsamples=itter_apvs->second.begin();
			std::map<int, std::map<int,int> > tSamples_buffer; // used for return data
			while(ittter_tsamples!=itter_apvs->second.end()){  // loop on the mpds

				TH1F *Single_Sample_temp=new TH1F("Single_sample","Single_sample",128,0,128);
				std::map<int,int>::iterator itttter_nstrips=ittter_tsamples->second.begin();
				while(itttter_nstrips!=ittter_tsamples->second.end()){
					Single_Sample_temp->Fill(ChNb[itttter_nstrips->first],itttter_nstrips->second);
					itttter_nstrips++;
				}
				TSpectrum *SingleTSample_peaksch = new TSpectrum(MAX_PEAKS_PEVNT);
				TH1 *background_level0=SingleTSample_peaksch->Background(Single_Sample_temp,20,"");
				TH1F *Commonmode_histo_temp=new TH1F("commonmode","commonmode",4000,-2000,2000);

				for(Int_t strips_counter_temp=background_level0->GetXaxis()->GetFirst();strips_counter_temp<=background_level0->GetXaxis()->GetLast();strips_counter_temp++){
					if(background_level0->GetBinContent(strips_counter_temp)<EVENTS_THR){
						Commonmode_histo_temp->Fill(background_level0->GetBinContent(strips_counter_temp));
						}
				}
				float CommonMode_temp=Commonmode_histo_temp->GetMean();
				//cout<<(int)CommonMode_temp<<endl;
				itttter_nstrips=ittter_tsamples->second.begin();
				std::map<int,int> sgSample_buffer;
				while(itttter_nstrips!=ittter_tsamples->second.end()) {
					sgSample_buffer.insert(make_pair(itttter_nstrips->first,itttter_nstrips->second-(int)CommonMode_temp));
					//printf("nstrips=%d,adc=%d\n",itttter_nstrips->first,itttter_nstrips->second-(int)CommonMode_temp);
					itttter_nstrips++;
				}
				tSamples_buffer.insert(make_pair(ittter_tsamples->first,sgSample_buffer));
				//printf("first bin maybe =%d, last bin =%d\n",background_level0->GetYaxis()->GetFirst(),background_level0->GetXaxis()->GetLast());
				delete Commonmode_histo_temp;
				delete background_level0;
				delete SingleTSample_peaksch;
				delete Single_Sample_temp;
				ittter_tsamples++;
			}
			APVs_buffer.insert(make_pair(itter_apvs->first,tSamples_buffer));

			itter_apvs++;
		}
		mpd_buffer.insert(make_pair(iter_mpd->first,APVs_buffer));
		iter_mpd++;
	}
	//sEvent_Return.insert(make_pair(SingleEvts.begin()->first,mpd_buffer));


/*	// check function
	iter_mpd=sEvent_Return.begin()->second.begin();
	while(iter_mpd!=sEvent_Return.begin()->second.end()){
		std::map<int, std::map<int, std::map<int,int> > >::iterator itter_apvs=iter_mpd->second.begin();
		while(itter_apvs!=iter_mpd->second.end()){
			std::map<int, std::map<int,int> > ::iterator ittter_tsample=itter_apvs->second.begin();
			while(ittter_tsample!=itter_apvs->second.end()){
				std::map<int,int>::iterator itttter_nstrips=ittter_tsample->second.begin();
				while(itttter_nstrips!=ittter_tsample->second.end()){
					cout<<"EvntsID"<<SingleEvts.begin()->first<<"MPD="<<iter_mpd->first<<" APV="<<itter_apvs->first<<" Tsample="<<ittter_tsample->first<<" nstrips="<<itttter_nstrips->first<< "  ADC"<<itttter_nstrips->second<<endl;

					itttter_nstrips++;
				}
				ittter_tsample++;
			}
			itter_apvs++;
		}
		iter_mpd++;
	}
	// end of check function
	*/
	return sEvent_Return;
}

// remove peak first before get the Common mode
//      Event ID,      MPDID,         APVID            Tsample    CommonMode
std::map<int, std::map <int, std::map <int, std::map < int,  int > > > > GEMEventDecoder::eDGetCommonModeRmPk() {
	// eventID, MPD     APV     Sample Comod
	std::map < int, std::map < int, std::map < int, std::map< int,int > > > > CommonMode_Return;

	std::map< int, std::map<int, std::map<int, std::map<int,int> > > >::iterator iter_mpd=SingleEvts.begin()->second.begin();
	std::map< int, std::map<int, std::map<int, std::map<int,int> > > > mpd_buffer;

	while(iter_mpd!= SingleEvts.begin()->second.end()){

		std::map<int, std::map<int, std::map<int,int> > >::iterator itter_apvs=iter_mpd->second.begin();
		std::map<int, std::map<int, std::map<int,int> > > APVs_buffer;
		while(itter_apvs!= iter_mpd->second.end()){


			std::map<int, std::map<int,int> > ::iterator ittter_tsamples=itter_apvs->second.begin();
			std::map<int, std::map<int,int> > tSamples_buffer; // used for return data
			while(ittter_tsamples!=itter_apvs->second.end()){  // loop on the mpds

				TH1F *Single_Sample_temp=new TH1F("Single_sample","Single_sample",128,0,128);
				std::map<int,int>::iterator itttter_nstrips=ittter_tsamples->second.begin();
				while(itttter_nstrips!=ittter_tsamples->second.end()){
					Single_Sample_temp->Fill(ChNb[itttter_nstrips->first],itttter_nstrips->second);
					itttter_nstrips++;
				}
				TSpectrum *SingleTSample_peaksch = new TSpectrum(MAX_PEAKS_PEVNT);
				TH1 *background_level0=SingleTSample_peaksch->Background(Single_Sample_temp,20,"");
				TH1F *Commonmode_histo_temp=new TH1F("commonnmode","commonmode",8000,-4000,4000);
				for(Int_t strips_counter_temp=background_level0->GetXaxis()->GetFirst();strips_counter_temp<=background_level0->GetXaxis()->GetLast();strips_counter_temp++){
					if(background_level0->GetBinContent(strips_counter_temp)<EVENTS_THR){
						Commonmode_histo_temp->Fill(background_level0->GetBinContent(strips_counter_temp));
						}
				}
				float CommonMode_temp=Commonmode_histo_temp->GetMean();
				cout<<(int)CommonMode_temp<<endl;
				itttter_nstrips=ittter_tsamples->second.begin();
				std::map<int,int> sgSample_buffer;
				while(itttter_nstrips!=ittter_tsamples->second.end()) {
					sgSample_buffer.insert(make_pair(itttter_nstrips->first,itttter_nstrips->second-(int)CommonMode_temp));
					//printf("nstrips=%d,adc=%d\n",itttter_nstrips->first,itttter_nstrips->second-(int)CommonMode_temp);
					itttter_nstrips++;
				}

				tSamples_buffer.insert(make_pair(ittter_tsamples->first,sgSample_buffer));
				//printf("first bin maybe =%d, last bin =%d\n",background_level0->GetYaxis()->GetFirst(),background_level0->GetXaxis()->GetLast());
				delete Commonmode_histo_temp;
				delete SingleTSample_peaksch;
				delete Single_Sample_temp;
				ittter_tsamples++;
			}
			APVs_buffer.insert(make_pair(itter_apvs->first,tSamples_buffer));

			itter_apvs++;
		}
		mpd_buffer.insert(make_pair(iter_mpd->first,APVs_buffer));
		iter_mpd++;
	}

	//CommonMode_Return.insert(SingleEvts.begin()->first,mpd_buffer);
	//	while(iter_mpd != SingleEvts.begin()->second.end()) {
//
//		map<int,map<int,map<int,int> > >::iterator itter_APV=iter_mpd->second.begin();
//
//		while(itter_APV!=iter_mpd->second.end()) {
//
//			map<int,map<int,int> >::iterator ittter_Tsample=itter_APV->second.begin();
//			while(ittter_Tsample!=itter_APV->second.end()) {
//				map<int,int>::iterator itttter_sgsimple=ittter_Tsample->second.begin();
//
//				TH1F *Single_Sample_temp=new TH1F("Single_sample","Single_sample",128,0,128);
//
//				while(itttter_sgsimple!=ittter_Tsample->second.end()){
//					//if(itttter_sgsimple->second<EVENTS_THR)
//					{    // only buffer the background candidate,(because the efficiency to estimate the eadge is not good)
//						Single_Sample_temp->Fill(ChNb[itttter_sgsimple->first],itttter_sgsimple->second);
//					};
//					itttter_sgsimple++;
//				}
//
//				TSpectrum *SingleTSample_peaksch = new TSpectrum(MAX_PEAKS_PEVNT);
//				TH1 *Back_ground_temp=SingleTSample_peaksch->Background(Single_Sample_temp,20,"");
//				std::cout<<Back_ground_temp->GetXaxis()->GetLast()<<endl;
//				TH1F *Common_mode_histo_temp=new TH1F("common_mode","common_mode",128,0,128);
//				for(int i=Back_ground_temp->GetXaxis()->GetFirst(); i<=Back_ground_temp->GetXaxis()->GetLast();i++){
//					if(Back_ground_temp->GetBinContent(i)<EVENTS_THR) {
//						Common_mode_histo_temp->Fill(Back_ground_temp->GetBinContent(i));
//					}
//				delete SingleTSample_peaksch;
//				};
//				float Common_Mode_mean_temp=Common_mode_histo_temp->GetMean();
//				delete Back_ground_temp;
//				delete Single_Sample_temp;
//
//				//eDRemovePeak((++ittter_Tsample)->second);
//
//				ittter_Tsample++;
//			}
//
//			itter_APV++;
//		}
//		iter_mpd++;
//	}
	return CommonMode_Return;
};

//      Event ID,      MPDID,         APVID             Nstrips   Sigma
std::map<int, std::map <int, std::map <int, std::map < int,  int > > > > GEMEventDecoder::eDGetSigma() {

	//TCanvas *Canvas_Raw=new TCanvas("canvas","canvas",1000,1000);
	//Canvas_Raw->Divide(2,2);
	//         eventID,        MPD             APV         strips 	sigma
	std::map < int, std::map < int, std::map < int, std::map< int,int > > > > CommonMode_Return;
	std::map< int, std::map<int, std::map<int, std::map<int,int> > > >::iterator iter_mpd=SingleEvts.begin()->second.begin();
	std::map<int,std::map<int,std::map<int,std::map<int,int> > > >MPDs_CommonSbtr_buffer_temp;

	while(iter_mpd!=SingleEvts.begin()->second.end()) {
		//printf("%s, TEST POINT 0\n",__FUNCTION__);
		std::map<int, std::map<int, std::map<int,int> > >::iterator itter_apv=iter_mpd->second.begin();
		//   APV   Tsample  Nstrp ADC
		map<int,map<int,map<int,int> > > APVs_CommonSbtr_buffer_temp;
		//printf("%s, TEST POINT 01\n",__FUNCTION__);
		while(itter_apv!=iter_mpd->second.end()) {	// loop on the apvs
			// six time sample StripID, ADC value
			std::map<int,map<int,int> >SingleAPV_RemovePeak_Buffer_temp;
			std::map<int, std::map<int,int> >::iterator ittter_TSample=itter_apv->second.begin();

			while(ittter_TSample!=itter_apv->second.end()) {  // loop on all the six time sample and remove the peak in each time sample
				SingleAPV_RemovePeak_Buffer_temp.insert(make_pair(ittter_TSample->first,eDRemovePeak(ittter_TSample->second)));
				ittter_TSample++;
			}
			//common mode subtraction
			std::map<int,map<int,int> > SingleAPV_RemovePeak_CommonMDSubtr_Buffer_temp;
			std::map<int,map<int,int> >::iterator iter_tsample_temp=SingleAPV_RemovePeak_Buffer_temp.begin();

			while(iter_tsample_temp!=SingleAPV_RemovePeak_Buffer_temp.end()){

				std::map<int,int>::iterator itter_nstrips_temp=iter_tsample_temp->second.begin();
				float Sum_temp=0;
				int Counter_temp=0;
				float Common_Mode_temp=0;
				while(itter_nstrips_temp!=iter_tsample_temp->second.end()){
					Sum_temp=Sum_temp+(itter_nstrips_temp->second);
					Counter_temp++;
					itter_nstrips_temp++;
				}
				Common_Mode_temp=Sum_temp/((float)Counter_temp);    // get the common mode
				// finish calculation common mode
				// common mode subtraction
				itter_nstrips_temp=iter_tsample_temp->second.begin();
				std::map<int,int> Single_sample_temp;
				while(itter_nstrips_temp!=iter_tsample_temp->second.end()) {
					Single_sample_temp.insert(make_pair(itter_nstrips_temp->first,itter_nstrips_temp->second-(int)Common_Mode_temp));
					itter_nstrips_temp++;
				}
				SingleAPV_RemovePeak_CommonMDSubtr_Buffer_temp.insert(make_pair(iter_tsample_temp->first,Single_sample_temp));
				iter_tsample_temp++;
			}
			APVs_CommonSbtr_buffer_temp.insert(make_pair(itter_apv->first,SingleAPV_RemovePeak_CommonMDSubtr_Buffer_temp));
			// finish common mode subtraction
			itter_apv++;
		}
		MPDs_CommonSbtr_buffer_temp.insert(make_pair(iter_mpd->first,APVs_CommonSbtr_buffer_temp));
		iter_mpd++;
	}
	//printf("%s, TEST POINT 2\n",__FUNCTION__);
	// finish calculater commonmode subtraction
	// start calculate sigma
	//      MPD           APV         Timesample  NSTR  ADC
	iter_mpd=MPDs_CommonSbtr_buffer_temp.begin();
	map<int,map<int,map<int,int> > > MPDs_sigma_buffer_temp;
	while(iter_mpd!=MPDs_CommonSbtr_buffer_temp.end()) {
		std::map<int,std::map<int,map<int,int> > >::iterator itter_APVs=iter_mpd->second.begin();
		map<int,map<int,int> > APVs_sigma_buffer_temp;
		while(itter_APVs!=iter_mpd->second.end()) {

			map<int,int> Ch_sigma_buffer_temp;
			for(int i =0 ; i <128; i ++) {

				TH1F *Sigma_histo_temp=new TH1F("Sigma_histo_temp","Sigma_histo_temp",8192,-4096,4096);
				map<int,map<int,int>>::iterator ittter_tsample=itter_APVs->second.begin();
				//cout<<"asasasa****  "<<itter_APVs->second.size()<<endl;
				while(ittter_tsample!=itter_APVs->second.end()) {
					if(ittter_tsample->second[i]!=NULL){
						Sigma_histo_temp->Fill(ittter_tsample->second[i]);
					   }else
						 {
							break;
						 }
					ittter_tsample++;
				    }
				// if all the (maybe 6) timesamples have effective data
				if(Sigma_histo_temp->GetEntries()==itter_APVs->second.size()){
					Ch_sigma_buffer_temp.insert(make_pair(i,Sigma_histo_temp->GetRMS()));
				}
				delete Sigma_histo_temp;
			}
			APVs_sigma_buffer_temp.insert(make_pair(itter_APVs->first,Ch_sigma_buffer_temp));
			itter_APVs++;
		}
		MPDs_sigma_buffer_temp.insert(make_pair(iter_mpd->first,APVs_sigma_buffer_temp));
		iter_mpd++;
	}
	CommonMode_Return.insert(make_pair(SingleEvts.begin()->first,MPDs_sigma_buffer_temp));
	return CommonMode_Return;

};

//      Event ID,      MPDID,         APVID             Nstrips   Sigma
std::map<int, std::map <int, std::map <int, std::map < int,  int > > > > GEMEventDecoder::eDGetSigma(std::map<int, std::map <int, std::map <int, std::map < int,  int > > > > CommonMode_Input) {

};

//    channels  ADC
std::map< int, int > GEMEventDecoder::eDRemovePeak(map<int,int> SingleTSample_Input){

	std::map<int,int> Single_Sample_return;
	std::map < int, int > SingleTSample_temp=SingleTSample_Input;
	map<int,int>::iterator iter_Nstrps=SingleTSample_temp.begin();
	TH1F *SingleTSample_Histo=new TH1F("singleEvnt","singleEvnt",128,0,128);
	Byte_t Event_candidate_flg=0;
	while(iter_Nstrps!=SingleTSample_temp.end()) {
		if(iter_Nstrps->second > EVENTS_THR) Event_candidate_flg=1;                // increase the the event flag which indicate there maybe a events for this events
		SingleTSample_Histo->Fill(ChNb[iter_Nstrps->first],iter_Nstrps->second);
		//printf("Strips=%d   ADC=%d\n", ChNb[iter_Nstrps->first],iter_Nstrps->second);
		iter_Nstrps++;
	};

	TH1F *SingleTSamplerm_Histo;
	if(Event_candidate_flg==1){

		TH1F *PeakSearch_Hito=(TH1F*) SingleTSample_Histo->Clone("h2");
		TSpectrum *SingleTSample_peaksch = new TSpectrum(MAX_PEAKS_PEVNT);
		int NfoundPeak=SingleTSample_peaksch->Search(PeakSearch_Hito,2,"",0.05);    // create defaut canvas here????
		float *PeakPos= SingleTSample_peaksch->GetPositionX();
		vector<int> Remove_table_temp;
		for(int i =0; i < NfoundPeak; i++){
				for(int j=-9; j<9; j++) {
					if(
							(SingleTSample_Histo->GetXaxis()->FindBin(PeakPos[i])+j)>=0 &&
							(SingleTSample_Histo->GetXaxis()->FindBin(PeakPos[i])+j)<128&&
							(SingleTSample_Histo->GetBinContent(SingleTSample_Histo->GetXaxis()->FindBin(PeakPos[i]))>800)&&
							(NfoundPeak<5)&&
							(SingleTSample_Histo->GetBinContent(SingleTSample_Histo->GetMaximumBin())>900)
							)
					{

						Remove_table_temp.push_back(SingleTSample_Histo->GetXaxis()->FindBin(PeakPos[i])+j);			}
				}

			}
		vector<int>::iterator Remove_table_iter=Remove_table_temp.begin();
		for(;Remove_table_iter<Remove_table_temp.end();Remove_table_iter++){
			SingleTSample_temp.erase(CHNb_LogicAddr[*Remove_table_iter]);
			};
		// finish erase the peak candidate

		iter_Nstrps=SingleTSample_temp.begin();

		SingleTSamplerm_Histo=new TH1F("singleEvnt1","singleEvnt1",128,0,128);
		while (iter_Nstrps!=SingleTSample_temp.end()){
			SingleTSamplerm_Histo->Fill(ChNb[iter_Nstrps->first],iter_Nstrps->second);
			iter_Nstrps++;
		   };
		if(SingleTSamplerm_Histo->GetBinContent(SingleTSamplerm_Histo->GetMaximumBin())<900){
			Single_Sample_return=SingleTSample_temp;
		}
		delete PeakSearch_Hito;
		delete SingleTSample_peaksch;
	}
	else{
		Single_Sample_return=SingleTSample_temp;
	}

	delete SingleTSamplerm_Histo;
	delete SingleTSample_Histo;
	return Single_Sample_return;
}

