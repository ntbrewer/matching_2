#!/bin/bash

# this loop will toggle the Central Module HV script ON/OFF 
# for puposes of testing pixie channel burnout.
# NTBREWER 9-4-2015

CNT=50
until [ $CNT -lt 0 ]; do
    ./HV_MTAS_ALL_C.sh ON
    sleep 120 
    ./HV_MTAS_ALL_C.sh OFF
    sleep 120
    let CNT-=1
done 
