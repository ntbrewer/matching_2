/*!
  \file ReadBuffData.RevD.fip32495.cpp
  Read Buffer Data program for the following configuration 
  from pixie16_revd_general_release_08292015
  Jun 15  2015 fippixie16_revdgeneral_r32495.bin
  Aug 19  2015 syspixie16_revdgeneral_r33157.bin
  Id:
  utilities.c 29501
  pixie16app.c 32533
  communication.c 27938
  2cm24c64.c 15626
  pixie16sys.c 27118
  tools.c 28166

  \brief retrieve data from raw buffer array ibuf
*/

/*----------------------------------------------------------------------
 * Copyright (c) 2005, XIA LLC
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, 
 * with or without modification, are permitted provided 
 * that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above 
 *     copyright notice, this list of conditions and the 
 *     following disclaimer.
 *   * Redistributions in binary form must reproduce the 
 *     above copyright notice, this list of conditions and the 
 *     following disclaimer in the documentation and/or other 
 *     materials provided with the distribution.
 *   * Neither the name of XIA LLC nor the names of its
 *     contributors may be used to endorse or promote
 *     products derived from this software without 
 *     specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND 
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
 * IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE 
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON 
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR 
 * TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF 
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF 
 * SUCH DAMAGE.
 *----------------------------------------------------------------------*/

#include <iostream>
#include <vector>

#include <cmath>

// data related to pixie packet structure
#include "pixie16app_defs.h"

// our event structure
#include "param.h"
#include "RawEvent.h"

using pixie::word_t;
using pixie::halfword_t;
using std::cout;
using std::endl;

extern StatsData stats;

// define tst bit function from pixie16 files
unsigned long TstBit(unsigned short bit, unsigned long value)
{
  return ((value & (unsigned long)(pow(2.0, (double)bit))) >> bit);
}

/*!
  \brief extract channel information from raw data
  
  ReadBuffData extracts channel information from the raw data arrays
  and places it into a structure called evt.  A pointer to each
  of the evt objects is placed in the eventlist vector for later time
  sorting.
*/
int ReadBuffData(word_t *buf, unsigned long *bufLen,
		 vector<ChanEvent*> &eventList)
{						
  // multiplier for high bits of 48-bit time
  static const double HIGH_MULT = pow(2., 32.); 

  word_t modNum;

  unsigned long numEvents = 0;
  word_t *bufStart = buf;

  /* Determine the number of words in the buffer */
  *bufLen = *buf++;

  /* Read the module number */
  modNum = *buf++;

  if( *bufLen > 0 ) { // check if the buffer has data
    if (*bufLen == 2) {
      // this is an empty channel
      return 0;
    }
    do {
      ChanEvent *currentEvt = new ChanEvent;

      // decoding event data... see pixie16app.c
      // buf points to the start of channel data
      word_t chanNum      = (buf[0] & 0x0000000F);
      word_t slotNum      = (buf[0] & 0x000000F0) >> 4;
      word_t crateNum     = (buf[0] & 0x00000F00) >> 8;
      word_t headerLength = (buf[0] & 0x0001F000) >> 12;

      word_t eventLength  = (buf[0] & 0x7FFE0000) >> 17; // Event Length now in [30:17]
      word_t finishCode   = (buf[0] & 0x80000000) >> 31;

      //currentEvt->virtualChannel = ((buf[0] & 0x20000000) != 0);
      //currentEvt->saturatedBit   = ((buf[0] & 0x40000000) != 0);
      currentEvt->pileupBit      = (finishCode != 0); //!!!(NTB) should be modified to match Manual ie Pileup to FinishCode throughout but this is a major job. TBD. 

/*      // MODIFIED to ignore saturated bit (DTM)
      //      word_t eventLength  = (buf[0] & 0x7FFE0000) >> 17;
      word_t eventLength  = (buf[0] & 0x3FFE0000) >> 17;
      word_t finishCode   = (buf[0] & 0x80000000) >> 31;
Remodified (NTB) to use interpret saturated and pileup bits
*/
      // Rev. D header lengths not clearly defined in pixie16app_defs
      //! magic numbers here for now
      // make some sanity checks
      if (headerLength == stats.headerLength) {
	// this is a manual statistics block inserted by the poll program
	stats.DoStatisticsBlock(&buf[1], modNum);
	buf += eventLength;
	numEvents = readbuff::STATS;
	continue;
      }
      if (headerLength != 4 && headerLength != 8 &&
	  headerLength != 12 && headerLength != 16) { // (NTB) not sure if this (8,12,16) is appropriate for this version of the pixie cards. 
	cout << "  Unexpected header length: " << headerLength << endl;
	cout << "    Buffer " << modNum << " of length " << *bufLen << endl;
	cout << "    CHAN:SLOT:CRATE " 
	     << chanNum << ":" << slotNum << ":" << crateNum << endl;
	// advance to next event and continue
	// buf += EventLength;
	// continue;

	// skip the rest of this buffer (nope, NTB)
	return numEvents;
    //(NTB) return readbuff::ERROR;
      }

      word_t lowTime     = buf[1];//ok

      word_t highTime    = buf[2] & 0x0000FFFF;//ok
      word_t cfdTime     = ((buf[2] & 0x7FFF0000) >> 16)/32768; //(NTB)
     //adjusted by xia for 14bit allowance. so our 12bit is written too high. (NTB)
      word_t cfdForcedTrig = (buf[2] & 0x80000000) >> 31; //new NTB

      word_t energy      = buf[3] & 0x00007FFF;//changed NTB
      word_t traceFlag = (buf[3] & 0x00008000) >> 15; //new
      word_t traceLength = (buf[3] & 0xFFFF0000) >> 16; //ok
      word_t energy_alt = buf[3] & 0x0000FFFF; // for comparison to software documentation
      /*if ( chanNum ==15 && modNum==4) {
      //if (traceFlag != 0 && chanNum ==15 && modNum==4) {
        cout << " Energy 15bit = " << energy << " 16bit = " << energy_alt << " ." << endl;
        cout << " traceFlag = " << traceFlag << "." << endl; 
      }*/
      // one last sanity check
      if ( traceLength / 2 + headerLength != eventLength ) {
	cout << "  Bad event length (" << eventLength
	     << ") does not correspond with length of header (" << headerLength
	     << ") and length of trace (" << traceLength << ")" << endl;
	buf += eventLength;
	continue;
      }

      // handle multiple crates
      modNum += 100 * crateNum;
      //TBD pass new vars to currentEvt and process as necessary
      currentEvt->chanNum = chanNum;
      currentEvt->modNum = modNum;
      currentEvt->energy = energy;
      currentEvt->trigTime = lowTime;
      currentEvt->cfdTime  = cfdTime;
      currentEvt->eventTimeHi = highTime;
      currentEvt->eventTimeLo = lowTime;
      currentEvt->time = highTime * HIGH_MULT + lowTime;//should also have + cfdTime??
      //Do I need to check CFDForced = 1 -> cfdTime == 0??
      //For checking syncronization
      /*if (chanNum == 15) {
      cout << "sync check. " << endl;
      cout << "chanNum " << chanNum << " modNum " << modNum << " energy " << energy << " time " << highTime * HIGH_MULT + lowTime << endl;
      }*/
      buf += headerLength;
      /* Check if trace data follows the channel header */
      if ( traceLength > 0 ) {
	// sbuf points to the beginning of trace data
	halfword_t *sbuf = (halfword_t *)buf; 
	currentEvt->trace.reserve(traceLength); //NTB
	
	/*if(currentEvt->saturatedBit)
	  currentEvt->trace.SetTraceInfo("saturation", 1); //NTB*/

	// Read the trace data (2-bytes per sample, i.e. 2 samples per word)
	for(unsigned int k = 0; k < traceLength; k ++) {
	  currentEvt->trace.push_back(sbuf[k]);
	}
	buf += traceLength / 2;
      }
      eventList.push_back(currentEvt);

      numEvents++;
    } while ( buf < bufStart + *bufLen );
  } else {// if buffer has data
    cout << "ERROR BufNData " << *bufLen << endl;
    cout << "ERROR IN ReadBuffData" << endl;
    cout << "LIST UNKNOWN" << endl;
    return readbuff::ERROR;
  }
  
  return numEvents;
}
