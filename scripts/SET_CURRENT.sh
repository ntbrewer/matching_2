#!/bin/bash
# Bash script to set Voltage on ONE MTAS HV channel 
#    	setVoltage   -  voltage to be set
#	setCurrent	-	current limit
#	setRamp		-	ramping speed
#	setStatus	- 	1 ON, 0OFF
#	ip		- 	IP address of the MTAS MPOD
# variables 
#	$1		-	channel name (eg.U100 - first modeul channel 0)
#	$2		-	ON/OFF switch
#	$3		-	voltage to be set
# USAGE:
# ./HV_MTAS_SET_ONE.sh <channel> <ON/OFF> <Voltage>
#
# based on MPOD example, this script should set up one channel
#M.Karny Jan,31 2011

#setting variables
ip=192.168.13.237
path=/usr/share/snmp/mibs
setCurrent=0.000500
setStatus=1
setRamp=100.


# seting up index to parameter $1
index=$1

# seting up the voltage,limit,rampspeed 

iLimit=$(snmpset -OqvU -v 2c -M $path -m +WIENER-CRATE-MIB -c guru $ip outputCurrent.$index F $setCurrent)


voltage=$(snmpget -OqvU -v 2c -M $path -m +WIENER-CRATE-MIB -c guru $ip outputVoltage.$index)
iLimit=$(snmpget -OqvU -v 2c -M $path -m +WIENER-CRATE-MIB -c guru $ip outputCurrent.$index)
sense=$(snmpget -OqvU -v 2c -M $path -m +WIENER-CRATE-MIB -c guru $ip outputMeasurementSenseVoltage.$index) 
current=$(snmpget -OqvU -v 2c -M $path -m +WIENER-CRATE-MIB -c guru $ip outputMeasurementCurrent.$index)
rampspeed=$(snmpget -OqvU -v 2c -M $path -m +WIENER-CRATE-MIB -c guru $ip outputVoltageRiseRate.$index)
status=$(snmpget -OqvU -v 2c -M $path -m +WIENER-CRATE-MIB -c guru $ip outputSwitch.$index)



echo 'Values set for channel' $1 $'\n Voltage:\t' $voltage $'\n I_limit:\t' $iLimit $'\n SenseVoltage:\t' $sense $'\n Meas. current:\t'  $current $'\n Rampspeed:\t' $rampspeed $'\n Status:\t' $status 
#echo $'Channel\t Voltage\t I_limit\t SenseVoltage:\t Measured current:\t Rampspeed:\t Status:\n'
#echo $1$'\t' $voltage$'\t' $iLimit$'\t' $sense$'\t' $current$'\t' $ramspeed$'\t' $status$'\t' 

