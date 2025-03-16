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

#include <iostream>
#include <fstream>
#include <string>

#include <gtest/gtest.h>

#include "OptionsTestHelpers.hpp"

void expectEqOptions(const cvOneD::options& actual, const cvOneD::options& expected) {
    // Compare modelName
    EXPECT_EQ(expected.modelName, actual.modelName);
    
    // Compare node data
    EXPECT_EQ(expected.nodeName, actual.nodeName);
    EXPECT_EQ(expected.nodeXcoord, actual.nodeXcoord);
    EXPECT_EQ(expected.nodeYcoord, actual.nodeYcoord);
    EXPECT_EQ(expected.nodeZcoord, actual.nodeZcoord);

    // Compare joint data
    EXPECT_EQ(expected.jointName, actual.jointName);
    EXPECT_EQ(expected.jointNode, actual.jointNode);
    EXPECT_EQ(expected.jointInletName, actual.jointInletName);
    EXPECT_EQ(expected.jointOutletName, actual.jointOutletName);

    // Compare joint inlet and outlet lists
    EXPECT_EQ(expected.jointInletListNames, actual.jointInletListNames);
    EXPECT_EQ(expected.jointInletListNumber, actual.jointInletListNumber);
    EXPECT_EQ(expected.jointInletList, actual.jointInletList);
    EXPECT_EQ(expected.jointOutletListNames, actual.jointOutletListNames);
    EXPECT_EQ(expected.jointOutletListNumber, actual.jointOutletListNumber);
    EXPECT_EQ(expected.jointOutletList, actual.jointOutletList);

    // Compare material data
    EXPECT_EQ(expected.materialName, actual.materialName);
    EXPECT_EQ(expected.materialType, actual.materialType);
    EXPECT_EQ(expected.materialDensity, actual.materialDensity);
    EXPECT_EQ(expected.materialViscosity, actual.materialViscosity);
    EXPECT_EQ(expected.materialPRef, actual.materialPRef);
    EXPECT_EQ(expected.materialExponent, actual.materialExponent);
    EXPECT_EQ(expected.materialParam1, actual.materialParam1);
    EXPECT_EQ(expected.materialParam2, actual.materialParam2);
    EXPECT_EQ(expected.materialParam3, actual.materialParam3);

    // Compare data table information
    EXPECT_EQ(expected.dataTableName, actual.dataTableName);
    EXPECT_EQ(expected.dataTableType, actual.dataTableType);
    EXPECT_EQ(expected.dataTableVals, actual.dataTableVals);

    // Compare segment data
    EXPECT_EQ(expected.segmentName, actual.segmentName);
    EXPECT_EQ(expected.segmentID, actual.segmentID);
    EXPECT_EQ(expected.segmentLength, actual.segmentLength);
    EXPECT_EQ(expected.segmentTotEls, actual.segmentTotEls);
    EXPECT_EQ(expected.segmentInNode, actual.segmentInNode);
    EXPECT_EQ(expected.segmentOutNode, actual.segmentOutNode);
    EXPECT_EQ(expected.segmentInInletArea, actual.segmentInInletArea);
    EXPECT_EQ(expected.segmentInOutletArea, actual.segmentInOutletArea);
    EXPECT_EQ(expected.segmentInFlow, actual.segmentInFlow);
    EXPECT_EQ(expected.segmentMatName, actual.segmentMatName);
    EXPECT_EQ(expected.segmentLossType, actual.segmentLossType);
    EXPECT_EQ(expected.segmentBranchAngle, actual.segmentBranchAngle);
    EXPECT_EQ(expected.segmentUpstreamSegment, actual.segmentUpstreamSegment);
    EXPECT_EQ(expected.segmentBranchSegment, actual.segmentBranchSegment);
    EXPECT_EQ(expected.segmentBoundType, actual.segmentBoundType);
    EXPECT_EQ(expected.segmentDataTableName, actual.segmentDataTableName);

    // Compare solver options
    EXPECT_EQ(expected.timeStep, actual.timeStep);
    EXPECT_EQ(expected.stepSize, actual.stepSize);
    EXPECT_EQ(expected.maxStep, actual.maxStep);
    EXPECT_EQ(expected.quadPoints, actual.quadPoints);
    EXPECT_EQ(expected.inletDataTableName, actual.inletDataTableName);
    EXPECT_EQ(expected.boundaryType, actual.boundaryType);
    EXPECT_EQ(expected.convergenceTolerance, actual.convergenceTolerance);
    EXPECT_EQ(expected.useIV, actual.useIV);
    EXPECT_EQ(expected.useStab, actual.useStab);
    // For now, we're not going to verify the outputType. Why not? Because, currently
    // the legacy serializer does not record the outputType. Instead, it stores it
    // in the global settings. 
    //
    // A more consistent behavior would be to store it within the options like the 
    // other settings and update clients to use it. Once that's done, we can test it here
    // in a consistent way.
}

std::string readFileContents(const std::string& filePath) {
    std::ifstream inputFile(filePath);
    if (!inputFile) {
        throw std::runtime_error("Could not open the file: " + filePath);
    }

    return std::string((std::istreambuf_iterator<char>(inputFile)),
                       std::istreambuf_iterator<char>());
}