#!/usr/bin/env python3
import os
import serial
from config_loader import Config
import time
import atexit
import pickle
import glob
import curses
from curses import wrapper
from curses import ascii as crsascii

clear = lambda: os.system('cls')

config = 'config.ini'

configContents = "[GEN]\nCOM = 1\nBAUD = 9600\nFILENAMEDATE = n\nFOLDER = default"

sensors = {k: [] for k in 
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

ser = serial.Serial()

def main(stdscr):

  # if config.ini exists, contine
  #  else create it and exit
  if not os.path.exists(config):
    with open(config, 'w') as cfgFile:
      cfgFile.write(configContents)
    print("created config file")
    leave()
  
  # get settings from file
  try:
    com = 'COM' + Config.gen('COM')
    baudrate = int(Config.gen('BAUD'))
  except:
    print("invalid inputs")
    leave()
  
  # name dataFiles after date and time or user chosen
  dataPath = Config.gen('FOLDER')
  if Config.gen('FILENAMEDATE')[0] == 'y':
    tm = time.localtime()
    dataFile = f'{tm[7]:03}{tm[3]:02}{tm[4]:02}{tm[5]:02}'
  else:
    dataFile = input("file to store data: ")
    while dataFile == "":
      dataFile = input("file to store data: ")
    while os.path.exists(dataFile):
      if input("overwrite existing file? (y for yes): ") == "y":
         break
      else:
         dataFile = input("file to store data: ")
         while dataFile == "":
           dataFile = input("file to store data: ")

  dataPath = dataPath + '/' +  dataFile

  # from here on we want a nicer output
  stdscr.clear()
  # open serial port
  ser.baudrate = baudrate
  ser.port = com
  ser.timeout = 2
  cnt = 0;
  while not ser.is_open:
    try:
      ser.open()
    except:
      stdscr.clear()
      stdscr.addstr(0, 0, 'opening port: ' + com + ' at ' + str(baudrate) + " try: " + str(cnt))
      stdscr.refresh()
      cnt += 1

  # collect and log data
  numMismatch = 0
  maxMismatch = 5

  allKeys = list(sensors.keys())
  buf = bytearray()
  stdscr.nodelay(1) # make getch() non-blocking
  while True:
    # rewrite of ser.readline()
    #   used since readline() is slow
    try:
      buf = buf + ser.read(max(1, ser.in_waiting))
      i = buf.find(b'\n')
      if i >= 0:
          line = buf[:i+1]
          buf = buf[i+1:]
      else:
          continue
    except Exception as e:
      stdscr.clear()
      stdscr.addstr(0, 0, 'error')
      stdscr.refresh()
      continue

    if line == '':
      stdscr.clear()
      stdscr.addstr(0, 0, 'empty buffer')
      stdscr.refresh()
      continue

    vals = line.split(b':')
    if len(vals) != len(allKeys):
      numMismatch += 1
      if numMismatch >= maxMismatch:
        numMismatch = 0
        stdscr.clear()
        stdscr.addstr(0, 0, 'mismatch in sensor number')
        stdscr.addstr(1, 0, 'press enter to continue')
        stdscr.refresh()
        stdscr.nodelay(0)
        stdscr.getkey()
        stdscr.nodelay(1)
        continue
      else:
        continue;

    numMismatch = 0 # if we get here, no consecutive mismatch
    output = ''
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
    output = output + 'saving to: ' + dataPath + '\n'
    output = output + 'len of TPS: ' + str(len(sensors['TPS'])) + '\n'
    output = output + 'shift-s to save and exit'
    stdscr.clear()
    stdscr.scrollok(1)
    stdscr.addstr(output)
    stdscr.refresh()

    if stdscr.getch() == ord('S'):
      break

  with open(dataPath, 'wb') as df:
    pickle.dump(sensors, df) # save file
  
# end main

def leave():
  input("press enter to exit")
  exit()

wrapper(main)
#if __name__ == '__main__':
#  main()
