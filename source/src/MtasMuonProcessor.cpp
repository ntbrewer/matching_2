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
#include <list>

 
using std::cout;
using std::endl;
using std::vector;
using std::string;
using std::pair;
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

	//TAS spectras	DeclareHistogram1D(, , " ");
	DeclareHistogram1D(MTAS_TOTAL,EnergyBins, "Mtas Total ");
	DeclareHistogram1D(MTAS_CENTRAL,EnergyBins, "Mtas Central ");
	DeclareHistogram1D(MTAS_INNER,EnergyBins, "Mtas Inner ");
	DeclareHistogram1D(MTAS_MIDDLE,EnergyBins, "Mtas Middle ");
	DeclareHistogram1D(MTAS_OUTER,EnergyBins, "Mtas Outer ");
	DeclareHistogram1D(MTAS_TOTAL_CR,EnergyBins, "Mtas Total crunch E/10 (~100 kev/ch)");
	DeclareHistogram1D(MTAS_CENTRAL_CR,EnergyBins, "Mtas Central crunch E/10 (~100 kev/ch) ");
	DeclareHistogram1D(MTAS_INNER_CR,EnergyBins, "Mtas Inner crunch E/10 (~100 kev/ch)");
	DeclareHistogram1D(MTAS_MIDDLE_CR,EnergyBins, "Mtas Middle crunch E/10 (~100 kev/ch)");
	DeclareHistogram1D(MTAS_OUTER_CR,EnergyBins, "Mtas Outer crunch E/10 (~100 kev/ch)");
	DeclareHistogram1D(MTAS_TOTAL_CORR,EnergyBins, "Corr. Mtas Total ");
	DeclareHistogram1D(MTAS_CENTRAL_CORR,EnergyBins, "Corr. Mtas Central ");
	DeclareHistogram1D(MTAS_INNER_CORR,EnergyBins, "Corr. Mtas Inner ");
	DeclareHistogram1D(MTAS_MIDDLE_CORR,EnergyBins, "Corr. Mtas Middle ");
	DeclareHistogram1D(MTAS_OUTER_CORR,EnergyBins, "Corr. Mtas Outer ");
	DeclareHistogram1D(MTAS_TOTAL_CR_CORR,EnergyBins, "Corr. Mtas Total crunch E/10 (~100 kev/ch)");
	DeclareHistogram1D(MTAS_CENTRAL_CR_CORR,EnergyBins, "Corr. Mtas Central crunch E/10 (~100 kev/ch)");
	DeclareHistogram1D(MTAS_INNER_CR_CORR, EnergyBins, "Corr. Mtas Inner crunch E/10 (~100 kev/ch)");
	DeclareHistogram1D(MTAS_MIDDLE_CR_CORR,EnergyBins, "Corr. Mtas Middle crunch E/10 (~100 kev/ch)");
	DeclareHistogram1D(MTAS_OUTER_CR_CORR,EnergyBins, "Corr. Mtas Outer crunch E/10 (~100 kev/ch)");

	DeclareHistogram1D(MTAS_TOTAL_AFTER,EnergyBins, "Mtas Total After HE ");
	DeclareHistogram1D(MTAS_CENTRAL_AFTER,EnergyBins, "Mtas Central After HE");
	DeclareHistogram1D(MTAS_INNER_AFTER,EnergyBins, "Mtas Inner After HE");
	DeclareHistogram1D(MTAS_MIDDLE_AFTER,EnergyBins, "Mtas Middle After HE");
	DeclareHistogram1D(MTAS_OUTER_AFTER,EnergyBins, "Mtas Outer After HE");
	DeclareHistogram1D(MTAS_TOTAL_CR_AFTER,EnergyBins, "Mtas Total After HE (~100 kev/ch) ");
	DeclareHistogram1D(MTAS_CENTRAL_CR_AFTER,EnergyBins, "Mtas Central After HE (~100 kev/ch)");
	DeclareHistogram1D(MTAS_INNER_CR_AFTER, EnergyBins, "Mtas Inner After HE (~100 kev/ch)");
	DeclareHistogram1D(MTAS_MIDDLE_CR_AFTER,EnergyBins, "Mtas Middle After HE (~100 kev/ch)");
	DeclareHistogram1D(MTAS_OUTER_CR_AFTER,EnergyBins, "Mtas Outer After HE (~100 kev/ch)");

	DeclareHistogram1D(MTAS_TOTAL_NEAT,EnergyBins, "Mtas Total Interesting ");
	DeclareHistogram1D(MTAS_CENTRAL_NEAT,EnergyBins, "Mtas Central Interesting");
	DeclareHistogram1D(MTAS_INNER_NEAT,EnergyBins, "Mtas Inner Interesting");
	DeclareHistogram1D(MTAS_MIDDLE_NEAT,EnergyBins, "Mtas Middle Interesting");
	DeclareHistogram1D(MTAS_OUTER_NEAT,EnergyBins, "Mtas Outer Interesting");
	DeclareHistogram1D(MTAS_TOTAL_CR_NEAT,EnergyBins, "Mtas Total Interesting (~100 kev/ch) ");
	DeclareHistogram1D(MTAS_CENTRAL_CR_NEAT,EnergyBins, "Mtas Central Interesting (~100 kev/ch)");
	DeclareHistogram1D(MTAS_INNER_CR_NEAT, EnergyBins, "Mtas Inner Interesting (~100 kev/ch)");
	DeclareHistogram1D(MTAS_MIDDLE_CR_NEAT,EnergyBins, "Mtas Middle Interesting (~100 kev/ch)");
	DeclareHistogram1D(MTAS_OUTER_CR_NEAT,EnergyBins, "Mtas Outer Interesting (~100 kev/ch)");

	DeclareHistogram1D(MTAS_MULT,S6,"MTAS Multiplicity");

	//2D spectras 	DeclareHistogram2D(, , , " ");
	DeclareHistogram2D(MTAS_E_AFTER_V_T,SE,SB, "MTAS Energy v time after HE (10KeV/ch vs ms) ");
	DeclareHistogram2D(MTAS_E_AFTER_V_T_CR,SE,SB, "MTAS Energy v time after HE (100KeV/ch vs ms) ");
	DeclareHistogram2D(MTAS_MCUT_E_AFTER_V_T,SE,SB, "MTAS Mult.>2 Energy v time after HE (10KeV/ch vs ms) ");
	DeclareHistogram2D(MTAS_MCUT_E_AFTER_V_T_CR,SE,SB, "MTAS Mult.>2 Energy v time after HE (100KeV/ch vs ms) ");
	DeclareHistogram2D(MTAS_E_NEAT_V_T,SE,SB, "MTAS Energy v time Interesting (10KeV/ch vs ms) ");
	DeclareHistogram2D(MTAS_E_NEAT_V_T_CR,SE,SB, "MTAS Energy v time Interesting (100KeV/ch vs ms) ");
	DeclareHistogram2D(MTAS_LOC_V_E_AFTER, EnergyBins,S6, " MTAS Energy per Module after HE");

	DeclareHistogram2D(MTAS_TMULT_V_E,EnergyBins , S5, " MTAS Total Mult vs. Total Energy");
	DeclareHistogram2D(MTAS_PREV_TMULT_V_E,EnergyBins , S5, " MTAS Total Prev. Mult vs. Total Prev. Energy/10");
	DeclareHistogram2D(MTAS_CURR_IMULT_V_E,EnergyBins ,S3 , " MTAS Inner Mult vs. Total Energy");
	DeclareHistogram2D(MTAS_CURR_MOMULT_V_E,EnergyBins ,S4 , " MTAS MO Mult vs. Total Energy ");
	DeclareHistogram2D(MTAS_CURR_IDE_V_E,EnergyBins ,SD , " MTAS dE(LR) when 'Muon' IMult=1");

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
        vector<MtasData> mtasFList;
        vector<MtasData> mtasBList;
	
	
	int mtasMult=mtasSummary->GetMult();
	plot(MTAS_MULT, mtasMult);
	//double maxLocation =0; 
	int numCpmts = 0;
	double lasttime =-1.;
	static double previousHighEtime =-1.0;
	static double currentHighEtime =-1.0;
	static double previousHighE =-1.0;
	static double currentHighE =-1.0;
	vector <double> totalMtasEnergy (5,-1);
	vector <double> corrMtasEnergy (5,-1);
	vector <double> corrMtasChanEnergy (49,-1);
	static int prevTMult=0, currTMult=0; 
	int IsInteresting = 0;
	static int prevIMult=0, currIMult=0; 
	static int prevMOMult=0,currMOMult=0;
	double currIdE=0;
        FBEventsMatch_.clear();
	//Preprocess events, make detector pairs and sums of rings.
	if (currentHighE > 1000. ) 
 	{	
	    previousHighEtime=currentHighEtime;
	    previousHighE=currentHighE;
	    prevMOMult=currMOMult;
	    prevIMult=currIMult;
	    prevTMult=currTMult;
	    if ( currIMult>2 && currMOMult==1) //currMOMult<3 && currMOMult>0
	    {
	    IsInteresting=1;	
	    }
	} else 
	{
	    previousHighEtime=0.;
	    previousHighE=0.;
	    prevMOMult=-1;
	    prevIMult=-1;
	    prevTMult=-1;
	}

 	currIMult=0;
	currMOMult=0;
	currTMult=0;
	for(vector<ChanEvent*>::const_iterator it = mtasList.begin(); it != mtasList.end(); it++)
	{
		string subtype = (*it)->GetChanID().GetSubtype();
                double energy = ((*it)-> GetCalEnergy());
	        int loc= (*it)->GetChanID().GetLocation();
	        int hasSat=(*it)->IsSaturated();
            const vector<int> tr=(*it)->GetTraceRef();
    /*        if (tr.size()!=0) {
                cout << tr.size() <<endl;
            }
            */
		int pileup=(*it)->IsPileup();
		double time= (*it)->GetTime()* pixie::clockInSeconds;
		if (time > lasttime ) 
		{
		lasttime=time;
		}
        /*static int pilecnt=0;
        if (pileup>0) {
            pilecnt+=1;
            if (pilecnt%100==0||energy>10) {
                cout << pilecnt << " " << energy << " dt:" <<  time - previousHighEtime << endl;
            }
        }*/
    	if (hasSat>0 || energy > 31500) //pileup >0
		{
		    continue;
		}
	        MtasData ev(subtype, energy, (*it)-> GetCalEnergy(), time,
			   loc, hasSat, pileup );
		if(subtype[0] =='C')
		    numCpmts++;
		if (energy== 0)
			continue;
		if (subtype[2]=='F')		
		    mtasFList.push_back(ev);
		if (subtype[2]=='B')
		    mtasBList.push_back(ev);

		if (subtype[0]=='C'){
		  totalMtasEnergy.at(1) += energy/12.0;
		  totalMtasEnergy.at(0) += energy/12.0;
		}
	    if (subtype[0]=='I') {
		  totalMtasEnergy.at(2) += energy/2.0;
		  totalMtasEnergy.at(0) += energy/2.0;
		}
	    if (subtype[0]=='M') {
		  totalMtasEnergy.at(3) += energy/2.0;
		  totalMtasEnergy.at(0) += energy/2.0;
		}
	    if (subtype[0]=='O') {
		  totalMtasEnergy.at(4) += energy/2.0;
		  totalMtasEnergy.at(0) += energy/2.0;
		}
	}

	plot(MTAS_TOTAL,totalMtasEnergy.at(0));
	plot(MTAS_CENTRAL,totalMtasEnergy.at(1));
	plot(MTAS_INNER,totalMtasEnergy.at(2));
	plot(MTAS_MIDDLE,totalMtasEnergy.at(3));
	plot(MTAS_OUTER,totalMtasEnergy.at(4));
	plot(MTAS_TOTAL_CR,totalMtasEnergy.at(0)/10);
	plot(MTAS_CENTRAL_CR,totalMtasEnergy.at(1)/10);
	plot(MTAS_INNER_CR,totalMtasEnergy.at(2)/10);
	plot(MTAS_MIDDLE_CR,totalMtasEnergy.at(3)/10);
	plot(MTAS_OUTER_CR,totalMtasEnergy.at(4)/10);

	//PreProcess: Make a map of pairs of pmts.
	for(vector<MtasData>::const_iterator fit = mtasFList.begin(); fit != mtasFList.end(); fit++)
	{
	    string subtypeF = (*fit).st;
	    string modF (subtypeF, 0 ,2 );

	    for(vector<MtasData>::const_iterator bit = mtasBList.begin(); bit != mtasBList.end(); bit++)
	    {
	      string subtypeB = (*bit).st;
	      string modB (subtypeB, 0 ,2 );
	      if (modF==modB) {
		    corrMtasChanEnergy.at((*fit).loc)=(*fit).calE;
		    corrMtasChanEnergy.at((*bit).loc)=(*bit).calE;
		    if (subtypeB[0]=='C'){
		      corrMtasEnergy.at(1) += ((*fit).calE + (*bit).calE)/numCpmts;
		      corrMtasEnergy.at(0) += ((*fit).calE + (*bit).calE)/numCpmts;
		    } 
	        if (subtypeB[0]=='I') {
		      corrMtasEnergy.at(2) += ((*fit).calE + (*bit).calE)/2.0;
		      corrMtasEnergy.at(0) += ((*fit).calE + (*bit).calE)/2.0;
		      currTMult+=1;
		      currIMult+=1;
		      currIdE=((*fit).calE-(*bit).calE);
		    }
	        if (subtypeB[0]=='M') {
		       corrMtasEnergy.at(3) += ((*fit).calE + (*bit).calE)/2.0;
		       corrMtasEnergy.at(0) += ((*fit).calE + (*bit).calE)/2.0;
		       currMOMult+=1;
		       currTMult+=1;
		    }
	        if (subtypeB[0]=='O') {
		      corrMtasEnergy.at(4) += ((*fit).calE + (*bit).calE)/2.0;
		      corrMtasEnergy.at(0) += ((*fit).calE + (*bit).calE)/2.0;
  		      currMOMult+=1;
		      currTMult+=1;
		    }
		    pair<MtasData, MtasData> match((*fit),(*bit));
		    FBEventsMatch_.push_back(match);

	      }

	    }
	}
	if (corrMtasEnergy.at(1)>0.) 
	{
	    currTMult+=1;
	}
	currentHighEtime=lasttime;
	currentHighE=corrMtasEnergy.at(0);

	plot(MTAS_TOTAL_CORR,corrMtasEnergy.at(0));
	plot(MTAS_CENTRAL_CORR,corrMtasEnergy.at(1));
	plot(MTAS_INNER_CORR,corrMtasEnergy.at(2));
	plot(MTAS_MIDDLE_CORR,corrMtasEnergy.at(3));
	plot(MTAS_OUTER_CORR,corrMtasEnergy.at(4));
	plot(MTAS_TOTAL_CR_CORR,corrMtasEnergy.at(0)/10);
	plot(MTAS_CENTRAL_CR_CORR,corrMtasEnergy.at(1)/10);
	plot(MTAS_INNER_CR_CORR,corrMtasEnergy.at(2)/10);
	plot(MTAS_MIDDLE_CR_CORR,corrMtasEnergy.at(3)/10);
	plot(MTAS_OUTER_CR_CORR,corrMtasEnergy.at(4)/10);

	plot(MTAS_PREV_TMULT_V_E,previousHighE/10,prevTMult);
	//Process Loop
	   /*for(vector< pair<MtasData,MtasData> >:: iterator mit = FBEventsMatch_.begin(); mit != FBEventsMatch_.end(); mit++)
	{
	    double energy=0.;
	    char ring = (*mit).first.st[0];
	    
	    if (ring=='C') {
		energy=((*mit).first.E+(*mit).second.E)/numCpmts;
	    } else {
		energy=((*mit).first.E+(*mit).second.E)/2.;
	    }
	

	    
	    
	}*/
	double dt = currentHighEtime-previousHighEtime;
	if (dt>0 && dt < (20.48)*1e-6 )
	{
	    plot(MTAS_TOTAL_AFTER,corrMtasEnergy.at(0));
	    plot(MTAS_CENTRAL_AFTER,corrMtasEnergy.at(1));
	    plot(MTAS_INNER_AFTER,corrMtasEnergy.at(2));
	    plot(MTAS_MIDDLE_AFTER,corrMtasEnergy.at(3));
	    plot(MTAS_OUTER_AFTER,corrMtasEnergy.at(4));
	    plot(MTAS_TOTAL_CR_AFTER,corrMtasEnergy.at(0)/10.);
	    plot(MTAS_CENTRAL_CR_AFTER,corrMtasEnergy.at(1)/10.);
	    plot(MTAS_INNER_CR_AFTER,corrMtasEnergy.at(2)/10.);
	    plot(MTAS_MIDDLE_CR_AFTER,corrMtasEnergy.at(3)/10.);
	    plot(MTAS_OUTER_CR_AFTER,corrMtasEnergy.at(4)/10.);
	    //2d plotting
	    plot(MTAS_E_AFTER_V_T, corrMtasEnergy.at(0), dt*1e8);
	    plot(MTAS_E_AFTER_V_T_CR, corrMtasEnergy.at(0)/10, dt*1e8);
	    if (currTMult>2) 
	    {
		plot(MTAS_MCUT_E_AFTER_V_T, corrMtasEnergy.at(0), dt*1e8);
		plot(MTAS_MCUT_E_AFTER_V_T_CR, corrMtasEnergy.at(0)/10, dt*1e8);
	    }
	    /*for ( int id=1; id!=49; id++) //Calibrations
	    {
	    	plot(MTAS_LOC_V_E_AFTER, corrMtasChanEnergy.at(id),id);
	    }*/
	    if (IsInteresting>0  && currTMult>2) 
	    {
		plot(MTAS_TOTAL_NEAT,corrMtasEnergy.at(0));
		plot(MTAS_CENTRAL_NEAT,corrMtasEnergy.at(1));
		plot(MTAS_INNER_NEAT,corrMtasEnergy.at(2));
		plot(MTAS_MIDDLE_NEAT,corrMtasEnergy.at(3));
		plot(MTAS_OUTER_NEAT,corrMtasEnergy.at(4));

		//2d plotting
		plot(MTAS_E_NEAT_V_T, corrMtasEnergy.at(0), dt*1e8);
		plot(MTAS_E_NEAT_V_T_CR, corrMtasEnergy.at(0)/10, dt*1e8);

		}
		if (IsInteresting>0) 
	    {
	    	plot(MTAS_TOTAL_CR_NEAT,corrMtasEnergy.at(0)/10.);
		    plot(MTAS_CENTRAL_CR_NEAT,corrMtasEnergy.at(1)/10.);
	    	plot(MTAS_INNER_CR_NEAT,corrMtasEnergy.at(2)/10.);
	     	plot(MTAS_MIDDLE_CR_NEAT,corrMtasEnergy.at(3)/10.);
    		plot(MTAS_OUTER_CR_NEAT,corrMtasEnergy.at(4)/10.);
	        plot(MTAS_TMULT_V_E,corrMtasEnergy.at(0),currTMult);
    		plot(MTAS_CURR_IMULT_V_E,corrMtasEnergy.at(0),currIMult);
            plot(MTAS_CURR_MOMULT_V_E,corrMtasEnergy.at(0),currMOMult);	    
	    
		    if (currIMult == 1) {
		        plot(MTAS_CURR_IDE_V_E,corrMtasEnergy.at(0),currIdE+4000);
		    }
		}
	}

	EndProcess(); // update the processing time
    
	return true;
}
