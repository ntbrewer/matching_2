/** \file ThermProcessor.h
 * 
 * Class for handling Thermometer Signals
 */

#ifndef __THERMPROCESSOR_H_
#define __THERMPROCESSOR_H_

#include <map>
#include <string>

#include "EventProcessor.h"
#include "RawEvent.h"

class ThermometerProcessor : public EventProcessor
{
 public:
    ThermometerProcessor(); // no virtual c'tors
    virtual void DeclarePlots(void) const;
    virtual bool Process(RawEvent &event);

 private:
    struct ThermData
    {
	ThermData(std::string type ="");
	ThermData(ChanEvent *chan);

	std::string detSubtype;
	double energy;
	double calEnergy;
	double time;
	unsigned int location;
    };
    
    virtual bool RetrieveData(RawEvent &event);
    virtual void AnalyzeData(void);

    std::map<std::string, struct ThermData> ThermometerMap;
}; //Class ThermProcessor
#endif // __ThermPROCESSOR_H_
