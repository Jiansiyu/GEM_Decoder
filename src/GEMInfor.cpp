/*
 * GEMInfor.cpp
 *
 *  Created on: Oct 24, 2016
 *      Author: Siyu Jian
 */

#include "GEMInfor.h"

GEMInfor::GEMInfor() {
	// TODO Auto-generated constructor stub
	GEMInfor_Initialize();
}

GEMInfor::~GEMInfor() {
	// TODO Auto-generated destructor stub
}

int GEMInfor::GEMInfor_Initialize() {

	GEMInfor_fSlot  		  =-1;
	GEMInfor_fNumTrigger	  =-1;
	GEMInfor_fCalibLatency    =-1;
	GEMInfor_fClockPhase[0]   =-1;
	GEMInfor_fClockPhase[1]   =-1;
	GEMInfor_fCommonBaseline  =-1;
	GEMInfor_fCommonMode	  =-1;
	GEMInfor_fEventBuilder    =-1;
	GEMInfor_fLevelZero		  =-1;
	GEMInfor_fLevelOne        =-1;
	GEMInfor_fApvCount        =-1;

	GEMInfor_fAPVIndex.clear();
	GEMInfor_fAdcGain.clear();
	if((GEMInfor_fAPVIndex.size()==0)&&(GEMInfor_fAdcGain.size()==0))  {
		return 1;
	}
	else
	{
		return -1;
	};
};

// pass the input variables to local variables
int GEMInfor::GEMInfor_clone(GEMInfor *GEMInfor_input) {

	this->GEMInfor_fSlot          = GEMInfor_input->GEMInfor_fSlot;
	this->GEMInfor_fNumTrigger    = GEMInfor_input->GEMInfor_fNumTrigger;
	this->GEMInfor_fCalibLatency  = GEMInfor_input->GEMInfor_fCalibLatency;
	this->GEMInfor_fClockPhase[0] = GEMInfor_input->GEMInfor_fClockPhase[0];
	this->GEMInfor_fClockPhase[1] = GEMInfor_input->GEMInfor_fClockPhase[1];
	this->GEMInfor_fCommonBaseline= GEMInfor_input->GEMInfor_fCommonBaseline;
	this->GEMInfor_fCommonMode    = GEMInfor_input->GEMInfor_fCommonMode;
	this->GEMInfor_fEventBuilder  = GEMInfor_input->GEMInfor_fEventBuilder;
	this->GEMInfor_fLevelZero     = GEMInfor_input->GEMInfor_fLevelZero;
	this->GEMInfor_fLevelOne      = GEMInfor_input->GEMInfor_fLevelOne;
	this->GEMInfor_fApvCount      = GEMInfor_input->GEMInfor_fApvCount;

	this->GEMInfor_fAPVIndex      = GEMInfor_input->GEMInfor_fAPVIndex;
	this->GEMInfor_fAdcGain       = GEMInfor_input->GEMInfor_fAdcGain;
	this->GEM_APV_Attached        = GEMInfor_input->GEM_APV_Attached;

	return 1;
}


// pass the local variables to the input class
int GEMInfor::GEMInfor_clone(GEMInfor &GEMInfor_input) {

	GEMInfor_input.GEMInfor_fSlot            = this->GEMInfor_fSlot;
	GEMInfor_input.GEMInfor_fNumTrigger      = this->GEMInfor_fNumTrigger;
	GEMInfor_input.GEMInfor_fCalibLatency    = this->GEMInfor_fCalibLatency;
	GEMInfor_input.GEMInfor_fClockPhase[0]   = this->GEMInfor_fClockPhase[0];
	GEMInfor_input.GEMInfor_fClockPhase[1]   = this->GEMInfor_fClockPhase[1];
	GEMInfor_input.GEMInfor_fCommonBaseline  = this->GEMInfor_fCommonBaseline;
	GEMInfor_input.GEMInfor_fCommonMode      = this->GEMInfor_fCommonMode;
	GEMInfor_input.GEMInfor_fEventBuilder    = this->GEMInfor_fEventBuilder;
	GEMInfor_input.GEMInfor_fLevelZero       = this->GEMInfor_fLevelZero;
	GEMInfor_input.GEMInfor_fLevelOne        = this->GEMInfor_fLevelOne;
	GEMInfor_input.GEMInfor_fApvCount        = this->GEMInfor_fApvCount;

	GEMInfor_input.GEMInfor_fAPVIndex        = this->GEMInfor_fAPVIndex;
	GEMInfor_input.GEMInfor_fAdcGain         = this->GEMInfor_fAdcGain;
	GEMInfor_input.GEM_APV_Attached          = this->GEM_APV_Attached;


	return 1;

}


GEMAPVinfor::GEMAPVinfor() {
	GEMAPVinfor_fI2C=-1;
	GEMAPVinfor_fADC=-1;

		//APV configuration variables
	GEMAPVinfor_fNumSample=-1;         // number of time samples per trigger
	GEMAPVinfor_fLatency=-1;
	GEMAPVinfor_fMode=-1;

		//APV raw strips data
	GEMAPVinfor_RawData.clear();
};

GEMAPVinfor::~GEMAPVinfor() {

};

// clone this information to the input  varibales
int GEMAPVinfor::GEMAPVinfor_clone(GEMAPVinfor &GEMAPVinfor_Input) {

	GEMAPVinfor_Input.GEMAPVinfor_fI2C        =  this->GEMAPVinfor_fI2C;
	GEMAPVinfor_Input.GEMAPVinfor_fADC        =  this->GEMAPVinfor_fADC;
	GEMAPVinfor_Input.GEMAPVinfor_fNumSample  =  this->GEMAPVinfor_fNumSample;
	GEMAPVinfor_Input.GEMAPVinfor_fLatency    =  this->GEMAPVinfor_fLatency;
	GEMAPVinfor_Input.GEMAPVinfor_fMode       =  this->GEMAPVinfor_fMode;
	if((this->GEMAPVinfor_RawData).size()!=0) {
		GEMAPVinfor_Input.GEMAPVinfor_RawData=this->GEMAPVinfor_RawData;
	}
	return 1;
}
