/* Copyright (c) Stanford University, The Regents of the University of
 *               California, and others.
 *
 * All Rights Reserved.
 *
 * See Copyright-SimVascular.txt for additional details.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject
 * to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

//
//  Segment.cxx: a class to handle 1D network segments
//  ~~~~~~~~~
//
//  SYNOPSIS...This the C representation of a model segment.  
//             Again, this corresponds roughly to the Java 
//             version but not exactly.
//

#include "cvOneDSegment.h"
#include "cvOneDMthModelBase.h"

// Static Declarations

cvOneDSegment::cvOneDSegment(){
  InitialFlow = 0;
  InitialdFlowdT=0;
  InitialPressure=0;
  IsOutlet    = false;
  boundType   = BoundCondTypeScope::NOBOUND;
  lossType	= MinorLossScope::NONE;
  impedance = NULL;
  values = NULL;
  times = NULL;
  rpCapRd = NULL;
  waveValue = NULL;
  presLength = 0;
  impedanceLength = 0;
  rpCapRdLength = 0;
  waveValueLength = 0;
}

cvOneDSegment::cvOneDSegment(std::vector<double> const& zCoordinates, std::vector<double> const& initialAreas, double IF, bool IO){
  
  spatialCharacteristics.zCoordinates = zCoordinates;
  spatialCharacteristics.initialAreas = initialAreas;

  InitialFlow    = IF;
  InitialdFlowdT = 0;
  IsOutlet       = IO;
  boundType      = BoundCondTypeScope::NOBOUND;
  lossType	     = MinorLossScope::NONE;
  impedance = NULL;
  values = NULL;
  times = NULL;
  rpCapRd = NULL;
  waveValue = NULL;
  presLength = 0;
  impedanceLength = 0;
  rpCapRdLength = 0;
  waveValueLength = 0;

}

cvOneDSegment * cvOneDSegment::New(std::vector<double> const& zCoordinates, 
                  std::vector<double> const& initialAreas, 
                  double IF, 
                  bool IO){    
  // return new cvOneDSegment(IA, FA, IF, IO, IS);
  return new cvOneDSegment(zCoordinates, initialAreas, IF, IO);
}

void cvOneDSegment::Delete(void){
  delete this;    
}

void cvOneDSegment::setSegmentID(long id){
  SegmentID = id;
}

long cvOneDSegment::getSegmentID(void){
  return SegmentID;
}

void cvOneDSegment::setSegmentName(char *Name){
  SegmentName[0] = '\0';
  strcpy(SegmentName,Name);
}

char *cvOneDSegment::getSegmentName(void){
  return SegmentName;
}

void cvOneDSegment::setParentModel(void *mdl){
  parentModel = mdl;
}

void *cvOneDSegment::getParentModel(void){
  return parentModel;
}

double cvOneDSegment::getInletZ(){
  return spatialCharacteristics.zCoordinates.front();
}

double cvOneDSegment::getOutletZ(){
  return spatialCharacteristics.zCoordinates.back();
}

double cvOneDSegment::getSegmentLength(void){
  // The segment length is the distance between the start and end
  // of this segment.
  return spatialCharacteristics.zCoordinates.back() - spatialCharacteristics.zCoordinates.front();
}

void cvOneDSegment::setNumElements(long nels){
  NumElements = nels;
}

long cvOneDSegment::getNumElements(void){
  return NumElements;
}

double cvOneDSegment::getInitInletS(void){
  return spatialCharacteristics.initialAreas.front();
}

double cvOneDSegment::getInitOutletS(void){
  return spatialCharacteristics.initialAreas.back();
}

double cvOneDSegment::getInitialPressure(void){
  return InitialPressure;
}

double cvOneDSegment::getInitialFlow(void){
  return InitialFlow;
}

double cvOneDSegment::getInitialdFlowdT(void){
  //	return InitialdFlowdT;
	return 0;
}

void cvOneDSegment::setMeshType(MeshType mtype){
   SegmentMeshType = mtype;
}

MeshType cvOneDSegment::getMeshType(void){
  return SegmentMeshType;
}

MeshEvaluatorFunctionPtr cvOneDSegment::setMeshEvaluator(MeshEvaluatorFunctionPtr fcn){
  meshEvaluator = fcn;
  return meshEvaluator;
}

void cvOneDSegment::setInOutJoints(long inJoint, long outJoint){
  ConnJoints[0] = inJoint;
  ConnJoints[1] = outJoint;
}

long *cvOneDSegment::getInOutJoints(void){
  return ConnJoints;
}

void cvOneDSegment::tesselate(void){
  // Unimplimented...but coming
}

void cvOneDSegment::putInMatrix(void){
  // Unimplimented...but coming.
}

double cvOneDSegment::getMeshSpacing(long l){
  return segmentMesh -> ElementList[l].h;
}



