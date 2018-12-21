/** \file DssdProcessor.h
 *
 * Header file for DSSD analysis
 */

#ifndef __DSSD_PROCESSOR_H_
#define __DSSD_PROCESSOR_H_

#include "EventProcessor.h"

class DetectorSummary;
class RawEvent;

/**
 * \brief Handles detectors of type dssd_front and dssd_back
 */
class DssdProcessor : public EventProcessor 
{
 private:
    DetectorSummary *frontSummary; ///< all detectors of type dssd_front
    DetectorSummary *backSummary;  ///< all detectors of type dssd_back

    static const double cutoffEnergy; ///< cutoff energy for implants versus decays
 public:
    DssdProcessor(); // no virtual c'tors
    virtual void DeclarePlots(void) const;
    virtual bool Process(RawEvent &event);
};

#endif // __DSSD_POCESSOR_H_
