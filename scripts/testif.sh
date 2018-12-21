ip=192.168.13.237
ppath=/usr/share/snmp/mibs

CurrFail=$(snmpget -OqvU -v 2c -M $ppath -m +WIENER-CRATE-MIB -c guru $ip outputStatus.u4)
x=(`echo $CurrFail | tr '"' ' '`)
str='04'
echo string $str
echo CurrFail= $CurrFail,$x,

	    if [ $CurrFail == $str ]; then
echo "udalo sie stop"
else 
echo "NIE DOBRZE"
fi
	    if [ $x == 04 ]; then
echo "udalo sie stop"
else 
echo "NIE DOBRZE"
fi

exit
