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
    virtual bool PreProcess(RawEvent &event);
 private:
    virtual bool AnalyzeData(RawEvent &event);
}; //Class ThermometerProcessor

#endif // __ThermPROCESSOR_H_
