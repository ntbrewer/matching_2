/*! \file MtasProcessor.cpp
 *
 * The MTAS processor handles detectors of type mtas  */

#include "damm_plotids.h"
#include "param.h"
#include "MtasProcessor.h"
#include "DetectorDriver.h"
#include "RawEvent.h"
#include <limits.h>
#include <iostream>
#include <fstream>
#include <math.h>

 
using std::cout;
using std::endl;
using std::vector;
using std::string;

MtasProcessor::MtasProcessor() :
  EventProcessor(), mtasSummary(NULL), siliSummary(NULL), logiSummary(NULL)
{
  firstTime = -1.;
    name = "mtas";
    associatedTypes.insert("mtas");
    associatedTypes.insert("sili");//silicons
    associatedTypes.insert("logi");//logic sygnals
    //associatedTypes.insert("ionc");//ionization chamber
}

void MtasProcessor::DeclarePlots(void) const
{
	using namespace dammIds::mtas;
    
	
	const int EnergyBins = SE; 
	const int positionBins = S6;
	const int timeBins     = S8;
	const int maxCycles    = S8;

	DeclareHistogram2D(MTAS_POSITION_ENERGY, EnergyBins, positionBins, "MTAS  vs E - RF");

	//TAS spectras
	for(int i=0; i<4; i++)
	{    	
		string titlePart;
		if(i == 1)
			titlePart = ", Background";
		if(i == 2)	
			titlePart = ", Light Pulser";
		if(i == 3)
			titlePart = ", Irradiation";       
		DeclareHistogram1D(MTAS_POSITION_ENERGY+200+i, EnergyBins, ("Total Mtas"+titlePart).c_str());
		DeclareHistogram1D(MTAS_POSITION_ENERGY+210+i, EnergyBins, ("Total Central"+titlePart).c_str());
		DeclareHistogram1D(MTAS_POSITION_ENERGY+220+i, EnergyBins, ("Total Inner"+titlePart).c_str());
		DeclareHistogram1D(MTAS_POSITION_ENERGY+230+i, EnergyBins, ("Total Middle"+titlePart).c_str());
		DeclareHistogram1D(MTAS_POSITION_ENERGY+240+i, EnergyBins, ("Total Outer"+titlePart).c_str());
	}
	
	//beta gated TAS spectras
	for(int i=0; i<5; i++)
	{    	
		string titlePart;
		if(i == 1)
			titlePart = ", Background";
		if(i == 2)	
			continue;
		if(i == 3)
			titlePart = ", Irradiation"; 
		if(i == 4)
			titlePart = ", Irr Bkg";	      
		DeclareHistogram1D(MTAS_POSITION_ENERGY+300+i, EnergyBins, ("Total Mtas, Beta-gated"+titlePart).c_str());
		DeclareHistogram1D(MTAS_POSITION_ENERGY+310+i, EnergyBins, ("Total Central, Beta-gated"+titlePart).c_str());
		DeclareHistogram1D(MTAS_POSITION_ENERGY+320+i, EnergyBins, ("Total Inner, Beta-gated"+titlePart).c_str());
		DeclareHistogram1D(MTAS_POSITION_ENERGY+330+i, EnergyBins, ("Total Middle, Beta-gated"+titlePart).c_str());
		DeclareHistogram1D(MTAS_POSITION_ENERGY+340+i, EnergyBins, ("Total Outer, Beta-gated"+titlePart).c_str());
	}
	
	//Sum spectras
	DeclareHistogram1D(MTAS_POSITION_ENERGY+200+25, EnergyBins, "Sum I");
	DeclareHistogram1D(MTAS_POSITION_ENERGY+200+35, EnergyBins, "Sum M");
	DeclareHistogram1D(MTAS_POSITION_ENERGY+200+45, EnergyBins, "Sum O");
	// K.C. Goetz: added for light pulser/other stability tests 7/30/13
	//DeclareHistogram2D(MTAS_TOTALENERGY_EVO, EnergyBins, S9, "Total Energy Time Evolution");
	//DeclareHistogram2D(MTAS_CENTRALENERGY_EVO, EnergyBins, S9, "Central Energy Time Evolution");
	//DeclareHistogram2D(MTAS_INNERENERGY_EVO, EnergyBins, S9, "Inner Energy Time Evolution");
	//DeclareHistogram2D(MTAS_MIDDLEENERGY_EVO, EnergyBins, S9, "Middle Energy Time Evolution");
	//DeclareHistogram2D(MTAS_OUTERENERGY_EVO, EnergyBins, S9, "Outer Energy Time Evolution");

	//for (int i=0; i<49; i++)
	//  {
	//    DeclareHistogram2D(MTAS_PMTENERGY_EVO+i, EnergyBins, S9, "Single PMT Energy");
	//  }

	// K. C. Goetz: added for MTAS March 2015 experiment
	for (int i=0; i<5; i++)
	  {
	    string titlePart;
	    if(i == 0)
	      titlePart = "Total MTAS ";
	    if(i == 1)
	      titlePart = "Total Central ";
	    if (i == 2)
	      titlePart = "Total Inner ";
	    if (i == 3)
	      titlePart = "Total Middle ";
	    if (i ==4)
	      titlePart = "Total Outer ";
	    DeclareHistogram2D(MTAS_EVO_NOLOGIC+i,EnergyBins,SA,(titlePart+"Full Story, No logic").c_str());
	    DeclareHistogram2D(MTAS_LIGHTPULSER_EVO+i, EnergyBins, SA, (titlePart+"LightPulser vs Cycle Time").c_str());
	  }
	DeclareHistogram2D(MTAS_REFERENCE_EVO, EnergyBins, SA, "Reference Moldule vs Time");

	//Beta-gated sum spectras
	DeclareHistogram1D(MTAS_POSITION_ENERGY+300+5, EnergyBins, "Sum I+M+O, Beta-gated");
	DeclareHistogram1D(MTAS_POSITION_ENERGY+300+25, EnergyBins, "Sum I, Beta-gated");
	DeclareHistogram1D(MTAS_POSITION_ENERGY+300+35, EnergyBins, "Sum M, Beta-gated");
	DeclareHistogram1D(MTAS_POSITION_ENERGY+300+45, EnergyBins, "Sum O, Beta-gated");

	//2D spectras
	DeclareHistogram2D(MTAS_POSITION_ENERGY+350, SC, SC, "MTAS  vs I, M, O");//SC???? 
	//DeclareHistogram2D(MTAS_POSITION_ENERGY+260, SC, timeBins, "MTAS  vs cycle time");
	//DeclareHistogram2D(MTAS_POSITION_ENERGY+360, SC, timeBins, "MTAS  vs cycle time");//??????????????????????????????????????????????????????
	DeclareHistogram2D(MTAS_POSITION_ENERGY+260, SC, SB, "MTAS  vs cycle time");
	DeclareHistogram2D(MTAS_POSITION_ENERGY+360, SC, SB, "MTAS  vs cycle time, B-gated");
	//DeclareHistogram2D(MTAS_POSITION_ENERGY+261, SC, SC, "MTAS  vs cycle time, MSU, BGD");
	DeclareHistogram2D(MTAS_POSITION_ENERGY+361, SD, SC, "MTAS  vs cycle time, MSU, BGD, B-gated");
	//DeclareHistogram2D(MTAS_POSITION_ENERGY+262, SC, SC, "MTAS  vs cycle time, MSU, BGU");
	DeclareHistogram2D(MTAS_POSITION_ENERGY+362, SD, SB, "MTAS  vs cycle time, MSU, BGU, B-gated");

	DeclareHistogram2D(MTAS_POSITION_ENERGY+355, SC, SC, "Gamma-Gamma Matrix");
	DeclareHistogram2D(MTAS_POSITION_ENERGY+275, S4, EnergyBins, "Energy vs Multiplicity, Central");
	DeclareHistogram2D(MTAS_POSITION_ENERGY+276, S4, S9, "Min Energy vs Multiplicity, Central");
	//cycle number
	DeclareHistogram2D(MTAS_POSITION_ENERGY+270, SD, S8, "MTAS  vs cycle nymber");	
	DeclareHistogram2D(MTAS_POSITION_ENERGY+271, S4, S8, "Strip Number  vs cycle nymber");
	DeclareHistogram2D(MTAS_POSITION_ENERGY+272, SC, S8, "Logic signals in cycle number vs cycle time");
	DeclareHistogram1D(MTAS_POSITION_ENERGY+273, EnergyBins, "Events in time");
	DeclareHistogram1D(MTAS_POSITION_ENERGY+274, S4, "Central Multiplicity");
	//sum of the front and back photomultipier
	for(int i=0; i<24; i++)
	{
		DeclareHistogram1D(MTAS_POSITION_ENERGY+100+2*i, EnergyBins, "Sum F+B, Beta-gated"); 
		
		DeclareHistogram1D(MTAS_POSITION_ENERGY+100+2*i+1, EnergyBins, "Sum F+B, Light Pulser");
	}
	
	//Silicons
	DeclareHistogram1D(MTAS_POSITION_ENERGY+500, S4, "Silicon Multiplicity");
	DeclareHistogram1D(MTAS_POSITION_ENERGY+510, S4, "Silicon Position, left chan - top detector");
	//DeclareHistogram2D(MTAS_POSITION_ENERGY+520, S4, SC, "Energy vs Silicon Position");	
	
	//Logic signals in cycle time

}

bool MtasProcessor::Process(RawEvent &event)
{
	using namespace dammIds::mtas;
	if (!EventProcessor::Process(event))
	return false;

	// first time through, grab the according detector summaries
	if (mtasSummary == NULL)
		mtasSummary = event.GetSummary("mtas");//czy musze to robic tak?
	if (siliSummary == NULL)	
		siliSummary = event.GetSummary("sili");
	if (logiSummary == NULL)
		logiSummary = event.GetSummary("logi");
	//DetectorSummary *ioncSummary = event.GetSummary("ionc");

	vector<ChanEvent*> mtasList= mtasSummary->GetList();
	vector<ChanEvent*> siliList= siliSummary->GetList();
	vector<ChanEvent*> logiList= logiSummary->GetList();
	//vector<ChanEvent*> ioncList= ioncSummary->GetList();
	
	
	map<string, struct MtasData> mtasMap;
	map<string, struct MtasData> siliMap;
	map<string, struct MtasData> logiMap;
	//map<string, struct MtasData> ioncMap;

	
	
//sort data
	double maxLocation =0; 
	double nrOfCentralPMTs = 0;   
	for(vector<ChanEvent*>::const_iterator mtasListIt = mtasList.begin(); mtasListIt != mtasList.end(); mtasListIt++)
	{
		string subtype = (*mtasListIt)->GetChanID().GetSubtype();
		if(subtype[0] =='C')
		    nrOfCentralPMTs ++;
		if (mtasMap.count(subtype)>0)
		{
			cout<<"Error: Detector "<<subtype<<" has "<< mtasMap.count(subtype)+1<<" signals in one event"<<endl;
			continue;//should I skip such events?
		}
			
		if ((*mtasListIt)->GetEnergy() == 0 || (*mtasListIt)->GetEnergy() > 30000)
				continue;
				
		mtasMap.insert(make_pair(subtype,MtasData((*mtasListIt))));
		
		if(maxLocation < (*mtasListIt)->GetChanID().GetLocation())
			maxLocation = (*mtasListIt)->GetChanID().GetLocation();
	}
	
	
	bool isBetaSignal = 0;

	extern DetectorDriver driver;
  
	for(vector<ChanEvent*>::const_iterator siliListIt = siliList.begin(); siliListIt != siliList.end(); siliListIt++)
	{
		string subtype = (*siliListIt)->GetChanID().GetSubtype();
		if (mtasMap.count(subtype)>0)
			cout<<"Error: Detector "<<subtype<<" has "<< siliMap.count(subtype)+1<<" signals in one event"<<endl;
		
		Calibration cal = driver.cal.at((*siliListIt)->GetID());
		if ((*siliListIt)->GetEnergy() < cal.GetMinThreshold() || (*siliListIt)->GetEnergy() > 30000)
			continue;
				
		siliMap.insert(make_pair(subtype,MtasData((*siliListIt))));
		//isBetaSignal = 1;
	}
	if(siliMap.size()>0)
	{
	    double maxSiliconSignal = siliSummary-> GetMaxEvent()->GetCalEnergy();//Jan 03 2011 Ola K
	    if(maxSiliconSignal > 2.)
            isBetaSignal = 1;
     }
        
	bool isTriggerOnSignal = 0;
	bool isTapeMoveOnSignal = 0;
	bool isTapeMoveOffSignal = 0;
	bool isMeasureOnSignal = 0;
	bool isMeasureOffSignal = 0;	
 	bool isBkgOnSignal = 0;
	bool isBkgOffSignal = 0;
 	bool isLightPulserOnSignal = 0;
	bool isLightPulserOffSignal = 0;	
 	bool isIrradOnSignal = 0;
	bool isIrradOffSignal = 0;
	
	double logicTreshold = 1;	//logic threshold !!!!!!! (value?????)	
	int logicSignalsValue = 0;
	for(vector<ChanEvent*>::const_iterator logiListIt = logiList.begin(); logiListIt != logiList.end(); logiListIt++)
	{
		string subtype = (*logiListIt)->GetChanID().GetSubtype();
		if (logiMap.count(subtype)>0)
			cout<<"Error: Detector "<<subtype<<" has "<< logiMap.count(subtype)+1<<" signals in one event"<<endl;
		logiMap.insert(make_pair(subtype,MtasData(((*logiListIt)))));
		
		//set logic flags
		if((*logiListIt)->GetEnergy() > logicTreshold)
		{
			if(subtype == "TRU") {
				isTriggerOnSignal = 1;
				measureOnTime = (*logiListIt)->GetTime() * pixie::clockInSeconds;
				cycleNumber ++;
				logicSignalsValue +=1;
			}
			
			if(subtype == "IRU")
			{
				isIrradOnSignal = 1;
				logicSignalsValue +=2;
			}
			if(subtype == "IRD")
			{
				isIrradOffSignal = 1;
				logicSignalsValue +=4;
			}	
			if(subtype == "LPU")
			{
				isLightPulserOnSignal = 1;
				logicSignalsValue +=8;
			}
			if(subtype == "LPD")
			{
				isLightPulserOffSignal = 1;
				logicSignalsValue +=16;
			}
			if(subtype == "TMU")
			{
				isTapeMoveOnSignal = 1;
				logicSignalsValue +=32;
			}
			if(subtype == "TMD")
			{
				isTapeMoveOffSignal = 1;
				logicSignalsValue +=64;
			}
			
			if(subtype == "BGU")
			{
				isBkgOnSignal = 1;
				logicSignalsValue +=128;
			}
			if(subtype == "BGD")
			{
				isBkgOffSignal = 1;
				logicSignalsValue +=256;
			}
			if(subtype == "MSU")
			{
				isMeasureOnSignal = 1;
				logicSignalsValue +=512;
			}
			if(subtype == "MSD")
			{
				isMeasureOffSignal = 1;
				logicSignalsValue +=1024;
			}		

		}
		
	}    
	//cycleNumber = 
	double cycleTime = -1;
	double actualTime = -1;
	double actualLogiTime = -1;
	double cycleLogiTime = -1;

	if(mtasSummary->GetMult() > 0)//I have at leat one element in mtasList
	{
		vector<ChanEvent*>::const_iterator mtasListIt = mtasList.begin();
		actualTime = (*mtasListIt)->GetTime() * pixie::clockInSeconds;
		if (firstTime == -1.) {
		  firstTime = actualTime;
		}
		cycleTime = actualTime - measureOnTime;
		//cout<<actualTime<<" act"<<endl;
		
	}
	if(logiSummary->GetMult() > 0)//I have at leat one element in mtasList
	{
		vector<ChanEvent*>::const_iterator logiListIt = logiList.begin();
		actualLogiTime = (*logiListIt)->GetTime() * pixie::clockInSeconds;
		cycleLogiTime = actualLogiTime - measureOnTime;
	}

	   
//Check flags and set main (static) flags

	//tape		
	if(isTapeMoveOffSignal && isTapeMoveOnSignal)
		cout<<"Error: tape movement signal and end of tape movement signal in the same event"<<endl;
		
	if(isTapeMoveOnSignal && isTapeMoveOn) 
		cout<<"Error: No end of tape movement signal in the last tape cicle"<<endl;
		
	if(isTapeMoveOnSignal)
		isTapeMoveOn = 1;
	if(isTapeMoveOffSignal)
		isTapeMoveOn = 0;

	//measurement  
	if(isMeasureOffSignal && isMeasureOnSignal)
		cout<<"Error: measurement signal and no measurement signal in the same event"<<endl;
		
	if(isMeasureOnSignal && isMeasureOn) 
		cout<<"Error: No end of measurement signal in the last tape cicle"<<endl;
		
	if(isMeasureOnSignal)
		isMeasureOn = 1;
	if(isMeasureOffSignal)
		isMeasureOn = 0; 

	//background		
	if(isBkgOffSignal && isBkgOnSignal)
		cout<<"Error: background signal and no background signal in the same event"<<endl;
		
	if(isBkgOnSignal && isBkgOn) 
		cout<<"Error: No end of background signal in the last tape cicle"<<endl;
		
	if(isBkgOnSignal)
		isBkgOn = 1;
	if(isBkgOffSignal)
		isBkgOn = 0; 
		
	//light pulser	
	if(isLightPulserOffSignal && isLightPulserOnSignal)
		cout<<"Error: light pulser signal and no light pulser signal in the same event"<<endl;
		
	if(isLightPulserOnSignal && isLightPulserOn) 
		cout<<"Error: No end of light pulser signal in the last tape cicle"<<endl;
		
	if(isLightPulserOnSignal)
		isLightPulserOn = 1;
	if(isLightPulserOffSignal)
		isLightPulserOn = 0;		
		
	//irradiation		
	if(isIrradOffSignal && isIrradOnSignal)
		cout<<"Error: irradiation signal and no irradiation signal in the same event"<<endl;
		
	if(isIrradOnSignal && isIrradOn) 
		cout<<"Error: No end of irradiation signal in the last tape cicle"<<endl;
		
	if(isIrradOnSignal)
		isIrradOn = 1;
	if(isIrradOffSignal)
		isIrradOn = 0;
 
//spectras 

	vector <double> totalMtasEnergy (6,-1);//0- all mtas, 1 - Central, 2 - Inner, 3 - Middle, 4 - Outer, 5 - reference module 
	vector <double> sumFrontBackEnergy(24,0);// 0-5 Central, 6-11 Inner, 12-17 Middle, 18-23 Outer
	int nrOfCentralPMT = 0;//!!!!!!!!!!!!!!!!!!!! Nov29
	double theSmallestCEnergy =60000;

	//vector <double> singlePMTenergy(49); // added by K.C. Goetz for stability testing 8/1/13

	// K.C. Goetz commented out /12 and /2 to make totalMtasEnergy.at a sum of all PMTs on a given ring and not an average on 7/30/13
	// K. C. Goetz put back in /12 and /2 for March 2015 experiment

	for(map<string, struct MtasData>::const_iterator mtasMapIt = mtasMap.begin(); mtasMapIt != mtasMap.end(); mtasMapIt++)
	{
		double signalEnergy = (*mtasMapIt).second.energy;
		double location = (*mtasMapIt).second.location;
		
		if((*mtasMapIt).first[0] == 'C')
		{
		  totalMtasEnergy.at(1) += signalEnergy/12;
		  totalMtasEnergy.at(0) += signalEnergy/12;	
			nrOfCentralPMT ++;//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			if(theSmallestCEnergy > signalEnergy)
			    theSmallestCEnergy = signalEnergy;
		}
		
		else if((*mtasMapIt).first[0] == 'I')
		{
		  totalMtasEnergy.at(2) += signalEnergy/2.;
		  totalMtasEnergy.at(0) += signalEnergy/2.;		
		}
		
		else if((*mtasMapIt).first[0] == 'M')
		{
		  totalMtasEnergy.at(3) += signalEnergy/2.;
		  totalMtasEnergy.at(0) += signalEnergy/2.;		
		}
		
		else if((*mtasMapIt).first[0] == 'O')
		{
		  totalMtasEnergy.at(4) += signalEnergy/2.;
		  totalMtasEnergy.at(0) += signalEnergy/2.;
		}

		else if((*mtasMapIt).first[0] == 'r')
		  {
		    totalMtasEnergy.at(5) += signalEnergy;
		  }

					 
		//F+B
		int moduleIndex = (location -1)/2;
		//K. C. Goetz removed warning for March 2015 experiment because of addition of reference module
		//if(moduleIndex > 24)
		//{
		//	cout<<"Warning: detector "<<(*mtasMapIt).first<<" location > 48"<<endl;
		//	continue;
		//}
		
		if(sumFrontBackEnergy.at(moduleIndex) == 0)//it's the first signal from this hexagon module
			sumFrontBackEnergy.at(moduleIndex) = -1* signalEnergy/2.;

		else if(sumFrontBackEnergy.at(moduleIndex) < 0)//second signal from this hexagon module
			sumFrontBackEnergy.at(moduleIndex) = -1*sumFrontBackEnergy.at(moduleIndex) + signalEnergy/2.;
			
		else //sumFrontBackEnergy.at(moduleIndex) > 0 - 3 or more signals in one event
			cout<<"Warning: detector "<<(*mtasMapIt).first<<" has 3 or more signals"<<endl;


		//Signal from a single PMT, added by K.C. Goetz on 9/1/13 for peak stability testing
		//int PMTindex = location;
		//singlePMTenergy.at(PMTindex) = signalEnergy;
		
	}
    if(nrOfCentralPMT != 12 || nrOfCentralPMT == 0)////////////!!!!!!!!!!!!!!!!!!!!!!!!!
        totalMtasEnergy.at(1) =-1;
    if(nrOfCentralPMT < 15)
        plot(MTAS_POSITION_ENERGY+274, nrOfCentralPMT);
        
     
    plot(MTAS_POSITION_ENERGY+275, nrOfCentralPMT, totalMtasEnergy.at(1));
    if(theSmallestCEnergy < S9)
        plot(MTAS_POSITION_ENERGY+276, nrOfCentralPMT, theSmallestCEnergy);
	for(unsigned int i=0; i<sumFrontBackEnergy.size(); i++)
	{
		if(sumFrontBackEnergy.at(i) < 0)//it was only one signal in the hexagon module
		{
			sumFrontBackEnergy.at(i)=-1;  
		}
	}

//Background  
	if(isMeasureOn && isBkgOn && !isLightPulserOn && !isTapeMoveOn)
	{
		//3201 - 3241, no B-gated and 3301 - 3341, B-gated
		for(int i=0; i<5; i++)
		{
			plot(MTAS_POSITION_ENERGY+201+i*10, totalMtasEnergy.at(i));
			if(isBetaSignal)
				plot(MTAS_POSITION_ENERGY+301+i*10, totalMtasEnergy.at(i));
		}
	}	
	/*if(mtasSummary->GetMult() > 0)//I have at leat one element in mtasList
	{
		vector<ChanEvent*>::const_iterator mtasListIt = mtasList.begin();
		actualTime = (*mtasListIt)->GetTime() * pixie::clockInSeconds;
		cycleTime = actualTime - measureOnTime;
	}*/

//Light Pulser  
	if(isLightPulserOn)
	{
		//3202 - 3242, no B-gated
		for(int i=0; i<5; i++)
			plot(MTAS_POSITION_ENERGY+202+i*10, totalMtasEnergy.at(i));
		
		//3101 - 3147 odd, no B-gated
		for(unsigned int i=0; i<sumFrontBackEnergy.size(); i++)
			plot(MTAS_POSITION_ENERGY+100+2*i+1, sumFrontBackEnergy.at(i));

		// Added by K. C. Goetz for March 2015 Experiment: Plot time evolved Light pulser spectra (vs cycle time), no B-gate
		// Hist #'s 5000-5005
		for (int i=0; i<5; i++)
		  plot(MTAS_LIGHTPULSER_EVO+i, totalMtasEnergy.at(i), cycleTime);

	}  
  
  
//Irradiation  
	if(isIrradOn && !isBkgOn)
	{
		//3203 - 3243, no B-gated and 3303 - 3343, B-gated
		for(int i=0; i<5; i++)
		{
			plot(MTAS_POSITION_ENERGY+203+i*10, totalMtasEnergy.at(i));
			if(isBetaSignal)
				plot(MTAS_POSITION_ENERGY+303+i*10, totalMtasEnergy.at(i));
		}
	} 
  
//Irradiation and Bkg 
	if(isIrradOn && isBkgOn && isBetaSignal)
	{
		//3304 - 3344, B-gated
		for(int i=0; i<5; i++)
			plot(MTAS_POSITION_ENERGY+304+i*10, totalMtasEnergy.at(i));
	}   
  
  

    //3260 mtas vs cycle time, no logic conditions
    if(totalMtasEnergy.at(0) < SC && cycleTime < SB)
	    plot(MTAS_POSITION_ENERGY+260, totalMtasEnergy.at(0), cycleTime);

    //3360
	if(totalMtasEnergy.at(0)<SC && cycleTime < SB)
		plot(MTAS_POSITION_ENERGY+360, totalMtasEnergy.at(0), cycleTime);

//the same spectra with logic conditions
	if(isMeasureOn && isBkgOn)
	{
	    //3262 mtas vs cycle time, no logic conditions
        //if(totalMtasEnergy.at(0) < SC)
	    //    plot(MTAS_POSITION_ENERGY+262, totalMtasEnergy.at(0), cycleTime);

        //3362
	    if(totalMtasEnergy.at(0)<SD && cycleTime < SB)
		    plot(MTAS_POSITION_ENERGY+362, totalMtasEnergy.at(0), cycleTime);
	}

 	//3270 Mtas vs cycle number
 	if(totalMtasEnergy.at(0)<SD && cycleNumber < S9)
       plot(MTAS_POSITION_ENERGY+270, totalMtasEnergy.at(0), cycleNumber);// Jan 03 2011
		
       if(isBetaSignal)
	 plot(MTAS_POSITION_ENERGY+273, actualTime - firstTime); 

 // Goetz March 2015 Exp: Full story of MTAS (with light pulser and background, no B-gate and etc)
 // Hist #'s: 4000-4005
	   for(int i=0; i<5; i++)
	     plot(MTAS_EVO_NOLOGIC+i, totalMtasEnergy.at(i), (actualTime - firstTime)/60);


				      
//"Regular" measurement 

	if(isMeasureOn && !isBkgOn && !isLightPulserOn && !isTapeMoveOn)
	{
		//3200 - 3240, no B-gated and 3300 - 3340, B-gated
		for(int i=0; i<5; i++)
		{
			plot(MTAS_POSITION_ENERGY+200+i*10, totalMtasEnergy.at(i));
			if(isBetaSignal)
				plot(MTAS_POSITION_ENERGY+300+i*10, totalMtasEnergy.at(i));
		}
		
        
		//3225, 3235, 3245 - sum spectra, no B-gated and 3305, 3325, 3335, 3345, B-gated 
		for(int i=0; i<6; i++)
		{
			plot(MTAS_POSITION_ENERGY+225, sumFrontBackEnergy.at(i+6));//Sum I
			plot(MTAS_POSITION_ENERGY+235, sumFrontBackEnergy.at(i+12));//Sum M
			plot(MTAS_POSITION_ENERGY+245, sumFrontBackEnergy.at(i+18));//Sum O

			if(isBetaSignal) 
			{
				plot(MTAS_POSITION_ENERGY+305, sumFrontBackEnergy.at(i+18));
				plot(MTAS_POSITION_ENERGY+305, sumFrontBackEnergy.at(i+12));
				plot(MTAS_POSITION_ENERGY+305, sumFrontBackEnergy.at(i+6));
				plot(MTAS_POSITION_ENERGY+325, sumFrontBackEnergy.at(i+6));//Sum I
				plot(MTAS_POSITION_ENERGY+335, sumFrontBackEnergy.at(i+12));//Sum M
				plot(MTAS_POSITION_ENERGY+345, sumFrontBackEnergy.at(i+18));//Sum O
			}
		
		}

		//7900 - 2D sum spectra for testing peak stability, added by K.C. Goetz 7/30/13
		//incremented by minute

		// summed spectra
		//plot(MTAS_TOTALENERGY_EVO, totalMtasEnergy.at(0), floor(actualTime/60));
		//plot(MTAS_CENTRALENERGY_EVO, totalMtasEnergy.at(1), floor(actualTime/60));
		//plot(MTAS_INNERENERGY_EVO, totalMtasEnergy.at(2), floor(actualTime/60));
		//plot(MTAS_MIDDLEENERGY_EVO, totalMtasEnergy.at(3), floor(actualTime/60));
		//plot(MTAS_OUTERENERGY_EVO, totalMtasEnergy.at(4), floor(actualTime/60));

		// single PMT spectra
		//for (int i=0; i<49; i++)
		//  {
		//    plot(MTAS_PMTENERGY_EVO+i, singlePMTenergy.at(i),floor(actualTime/60));
		//  }

		//cout << floor(actualTime/3600) << endl;

		  

	
		//3261 mtas vs cycle time
		//if(totalMtasEnergy.at(0) < SC)
		//    plot(MTAS_POSITION_ENERGY+261, totalMtasEnergy.at(0), cycleTime);
				
		if(isBetaSignal) 
		{
			//3100 - 3146 even, B-gated
			for(int i=0; i<24; i++)
				plot(MTAS_POSITION_ENERGY+100+2*i, sumFrontBackEnergy.at(i));
			
			//3350 mtas tot vs I, M, O, B-gated
			for(int i=6; i<24; i++)					
				if(totalMtasEnergy.at(0)<SC && sumFrontBackEnergy.at(i)<SC)
					plot(MTAS_POSITION_ENERGY+350, totalMtasEnergy.at(0), sumFrontBackEnergy.at(i));
				
			//3355 Gamma-Gamma matrix, B-gated I,M,O
			for(unsigned int i=6; i<sumFrontBackEnergy.size(); i++)
			{
				for(unsigned int j=6; j<sumFrontBackEnergy.size(); j++)
					if(i != j && sumFrontBackEnergy.at(i)<SC &&sumFrontBackEnergy.at(j)<SC)
					{
						 plot(MTAS_POSITION_ENERGY+355, sumFrontBackEnergy.at(i), sumFrontBackEnergy.at(j));
						 plot(MTAS_POSITION_ENERGY+355, sumFrontBackEnergy.at(j), sumFrontBackEnergy.at(i));
					}
			}
			
			

			
			//3361
			if(totalMtasEnergy.at(0)<SD && cycleTime < SC)
				plot(MTAS_POSITION_ENERGY+361, totalMtasEnergy.at(0), cycleTime);
		}
		
		/*//silicon spectras
		plot(MTAS_POSITION_ENERGY+500, siliMap.size());
		int siliconNumber;
		//double siliconEnergy;
		for(map<string, struct MtasData>::const_iterator siliMapIt = siliMap.begin(); siliMapIt != siliMap.end(); siliMapIt++)
		{
			siliconNumber = 0;
			if((*siliMapIt).first[2] == 'B')//bottom - channel from 9 to 15
				siliconNumber = 8;
			siliconNumber += (*siliMapIt).first[1]-48;//48 - position of '0' character in Ascii Table
			plot(MTAS_POSITION_ENERGY+510, siliconNumber);
			plot(MTAS_POSITION_ENERGY+271, siliconNumber, cycleNumber);// Jan 03 2011
			//siliconEnergy = (*siliMapIt).second.energy;
			//plot(MTAS_POSITION_ENERGY+520, siliconNumber, siliconEnergy);
		}*/
		
	}  
	
    if(isMeasureOn && !isLightPulserOn && !isTapeMoveOn)
	{
  	    //silicon spectras
		plot(MTAS_POSITION_ENERGY+500, siliMap.size());
		int siliconNumber;
		//double siliconEnergy;
		for(map<string, struct MtasData>::const_iterator siliMapIt = siliMap.begin(); siliMapIt != siliMap.end(); siliMapIt++)
		{
			siliconNumber = 0;
			if((*siliMapIt).first[2] == 'B')//bottom - channel from 9 to 15
				siliconNumber = 8;
			siliconNumber += (*siliMapIt).first[1]-48;//48 - position of '0' character in Ascii Table
			plot(MTAS_POSITION_ENERGY+510, siliconNumber);
			plot(MTAS_POSITION_ENERGY+271, siliconNumber, cycleNumber);// Jan 03 2011
			//siliconEnergy = (*siliMapIt).second.energy;
			//plot(MTAS_POSITION_ENERGY+520, siliconNumber, siliconEnergy);
		}
	}
  
	//logic signals
	if (logicSignalsValue > 0)
	{
	    //plot(MTAS_POSITION_ENERGY+272, cycleLogiTime, cycleNumber);//, logicSignalsValue);///////
	    incplot(MTAS_POSITION_ENERGY+272, cycleLogiTime, cycleNumber, logicSignalsValue);
	    /*cout << " C: " << cycleNumber<< " l: " <<
	       	 isTriggerOnSignal << isIrradOnSignal << isIrradOffSignal << isLightPulserOnSignal <<
	         isLightPulserOffSignal << isTapeMoveOnSignal << isTapeMoveOffSignal << 
		 isBkgOnSignal << isBkgOffSignal << isMeasureOnSignal << isMeasureOffSignal <<
		" t: " << cycleLogiTime << endl;   */
	    //cout << " C: " << cycleNumber << " t: " << cycleLogiTime << " v: " <<logicSignalsValue << endl;
	}

	// K. C. Goetz for March 2015 Experiment: reference crystal vs time in 1 minute intervals
	// Histogram # 5100
	plot(MTAS_REFERENCE_EVO,totalMtasEnergy.at(5),(actualTime - firstTime)/60); 

	EndProcess(); // update the processing time
    
	return true;
}

/*
int DetectorDriver::PlotRaw(const ChanEvent *chan) const
{
    int id = chan->GetID();
    float energy = chan->GetEnergy();
    
    plot(dammIds::misc::offsets::D_RAW_ENERGY + id, energy);
    
    return 0;
}
*/

MtasProcessor::MtasData::MtasData(ChanEvent *chan)
{
    detSubtype	= chan->GetChanID().GetSubtype();
    energy			= chan->GetEnergy();
    calEnergy		= chan->GetCalEnergy();
    time				= chan->GetTime();
    location		= chan->GetChanID().GetLocation();
}

bool MtasProcessor::isTapeMoveOn = false;
bool MtasProcessor::isMeasureOn = true;
bool MtasProcessor::isBkgOn = false;
bool MtasProcessor::isLightPulserOn = false;
bool MtasProcessor::isIrradOn = false;
//bool MtasProcessor::isIrradOn = false;
double MtasProcessor::measureOnTime = -1; 
unsigned MtasProcessor::cycleNumber = 0;
