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

#include <ranges>

#include <cmath>

#include "cvOneDSegmentSpatialCharacteristics.h"

#include "cvOneDException.h"

namespace cvOneD{

namespace{

double linearInterpolate(double x, double x1, double y1, double x2, double y2) {
    return y1 + (x - x1) * (y2 - y1) / (x2 - x1);
}
    
} // namespace


SegmentSpatialCharacteristics::SegmentSpatialCharacteristics(){}

SegmentSpatialCharacteristics::SegmentSpatialCharacteristics(
    std::vector<PositionalCharacteristic> const& valuesIn)
        : values(valuesIn) {}

bool PositionalCharacteristic::operator==(const PositionalCharacteristic& rhs) const {
    return this->z == rhs.z && this->area == rhs.area;
}

bool SegmentSpatialCharacteristics::operator==(const SegmentSpatialCharacteristics& other) const {
    return this->values.size() == other.values.size() && 
           std::equal(this->values.begin(), this->values.end(), other.values.begin());
}

void checkSpatialCharacteristics(SegmentSpatialCharacteristics const& ssc){
    // There must be at least two points
    if(ssc.values.size() < 2){
        throw cvException(string("ERROR: there are fewer than two points in a vessel segment.").c_str());
    }

    // The areas must be positive
    for(auto const& value : ssc.values){
        if(value.area < 0){
            throw cvException(string("ERROR: Negative area value " + std::to_string(value.area) + " found in vessel.").c_str());
        }
    }

    // The z-positions must be sequentially increasing
    for(size_t k = 0; k < (ssc.values.size()-1); ++k){
        double const zk = ssc.values.at(k).z;
        double const zkplus1 = ssc.values.at(k+1).z;
        if(zk >= zkplus1){
            throw cvException("ERROR: consecutive z-coordinates of the spatial characteristics must be increasing."
                " Found consecutive values: " + std::to_string(zk) + " -> next value -> " + std::to_string(zkplus1));
        }
    }
}

SegmentSpatialCharacteristics simpleSegmentSpatialCharacteristic(
    double segLength, double inletArea, double outletArea){
    std::vector<PositionalCharacteristic> positionalCharacteristics{
        {0.0, inletArea},{segLength, outletArea}};
    return SegmentSpatialCharacteristics(positionalCharacteristics);
}
        
double calcSegLength(SegmentSpatialCharacteristics const& ssc){
    return ssc.values.back().z - ssc.values.front().z;
}

std::pair<double,double> segInletAndOutletAreas(SegmentSpatialCharacteristics const&  ssc){
    return {ssc.values.front().area, ssc.values.back().area};
}


double getInterpolatedArea(double z, SegmentSpatialCharacteristics const& ssc) {
    double const zInlet = ssc.values.front().z;
    double const zOutlet = ssc.values.back().z;
    double const inletArea = ssc.values.front().area;
    double const outletArea = ssc.values.back().area;

    // Interpolate to get the area at a given z-position.
    return linearInterpolate(z, zInlet, inletArea, zOutlet, outletArea);
}

double getInterpolatedRadius(double z, SegmentSpatialCharacteristics const& ssc) {
    // Interpolate to get the area at a given z-position.
    double const interpolatedArea = getInterpolatedArea(z,ssc);

    // Return the radius
    return sqrt(interpolatedArea / M_PI);
}

} // namespace cvOneD
