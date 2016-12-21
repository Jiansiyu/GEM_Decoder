/*
 * GEMConfig.h
 *
 *  Created on: Nov 3, 2016
 *      Author: newdriver
 *
 *      maybe a better way is change this to a namespace, will change it latter
 */

#ifndef GEMCONFIG_H_
#define GEMCONFIG_H_

const unsigned int KMAX_NSAMPLE = 6;
const unsigned int KNSTRIPS     = 128;

const int ChNb[128] = {
	    0,   32,   64,   96,    8,   40,   72,  104,
	   16,   48,   80,  112,   24,   56,   88,  120,
	    1,   33,   65,   97,    9,   41,   73,  105,
	   17,   49,   81,  113,   25,   57,   89,  121,
	    2,   34,   66,   98,   10,   42,   74,  106,
	   18,   50,   82,  114,   26,   58,   90,  122,
	    3,   35,   67,   99,   11,   43,   75,  107,
	   19,   51,   83,  115,   27,   59,   91,  123,
	    4,   36,   68,  100,   12,   44,   76,  108,
	   20,   52,   84,  116,   28,   60,   92,  124,
	    5,   37,   69,  101,   13,   45,   77,  109,
	   21,   53,   85,  117,   29,   61,   93,  125,
	    6,   38,   70,  102,   14,   46,   78,  110,
	   22,   54,   86,  118,   30,   62,   94,  126,
	    7,   39,   71,  103,   15,   47,   79,  111,
	   23,   55,   87,  119,   31,   63,   95,  127
};

const int CHNb_LogicAddr[128]= {
		   0,   16,   32,   48,   64,   80,   96,  112,
		   4,   20,   36,   52,   68,   84,  100,  116,
		   8,   24,   40,   56,   72,   88,  104,  120,
		  12,   28,   44,   60,   76,   92,  108,  124,
		   1,   17,   33,   49,   65,   81,   97,  113,
		   5,   21,   37,   53,   69,   85,  101,  117,
		   9,   25,   41,   57,   73,   89,  105,  121,
		  13,   29,   45,   61,   77,   93,  109,  125,
		   2,   18,   34,   50,   66,   82,   98,  114,
		   6,   22,   38,   54,   70,   86,  102,  118,
		  10,   26,   42,   58,   74,   90,  106,  122,
		  14,   30,   46,   62,   78,   94,  110,  126,
		   3,   19,   35,   51,   67,   83,   99,  115,
		   7,   23,   39,   55,   71,   87,  103,  119,
		  11,   27,   43,   59,   75,   91,  107,  123,
		  15,   31,   47,   63,   79,   95,  111,  127
};

struct APV_DataStruct {
		int EventID;
		int detID;
		int MPDID;
		int APVID;
		int SampeID;
		int StripADC[KMAX_NSAMPLE][KNSTRIPS];
	};

const int MAX_PEAKS_PEVNT=20;    // set the maximum peaks for single event and single APV
const int EVENTS_THR  =850;
#endif /* GEMCONFIG_H_ */
