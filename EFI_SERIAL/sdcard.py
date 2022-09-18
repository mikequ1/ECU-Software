import pickle
import glob
import time
import copy


sensorsMain = {k: [] for k in 
  ['micros',
   'totalRevs',
   'ECT',
   'IAT',
   'MAP',
   'MAP_AVG',
   'TPS',
   'AFR',
   'RPM',
   'injectorPulseTime',
   'lastPulse',
   'MAP_INDEX',
   'RPM_INDEX',
   'STARTUP_MOD',
   'STARTING_BOOL',
   'INJECTED',
   'MAPTrough',
   'dMAP',
   'gMAP',
   'SD_CONNECTED',
   'SD_FILE_NAME',
   'OVER_MAX_TEMP',
   'AFR_HIGH_RANGE']}



def main():
  
  #decide reading from serial or SDCard
  #if input("serial/SDCard (s1 for serial, s2 for SD card)") == "s2":
  files = glob.glob('./tests/*')
  #for n, i in zip(range(0, len(files)), files):
  #  print('%d %s' %(n,i))
  
  #fileN = int(input('which file?\n'))
  #nFile = open(files[fileN], "rb")
  
    # name dataFiles after date and time or user chosen
  tm = time.localtime()
  dataFile = f'{tm[7]:03}{tm[3]:02}{tm[4]:02}{tm[5]:02}'

  for n in range(0, len(files)):
    sensors = copy.deepcopy(sensorsMain)
  
    if str(files[n]).find('sdlog') == -1:
      continue

    dataPath = "./W20/" + dataFile + str(n)
    #print(dataPath)

    allKeys = list(sensors.keys())
  
    with open(files[n], "rb") as fp:
      line = fp.readline()
      while line:
        if(line == ""): 
          break
        vals = line.split(b':')
        if len(vals) != len(allKeys):
          print('error: mismatch in sensor number')

        output = ''
        #print(len(allKeys))
        for k in range(len(allKeys)):
          try:
            sensors[allKeys[k]].append(float(vals[k]))
          except ValueError:
            sensors[allKeys[k]].append(vals[k])
          
        for k in sensors:
          try:
            output = output + k + ": " + str(float(vals[allKeys.index(k)])) + '\n'
          except ValueError:
            output = output + k + ": " + str(vals[allKeys.index(k)]) + '\n'

        line = fp.readline()
      
    with open(dataPath, 'wb') as df:
        pickle.dump(sensors, df) # save file
  
  input(str(len(files)) + ' files converted')
            
if __name__== "__main__":
  main()
