#!/opt/rh/rh-python34/root/usr/bin/python3

####################################################
#
#  OUTLINE:
#     analize input (input parms ie : -ICMO)
#     Run pixie? (rm, rerun, reload) use Pexpect
#     fit peaks     
#     validate peaks 
#     set voltages
#     
#
####################################################


#Import needs
import pexpect as pxp
from Pyspectr.pydamm import *
import numpy as np
from progressbar import ProgressBar, ReverseBar


####################################################
#                set default values
#---------------------------------------------------

peaklist=[1460]
channels=range(100,148)  #From 100 to 147 inclusive
#channels=[109,110,120]
wait = 600 # Units in seconds. To be adjusted as necessary for the given source.
freq = 10
xmargin = 500

#set safetys
maxVoltIMO=1450
maxVoltC=1250

#Read in channel map. 

#define dicts
hv_response=dict()
for i in channels:
    hv_response[repr(i)]=[10,0,0,0]
#inf=open('response.dat','r')
#lines = inf.readlines()
#hv_response=eval(lines[0])

hvmap=dict()
lines = np.genfromtxt('hvmap.csv',(np.character,int),delimiter=' , ')
for i in lines:
    hvmap[i[1].decode()]={'id':i[0].decode(),'val':10}
       
####################################################



#Make definitions
def validate_fit(fitlist,isC):
    if isC:
        width = 250
    else:
        width=80
    if len(fitlist)==7 and not hasattr(fitlist[0],'len'):
        if 30<fitlist[5]<width and 100<fitlist[6]:
            return(True)
        else:
            return(False)
    else:
        print('bad form')
        return(False)   

def run_pixie(waitTime=wait,frequency=freq):  
   p = pxp.spawn('pixie_ldf_c_slim junk')
   p.send('lof\nzero\nshm\ngo\n')
   print('sleeping')
   pBar = ProgressBar(widgets=[ReverseBar('<')],maxval=waitTime).start()
   for i in range(0,round(waitTime/frequency)):
       pxp.time.sleep(frequency)
       pBar.update(frequency*(i+1))
   return(p)

def rerun_pixie(waitTime=wait,frequency=freq):  
   p = pxp.spawn('pixie_ldf_c_slim junk')
   p.send('shm\ngo\n')
   print('sleeping')
   pBar = ProgressBar(widgets=[ReverseBar('<')],maxval=waitTime).start()
   for i in range(0,round(waitTime/frequency)):
       pxp.time.sleep(frequency)
       pBar.update(frequency*(i+1))
   return(p)
   
def close_pixie(pixie):
    pixie.close()
       
def get_voltage(mtasid):
    return(eval(pxp.run('snmpget -OqvU -v 2c -M /usr/share/snmp/mibs -m +WIENER-CRATE-MIB -c guru 192.168.13.237 outputVoltage.'+hvmap[repr(mtasid+100)]['id']).decode().split('\r\n')[0]))

def set_voltage(mtasid,volts):
    if mtasid<12:
        if volts>maxVoltC:
            print('C HV too high')
            return(False)
    else:
        if volts>maxVoltIMO:
            print('IMO HV too high')
            return(False)
    #pxp.time.sleep(1)
    return(eval(pxp.run('snmpset -OqvU -v 2c -M /usr/share/snmp/mibs -m +WIENER-CRATE-MIB -c guru 192.168.13.237 outputVoltage.'+hvmap[str(mtasid+100)]['id']+' F '+str(volts))))

def set_all_on_off(onOff):
    for mtasid in range(0,48):
        print(pxp.run('snmpset -OqvU -v 2c -M /usr/share/snmp/mibs -m +WIENER-CRATE-MIB -c guru 192.168.13.237 outputSwitch.'+hvmap[str(mtasid+100)]['id']+' i '+str(onOff)))
    
def set_rise_rate_all(rate):
    for mtasid in range(0,48,16):
        print(eval(pxp.run('snmpset -OqvU -v 2c -M /usr/share/snmp/mibs -m +WIENER-CRATE-MIB -c guru 192.168.13.237 outputVoltageRiseRate.'+hvmap[str(mtasid+100)]['id']+' F '+str(rate))))

def set_dv(mtasid,dvolts):
    print(set_voltage(mtasid,get_voltage(mtasid)+dvolts))

def open_damm():
    damm = pxp.spawn('damm')
    damm.sendline('in junk.his')
    damm.sendline('z on')
    return(damm)

def close_damm(damm):
    damm.sendline('end')
    damm.close()
    
def zero_his(mtasid,damm):
    damm.sendline('z '+repr(mtasid))    
    
def write_out_HV(filename='dat_files/hv_mtas.dat'):
    ouf = open(filename,'w')
    for i in channels:
        ouf.write(str(i)+' , '+repr(get_voltage(i-100))+'\n')
    ouf.close()
    
def read_in_HV(filename='dat_files/hv_mtas.dat'):
    inf = open(filename,'r')
    lines = inf.readlines()
    hv_array =[]
    for i in lines:
        hv_array.append(eval(i))
    return(hv_array)
    
def set_HV_from_read(hv_array):
    for i in hv_array:
        print('Setting... ')
        print(set_voltage(i[0]-100,i[1]))

#def interpolate(dic,id):

def dict_add(id,dic,volt,pk):
    diff = (dic[repr(id)][1]-volt)/(dic[repr(id)][2]-pk)
    dic[repr(id)][1]=volt
    dic[repr(id)][2]=pk
    n=dic[repr(id)][3]
    if (n!=0):
        dic[repr(id)][0]=dic[repr(id)][0]*n/(n+1)+diff/(n+1)
    dic[repr(id)][3]+=1



def matching2():
    p=run_pixie(wait) #comment out if you wish to run pixie yourself elsewhere
        
    e = Experiment('junk.his',size=0)
    e.dl(20,4000)
    e.pzot()
    for i in peaklist:
        e.pk(i)
    errv=[]
    fit_resv=[]
    ouf = open('fit_reports.dat', 'w') #clear old file, if any.
    ouf.close() #didn't clear??
    xmin=min(peaklist)-xmargin
    xmax=max(peaklist)+xmargin

    #loop over channels and set voltages
    #loop over channel (100-148)
    #hisid = 3050 + channel
    #set peaks
    #fit peaks
    #fit valid?
    #if not try variations until valid
    #if so
    #add pk and currentHV to interpolator
    #get dv from interpolator 
    #set voltage to currentHV plus dv 
    #zero his
    #repeat until converges
    damm=open_damm()
    if True:
      t = pxp.time.time()
      for i in channels:
          his_id = 3050 + i
          e.pzot()
          for l in peaklist:
              e.pk(l) 
          fit_res = e.fit_peaks( his_id , rx=(xmin,xmax) )
          fv = False
          k=0
          while not fv:
              fv=True
              for j in fit_res:
                  fit_valid=validate_fit(j,i<112)
    #              input('ok?')
                  if fit_valid:
                      continue
                  else:
                      fv=False
              if not fit_valid and k!=8:
                  e.pzot()
                  shift=500-150*k
                  for l in peaklist:                
                      e.pk(l+shift)
                  print(e.peaks)    
                  fit_res=e.fit_peaks(his_id,rx=(max(xmin+shift-50*k,300), xmax+shift+50*k))
                  k+=1
              elif k==8:
                  print('error on chan '+repr(i)+' skipping..')
                  errv.append(i)
                  fv=True
          if not i in errv:
              #should be changed to loop over peaks
              ouf = open('fit_reports.dat','a')
              ouf.write(repr(fit_res)+'\n')
              dict_add(i,hv_response,fit_res[0][1],get_voltage(i-100))
              chan_response = hv_response[repr(i)]
              cvr = chan_response[0]
              dv = (peaklist[0]-chan_response[1])/cvr
              ouf.write(repr(i)+' '+repr(dv)+'\n')
              if abs(dv)>=0.3:
                  if ((i<112 and chan_response[2]+dv < maxVoltC) or (i>111 and chan_response[2]<maxVoltIMO) ): 
                      set_dv(i-100,dv)
                      zero_his(his_id,damm)
                      #pxp.time.sleep(1)
                      #print('ok')
          
              ouf.close()
      dt=pxp.time.time()-t
      print('dt='+repr(dt)+'\n')
    #pxp.time.sleep(wait)

    p.sendcontrol('c')
    p.send('end\n')
    p.close()


#TBD    
#loop over pairs 
#loop over rings and calibrate
#end pixie

