/*
 * GEMEventDecoder.cpp
 *
 *  Created on: Nov 13, 2016
 *      Author: newdriver
 */

#include "GEMEventDecoder.h"

GEMEventDecoder::GEMEventDecoder() {
	// TODO Auto-generated constructor stub

}

GEMEventDecoder::~GEMEventDecoder() {
	// TODO Auto-generated destructor stub
}

GEMEventDecoder::GEMEventDecoder(std::map < int, std::map <int, std::map <int, std::map < int, std::map< int, int > > > > > SingleEvts_Input) {

};


GEMEventDecoder::GEMEventDecoder(std::map <int, std::map <int, std::map < int, std::map< int, int > > > > SingleEvts_Input) {

};

//      Event ID,      MPDID,         APVID            Tsample    CommonMode
std::map<int, std::map <int, std::map <int, std::map < int,  int > > > > GEMEventDecoder::eDGetCommonMode() {

};

// remove peak first befor get the Common mode
//      Event ID,      MPDID,         APVID            Tsample    CommonMode
std::map<int, std::map <int, std::map <int, std::map < int,  int > > > > GEMEventDecoder::eDGetCommonModeRmPk() {

};

//      Event ID,      MPDID,         APVID             Nstrips   Sigma
std::map<int, std::map <int, std::map <int, std::map < int,  int > > > > GEMEventDecoder::eDGetSigma() {

};
//      Event ID,      MPDID,         APVID             Nstrips   Sigma
std::map<int, std::map <int, std::map <int, std::map < int,  int > > > > GEMEventDecoder::eDGetMean() {

};

//Event ID, MPDID,    APVID      Nstr  ADC
std::map<int, std::map <int, std::map < int, std::map< int, int > > > > GEMEventDecoder::eDGetCommonModeSubtraction() {

}
