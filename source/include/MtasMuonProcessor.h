/** \file MtasMuonProcessor.h
 *
 * Header file for analysis of muons in Mtas
 */

#ifndef __MTAS_MUON_PROCESSOR_H_
#define __MTAS_MUON_PROCESSOR_H_

#include "EventProcessor.h"
#include <vector>

class DetectorSummary;
class RawEvent;
class ChanEvent;


//extern std::vector<int> emptyTrace;

class MtasMuonProcessor : public EventProcessor 
{
	private:
		DetectorSummary *mtasSummary; 
                static double measureOnTime;
                double firstTime;
	public:
		MtasMuonProcessor(); 
		virtual void DeclarePlots(void) const;
		virtual bool Process(RawEvent &event);
	public:
	    struct MtasData{
	        MtasData() {
		    st = " ";
	            E = 0.0;
	            calE = 0.0;
		    loc = -1;
	            sat = 0;
	            pile = 0;
		//    tr = emptyTrace;
	        }

		MtasData(std::string subtype, double energy,double Calenergy, double time, int location,
                   int saturated, int pileup) { //, std::vector<int> trace) {	
	            st = subtype;
		    E = energy;
		    calE = Calenergy;
        	    t = time;
	            loc = location;
        	    sat = saturated;
		    pile = pileup;
		    //tr = trace;
		}

		std::string st;
		double E;
		double calE;
		double t;
		int loc;
		int sat;
		int pile;
		//std::vector<int> tr;
	    }; 
	    std::vector<std::pair<MtasData, MtasData> > FBEventsMatch_; 
/*struct MtasData 
		{
			MtasData(ChanEvent *chan);
			
			std::string detSubtype;
			double energy;
			double calEnergy;
			double time;
			double location;
			int saturation;
			int pileup;
			std::vector<int> trace;
    		};*/
};

#endif // __MTAS_MUON_PROCESSOR_H_
