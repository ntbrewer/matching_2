/*! \file EventProcessor.h
 * 
 * Generic event processor which particular processing 
 * of individual devices can be derived from
 */

#ifndef __EVENTPROCESSOR_H_
#define __EVENTPROCESSOR_H_

#include <map>
#include <set>
#include <string>

#include <sys/times.h>

// forward declarations
class DetectorDriver;
class DetectorSummary;
class RawEvent;

#ifdef useroot
class TTree;
#endif

class EventProcessor {
 private:
    // things associated with timing
    tms tmsBegin;
    double userTime;
    double systemTime;
    double clocksPerSecond;


 protected:
    // define the associated detector types and only initialize if present
    std::string name;
    std::set<std::string> associatedTypes;    
    bool initDone;
    bool didProcess;
    // map of associated detector summary
    std::map<std::string, const DetectorSummary *> sumMap;

 public:
    EventProcessor();
    virtual ~EventProcessor();

    // declare associated damm plots (called by drrsub_)
    virtual void DeclarePlots(void) const;
    virtual const std::set<std::string>& GetTypes(void) const {
      return associatedTypes; 
    }
    virtual bool DidProcess(void) const {
      return didProcess;
    }
    // return true on success
    virtual bool HasEvent(void) const;
    virtual bool Init(DetectorDriver &driver);
    virtual bool PreProcess(RawEvent &event);
    virtual bool Process(RawEvent &event);   
    void EndProcess(void); // stop the process timer

#ifdef useroot
    virtual bool AddBranch(TTree *tree);
    virtual void FillBranch(void);
#endif
};

#endif // __EVENTPROCESSOR_H_
