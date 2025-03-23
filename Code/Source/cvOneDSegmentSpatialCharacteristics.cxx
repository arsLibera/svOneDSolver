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
#include <algorithm>
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
    std::vector<double> const& zCoordinates, std::vector<double> const& areas){

    if(zCoordinates.size() != areas.size()){
        throw cvException("ERROR: input spatial characteristic z coordinates "
            "and corresponding areas must have the same size.");
    }

    for (size_t i = 0; i < zCoordinates.size(); ++i) {
        this->values.push_back({zCoordinates.at(i), areas.at(i)});
    }
}

bool PositionalCharacteristic::operator==(const PositionalCharacteristic& rhs) const {
    return this->z == rhs.z && this->area == rhs.area;
}

bool SegmentSpatialCharacteristics::operator==(const SegmentSpatialCharacteristics& other) const {
    return this->values.size() == other.values.size() && 
           std::equal(this->values.begin(), this->values.end(), other.values.begin());
}

void SegmentSpatialCharacteristics::verifyValidData() const{
    // There must be at least two points
    if(this->values.size() < 2){
        throw cvException("ERROR: there are fewer than two points in a vessel segment.");
    }

    // The areas must be positive
    for(auto const& value : this->values){
        if(value.area < 0){
            throw cvException("ERROR: Negative area value " + std::to_string(value.area) + " found in vessel.");
        }
    }

    // The z-positions must be sequentially increasing
    for(size_t k = 0; k < (this->values.size()-1); ++k){
        double const zk = this->values.at(k).z;
        double const zkplus1 = this->values.at(k+1).z;
        if(zk >= zkplus1){
            throw cvException("ERROR: consecutive z-coordinates of the spatial characteristics must be increasing."
                " Found consecutive values: " + std::to_string(zk) + " -> next value -> " + std::to_string(zkplus1));
        }
    }
}

double SegmentSpatialCharacteristics::length() const {
    return this->values.back().z - this->values.front().z;
}

std::pair<double,double> SegmentSpatialCharacteristics::inletAndOutletAreas() const {
    return {this->values.front().area, this->values.back().area};
}

std::pair<double,double> SegmentSpatialCharacteristics::inletAndOutletZCoordinates() const{
    return {this->values.front().z, this->values.back().z};
}

double SegmentSpatialCharacteristics::getInterpolatedArea(double z) const {
    // If it's outside the domain (which it shouldn't ever be)
    // we'll just use the boundary values.
    if (z <= this->values.front().z) {
        return this->values.front().area;
    }
    if (z >= this->values.back().z) {
        return this->values.back().area;
    }

    // Find the iterator to the z-position just before and after this point
    // (z guaranteed to lie in the domain because of the prior check, unless
    // somehow there's fewer than two points...which means bad input data)
    auto iter = std::find_if(this->values.begin(), this->values.end(),
            [z](const PositionalCharacteristic& pos) { 
                return pos.z >= z; 
            }
        );

    auto iterPrev = std::prev(iter);

    // Return the interpolated area
    return linearInterpolate(z, 
        iterPrev->z, iterPrev->area, iter->z, iter->area);
}

double SegmentSpatialCharacteristics::getInterpolatedRadius(double z) const {
    // Interpolate to get the area at a given z-position.
    double const interpolatedArea = getInterpolatedArea(z);

    // Return the radius
    return sqrt(interpolatedArea / M_PI);
}

std::vector<PositionalCharacteristic> const& SegmentSpatialCharacteristics::getValues() const{
    return values;
}

SegmentSpatialCharacteristics simpleSegmentSpatialCharacteristic(
    double segLength, double inletArea, double outletArea){
    std::vector<double> const z{0, segLength};
    std::vector<double> const area{inletArea, outletArea};

    return SegmentSpatialCharacteristics(z, area);
}
        
} // namespace cvOneD
