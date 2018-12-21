/** \file StatsData.cpp
 *  \brief Stores statistics data from the data stream in its original format
 */

#include <iostream>

#include <cmath>
#include <cstring>

#include "param.h"
#include "RawEvent.h"

#include "pixie16app_defs.h"

StatsData stats;

using std::cout;
using std::endl;

using pixie::word_t;

/** Clear the statistics data structures */
StatsData::StatsData()
{
  cout << "Allocating " << sizeof(oldData) + sizeof(data) 
       << " bytes for statistics data" << endl;
  bzero(oldData, sizeof(oldData));
  bzero(data, sizeof(data));
}

/** Copy the statistics data from the data stream to a memory block,
 *   preserving a copy of the old statistics data so that the incremental
 *   change can be determined */
 
void StatsData::DoStatisticsBlock(word_t *buf, int vsn)
{
  if (memcmp(data[vsn], buf, sizeof(word_t)*statSize) != 0) {
    memcpy(oldData[vsn], data[vsn], sizeof(word_t)*statSize);
    memcpy(data[vsn], buf, sizeof(word_t)*statSize);    
  }
}

/** Return the most recent statistics live time for a given id */
double StatsData::GetCurrTime(unsigned int id) const
{
  // from Pixie15DSP_r15428.var  
  const size_t offset = 0x4a340;
  const size_t ltPosHi = 0x4a37f - offset;
  const size_t ltPosLo = 0x4a38f - offset;

  int mod = id / NUMBER_OF_CHANNELS;
  int ch  = id % NUMBER_OF_CHANNELS;

  double d;

  d = data[mod][ltPosHi + ch] * pow(2.0, 32);
  d += data[mod][ltPosLo + ch];

  d *= 16.0 * 1.0e-6 / SYSTEM_CLOCK_MHZ;

  return d;
}

/** Return the change in the number of fast peaks between the two most 
 *  recent statistics blocks for a given id */
double StatsData::GetDiffPeaks(unsigned int id) const
{
  // from Pixie16DSP_r15428.var
  const size_t offset = 0x4a340;
  const size_t peaksPosHi = 0x4a39f - offset;
  const size_t peaksPosLo = 0x4a3af - offset;

  int mod = id / NUMBER_OF_CHANNELS;
  int ch  = id % NUMBER_OF_CHANNELS;

  double d;

  d = data[mod][peaksPosHi + ch] - oldData[mod][peaksPosHi + ch];
  d *= pow(2.0, 32);
  d += data[mod][peaksPosLo + ch] - oldData[mod][peaksPosLo + ch];

  return d;
}

/** Return the elapsed live time between the two most recent statistics 
 *  blocks for a given channel */
double StatsData::GetDiffTime(unsigned int id) const
{
  // from Pixie15DSP_r15428.var  
  const size_t offset = 0x4a340;
  const size_t ltPosHi = 0x4a37f - offset;
  const size_t ltPosLo = 0x4a38f - offset;

  int mod = id / NUMBER_OF_CHANNELS;
  int ch  = id % NUMBER_OF_CHANNELS;

  double d;

  d = data[mod][ltPosHi + ch] - oldData[mod][ltPosHi + ch];
  d *= pow(2.0, 32);
  d += data[mod][ltPosLo + ch] - oldData[mod][ltPosLo + ch];

  d *= 16.0 * 1.0e-6 / SYSTEM_CLOCK_MHZ;

  return d;
}
