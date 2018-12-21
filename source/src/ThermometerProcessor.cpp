/*****************************************
Source File for the ThermometerProcessor
   N.T.B. 16 Mar 2017
********************************************/
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <fstream>
#include <iostream>
#include <numeric>
#include <string>
#include <unistd.h> //not standard c/c++, makes compliant with UNIX standards
#include <vector>

#include "damm_plotids.h"
#include "DetectorDriver.h"
#include "ThermometerProcessor.h"
#include "RawEvent.h"

extern "C" void count1cc_(const int &, const int &, const int &);
extern "C" void set2cc_(const int &, const int &, const int &, const int &);

using namespace std;
using namespace dammIds::thermometerprocessor;

ThermometerProcessor::ThermometerProcessor(): EventProcessor()
{
    name = "therm";
    associatedTypes.insert("therm"); //associate with thermometer
}

void ThermometerProcessor::DeclarePlots(void) const
{
    DeclareHistogram1D(THERM_AMP, SE, "Therm Amplitude");
    DeclareHistogram1D(THERM_DT, SA, "Time Difference");
    DeclareHistogram1D(THERM_AVG_MV, SA, "Avg. Amplitude in mV");
    DeclareHistogram1D(THERM_TEMP, SD, "Therm Temperature K");
    //DeclareHistogram1D(THERM_LOWE, SE, "LowE");
    //DeclareHistogram1D(THERM_HIGHE, SE, "HighE");
    DeclareHistogram1D(THERM_AVG, SE, "Avg Temperature in K");
    DeclareHistogram2D(THERM_AMP_V_DT, SD, SA, "Amp (mV) vs. Time Diff");
    DeclareHistogram2D(THERM_AMP_V_T, SA, SD, "Amp (mV) vs. Time (min)");
}

bool ThermometerProcessor::PreProcess(RawEvent &event) 
{
    if (!EventProcessor::Process(event)) //ensure that there is an event and starts the process timing
	return false;
    
    if(!AnalyzeData(event)) {
	  EndProcess();
	  return (didProcess = false);
    } else {
	  EndProcess();
	  return true;
    }
}

bool ThermometerProcessor::AnalyzeData(RawEvent &event)
{
    static const DetectorSummary* thermometerSummary = event.GetSummary("therm");

    if(thermometerSummary)
    {
	    for(vector<ChanEvent*>::const_iterator itTherm = thermometerSummary->GetList().begin();
	        itTherm != thermometerSummary->GetList().end(); itTherm++)
	    {
	        ChanEvent *chan = *itTherm;
	        string detSubtype = chan->GetChanID().GetSubtype();

           double energy =( chan->GetEnergy())/2.0;
           double calEnergy = chan->GetCalEnergy(); // amplitude in mV
           double time = (chan->GetTime()) * pixie::clockInSeconds;
           double lowE =-1.;
           double highE =-1.;
           static double previousLowT,previousHighT,dLowT, dHighT;
           static double previousTime = time;
           static double firstTime = time;
           double tempK = (calEnergy-0.509)/0.00645 + 273.15; // Temperature in K
           double sampleT = 10.; //# of seconds to average, 10 for 200Hz
           double timeMin = (time - firstTime)/sampleT;
           static int sample = 0, n=0;
           static double Eavg =0, eAvgV=calEnergy;
           double energyCut = 5000;
           if (energy< energyCut) 
           {
           		lowE = calEnergy;
           		dLowT = time - previousLowT;
           		previousLowT = time;
           } else
           {
           		highE = calEnergy;
           		dHighT = time - previousHighT;
           		previousHighT = time;
           }
           
           if ( timeMin > 1 ) 
           {
           		firstTime = time;
           		sample++;
           		eAvgV = Eavg/n;
           		tempK = (eAvgV-0.509)/0.00645 + 273.15;
            	//tempK = eAvgV*1000;
           		plot(THERM_AVG, tempK);
              plot(THERM_AMP_V_T, eAvgV*1000, timeMin*sample*sampleT/60. );
           		Eavg =0;
           		n=0;
           		tempK =-1.;
           } else if (lowE>0)
           {
           		Eavg+=lowE;
           		n++;
           }
           
           plot(THERM_AMP, energy); // plots full ADC range
           //plot(THERM_LOWE, lowE*1000);
           //plot(THERM_HIGHE, highE*1000);diagnostics for energy cut
           plot(THERM_TEMP, tempK);
           
           if (lowE > 0)
           {
	           plot(THERM_AMP_V_DT, lowE, 1/dLowT );
             plot(THERM_DT, 1/dLowT);
		       }
					 if (highE > 0)
           {
	           plot(THERM_AMP_V_DT, highE, 1/dHighT );
             plot(THERM_DT, 1/dHighT);	           
		       }
           previousTime = time;
           chan->SetTemperature(eAvgV);
        } 
    } else 
    {
        return false;
    }
    
    return true;
} // void ThermometerProcessor::AnalyzeData

