# Copyright (c) Stanford University, The Regents of the University of
# California, and others.
#
# All Rights Reserved.
#
# See Copyright-SimVascular.txt for additional details.
#
# Permission is hereby granted, free of charge, to any person obtaining
# a copy of this software and associated documentation files (the
# "Software"), to deal in the Software without restriction, including
# without limitation the rights to use, copy, modify, merge, publish,
# distribute, sublicense, and/or sell copies of the Software, and to
# permit persons to whom the Software is furnished to do so, subject
# to the following conditions:
#
# The above copyright notice and this permission notice shall be included
# in all copies or substantial portions of the Software.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
# THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
# PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
# PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
# LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
# NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

import sys,os
import numpy as np

# =============================================
# Get list of files containing a certain string
# =============================================
def getFileContaining(searchString):
  fileList = []
  # Get all the files in the current Dir
  directory = os.listdir('.')
  for fname in directory:
    if searchString in fname:
      fileList.append(fname)
  # Return List
  return fileList

# ======================
# RESULT EXTRACTOR CLASS
# ======================
class resultExtractor():
  pass

# =========================================================
# Class to extract multiple quantities from TXT file result
# =========================================================
class singleTXTFileExtractor(resultExtractor):
  # Constructor
  def __init__(self,fileNameTempl,resultType,isRow,rowColNum):
    # Use tree
    self.fileNameTempl = fileNameTempl
    self.resultType = resultType
    if(isRow == 0):
      self.isRow = False
    else:
      self.isRow = True
    self.rowColNum = rowColNum

  # Construct the result file name
  def getResultFileName(self):
    if(self.resultType == 0):
      # flow
      return self.fileNameTempl + 'flow.dat'
    elif(self.resultType == 1):
      # area
      return self.fileNameTempl + 'area.dat'
    elif(self.resultType == 2):
      # pressure
      return self.fileNameTempl + 'pressure.dat'
    elif(self.resultType == 3):
      # re
      return self.fileNameTempl + 'Re.dat'

  # Get Simulation Results
  def getSimulationResults(self):
    fileName = self.getResultFileName()
    # Open the file with numpy
    resMat = np.loadtxt(fileName)
    if(self.isRow):
      return resMat[self.rowColNum,:]
    else:
      return resMat[:,self.rowColNum]

# =============================================
# Class to extract flows from multiple branches
# =============================================
class multipleFlowExtractor(resultExtractor):
  # Constructor
  def __init__(self,branchNameTmplList):
    # Store local qtys
    self.branchNameTmplList = branchNameTmplList
    self.totBranches = len(branchNameTmplList)
  
  # Get Simulation Results
  def getSimulationResults(self):
    res = np.zeros((self.totBranches,))
    # Loop on the files
    for loopA in xrange(self.totBranches):
      # Get Flow Result File
      fileName = self.branchNameTmplList[loopA] + 'flow.dat'
      # Open the file with numpy
      tmpRead = np.loadtxt(fileName)
      res[loopA] = tmpRead[tmpRead.shape[0]-1,tmpRead.shape[1]-1]
    # Return values
    return np.asarray(res)

# =================================================
# Class to extract Pressures from multiple branches
# =================================================
class multiplePressureExtractor(resultExtractor):
  # Constructor
  def __init__(self,branchNameTmplList):
    # Store local qtys
    self.branchNameTmplList = branchNameTmplList
    self.totBranches = len(branchNameTmplList)
  
  # Get Simulation Results
  def getSimulationResults(self):
    res = np.zeros((self.totBranches,))
    # Loop on the files
    for loopA in xrange(self.totBranches):
      # Get Flow Result File
      fileName = self.branchNameTmplList[loopA] + 'pressure.dat'
      # Open the file with numpy
      tmpRead = np.loadtxt(fileName)
      res[loopA] = tmpRead[tmpRead.shape[0]-1,tmpRead.shape[1]-1]
    # Return values
    return np.asarray(res)

# =======================================================
# Class to extract min and max WSS across the whole model
# =======================================================
class minMaxWSSExtractor(resultExtractor):
  # Constructor
  def __init__(self):
    pass
  
  # Get Simulation Results
  # CAREFULL: Returns the min/max WSS in space at the last timestep!!!
  def getSimulationResults(self):
    maxWSS = -sys.float_info.max
    minWSS = sys.float_info.max
    # Get a list of all files containing WSS
    fileList = getFileContaining('wss.dat')
    # Loop on all the WSS result file
    for loopA in xrange(len(fileList)):
      # Get Flow Result File
      fileName = fileList[loopA]
      # Open the file with numpy
      currBranchWSS = np.loadtxt(fileName)[:,-1]
      maxBranchWSS = currBranchWSS.max()
      minBranchWSS = currBranchWSS.min()
      # Store if needed
      if(maxBranchWSS > maxWSS):
        maxWSS = maxBranchWSS
      if(minBranchWSS < minWSS):
        minWSS = minBranchWSS

    # Return values
    return np.array([minWSS,maxWSS])

    
