
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
// structure doesn't need to know that per se. The input parser does.
struct PositionalCharacteristic{
    double z; // The position of this point along the vessel axis
    double area; // The initial cross-sectional area of the vessel cavity

    bool operator==(const PositionalCharacteristic& other) const;
};

class SegmentSpatialCharacteristics{

  public:
    SegmentSpatialCharacteristics();

    // Construct from a vector of positions z and corresponding vector of areas
    // (with the same length).
    // 
    // Does not full validate incoming data, instead, that is done when verify
    // is called (currently). We could roll those together later.
    SegmentSpatialCharacteristics(std::vector<double> const& z, std::vector<double> const& areas);
    
    // Throws if the data is invalid (e.g., if there are fewer than two points)
    void verifyValidData() const; 

    // Methods for interacting with the data as required by clients
    double length() const;
    std::pair<double,double> inletAndOutletZCoordinates() const;
    std::pair<double,double> inletAndOutletAreas() const;
    double getInterpolatedArea(double z) const;
    double getInterpolatedRadius(double z) const;

    // This is exposed for the serializer only. Other clients should use the
    // methods for interacting with the data. For a better OOP structure,
    // we would need to refactor this to avoid exposing the data.
    std::vector<PositionalCharacteristic> const& getValues() const;

    // For testing comparisons
    bool operator==(const SegmentSpatialCharacteristics& other) const;

  private:
    std::vector<PositionalCharacteristic> values = {};
};

// Create a simple segmented spatial characteristic from legacy input values
SegmentSpatialCharacteristics simpleSegmentSpatialCharacteristic(
    double segLength, double inletArea, double outletArea);

} // namespace cvOneD

#endif // CVONEDSEGMENTSPATIALCHARACTERISTICS_H

