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

# imports 
import sys
import re
import numpy as np

# =====================================
# CLASS TO PROCESS TEMPLATE INPUT FILES
# =====================================
class runFileTemplate():

  # Constructor
  def __init__(self,templateFileName):
    # Read Stream In
    self.stream = open(templateFileName,'r').read()
    parNumbers = re.findall(r'<params,(.*?)>', self.stream)
    parInt = [int(value) for value in parNumbers]
    self.dims = len(np.unique(parInt))

  # Generate Realization File
  def generate(self,x,outFile):
    # Check if the dimensions are compatible
    if(self.dims != len(x)):
      print 'Dimensions in Template: ' + str(self.dims)
      print 'Dimensions in Vector: ' + str(len(x))
      print 'Template file dimensions not compatible. Terminating.'
      sys.exit(-1)
    # Replace Variables
    localStream = self.stream
    for loopA in xrange(len(x)):
      localStream = localStream.replace('<params,' + str(loopA) + '>', '%8.4e' % (x[loopA]))
    # Save to realization file
    text_file = open(outFile,"a+")
    text_file.write(localStream)