
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

#ifndef CVONEDSEGMENTSPATIALCHARACTERISTICS_H
#define CVONEDSEGMENTSPATIALCHARACTERISTICS_H

#include <utility>
#include <vector>

namespace cvOneD{

// We're replacing the inlet/outlet area with a set of distributed 
// measurements that can be interpolated. We need the z-position as
// well, so we'll roll the start/end positions into this structure.
// (and the segment length...but that should always have been
// calculated from the z-positions since it's redundant information)
//
// This is equivalent to the previous implementation when only 
// using four values: inlet z position, outlet z position, 
// initial inlet area, initial outlet area
//
// It's always necessary that there are at least two elements, and
// that first and last elements are the inlet and outlet, but the 
// structure doesn't need to know that. The input parser does.

struct PositionalCharacteristic{
    double z; // The position of this point along the vessel axis
    double area; // The initial cross-sectional area of the vessel cavity

    bool operator==(const PositionalCharacteristic& other) const;
};

// We might want to consider encapsulating this data instead so that
// solver and other output clients are unaware of the implementaiton
// details, so that if we were to change the format only the input
// parser would really rely on this. 
//
// Of course the algorithms below sort of manage that, but we could
// make it an explicit boundary.
struct SegmentSpatialCharacteristics{
    std::vector<PositionalCharacteristic> values = {};

    SegmentSpatialCharacteristics();
    SegmentSpatialCharacteristics(std::vector<PositionalCharacteristic> const& valuesIn);
    
    bool operator==(const SegmentSpatialCharacteristics& other) const;
};

// Verify that there aren't bad values
void checkSpatialCharacteristics(SegmentSpatialCharacteristics const& ssc);

// Create a simple segmented spatial characteristic from legacy input values
SegmentSpatialCharacteristics simpleSegmentSpatialCharacteristic(
    double segLength, double inletArea, double outletArea);

// Compute the distance between start and end of the segment
double calcSegLength(SegmentSpatialCharacteristics const& ssc);

// Retrieve the inlet/outlet areas
std::pair<double,double> segInletAndOutletAreas(SegmentSpatialCharacteristics const& ssc);

// Get the area or radius at a particular point
double getInterpolatedArea(double z, SegmentSpatialCharacteristics const& ssc);
double getInterpolatedRadius(double z, SegmentSpatialCharacteristics const& ssc);

} // namespace cvOneD

#endif // CVONEDSEGMENTSPATIALCHARACTERISTICS_H

