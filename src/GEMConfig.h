/*
 * GEMConfig.h
 *
 *  Created on: Nov 3, 2016
 *      Author: newdriver
 */

#ifndef GEMCONFIG_H_
#define GEMCONFIG_H_

const unsigned int KMAX_NSAMPLE = 6;
const unsigned int KNSTRIPS     = 128;


struct APV_DataStruct {
		int EventID;
		int detID;

		int MPDID;
		int APVID;
		int SampeID;
		int StripADC[ KMAX_NSAMPLE ] [ KNSTRIPS ];
	};


#endif /* GEMCONFIG_H_ */
