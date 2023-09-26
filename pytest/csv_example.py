# Copyright (c) 2023 Vector Informatik GmbH
# 
# Permission is hereby granted, free of charge, to any person obtaining
# a copy of this software and associated documentation files (the
# "Software"), to deal in the Software without restriction, including
# without limitation the rights to use, copy, modify, merge, publish,
# distribute, sublicense, and/or sell copies of the Software, and to
# permit persons to whom the Software is furnished to do so, subject to
# the following conditions:
# 
# The above copyright notice and this permission notice shall be
# included in all copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
# LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
# OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
# WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

import sys
import csv

import pybtf

# Class to parse command line parameters
# The syntax is "python csv_example.py <input-file.csv> <output-file.btf>"
class CommandLine():
  def __init__(self):
    if len(sys.argv) != 3:
      self.printInfo()

    self.csvFile = sys.argv[1]
    if self.csvFile[-4:] != '.csv':
      self.printInfo()
      exit()

    self.btfFile = sys.argv[2]
    if self.btfFile[-4:] != '.btf':
      self.printInfo()
      exit()
      
  def printInfo(self):
    sys.stdout.write(
      '-------------------------------------------------------------------------------\n' +
      ' T32 to btf converter\n' +
      '-------------------------------------------------------------------------------\n' +
      ' call: \'python csv_example.py <input-file.csv> <output-file.btf>\'\n' +
      '-------------------------------------------------------------------------------\n'
    )
    exit()

def main():
  # parse command line parameters
  cl = CommandLine()

  # get a btf file object
  btfFile = pybtf.pybtf.BtfFile(cl.btfFile, pybtf.pybtf.BtfFile.Timescale.nano_seconds, True, True, False, False)

  # parse input file
  print('> Parsing trace data from input file: %s'%(cl.csvFile))
  csv_reader = csv.reader(open(cl.csvFile, 'r'))
  ts = 0
  startFlag = False
  currentTask = ''
  currentIsr = ''
  offset = 0
  taskList = []
  isrList = []

  # go through every line in the csv file
  for i, l in enumerate(csv_reader):
    line = l[0].split(';')
    # each valid line has exactly 3 elements
    if len(line) == 3: 
      ts = int(line[0],10)-offset
      name = line[1][1:]
      ev =  line[2][1:]
      
      # first check if it is a task since they are uniquely identified in the csv with 'task:'.
      if 'task:' in name:
        # If nothing has started yet this is the first processEvent.
        if not startFlag:
          startFlag = True
          offset = ts
          currentTask = name.split(':')[1][1:]
          btfFile.processEvent(ts-offset, 'OsCore0', currentTask, 0, pybtf.pybtf.ProcessEvent.start, False)
          taskList.append(currentTask)
        else:
          # First preempt the current Task
          btfFile.processEvent(ts, 'OsCore0', currentTask, 0, pybtf.pybtf.ProcessEvent.preempt, False)
          currentTask = name.split(':')[1][1:]
          # Then check if it is the first event of the new current task.
          # Yes => start; No => resume
          if (currentTask not in taskList):
            btfFile.processEvent(ts, 'OsCore0', currentTask, 0, pybtf.pybtf.ProcessEvent.start, False)
            taskList.append(currentTask)
          else:
            btfFile.processEvent(ts, 'OsCore0', currentTask, 0, pybtf.pybtf.ProcessEvent.resume, False)

      # If we did not start yet skip this line.
      if not startFlag:
        continue

      # If it says fentry it is the start of a runnable event.
      if ('fentry' in ev):
        btfFile.runnableEvent(ts, 'OsCore0', name, pybtf.pybtf.RunnableEvent.start)

      # If it says ientry it is the start of an ISR.
      elif ('ientry' in ev):
        if(name not in isrList):
          btfFile.processEvent(ts, 'ISR', name, 0, pybtf.pybtf.ProcessEvent.start, True)
          # Remember the name of the ISR.
          currentIsr = name
          isrList.append(name)
        else:
          btfFile.processEvent(ts, 'ISR', name, 0, pybtf.pybtf.ProcessEvent.resume, True)
          currentIsr = name

      # If it says exit something gets terminated.
      elif ('exit' in ev):
        # If the name fits the ISR name the ISR gets terminated. Else a runnable is terminated.
        if(currentIsr == name):
          btfFile.processEvent(ts, 'ISR', name, 0, pybtf.pybtf.ProcessEvent.preempt, True)
        else:
          btfFile.runnableEvent(ts, 'OsCore0', name, pybtf.pybtf.RunnableEvent.terminate)
      
      # Switch event is already handled beforehand with the task loop.
      elif ('switch' in ev):
        pass
        #sys.stdout.write('Switch to %s\n'%(name))
      else:
        sys.stdout.write('Unhandled event: %s\n'%(ev))
  
  # Write btf file to disk and close all Processes and ISRs that are still open.
  for t in taskList:
    btfFile.processEvent(ts+100, 'OsCore0', t, 0, pybtf.pybtf.ProcessEvent.terminate, False)
  for i in isrList:
    btfFile.processEvent(ts+150,'ISR', i, 0, pybtf.pybtf.ProcessEvent.terminate, True)
  btfFile.finish()
  print('> Wrote trace data to btf file: %s'%(cl.btfFile))

if __name__ == '__main__':
    main()