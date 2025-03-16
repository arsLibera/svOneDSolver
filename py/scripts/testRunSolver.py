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

# Imports
import numpy as np
from mpi4py import MPI
from runParallelSolver import *
from simResultRecord import *

# Enums
ip1D = 0
ip2D = 1
dimType = ip2D

# Set result record
comm = MPI.COMM_WORLD
rank = comm.rank
size = comm.size

# Get input quantities for solver: NEED TO FILL WITH YOUR SPECIFIC SETTINGS!!!
# Folder with the oneDSolver Executable
exePath       = 'insert/exe/folder'
# Folder where the results are to be written
runDIR        = 'insert/test run/folder'
# Location of the input file
inputTemplate = 'insert/template/file'
# Location of the output files containing the result summary
outputFile    = 'insert/result/summary/file'
# Template File for result file e.g., simpleArtery_Flow_ARTERY_ for "simpleArtery" segment
resNameTempl = 'insert/result file/template name'

# Generate input variable realizations
# Generate in first processor and then distribute
if(rank == 0):
  np.random.seed(seed=0)
  totRuns = 15
  if(dimType == ip1D):
    x = np.zeros((15,1))
    x[:,0] = np.random.normal(loc=2.0, scale=0.2**2, size=totRuns)
  elif(dimType == ip2D): 
    x = np.zeros((15,2))
    x[:,0] = np.random.normal(loc=2.0, scale=0.2**2, size=totRuns)
    x[:,1] = np.random.normal(loc=200.0, scale=5.0**2, size=totRuns)
  else:
  	print 'ERROR: Invalid dimtype.'
  	print 'Terminating.'
  	MPI.Finalize()
  	sys.exit(0)
else:
  x = None

# Distribute x to all processors
x = comm.bcast(x,root=0)

# Set record for result interrogation
allRes = []
isRow = 0
rowColNum = 5
res = singleTXTFileExtractor(resNameTempl,ipFlow,isRow,rowColNum)
allRes.append(res)

# Create SolverWrap Object
slv = oneDSolverWrap(exePath,runDIR,inputTemplate,comm,allRes)

# Run Solver
output = slv.run(x)

if(rank == 0):
  # Print Output
  np.savetxt(outputFile,output)
  # Complete
  print 'Solution Completed!'
