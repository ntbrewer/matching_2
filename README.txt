Instructions to be expanded as necessary in situ.

Note: '>' = type the following

> cd ~/matching_2

open interactive session with:

> pydamm

the interactive session has a prompt like 'In [#]:' where # is a line number.

> %run matching2.py

you now have access to functions defined in the python script. i.e.

#Make definitions:
def validate_fit(fitlist,isC):
def run_pixie(waitTime=wait,frequency=freq):  
def rerun_pixie(waitTime=wait,frequency=freq):  
def close_pixie(pixie):
def get_voltage(mtasid):
def set_voltage(mtasid,volts):
def set_all_on_off(onOff):
def set_rise_rate_all(rate):
def set_dv(mtasid,dvolts):
def open_damm():
def close_damm(damm):
def zero_his(mtasid,damm):
def write_out_HV(filename='hv_mtas.dat'):
def read_in_HV(filename='hv_mtas.dat'):
def set_HV_from_read(hv_array):
def dict_add(id,dict,volt,pk):
def matching2():


first use write_out_HV to save current HV settings i.e.:

> write_out_HV('dat_files/hv_mtas.Apr0518.dat')
this is recoverable from 

> hv_array = read_in_HV(filename='hv_mtas.dat'):
> set_HV_from_read(hv_array):

then in a different window (if not already running) run poll2 and pacman. (instructions elsewhere)
in pacman start vme with:

> startvme (sometimes twice)

variable 'wait' in matching2.py can be adjusted. 

> matching2() (repeat as necessary)


When you are done, you should rerun :

> write_out_HV('dat_files/hv_mtas.Apr0518_final.dat')

to save your work. 


To do:
look to documentation in matching2.py to understand the output of this program and make comments here as necessary. 

damm or pydamm can be run to look at junk.his while the script is running. 

To make a manual correction:
use the functions:

> get_voltage(mtasid):
> set_voltage(mtasid,volts):

or 

> set_dv(mtasid,dvolts):

This latter function will adjust the voltage by an amount delta v (dv). 

channel moves ~10 ch/V.



