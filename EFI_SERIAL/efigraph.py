#!/usr/bin/env python3
import glob
import pickle
from mpl_toolkits.mplot3d import Axes3D  
import matplotlib.pyplot as plt
import numpy as np
import math

def main():

  # Choose and open file
  files = glob.glob('./W20/*')
  for n, i in zip(range(0, len(files)), files):
    print('%d %s' %(n,i))
  
  fileN = int(input('which file?\n'))

  with open(files[fileN], 'rb') as df:
    sensors = pickle.load(df)

  # What to display
  #
  # INDEXS USED
  #
  plt.figure(1)
  plt.hist2d(sensors['RPM_INDEX'],sensors['MAP_INDEX'], [10, 11], [[0, 10], [0, 11]])
  plt.colorbar()
  plt.grid()
  plt.gca().invert_yaxis()
  plt.xlabel("RPM INDEX")
  plt.ylabel("MAP INDEX")
  plt.title('MAP_INDEX vs RPM_INDEX')
 
  ms = list(sensors['micros'])
  revs = list(sensors['totalRevs'])
  # ECT
  # IAT
  mp = list(sensors['MAP'])
  mpavg = list(sensors['MAP_AVG'])
  tps = list(sensors['TPS'])
  # AFR
  rpm = list(sensors['RPM'])
  ipt = list(sensors['injectorPulseTime'])
  # lastPulse
  # MAP_INDEX
  # RPM_INDEX
  # STARTUP_MOD
  # STARTING_BOOL
  # INJECTED
  mpt = list(sensors['MAPTrough'])
  dmap = list(sensors['dMAP'])
  gmap = list(sensors['gMAP'])
  # SD_CONNECTED
  # SD_FILE_NAME
  # OVER_MAX_TEMP
  afr = list(sensors['AFR_HIGH_RANGE'])

  mptlen = list(np.ones(len(mpt)))

  dmapPos = [a > 0 for a in dmap]
  drevs = [b - a for b, a in zip(revs[1:-1], revs[0:-2])]
  drevs = [0] + drevs[:] + [0]
  print(len(drevs))
  print(len(ms))
  
  for i in range(len(rpm)):
    if(rpm[i] > 8000):
      rpm[i] = rpm[i - 1] # code basically implements this now

  #
  # Injection Time vs Micros
  #
  plt.figure(2)
  plt.plot(ms, mp)
  plt.plot(ms, mpavg, color='purple')
  plt.plot(ms, gmap, color='pink')
  #plt.plot(ms, np.convolve(mp, [20/28,5/28,2/28,1/28], 'same'), color='purple')
  ax2 = plt.twinx()
  ax2.scatter(ms, drevs, color='green', label='dRevs')
  ax2.scatter(ms, sensors['INJECTED'], color='orange', label='Injected')
  #ax2.scatter(mpt, mptlen, color='red', label='MAP Trough')
  #ax2.scatter(ms, dmapPos, color='red', label='dMAP is Positive')
  #for i in range(len(mpt)):
   # ax2.axvline(x=mpt[i])
  ax3 = plt.twinx()
  ax3.plot(ms, rpm, color='green', label='RPM')
  plt.tight_layout()
  plt.legend(loc='upper left')
  plt.title('MAP vs Micros')

  plt.figure(3)
  plt.plot(ms, mp)
  ax2 = plt.twinx()
  ax2.plot(ms, rpm, color='green', label='RPM')
  plt.tight_layout()
  plt.legend(loc='upper left')
  plt.title('MAP vs Micros')
  
  plt.figure(4)
  plt.plot(ms, list(sensors['ECT']))
  ax2 = plt.twinx()
  plt.tight_layout()
  plt.title('ECT vs Micros')
  
  plt.figure(5)
  plt.plot(ms, list(sensors['TPS']),color='red', label='TPS')
  ax4 = plt.twinx()
  ax4.plot(ms,rpm,color='green',label='RPM')
  plt.legend()
  plt.title('TPS, RPM vs Micros')
  
  plt.figure(6)
  plt.plot(ms, afr)
  plt.title('AFR vs Micros')
  
  plt.figure(7)
  plt.plot(ms, list(sensors['RPM_INDEX']), label="RPM Index")
  plt.plot(ms, list(sensors['MAP_INDEX']), label="MAP Index")
  plt.legend()
  plt.title("RPM Index & MAP Index over time")
  # plt.figure(7)
  # plt.gca(projection='3d')
  # plt.scatter([int(i) for i in sensors['RPM_INDEX']],[int(i) for i in sensors['MAP_INDEX']],afr)
  # #plt.gca().invert_yaxis()
  # plt.set_xlabel('RPM index')
  # plt.set_ylabel('MAP index')
  # plt.set_zlabel('AFR')
  
  #
  # dMicros
  #
  #dms = [b - a for b, a in zip(ms[1:-1], ms[0:-2])]
  #plt.figure(4)
  #plt.scatter(range(len(dms)),dms)
  #plt.grid()
  #plt.title('dMicros vs len(dMicros)')
  plt.show()

if __name__ == "__main__":
  main()
