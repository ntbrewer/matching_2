/**   \file TraceAnalyzer.h
 *    \brief Header file for the TraceAnalyzer class
 *
 *    SNL - 7-2-07
 */

#ifndef __TRACEANALYZER_H_
#define __TRACEANALYZER_H_

#include <string>
#include <vector>

#include <sys/times.h>

using std::string;
using std::vector;


/** \brief quick online trace analysis
 *
 * Trace class implements a quick online trapezoidal fiter for the
 * identification of double pulses with relatively little computation.
 */

class TraceAnalyzer {
 private:
    // things associated with timing
    double userTime;          ///< user time used by this class
    double systemTime;        ///< system time used by this class
    double clocksPerSecond;   ///< frequency of system clock

    vector<double> average;   ///< trace average
    vector<int> fastFilter;   ///< fast filter of trace
    vector<int> energyFilter; ///< energy filter of trace
    vector<int> thirdFilter;  /*< third filter of trace, used as a second
			       * threshold check */

    vector<int> flt;         ///< vector used in filter function
        
    int t1;                  ///< time of E1 pulse
    int t2;                  ///< time of E2 pulse
    double e1;               ///< energy of E1 pulse
    double e2;               ///< energy of E2 pulse

    int rownum850;           ///< rownumber of DAMM spectrum 850
    int rownum870;           ///< rownumber of DAMM spectrum 870

    int fastRise;            ///< rise time of fast filter (in samples)
    int slowRise1;           ///< rise time of energy filter (in samples)
    int slowRise2;           ///< rise time of slow threshold filter (in samples)
    int fastGap;             ///< gap time of fast filter (in samples)
    int slowGap1;            ///< gap time of energy filter (in samples)
    int slowGap2;            ///< gap time of slow threshold filter (in samples)
    int fastThresh;          ///< threshold of fast filter
    int slowThresh;          ///< threshold of slow filter
    double avgBaseline;      ///< Avg. Baseline of the trace for PSA
    double stdDevBaseline;   ///< Variance of Baseline of the trace for PSA
    int traceMax;            ///< Maximum Value of the trace
    int region;              ///< Region of Trace analysis FOM
    /** default filename containing filter parameters
     */
    static const std::string defaultFilterFile;
 public:
    int Init(const std::string &filterFile=defaultFilterFile);
    void DeclarePlots(void) const;
    int Analyze(const vector<int> &, const string &, const string &);
    vector<int> Filter(vector<int> &, int , int , int , int );
    void FilterFill(const vector<int> &, vector<int> &, int, int, int, int);
    void CalculateTraceMaxAndAvgBaseline(const vector<int> &, vector<int> &, int, int, int, int);
    void TracePlot(const vector<int> &);

    int GetTime(void) const {return t1;}
    int GetSecondTime(void) const {return t2;}
    double GetEnergy(void) const {return e1;}
    double GetAvgBaseline(void) const {return avgBaseline;}
    double GetStdDevBaseline(void) const {return stdDevBaseline;}
    int GetTraceMax(void) const {return traceMax;}
    int GetRegion(void) const {return region;}
    double GetSecondEnergy(void) const {return e2;}

    TraceAnalyzer();
    ~TraceAnalyzer();
};

#endif // __TRACEANALYZER_H_
