/*! \file MtasMuonProcessor.cpp
 *
 * The MTAS processor handles detectors of type mtas  */

#include "damm_plotids.h"
#include "param.h"
#include "MtasMuonProcessor.h"
#include "DetectorDriver.h"
#include "RawEvent.h"
#include <limits>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <math.h>

 
using std::cout;
using std::endl;
using std::vector;
using std::string;

MtasMuonProcessor::MtasMuonProcessor() :
  EventProcessor(), mtasSummary(NULL) 
{
  firstTime = -1.0;
    name = "mtas";
    associatedTypes.insert("mtas");
}

void MtasMuonProcessor::DeclarePlots(void) const
{
	using namespace dammIds::mtas;//MTAS_MUON
    
	const int EnergyBins = SE; 
	//QR:SE=16384,SB=2048,S8=256,S6=64,S3=8

	//TAS spectras	DeclareHistogram1D(MTAS_MUON, , " ");
	DeclareHistogram1D(MTAS_MULT,S6,"MTAS Multiplicity");

	//2D spectras 	DeclareHistogram2D(MTAS_MUON, , , " ");
}

bool MtasMuonProcessor::Process(RawEvent &event)
{
	using namespace dammIds::mtas;
	if (!EventProcessor::Process(event))
	    return false;

	// first time through, grab the according detector summaries
	if (mtasSummary == NULL)
		mtasSummary = event.GetSummary("mtas");
	vector<ChanEvent*> mtasList= mtasSummary->GetList();
	int mtasMult=mtasSummary->GetMult();

	for(vector<ChanEvent*>::const_iterator it = mtasList.begin(); it != mtasList.end(); it++)
	{
	   double energy = ((*it)-> GetEnergy());
	   int location = (*it)->GetChanID().GetLocation();
	   string subtype = (*it)->GetChanID().GetSubtype();
	   int hasSat=0;
	   
	}



	EndProcess(); // update the processing time
    
	return true;
}


