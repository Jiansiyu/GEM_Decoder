/*
 * GEMInfor.h
 *
 *  Created on: Oct 24, 2016
 *      Author: newdriver
 */

#ifndef GEMINFOR_H_
#define GEMINFOR_H_

#include <map>
#include <vector>

using namespace std;


// APV information class
class GEMAPVinfor {
public:
	GEMAPVinfor();
	virtual ~GEMAPVinfor();

// public variables configure variables
public:
	int GEMAPVinfor_fI2C;
	int GEMAPVinfor_fADC;

	//APV configuration variables
	int GEMAPVinfor_fNumSample;         // number of time samples per trigger
	int GEMAPVinfor_fLatency;
	int GEMAPVinfor_fMode;

	//APV raw strips data
	map<int,map<int,int> > GEMAPVinfor_RawData;
	//n time samples -> 128 channels -> pair<strips id, ADC value>

//public functions
public:
	int GEMAPVinfor_Initialize();           //clear all the data and set all the variables to -1

// test functions
public:
	int GEMAPVinfor_TestFcunctions();
};



//MPD decode information class
class GEMInfor {
public:
	GEMInfor();
	virtual ~GEMInfor();

// public variables
public:
	int GEMInfor_fSlot;
	int GEMInfor_fNumTrigger;
	int GEMInfor_fCalibLatency;
	int GEMInfor_fClockPhase[2];
	int GEMInfor_fCommonBaseline;
	int GEMInfor_fCommonMode; // common mode noise subtraction on/off
	int GEMInfor_fEventBuilder; // fpga event builder on/off
	int GEMInfor_fLevelZero;
	int GEMInfor_fLevelOne;
	int GEMInfor_fApvCount;			   // how many APVs attached to this MPD
	map<int, int> GEMInfor_fAPVIndex;  // APV I2C-ID   APV ADC-ID
	map<int,int> GEMInfor_fAdcGain;    //
	vector<GEMAPVinfor> GEM_APV_Attached;

// public functions
public:

	int GEMInfor_SaveTrees();    // used for generate certain data format trees
	int GEMInfor_Initialize();   // clear all the data and set all the variables to -1

	int GEMInfor_clone(GEMInfor *GEMInfor_input);        // clone the input functions to local functions
	int GEMInfor_clone(GEMInfor &GEMInfor_input);        // clone local variables to the input functions

// test functions
public:
	int GEMInfor_TestFunctions();

private:

};

#endif /* GEMINFOR_H_ */
