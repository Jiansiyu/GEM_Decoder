/*
 * GEMEventDecoder.h
 *
 *  Created on: Nov 13, 2016
 *      Author: newdriver
 *
 *      Main Event decode function,
 *    		generate CommenMode
 *    		generate Pedestal
 *    		Applt Zero Subtraction
 */

#ifndef GEMEVENTDECODER_H_
#define GEMEVENTDECODER_H_
#include <stdio.h>
#include <map>

using namespace std;
class GEMEventDecoder {
public:
	GEMEventDecoder();
	//					Event ID, MPDID,    APVID     Tsample    Nstr  ADC
	GEMEventDecoder(std::map < int, std::map <int, std::map <int, std::map < int, std::map< int, int > > > > > SingleEvts_Input);
	GEMEventDecoder(std::map <int, std::map <int, std::map < int, std::map< int, int > > > > SingleEvts_Input);
	virtual ~GEMEventDecoder();
public:
	//Event ID, MPDID,    APVID     Tsample    Nstr  ADC
	std::map<int, std::map <int, std::map <int, std::map < int, std::map< int, int > > > > > eDGetCommonMode();
	std::map<int, std::map <int, std::map <int, std::map < int, std::map< int, int > > > > > eDGetCommonModeRmPk();   // remove peak first befor get the Common mode
	std::map<int, std::map <int, std::map <int, std::map < int, std::map< int, int > > > > > eDGetSigma();
	std::map<int, std::map <int, std::map <int, std::map < int, std::map< int, int > > > > > eDGetMean();

	//MPDID,    APVID     Tsample    Nstr  ADC
	//std::map <int, std::map <int, std::map < int, std::map< int, int > > > > eDGetCommonMode();
	//std::map <int, std::map <int, std::map < int, std::map< int, int > > > > eDGetCommonModeRmPk();   // remove peak first befor get the Common mode
	//std::map <int, std::map <int, std::map < int, std::map< int, int > > > > eDGetSigma();
	//std::map <int, std::map <int, std::map < int, std::map< int, int > > > > eDGetMean();

// private virables
private:

private:
	std::map< int, int > eDRemovePeak(void *);


};

#endif /* GEMEVENTDECODER_H_ */