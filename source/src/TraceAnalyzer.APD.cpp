/**    \file TraceAnalyzer.cpp
 *     \brief defines the Trace class.
 *
 *     Implements a quick online trapezoidal filtering mechanism
 *     for the identification of double pulses
 *
 *     - SNL - 7-2-07 - created
 *     - SNL - 2-4-08 - Add plotting spectra
 */

#include <algorithm>
#include <fstream>
#include <iostream>
#include <numeric>
#include <vector>

#include <cstdlib>

#include "damm_plotids.h"
#include "RandomPool.h"
#include "RawEvent.h"
#include "StatsAccumulator.h"
#include "TraceAnalyzer.h"
#include <math.h>

using namespace std;

// external pool of random numbers defined in RandomPool.cpp
extern RandomPool randoms;

const string TraceAnalyzer::defaultFilterFile="filter.txt";

/**
 * Initialize the trace analysis class.  Set the row numbers
 * for spectra 850 and 870 to zero and reserve enough memory
 * for the filter vectors. This also reads in the filter parameters from
 * the filter file in the following order:
 *    fast rise, fast gap, fast threshold
 *    energy rise, energy gap
 *    3rd filter rise, 3rd filter gap, 3rd filter threshold
 */
int TraceAnalyzer::Init(const string &filterFile)
{   
    const int maxTraceLength = 6400;

    rownum850 = 0;
    rownum870 = 0;

    // reserves enough space in the vector to contain a reasonable sized trace
    // This improves the performance of the program as a whole since memory
    // does not need to be reallocated every time the size is increased.

    average.reserve(maxTraceLength);
    fastFilter.reserve(maxTraceLength);
    energyFilter.reserve(maxTraceLength);
    thirdFilter.reserve(maxTraceLength);
    flt.reserve(maxTraceLength);

    // read in the filter parameters
    ifstream in(filterFile.c_str());
    if (!in) {
      cout << "failed to open the filter parameter file" << endl;
      cout << "  using default values" << endl;
      return 1;
    }

    while (!in.eof()) {
      if ( isdigit(in.peek()) ) {
	in >> fastRise >> fastGap >> fastThresh;
	in >> slowRise1 >> slowGap1;
	in >> slowRise2 >> slowGap2 >> slowThresh;
	cout << "Trace analysis parameters are: "
	     << "  \nFast filter: " << fastRise << " " << fastGap << " " << fastThresh
	     << "  \nEnergy filter: " << slowRise1 << " " << slowGap1
	     << "  \n3rd filter: " << slowRise2 << " " << slowGap2 << slowThresh
	     << endl;
	// scale thresholds by the length of integration (i.e. rise time)
	fastThresh *= fastRise;
	slowThresh *= slowRise2;
	break;
      } else {
	// assume this is a comment
	in.ignore(1000, '\n');
      }
    }
    if (in.fail()) {
      cout << "problem reading filter parameters file" << endl;
      return 1;
    }

    return 0;
}

/**
 * Set default filter parameters
 */
TraceAnalyzer::TraceAnalyzer() : 
    userTime(0.), systemTime(0.)
{
  //  clocksPerSecond = sysconf(_SC_CLK_TCK);
    fastRise = fastGap = 5;
    slowRise1 = slowGap1 = 100;
    //? these are some legacy values, are they appropriate
    slowRise2 = 10; 
    slowGap2 = 20;

    fastThresh = fastRise * 3;
    slowThresh = slowRise2 * 2;
}

/** Output time processing traces */
TraceAnalyzer::~TraceAnalyzer() 
{
    cout << "Trace processor : " 
	 << userTime << " user time, "
	 << systemTime << " system time" << endl;
}

/**
 * Function to quickly analyze a trace online.  Trapezoidal filters
 * are run across the trace with varying parameters to extract E1
 * and E2 values and time differences
 *
 * This routine detects when the fast filter crosses its threshold,
 * verifies that the slower 3rd filter also is above its threshold,
 * and then samples the energy filter during its flattop.
 * detType and detSubtype not currently used
 */
int TraceAnalyzer::Analyze(const vector<int> &trace, 
			   const string &detType, const string &detSubtype)
{
    tms tmsBegin;

    times(&tmsBegin); // begin timing process
    
    average.clear();
    fastFilter.clear();
    energyFilter.clear();
    thirdFilter.clear();

    // quick trace analysis adapted from previous scan versions in
    // the xia_trace99.f file.
    const int baseLow = 5, baseHigh = 10;
    
    StatsAccumulator stats = accumulate
	(&trace[baseLow], &trace[baseHigh], StatsAccumulator());
    double basel  = stats.GetMean();
    // double sbasel = stats.GetStdDev();
    // sigma not used

    // make a trace of the running average
    const int averageLen = 10;
    // a lot of repeated arithmetic here too
    for(int i = 0; i < (signed)(trace.size()-averageLen); i++) {
      stats = accumulate
	(&trace[i], &trace[i+averageLen], StatsAccumulator());
      average.push_back(stats.GetMean() - basel);
    }
    
    // determine trace filters, these are trapezoidal filters
    // characterized by a risetime and gaptime and a range of
    // the filter from lo to hi.
    // filter(trace, lo, hi, gaptime, risetime);
    int high = trace.size();

    int fastSize = 2 * fastRise + fastGap;
    FilterFill(trace, fastFilter, fastSize, high, fastGap, fastRise);

    int slowSize1 = 2 * slowRise1 + slowGap1;
    FilterFill(trace, energyFilter, slowSize1, high, slowGap1, slowRise1);
    //Calculate Trace Maximum and Average Baseline for PSA
    CalculateTraceMaxAndAvgBaseline(trace, energyFilter, slowSize1, high, slowGap1, slowRise1);

    int slowSize2 = 2 * slowRise2 + slowGap2;
    FilterFill(trace, thirdFilter, slowSize2, high, slowGap2, slowRise2);

    size_t sample; // point at which to sample the slow trace
    // find the point at which the trace crosses the threshold
    t1 = -1;
    e1 = 0;
    
    vector<int>::iterator iThr  = fastFilter.begin() + baseHigh - fastSize;
    vector<int>::iterator iHigh = fastFilter.end(); 

    while (iThr < iHigh) {
      iThr = find_if(iThr, iHigh, bind2nd(greater<int>(), fastThresh));
      // check that the correlated energy is sufficiently high
      t1 = iThr - fastFilter.begin();
      sample = t1 + (slowRise2 + slowGap2 / 2) - (fastRise + fastGap / 2);
      if (sample < thirdFilter.size() && thirdFilter[sample] > slowThresh) {
	sample = t1 + (slowRise1 + slowGap1 / 2) - (fastRise + fastGap / 2);
	e1 = energyFilter[sample] + randoms.Get();
	// scale to the integration time
	e1 /= slowRise1; 
      }
      // find the trailing edge
      iThr += fastGap;
      iThr = find_if(iThr, iHigh, bind2nd(less<int>(), fastThresh));
      iThr += fastSize;
    }
    
    
    // find a second crossing point
    t2 = -1;
    e2 = 0;
  
    if ( t1 != -1 && iThr < fastFilter.end() ) {
      while (iThr != iHigh) {
	iThr = find_if(iThr, iHigh, bind2nd(greater<int>(), fastThresh));
	if (iThr != iHigh) {
	  t2 = iThr - fastFilter.begin();
	  sample = t2 + (slowRise2 + slowGap2 / 2) - (fastRise + fastGap / 2);
	  if (sample < thirdFilter.size() && 
	      thirdFilter[sample] - thirdFilter[t2 - fastSize] > slowThresh) {
	    sample = t2 + (slowRise1 + slowGap1 / 2) - (fastRise + fastGap / 2);
	    e2 = energyFilter[sample] - energyFilter[t2 - fastSize];
	    e2 += randoms.Get();
	    // scale to the integration time
	    e2 /= slowRise1;
	  }
	}
      }
    }

  TracePlot(trace);
  
  tms tmsEnd;
  times(&tmsEnd);
  userTime += (tmsEnd.tms_utime - tmsBegin.tms_utime) / clocksPerSecond;
  systemTime += (tmsEnd.tms_stime - tmsBegin.tms_stime) / clocksPerSecond; 

  return(0);
}

/**
 * Defines how to implement a trapezoidal filter characterized by two
 * moving sum windows of width risetime separated by a length gaptime.
 * Filter is calculated from channels lo to hi.
 */
vector<int> TraceAnalyzer::Filter(vector<int> &trace, int lo, int hi, 
			  int gapTime, int riseTime){
  flt.clear();

  if (lo != 0) {
      for(int i = 0; i < lo; i++) {
	  flt.push_back(0); 
      }
  }

  // repeated arithmetic here too
  for(int i = lo; i < hi; i++){
    int leftSum  = 0;
    int rightSum = 0;
    
    for (int j = i - 2*riseTime - gapTime; j < i - riseTime - gapTime; j++) {
	leftSum += trace[j];
    }

    for (int j = i - riseTime; j < i; j++) {
	rightSum += trace[j];
    }

    flt.push_back(rightSum - leftSum);
  }

  return flt;
}

/**
 * same thing as Filter() but fill the storage directly without passing 
 * through the temporary return value
 */
void TraceAnalyzer::FilterFill(const vector<int> &trace, vector<int> &res,
			int lo, int hi, int gapTime, int riseTime){
    res.assign(lo, 0);
    int leftSum = 0;
    int rightSum = 0;
    for (int i = lo; i < hi; i++) {
	    leftSum = accumulate(&trace[i - 2*riseTime - gapTime],
				 &trace[i - riseTime - gapTime], 0);
	    rightSum = accumulate(&trace[i - riseTime], &trace[i], 0);
	res.push_back(rightSum - leftSum);
    }

}

/**
 * same thing as Filter() but fill the storage directly without passing 
 * through the temporary return value
 */
void TraceAnalyzer::CalculateTraceMaxAndAvgBaseline(const vector<int> &trace, vector<int> &res,
			int lo, int hi, int gapTime, int riseTime){
    res.assign(lo, 0);
	avgBaseline = accumulate(&trace[lo - 2*riseTime - gapTime],
				 &trace[lo - riseTime - gapTime], 0);
	avgBaseline /= riseTime; 
    
    traceMax = ( int ) trace[lo];
    for (int i = lo; i < hi; i++) 
    {
      if (trace[i] > traceMax) 
      {
          traceMax = trace[i];
      }
    }
    
}

/** declare the damm plots */
void TraceAnalyzer::DeclarePlots() const
{
    using namespace dammIds::trace;

    const int traceBins = S8;
    const int numTraces = SE;
    const int numOther = S8;
    const int energyBins = SC;
    const int energyBins2 = SA;
    const int timeBins = S8;
    const int numTracePlots = 1;//Matt** Starting at 900 make this number of plots, up to 10
    //Matt** use with caution(Makes Big Files)

    for (int i=0; i<numTracePlots; i++) 
    {
        DeclareHistogram2D(DD_TRACE+i, traceBins, numTraces, "traces data");
    }
    DeclareHistogram2D(DD_FILTER1, traceBins, numOther, "fast filter Offset 2k");    
    DeclareHistogram2D(DD_FILTER2, traceBins, numOther, "energy filter Offset 2k");
    DeclareHistogram2D(DD_FILTER3, traceBins, numOther, "3rd filter Offset 2k");
    DeclareHistogram2D(DD_AVERAGE_TRACE, traceBins, numOther, "trace average");

    DeclareHistogram1D(D_ENERGY1, energyBins, "E1 from traces");    
    DeclareHistogram1D(D_ENERGY2, energyBins, "E2 from traces");

    DeclareHistogram2D(DD_DOUBLE_TRACE, traceBins, numOther, "double traces");
    DeclareHistogram2D(DD_ENERGY2__TDIFF, energyBins2, timeBins, "E2 vs DT", 2);
    DeclareHistogram2D(DD_ENERGY1__TIME, energyBins2, timeBins, "E1 vs T", 2);
    DeclareHistogram2D(DD_ENERGY2__ENERGY1, energyBins2, energyBins2, "E2 vs E1", 2);
    DeclareHistogram2D(DD_TRACEMAX_TQDC, SA, SA, "E1 vs. E1/(TraceMax-<BL>) *100", 2);
    DeclareHistogram2D(DD_RTRACE+1, traceBins, numTraces, "Region 1 Traces");
    DeclareHistogram2D(DD_RTRACE+2, traceBins, numTraces, "Region 2 Traces");
    DeclareHistogram2D(DD_RTRACE+3, traceBins, numTraces, "Region 3 Traces");


    
    //DeclareHistogram2D(DD_TRACEMAX_TQDC2, S9, SA, "E1 vs. 10*(TraceMax-<BL>)/E1 ", 2);
}

/**
 * From the trace analysis plot the damm spectra.
 *   including the first few traces of the analysis
 *   and their associated filters.
 *   as well as  E2 v E1 and E2 v time difference plots with
 *   varying conditions for double pulses.
 */
void TraceAnalyzer::TracePlot(const vector<int> &trace)
			      
{
    using namespace dammIds::trace;
    static int r1Cnt=-1;
    static int r2Cnt=-1;
    static int r3Cnt=-1;
    int rCnt=-1;
    if (traceMax > avgBaseline) 
    {
        double ratio = (double) (1000*e1 / (traceMax-avgBaseline) );
        plot(DD_TRACEMAX_TQDC, e1 , ratio );
        
        if (ratio > (7.3 * e1 - 11.17)) //Region I noise
        {
            region=1;
            r1Cnt++;
            rCnt=r1Cnt;
        } else if ( e1 < 100 && ratio < (7.3 *e1 - 11.17)) //Region II low E xray
        {
            region=2;
            r2Cnt++;
            rCnt=r2Cnt;
        } else //Region III Xrays and Betas
        {
            region=3;
            r3Cnt++;
            rCnt=r3Cnt;
        }

    }
    // plot trace and associated filters and derived energies
    for(unsigned int i = 0; i < trace.size(); i++){
      if(i < average.size() )
	plot(DD_AVERAGE_TRACE,i,rownum850,average[i]);
      if(i < thirdFilter.size() ) 
	plot(DD_FILTER3,i,rownum850,abs(thirdFilter[i]+2000));
      if(i < energyFilter.size() ) 
	plot(DD_FILTER2,i,rownum850,abs(energyFilter[i]+2000));
      if( i < fastFilter.size() )
	plot(DD_FILTER1,i,rownum850,abs(fastFilter[i]+2000));
     
      plot(DD_TRACE+rownum850/16384,i,rownum850,trace[i]);
    if (traceMax > avgBaseline) 
    {
        plot(DD_RTRACE+region,i,rCnt,trace[i]);
    }
    }
    
   
    plot(DD_TRACE,trace.size()+2,rownum850,e1);
    plot(DD_TRACE,trace.size()+3,rownum850,t1);
    if(e2>0)
      plot(DD_TRACE,trace.size()+4,rownum850,e2);
    if(t2>0)
      plot(DD_TRACE,trace.size()+5,rownum850,t2);
    rownum850++;
    
    // plot double pulse E and T spectra
    plot(D_ENERGY1,e1);
    if(e2 > 0) {
      plot(D_ENERGY2,e2);
      plot(DD_ENERGY2__TDIFF,e2,t2-t1);
      plot(DD_ENERGY2__ENERGY1,e2,e1);

      for (size_t i = 0; i < trace.size(); i++) {
	plot(DD_DOUBLE_TRACE,i,rownum870,trace[i]);
      }
      plot(DD_DOUBLE_TRACE,trace.size()+10,rownum870,e1);
      plot(DD_DOUBLE_TRACE,trace.size()+11,rownum870,t1);
      plot(DD_DOUBLE_TRACE,trace.size()+12,rownum870,e2);
      plot(DD_DOUBLE_TRACE,trace.size()+13,rownum870,t2);
	    
      rownum870++;    
    }
}
