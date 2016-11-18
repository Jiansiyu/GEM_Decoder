//============================================================================
// Name        : GEM_Analysis.cpp
// Author      : Siyu Jian
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <getopt.h>
#include <time.h>    // used for calculate the process time
#include <map>
//ROOT related libs
#include <TApplication.h>
#include "TH1F.h"
#include "TCanvas.h"

// user defined functions
#include "GEMInputHandler.h"
#include "GEMRawFileDecoder.h"
using namespace std;

// help usage
void Usage()
{

	}
/*
 * //进行物理地址到程序地址的转换   输入物理地址，输出程序地址        输入范围0-127    输出范围0-127
unsigned int process_physical_to_logical(unsigned int physical_addr)
  {
     unsigned char group_temp=physical_addr/8;
     unsigned char number_in_group_temp=physical_addr%8;
     unsigned int logical_addr_base=0;
     unsigned int logical_addr=0;

     logical_addr_base= (unsigned char)( group_temp/4)+( group_temp%4)*4;
     logical_addr= logical_addr_base+ number_in_group_temp*16;
     return logical_addr;
    };
//进行程序地址到物理地址的转换
unsigned int process_logical_to_physical(unsigned int logical_addr)
   {
      for(unsigned char i=0;i<128;i++)
        {
           if(logical_addr==process_physical_to_logical(i))
             return i;
          }
    }
*/
int main(int argc, char **argv)
{
		TApplication theApp("App", &argc, argv);   //Root application framework
		GEMInputHandler *GEMDecoderIuputHandler = new GEMInputHandler();
		GEMDecoderIuputHandler->GEMInputHandler_TestFunctions();
		//test functions
		TTree test_tree;
		GEMRawFileDecoder *GEMRawFileDecoder_TEST=new GEMRawFileDecoder("/home/newdriver/Research/Eclipse_workspace/GEM_Analysis/Debug/test_2458.dat", &test_tree);
		GEMRawFileDecoder_TEST->GEMRawFileDecoder_TestFunction();
		//theApp.Run(kTRUE);
		return 0;
}
