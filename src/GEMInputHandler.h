/*
 * GEMInputHandler.h
 *	mainly used for multi version and multi file Formate decode
 *  Created on: Oct 24, 2016
 *      Author: newdriver
 */

#ifndef GEMINPUTHANDLER_H_
#define GEMINPUTHANDLER_H_

#include <stdio.h>
#include <vector>

//CERN ROOT functions
#include <TString.h>
#include <TH1F.h>
using namespace std;
class GEMInputHandler {
public:
	GEMInputHandler();
	virtual ~GEMInputHandler();

// public variables
public:
	int GEMInputHandler_EventIndex=0;  // used for buffer current Events index, ready for the concurrent calculation


// public functions
public:


// test functions
public:
	int GEMInputHandler_TestFunctions();

private:

};

#endif /* GEMINPUTHANDLER_H_ */
