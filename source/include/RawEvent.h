/**   \file RawEvent.h
 *    \brief Header file for the rawevent
 *
 *    The variables and structures contained in a raw event
 *
 *    SNL - 7-2-07
 */

#ifndef __RAWEVENT_H_
#define __RAWEVENT_H_ 1

#include <vector>
#include <string>
#include <map>
#include <set>

#include "Correlator.h"

#include "pixie16app_defs.h"
#include "param.h"

using std::map;
using std::set;
using std::string;
using std::vector;

/**
 * \brief Channel identification

 * All parameters needed to uniquely specify the detector connected to a
 * specific channel are set here.  A vector of identifiers for all channels is
 * created on initialization in InitID().  Each event that is created in
 * ScanList() in PixieStd.cpp has its channel identifier attached to it in
 * the variable chanID inside RawEvent
 *
 * Identifier is a class that will contain basic channel information that
 * will not change including the damm spectrum number where the raw energies
 * will be plotted, the detector type and subtype, and the detector's physical
 * location (strip number, detector location, ...)
 */

class Identifier {
 private:
    string type;      /**< Specifies the detector type */
    string subtype;   /**< Specifies the detector sub type */
    int dammID;       /**< Damm spectrum number for plotting calibrated energies */
    int location;     /**< Specifies the real world location of the channel.
			 For the DSSD this variable is the strip number */
		int modNum;       /**< Specifies the Pixie Module for the channel*/
 public:
    void SetDammID(int a)     {dammID = a;}   /**< Set the dammid */
    void SetType(string a)    {type = a;}     /**< Set the detector type */
    void SetSubtype(string a) {subtype = a;}  /**< Set the detector subtype */
    void SetLocation(int a)   {location = a;} /**< Set the detector location */
    void SetModNum(int a)     {modNum = a;}    /**< Set the detector module number*/
    
    int GetDammID() const     {return dammID;}   /**< Get the dammid */
    string GetType() const    {return type;}     /**< Get the detector type */
    string GetSubtype() const {return subtype;}  /**< Get the detector subtype */
    int GetLocation() const   {return location;} /**< Get the detector location */
    int GetModNum() const {return modNum;}    /**< Set the detector module number*/

    Identifier();
    void Zero();

    bool operator==(const Identifier &x) const {
	return (type == x.type &&
		subtype == x.subtype &&
		location == x.location);
    } /**< Compare this identifier with another */
};

/**
 * \brief A channel event
 * 
 * All data is grouped together into channels.  For each pixie16 channel that
 * fires the energy, time (both trigger time and event time), and trace (if
 * applicable) are obtained.  Additional information includes the channels 
 * identifier, calibrated energies, trace analysis information.
*/
class ChanEvent {
 private:
    double energy;             /**< Raw channel energy */
    double calEnergy;          /**< Calibrated channel energy,
				  calibration performed in ThreshAndCal
				  function in the detector_driver.cpp */
    double calTime;            /**< Calibrated time, currently unused */
    vector<double> traceInfo;  /**< Values from trace analysis functions */
    vector<int> trace;         /**< Channel trace if present */
    unsigned long trigTime;    /**< The channel trigger time, trigger time and the lower 32 bits
				     of the event time are not necessarily the same but could be
				     separated by a constant value.*/
    unsigned long cfdTime;     /**< CFD trigger time in units of 1/256 pixie clock ticks */
    unsigned long eventTimeLo; /**< Lower 32 bits of pixie16 event time */
    unsigned long eventTimeHi; /**< Upper 32 bits of pixie16 event time */
    unsigned long runTime0;    /**< Lower bits of run time */
    unsigned long runTime1;    /**< Upper bits of run time */
    unsigned long runTime2;    /**< Higher bits of run time */

    double time;               /**< Raw channel time, 64 bit from pixie16 channel event time */
    int    modNum;             /**< Module number */
    int    chanNum;            /**< Channel number */

    bool   virtualChannel;     /**< Flagged if generated virtually in Pixie DSP */
    bool   pileupBit;          /**< Pile-up flag from Pixie */
    bool   saturatedBit;       /**< Saturation flag from Pixie */
    double kTemperature;
    void ZeroNums(void);       /**< Zero members which do not have constructors associated with them */
    
    // make the front end responsible for reading the data able to set the channel data directly
    friend int ReadBuffData(unsigned int *, unsigned long *, vector<ChanEvent *> &);
 public:
    static const double pixieEnergyContraction; ///< energies from pixie16 are contracted by this number


    void SetEnergy(double a)    {energy = a;}    /**< Set the raw energy in case we want
						    to extract it from the trace ourselves */
    void SetCalEnergy(double a) {calEnergy = a;} /**< Set the calibrated energy */
    void SetTime(double a)      {time = a;}      /**< Set the raw time */
    void SetCalTime(double a)   {calTime = a;}   /**< Set the calibrated time */
    void AddTraceInfo(double a) {traceInfo.push_back(a);} /**< Add one value to the traceinfo */
    vector<double> GetTraceInfo() const {return traceInfo;} 
    double GetEnergy() const      {return energy;}      /**< Get the raw energy */
    double GetCalEnergy() const   {return calEnergy;}   /**< Get the calibrated energy */
    double GetTime() const        {return time;}        /**< Get the raw time */
    double GetCalTime() const     {return calTime;}    /**< Get the calibrated time */
    const vector<int> &GetTraceRef() const {return trace;} /**< Get a reference to the trace */
  /*NTB to be adapted from Trace.hpp 2012
    needs     std::map<std::string, double> doubleTraceData;
    std::map<std::string, int>    intTraceData;
    and #include <map>
    void SetTraceInfo(std::string name, double value) {
        if (traceInfo.count(name) > 0) 
            traceInfo[name] = value;
        else
            InsertTraceInfo(name,value);
    }
    void InsertTraceInfo(std::string name, double value) {
        traceInfo.insert(make_pair(name,value));
    }
    bool HasTraceInfo(std::string name) const {
        return (traceInfo.count(name) > 0 );
    } */

    unsigned long GetTrigTime() const    
	{return trigTime;}    /**< Return the channel trigger time */
    unsigned long GetCfdTime() const    
	{return cfdTime;}    /**< Return the CFD trigger time */
    unsigned long GetEventTimeLo() const
	{return eventTimeLo;} /**< Return the lower 32 bits of event time */
    unsigned long GetEventTimeHi() const
	{return eventTimeHi;} /**< Return the upper 32 bits of event time */
    unsigned long GetRunTime0() const
	{return runTime0;}    /**< Return the lower bits of run time */
    unsigned long GetRunTime1() const
	{return runTime1;}    /**< Return the middle bits of run time */
    unsigned long GetRunTime2() const
	{return runTime2;}    /**< Return the higher bits of run time */
    bool IsPileup() const 
  {return pileupBit;}
    bool IsSaturated() const 
  {return saturatedBit;}

    const Identifier& GetChanID() const; /**< Get the channel identifier */
    int GetID() const;                   /**< Get the channel id defined as
					    pixie module # * 16 + channel number */
    double GetTraceInfo(unsigned int a) const; /**< Get a specific value from the traceinfo */

/****Added for SVP ****/
    double TrcQDC;
    int MaxValue;
    double StdDevBaseline, Phase, AveBaseline; 
    unsigned int MaxPos;
    int tRegion;

    void SetTrcQDC(double a) {TrcQDC=a;}       /**< Set the QDC From Integrating The Trace*/
    void SetMaxValue(double a)  {MaxValue=a;}        /**< Set the Maximum Value from Trace*/
    void SetStdDevBaseline(double a) {StdDevBaseline=a;}       /**< Set the Standard Deviation of the Baseline the Trace*/
    void SetAveBaseline(double a)  {AveBaseline=a;}        /**< Set the Average of the baseline from Trace*/
    void SetPhase(double a)    {Phase=a;}          /**< Set the Phase from Trace*/
    void SetMaxPos(unsigned int a) {MaxPos=a;}           /**< Set the Position of the Maximum from Trace */
    void SetRegion(int region) {tRegion=region;}  /**Set the Position of the Region defined in Trace */ 
    void SetTemperature(double a) {kTemperature=a;} /**Set the Temperature in K defined in ThermometerProcessor */ 
    double GetTrcQDC() const {return TrcQDC;} /**< Return the QDC From Integrating The Trace*/
    int GetMaxValue() const {return MaxValue;}       /**< Return the Maximum Value from Trace*/
    double GetStdDevBaseline() const {return StdDevBaseline;} /**< Return the Standard Deviation from  The Trace*/
    double GetAveBaseline() const {return AveBaseline;}           /**< Return the Average of the Baseline from Trace*/
    double GetPhase()   {return Phase;}       /**< Return the phase from Trace*/
    unsigned int GetMaxPos() const {return MaxPos;} /**< Return position of Maximum in trace */
    int GetRegion() const {return tRegion;}
    double GetTemperature() const {return kTemperature;}
/****END SVP ADDITIONS****/

    ChanEvent();
    void ZeroVar();
};

/** \brief Summary of all channels of one detector type
 * For each group of detectors that exists in the analysis, a detector summary
 * is created.  The detector summary includes the multiplicity, maximum
 * deposited energy, and physical location (strip number, detector number ...)
 * where the maximum energy was deposited for a type of detectors.  Also 
 * pointers to all channels of this type are stored in a vector, as well as the
 * channel event where the maximum energy was deposited.  Lastly, the detector
 * summary records the detector name to which it applies.
 */
class DetectorSummary {
 private:
    string name;                  /**< detector name associated with this summary */
    vector<ChanEvent*> eventList; /**< list of events associated with this detector group */
    ChanEvent* maxEvent;          /**< event with maximum energy deposition */
 public:
    DetectorSummary();
    void Zero();
    void AddEvent(ChanEvent *ev); /**< Add a channel event to the summary */

    void SetName(string a) {name = a;} /**< Set the detector type name */
    
    const ChanEvent* GetMaxEvent(void) const 
	{return maxEvent;}          /**< Get the max event */
    int GetMult() const 
	{return eventList.size();} /**< Get the multiplicity */
    const string& GetName() const          
	{return name;}              /**< Get the detector type name */
    const vector<ChanEvent*>& GetList() const 
	{return eventList;}         /**< Get the list of all channels in the
				       rawevent that are of this detector type */
};

/** \brief The all important raw event
 *
 * The rawevent serves as the basis for the experimental analysis.  The rawevent 
 * includes a vector of individual channels that have been deemed to be close to
 * each other in time.  This determination is performed in ScanList() from
 * PixieStd.cpp.  The rawevent also includes a map of detector summaries which
 * contains a detector summary for each detector type that is used in the analysis
 *
 *  The rawevent is intended to be versatile enough to remain unaltered unless
 * LARGE changes are made to the pixie16 code.  Be careful when altering the
 * rawevent.
 */
class RawEvent {
 private:
    // no private variables at this time    
    set<string> usedDetectors;           /**< list of detectors in the map */
    map<string, DetectorSummary> sumMap; /**< An STL map containing DetectorSummary classes
					     associated with detector types */
    vector<ChanEvent*> eventList;        /**< A vector of pointers to all the channels that are close
					    enough in time to be considered a single event */
    Correlator correlator;               /**< class to correlate decay data with implantation data */
 public:   
    RawEvent();
    void Clear(void);
    size_t Size(void) const;
    void Init(const set<string> &, const set<string> &);
    void AddChan(ChanEvent* event);       
    void Zero(const set<string> &);

    Correlator &GetCorrelator()
	{return correlator;} /**< get the correlator */
    const set<string>& GetUsedDetectors() const 
	{return usedDetectors;} /**< get the list of detectors in the map */
    DetectorSummary *GetSummary(const string& a);
    const DetectorSummary *GetSummary(const string &a) const;
    const vector<ChanEvent *> &GetEventList(void) const
	{return eventList;} /**< Get the list of events */
};

class StatsData {
 private:
  static const size_t statSize = N_DSP_PAR - DSP_IO_BORDER;
  static const size_t maxVsn = 14;

  pixie::word_t oldData[maxVsn][statSize]; /**< Older statistics data to calculate the change in statistics */
  pixie::word_t data[maxVsn][statSize];    /**< Statistics data from each module */
 public:
  static const pixie::word_t headerLength = 1;

  StatsData(void);
  void DoStatisticsBlock(pixie::word_t *buf, int vsn);

  double GetCurrTime(unsigned int id) const;
  double GetDiffPeaks(unsigned int id) const;
  double GetDiffTime(unsigned int id) const;
};

#endif // __RAWEVENT_H_
