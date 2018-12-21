#!/bin/bash
# Bash script to put all the MTAS HV channels On/OFF
#    	setVoltage   -  voltage to be set
#	setCurrent	-	current limit
#	setRamp		-	ramping speed
#	setStatus	- 	1 ON, 0 OFF
#	ip		- 	IP address of the MTAS MPOD
# based on MPOD example, this script should go through all the channels
#M.Karny Jan,26 2011

#setting variables
ip=192.168.13.237
path=/usr/share/snmp/mibs
#setVoltage=800
setCurrent=0.000500
setStatus=$1
setRamp=10

# checking number of channels in the crate

channelCount=$(snmpget -Oqv -v 2c -M $path -m +WIENER-CRATE-MIB -c guru $ip outputNumber.0)

# checking the names of the channels
indices=$(snmpwalk -Oqv -v 2c -M $path -m +WIENER-CRATE-MIB -c guru $ip outputIndex)
# truncating names so only type U??? are left and putting into x array
x=(`echo $indices | tr ' ' ' '`)
echo $indices | tr ' ' ' '
#LOOP
COUNTER=0
while [ $COUNTER -lt $channelCount ]; do
# looking up the next value in 'x' array
index=$(echo ${x[${COUNTER}]})

# seting up the voltage,limit,rampspeed 
#voltage=$(snmpset -OqvU -v 2c -M $path -m +WIENER-CRATE-MIB -c guru $ip outputVoltage.$index F $setVoltage)
iLimit=$(snmpset -OqvU -v 2c -M $path -m +WIENER-CRATE-MIB -c guru $ip outputCurrent.$index F $setCurrent)
rampspeed=$(snmpset -OqvU -v 2c -M $path -m +WIENER-CRATE-MIB -c guru $ip outputVoltageRiseRate.$index F $setRamp)

#turning ON/OFF $index channel
status=$(snmpset -OqvU -v 2c -M $path -m +WIENER-CRATE-MIB -c guru $ip outputSwitch.$index i $setStatus)

#checking status of the channel reading values into the variables
voltage=$(snmpget -OqvU -v 2c -M $path -m +WIENER-CRATE-MIB -c guru $ip outputVoltage.$index)
iLimit=$(snmpget -OqvU -v 2c -M $path -m +WIENER-CRATE-MIB -c guru $ip outputCurrent.$index)
sense=$(snmpget -OqvU -v 2c -M $path -m +WIENER-CRATE-MIB -c guru $ip outputMeasurementSenseVoltage.$index) 
current=$(snmpget -OqvU -v 2c -M $path -m +WIENER-CRATE-MIB -c guru $ip outputMeasurementCurrent.$index)
rampspeed=$(snmpget -OqvU -v 2c -M $path -m +WIENER-CRATE-MIB -c guru $ip outputVoltageRiseRate.$index)
status=$(snmpget -OqvU -v 2c -M $path -m +WIENER-CRATE-MIB -c guru $ip outputSwitch.$index)
#data print out
echo "$voltage $iLimit $sense $current $rampspeed $status"
#next channel
let COUNTER=COUNTER+1
done
