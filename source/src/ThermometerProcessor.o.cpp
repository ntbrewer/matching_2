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
    DeclareHistogram1D(THERM_DT, SA, "Time Difference");
    DeclareHistogram1D(THERM_AMP, SE, "Therm Amplitude");
    DeclareHistogram1D(THERM_TEMP, SD, "Therm Temperature K");
    DeclareHistogram2D(THERM_AMP_V_DT, SD, SA, "Amp/Temp vs. Time Diff");
    DeclareHistogram2D(THERM_AMP_V_T, SD, SD, "Amp/Temp vs. Time (min)");
}

bool ThermometerProcessor::Process(RawEvent &event) 
{
    if (!EventProcessor::Process(event)) //ensure that there is an event and starts the process timing
	return false;

    if(!RetrieveData(event)) {
	EndProcess();
	return (didProcess = false);
    } else {
	AnalyzeData();
	EndProcess();
	return true;
    }
}

bool ThermometerProcessor::RetrieveData(RawEvent &event)
{   
    ThermometerMap.clear();

    static const DetectorSummary* thermometerSummary = event.GetSummary("therm");

    if(thermometerSummary)
    {
	for(vector<ChanEvent*>::const_iterator itTherm = thermometerSummary->GetList().begin();
	    itTherm != thermometerSummary->GetList().end(); itTherm++)
	{
	    ChanEvent *chan = *itTherm;
	    
	    string detSubtype = chan->GetChanID().GetSubtype();
	    ThermometerMap.insert(make_pair(detSubtype, ThermData(chan)));
	}
    }
    if(ThermometerMap.empty())
    {
        cout << "empty Map" <<endl;	
        return(false);
    }
    else
	return(true);

}//bool ThermometerProcessor::RetrieveData

void ThermometerProcessor::AnalyzeData(void)
{
        for(map<string, ThermData>::const_iterator itTherm = ThermometerMap.begin(); itTherm !=ThermometerMap.end(); itTherm++) 
        {
           double energy = (*itTherm).second.energy;
           double calEnergy = (*itTherm).second.calEnergy;
           double time = (*itTherm).second.time;
           
           static double previousTime = time;
           static double firstTime = time;
           double dTime = time - previousTime;
           double TempK = ((0.190 + sqrt(5.5*1e-5*energy -0.197))-0.509)/0.00645 + 273.15; // Temperature in K
           
           //cout << "E " << energy/2 << " cE "<< calEnergy <<" t " << time << " fT "<< (time - firstTime)/1e8 << " pT "<< previousTime <<" dT "<< dTime/1000 << endl;
           plot(THERM_AMP, energy/2);
           plot(THERM_TEMP, TempK);
           plot(THERM_DT, dTime);
           plot(THERM_AMP_V_DT, energy/2, dTime/1000 );
           plot(THERM_AMP_V_T, TempK, (time-firstTime)/1e8 );

           previousTime = time;
        } 

} // void ThermometerProcessor::AnalyzeData


ThermometerProcessor::ThermData::ThermData(ChanEvent* chan)
{
    location       = chan->GetChanID().GetLocation();
    detSubtype	= chan->GetChanID().GetSubtype();
    energy	= chan->GetEnergy();
    calEnergy = chan->GetCalEnergy();
    time = chan->GetTime();
}
