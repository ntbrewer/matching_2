#ifndef __DAMM_PLOTIDS_H_
#define __DAMM_PLOTIDS_H_ 1

const int GENERIC_CHANNEL = 10;

namespace dammIds {
    namespace ge { // in GeProcessor.cpp
	const unsigned int MAX_CLOVERS = 4; // for *_DETX spectra

	const int D_CLOVER_ENERGY_DETX        = 691; // for x detectors
	const int D_CLOVER_ENERGY_ALL         = 695;
	namespace decayGated {
	    // 0-5 granularities (1 us -> 100 ms)
	    const int DD_ENERGY__DECAY_TIME_GRANX = 705; 
	}
    }
    // in mcp.cpp
    namespace mcp {	
	const int D_POSX   = 1921;
	const int D_POSY   = 1922;
	const int DD_POSXY = 1923;
	}

    // in GeProcessor.cpp
    namespace ge {
	const int D_ENERGY = 1500;
	namespace betaGated {
	    const int D_ENERGY        = 1501;
	    const int D_ENERGY_BETA0  = 1550;
	    const int D_ENERGY_BETA1  = 1551;
	    const int D_ENERGY_NTOF1  = 1552;
	    const int D_ENERGY_NTOF2  = 1553;
	    const int D_ENERGY_HEN3   = 1554;
	    const int D_TDIFF         = 1602;
	    const int DD_TDIFF__GAMMA_ENERGY = 1603;
	    const int DD_TDIFF__BETA_ENERGY  = 1604;
	}
	// correlated with a decay
	namespace decayGated {
	    const int D_ENERGY = 1502;
	}
	namespace implantGated {
	    const int D_ENERGY = 1503;
	}
	const int D_ENERGY_NTOF1 = 1504;
	const int D_ENERGY_NTOF2 = 1505;
	const int D_ENERGY_HEN3  = 1506;
	namespace matrix {
	    const int DD_ENERGY_PROMPT = 1600;
	    const int D_TDIFF          = 1601;
	    namespace betaGated {
		const int DD_ENERGY_PROMPT = 1660;
	    }
	    namespace decayGated {
		const int DD_ENERGY_PROMPT = 1670;
	    }
	    const int DD_ADD_ENERGY_PROMPT = 1680;
	    namespace betaGated {
		const int DD_ADD_ENERGY_PROMPT = 1681;
	    }
	    namespace decayGated {
		const int DD_ADD_ENERGY_PROMPT = 1682;
	    }
	}
	const int D_ADD_ENERGY      = 1700;
	const int D_ADD_ENERGY_DETX = 1701; // for x detectors 
	namespace betaGated {
	    const int D_ADD_ENERGY      = 1710;
	    const int D_ADD_ENERGY_DETX = 1711; // for x detectors
	}
	namespace decayGated {
	    const int D_ADD_ENERGY      = 1720;
	    const int D_ADD_ENERGY_DETX = 1721; // for x detectors
	}
	const int D_MULT = 1800;
    } // end namespace ge

    // in ScintProcessor.cpp
    namespace scint {
	namespace neutr {
	    namespace betaGated {
		const int D_ENERGY_DETX = 1900; // for 3 detectors (1-3)
	    }
	    namespace gammaGated {
		const int D_ENERGY_DETX = 1903; // for 3 detectors (1-3)
	    }
	    namespace betaGammaGated {
		const int D_ENERGY_DETX = 1906; // for 3 detectors (1-3)
	    }
	} // neutr namespace
    } // scint namespace
    namespace vandle{ //in VandleProcessor.cpp
	const int D_PROBLEMS  = 2600;
	const int DD_PROBLEMSQDC = 2601;

	//HISTOGRAMS FOR INDIVIDUAL ENDS OF BARS
	const int DD_TQDCBARS         = 2000;
	const int DD_MAXIMUMBARS      = 2001;
	const int DD_TIMEDIFFBARS     = 2002;
	const int DD_TOFBARS          = 2003;
	const int DD_CORTOFBARS       = 2004;
	const int DD_TQDCBETA         = 2005;
	const int DD_MAXBETA          = 2006;

	//HISTOGRAMS FOR EACH BAR
	const int DD_TQDCAVEVSTDIFF   = 2010;

	//HISTOGRAMS FOR EACH BAR W.R.T. EACH TRIGGER
	const int DD_TDIFFVSTOF       = 2030;
	const int DD_MAXRVSTOF        = 2070;
	const int DD_MAXLVSTOF        = 2110;
	const int DD_MAXBETA1VSTOF    = 2150;
	const int DD_MAXBETA2VSTOF    = 2190;
	const int DD_TQDCAVEVSTOF     = 2230;

	const int DD_TDIFFVSCORTOF    = 2270;
	const int DD_MAXRVSCORTOF     = 2310;
	const int DD_MAXLVSCORTOF     = 2350;
	const int DD_MAXBETA1VSCORTOF = 2390;
	const int DD_MAXBETA2VSCORTOF = 2430;
	const int DD_TDIFFVSCORCORTOF = 2470;
	const int DD_TQDCAVEVSCORTOF  = 2510;
    }//vandle namespace
    namespace waveformprocessor{
	const int D_DISCRIM     = 5100;
	const int DD_NGVSE      = 5101;
    } // waveformprocessor namespace
    namespace pulserprocessor{ //in PulserProcessor.cpp 
	const int D_TIMEDIFF     = 5000;
	const int D_PROBLEMSTUFF = 5001;

	const int DD_QDC         = 5002;
	const int DD_MAX         = 5003;
	const int DD_PVSP        = 5004;
	const int DD_MAXVSTDIFF  = 5005;
	const int DD_QDCVSMAX    = 5006;
	const int DD_AMPMAPSTART = 5007;
	const int DD_AMPMAPSTOP  = 5008;
    } // pulser namespace
    // in correlator.cpp
    namespace correlator {
	const int D_CONDITION = 6000;
	const int D_TIME_BW_IMPLANTS = 6001;
    } // correlator namespace
    // in dssd_sub.cpp
    namespace dssd {
	const int DD_IMPLANT_POSITION = 725;
	const int DD_DECAY_POSITION   = 726;
	const int DD_IMPLANT_FRONT_ENERGY__POSITION = 741;
	const int DD_IMPLANT_BACK_ENERGY__POSITION  = 742;
	const int DD_DECAY_FRONT_ENERGY__POSITION   = 743;
	const int DD_DECAY_BACK_ENERGY__POSITION    = 744;

	const int DD_ENERGY__DECAY_TIME_GRANX = 750;
    } // dssd namespace
    // in dssd_sub.cpp
    namespace ssd {
	const int SSD1_POSITION_ENERGY = 2701;
	const int SSD2_POSITION_ENERGY = 2702;
	const int SSD3_POSITION_ENERGY = 2703;
    } // ssd namespace
    namespace thermometerprocessor {
        const int THERM_AMP = 2800;
        const int THERM_DT = 2801;
        const int THERM_AVG_MV = 2802;
        const int THERM_TEMP = 2803;
        const int THERM_LOWE = 2804;
        const int THERM_HIGHE = 2805;
        const int THERM_AVG = 2806;
        const int THERM_AMP_V_DT = 2807;
        const int THERM_AMP_V_T = 2808;
    }
    namespace mtas {
	const int MTAS_POSITION_ENERGY = 3000;
	const int MTAS_TOTAL_ENERGY = 3100;
	const int MTAS_EVO_NOLOGIC = 3600;
	const int MTAS_LIGHTPULSER_EVO = 3700;
	const int MTAS_REFERENCE_EVO = 3800;
	//For MtasMuonProcessor 1D (4000-4299) 
	const int MTAS_TOTAL = 4000;
	const int MTAS_CENTRAL = 4001;
	const int MTAS_INNER = 4002;
	const int MTAS_MIDDLE = 4003;
	const int MTAS_OUTER = 4004;
	const int MTAS_TOTAL_CR = 4005;
	const int MTAS_CENTRAL_CR = 4006;
	const int MTAS_INNER_CR = 4007;
	const int MTAS_MIDDLE_CR = 4008;
	const int MTAS_OUTER_CR = 4009;
	const int MTAS_TOTAL_CORR = 4010;
	const int MTAS_CENTRAL_CORR = 4011;
	const int MTAS_INNER_CORR = 4012;
	const int MTAS_MIDDLE_CORR = 4013;
	const int MTAS_OUTER_CORR = 4014;
	const int MTAS_TOTAL_CR_CORR = 4015;
	const int MTAS_CENTRAL_CR_CORR = 4016;
	const int MTAS_INNER_CR_CORR = 4017;
	const int MTAS_MIDDLE_CR_CORR = 4018;
	const int MTAS_OUTER_CR_CORR = 4019;
	const int MTAS_TOTAL_AFTER = 4020;
	const int MTAS_CENTRAL_AFTER = 4021;
	const int MTAS_INNER_AFTER = 4022;
	const int MTAS_MIDDLE_AFTER = 4023;
	const int MTAS_OUTER_AFTER = 4024;
	const int MTAS_TOTAL_CR_AFTER = 4025;
	const int MTAS_CENTRAL_CR_AFTER = 4026;
	const int MTAS_INNER_CR_AFTER = 4027;
	const int MTAS_MIDDLE_CR_AFTER = 4028;
	const int MTAS_OUTER_CR_AFTER = 4029;
	const int MTAS_TOTAL_NEAT = 4030;
	const int MTAS_CENTRAL_NEAT = 4031;
	const int MTAS_INNER_NEAT = 4032;
	const int MTAS_MIDDLE_NEAT = 4033;
	const int MTAS_OUTER_NEAT = 4034;
	const int MTAS_TOTAL_CR_NEAT = 4035;
	const int MTAS_CENTRAL_CR_NEAT = 4036;
	const int MTAS_INNER_CR_NEAT = 4037;
	const int MTAS_MIDDLE_CR_NEAT = 4038;
	const int MTAS_OUTER_CR_NEAT = 4039;
	const int MTAS_MULT = 4050;

	//For MtasMuonProcessor 2D 4300-4499
	const int MTAS_E_AFTER_V_T = 4300;
	const int MTAS_E_AFTER_V_T_CR = 4301;
	const int MTAS_MCUT_E_AFTER_V_T = 4302;
	const int MTAS_MCUT_E_AFTER_V_T_CR = 4303;
	const int MTAS_E_NEAT_V_T = 4304;
	const int MTAS_E_NEAT_V_T_CR = 4305;
	const int MTAS_LOC_V_E_AFTER = 4306;

	const int MTAS_TMULT_V_E = 4310;
	const int MTAS_PREV_TMULT_V_E = 4311;
	const int MTAS_CURR_IMULT_V_E = 4312;
	const int MTAS_CURR_MOMULT_V_E = 4313;
	const int MTAS_CURR_IDE_V_E = 4314;

    } // mtas namespace


    // in MtcProcessor.cpp
    namespace mtc {
	const int D_TDIFF0        = 1510;
	const int D_TDIFF1        = 1511;
	const int D_TDIFFSUM      = 1512;
	const int D_MOVETIME      = 1513;
	const int D_COUNTER       = 1520;
	const int D_COUNTER_MOVE0 = 1521;
	const int D_COUNTER_MOVE1 = 1522;
    } // mtc namespace
    namespace trace {
// in trace.cpp
	const int DD_TRACE            = 900;
    const int DD_RTRACE           = 910;
	const int DD_FILTER1          = 851;
	const int DD_FILTER2          = 852;
	const int DD_FILTER3          = 853;
	const int DD_AVERAGE_TRACE    = 854;
	const int D_ENERGY1           = 865;
	const int D_ENERGY2           = 866;	
	const int DD_DOUBLE_TRACE     = 870;
	const int DD_ENERGY2__TDIFF   = 871;
	const int DD_ENERGY2__ENERGY1 = 872;
    const int DD_ENERGY1__TIME    = 873;
    const int DD_TRACEMAX_TQDC    = 874;
    const int DD_TRACEMAX_TQDC2   = 875;
    } // trace namespace
    namespace misc {
// in detector_driver.cpp
	const int D_HAS_TRACE = 800;
	const int D_SAT_ID = 801;
  const int D_NOTSAT_ID = 802;
	
// in pixie_std.cpp
	const int D_HIT_SPECTRUM = 1000;
	const int D_SUBEVENT_GAP = 1001;
	const int D_EVENT_LENGTH = 1002;
	const int D_EVENT_GAP    = 1003;
	const int D_EVENT_MULTIPLICITY = 1004;
	const int D_BUFFER_END_TIME    = 1005;
	const int DD_RUNTIME_SEC       = 1006;
	const int DD_DEAD_TIME_CUMUL   = 1007; // not currently used
	const int DD_BUFFER_START_TIME = 1008;
	const int DD_RUNTIME_MSEC      = 1010;
	const int D_NUMBER_OF_EVENTS   = 1011;
	namespace offsets {
	    const int D_RAW_ENERGY  = 100;
	    const int D_SCALAR      = 300;
	    const int D_TIME        = 400;
	    const int D_CAL_ENERGY  = 500;
	    const int D_FRONT_STRIP = 600;
	    const int D_BACK_STRIP  = 650;
	}
    } // misc namespace
}

// function prototypes
/*!
  plotting routine to interact with DAMM
*/
void plot(int dammid, double val1, double val2 = -1, double val3 = -1,
	  const char* name="h");

void incplot(int dammid, double val1, double val2 = -1, double val3 = -1,
	  const char* name="h");

// miscellaneous damm fortran functions
extern "C" bool bantesti_(const int &, const int &, const int &);
extern "C" void count1cc_(const int &, const int &, const int &);
extern "C" void set2cc_(const int &, const int &, const int &, const int &);
extern "C" void inc2cc_(const int &, const int &, const int &, const int &);

// c++ wrapper function for hd* calls
void DeclareHistogram1D(int dammId, int xSize, const char* title,
			int halfWordsPerChan, int xHistLength,
			int xLow, int xHigh);
void DeclareHistogram1D(int dammId, int xSize, const char* title,
			int halfWordsPerChan = 2);
void DeclareHistogram2D(int dammId, int xSize, int ySize,
			const char *title, int halfWordsPerChan,
			int xHistLength, int xLow, int xHigh,
			int yHistLength, int yLow, int yHigh);
void DeclareHistogram2D(int dammId, int xSize, int ySize,
			const char* title, int halfWordsPerChan = 1);

// powers of 2 for damm sizes
extern const int S1, S2, S3, S4, S5, S6, S7, S8, S9, SA, SB, SC, SD, SE, SF;

#endif // __DAMM_PLOTIDS_H_
