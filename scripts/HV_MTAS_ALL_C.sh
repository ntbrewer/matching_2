#!/bin/bash
# Bash script to put the MTAS HV channels On/OFF
#    	setVoltage      -  	voltage to be set
#	setCurrent	-	current limit
#	setRamp		-	ramping speed
#	setStatus	- 	1 ON, 0 OFF
#	ip		- 	IP address of the MTAS MPOD
# data for setVoltage are taken either from parameter $2 or from file hv_mtas.dat
# hv_mtas.dat
#	DID U100 1000	- detector ID (DID), channel (U100) voltage (1000)V
# variables:
#	$1	- 	ON/OFF
#	$2	-	HV to be set if omitted for $1=ON data read from file hv_mtas.dat
#USAGE:
#./HV_MTAS_ALL.sh <ON/OFF> <voltage>
#M.Karny Jan,26 2011

#setting variables
ip=192.168.13.237
path=/usr/share/snmp/mibs
setVoltage=600
setCurrent=0.100
setStatus=0
setRamp=50
args=("$@")
file=0
filename=hv_mtas.dat

if [ $# -eq 2 ]; then
setVoltage=$args[2]
file=0
fi

if [ $# -eq 1 ]; then
 if [ $1 = "OFF" -o $1 = "off" -o $1 = "Off" ]; then
	setStatus=0
	s_setStatus="Off"
	echo "in off - HV for ALL channels will be turn OFF"
   else
	if [ $1 = "ON" -o $1 = "on" -o $1 = "On" ]; then
		setStatus=1
		s_setStatus="On"
		echo "HV for ALL channels will be turn ON"
		file=1
		echo "Voltages for channels will be read from file $filename"
  	  else
		setStatus=0	
		s_setStatus="On"
		echo "ON or OFF there is not third option!!!"
		exit
	fi
 fi	
#declaring channel and voltage as arrays
 declare -a channel
 declare -a voltage
#redirecting stdin to the filedescriptor 10  $filename
 exec 10<&0
 exec < $filename
#fi
	echo "################################################"
	echo "index volt iLimit sense current rampspeed status"	
#LOOP
let COUNTER=0
#while read PMT word1 word2; do
while [ $COUNTER -lt 12 ]; do
#reading line from STDinp which was redirected to $filename
    read PMT word1 word2
	channel[$COUNTER]=$word1
	voltage[$COUNTER]=$word2

# looking up the next value in 'channel' array
	index=$(echo ${channel[${COUNTER}]})
	setVoltage=$(echo ${voltage[${COUNTER}]})
# seting up the voltage,limit,rampspeed 
	voltage=$(snmpset -OqvU -v 2c -M $path -m +WIENER-CRATE-MIB -c guru $ip outputVoltage.$index F $setVoltage)
	iLimit=$(snmpset -OqvU -v 2c -M $path -m +WIENER-CRATE-MIB -c guru $ip outputCurrent.$index F $setCurrent)
	rampspeed=$(snmpset -OqvU -v 2c -M $path -m +WIENER-CRATE-MIB -c guru $ip outputVoltageRiseRate.$index F $setRamp)

	status=$(snmpget -OqvU -v 2c -M $path -m +WIENER-CRATE-MIB -c guru $ip outputSwitch.$index)
	if [ "$status" != "$s_setStatus" ]; then

#wlaczanie/wylaczanie kanalu
	status=$(snmpset -OqvU -v 2c -M $path -m +WIENER-CRATE-MIB -c guru $ip outputSwitch.$index i $setStatus)

	else
		if [ "$status" = "On" ]; then
		  echo "Channel $index already ON"
		else
		  echo "Channel $index already OFF"
		fi

	fi

#sprawdzanie wartosci w kanale i przypisywanie do zmiennych

	volt=$(snmpget -OqvU -v 2c -M $path -m +WIENER-CRATE-MIB -c guru $ip outputVoltage.$index)
	iLimit=$(snmpget -OqvU -v 2c -M $path -m +WIENER-CRATE-MIB -c guru $ip outputCurrent.$index)
	sense=$(snmpget -OqvU -v 2c -M $path -m +WIENER-CRATE-MIB -c guru $ip outputMeasurementSenseVoltage.$index) 
	current=$(snmpget -OqvU -v 2c -M $path -m +WIENER-CRATE-MIB -c guru $ip outputMeasurementCurrent.$index)
	rampspeed=$(snmpget -OqvU -v 2c -M $path -m +WIENER-CRATE-MIB -c guru $ip outputVoltageRiseRate.$index)
	status=$(snmpget -OqvU -v 2c -M $path -m +WIENER-CRATE-MIB -c guru $ip outputSwitch.$index)
#wypisywanie zmiennych

	echo "$PMT $index $volt $iLimit $sense $current $rampspeed $status"
#nastepny kanal
	((COUNTER++))
#let COUNTER=COUNTER+1
done
#restore stdin from filedescriptor 10
#and close filedescriptor 10
exec 0<&10 10<&-
else
 echo "Sorry it is not yet impemented"
fi

exit

