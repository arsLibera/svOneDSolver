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

 #include <gtest/gtest.h>
 #include <gmock/gmock.h>  
 
 #include "cvOneDSegmentSpatialCharacteristics.h"

namespace{

// Helper functions to evaluate the actual values
std::vector<double> getInterpolatedAreas(const cvOneD::SegmentSpatialCharacteristics& seg,
                                         const std::vector<double>& zValues) {
    std::vector<double> interpolatedAreas;
    for (double z : zValues) {
        interpolatedAreas.push_back(seg.getInterpolatedArea(z));
    }
    return interpolatedAreas;
}

std::vector<double> getInterpolatedRadii(const cvOneD::SegmentSpatialCharacteristics& seg,
                                         const std::vector<double>& zValues) {
    std::vector<double> interpolatedRadii;
    for (double z : zValues) {
        interpolatedRadii.push_back(seg.getInterpolatedRadius(z));
    }
    return interpolatedRadii;
}

// Matcher to compare vectors of doubles
MATCHER_P2(ElementsAreEqual, expected, tolerance, "") {
    if (arg.size() != expected.size()) {
        return false;
    }
    for (size_t i = 0; i < arg.size(); ++i) {
        if (std::abs(arg[i] - expected[i]) > tolerance) {
            return false;
        }
    }
    return true;
}

} // namespace

TEST(SegmentSpatialCharacteristics, legacyConversions){
    // Verify how we process the legacy conversions to and from
    // the spatial characteristics
    double const segLength = 3.1;
    double const inletArea = 0.99;
    double const outletArea = 0.75;

    auto const actSegment = cvOneD::simpleSegmentSpatialCharacteristic(segLength, inletArea, outletArea);

    // Check that there are exactly two positional values
    cvOneD::SegmentSpatialCharacteristics const expSegment({0, segLength},{inletArea, outletArea});
    EXPECT_EQ(actSegment, expSegment);

    // Check length and inlet/outlet values
    auto const [actInletZ, actOutletZ] = actSegment.inletAndOutletZCoordinates();
    auto const [actInletA, actOutletA] = actSegment.inletAndOutletAreas();

    EXPECT_EQ(actSegment.length(), segLength);
    EXPECT_EQ(actInletZ, 0.0);
    EXPECT_EQ(actOutletZ, segLength);
    EXPECT_EQ(actInletA, inletArea);
    EXPECT_EQ(actOutletA, outletArea);
}
 
TEST(SegmentSpatialCharacteristics, ctorThrows){
    // If the Z-coordinates and areas do not have the same number of elements, 
    // should throw
    auto const func = [](){ cvOneD::SegmentSpatialCharacteristics({0, 0.1},{0.2}); };
    EXPECT_THROW(func(), std::exception);
}

TEST(SegmentSpatialCharacteristics, verifyValidData) {
    using cvOneD::SegmentSpatialCharacteristics;

    // Valid input (should NOT throw)
    SegmentSpatialCharacteristics valid({0.0, 1.0, 2.0}, {0.5, 0.6, 0.7});
    EXPECT_NO_THROW(valid.verifyValidData());

    // Fewer than two points (should throw)
    SegmentSpatialCharacteristics tooShort({0.0}, {0.5});
    EXPECT_THROW(tooShort.verifyValidData(), std::exception);

    // Negative area (should throw)
    SegmentSpatialCharacteristics negativeArea({0.0, 1.0}, {0.5, -0.3});
    EXPECT_THROW(negativeArea.verifyValidData(), std::exception);

    // Non-increasing z values (should throw)
    SegmentSpatialCharacteristics nonIncreasingZ({0.0, 1.0, 0.5}, {0.5, 0.6, 0.7});
    EXPECT_THROW(nonIncreasingZ.verifyValidData(), std::exception);
}

TEST(SegmentSpatialCharacteristics, getInterpolatedAreaOrRadius) {
    cvOneD::SegmentSpatialCharacteristics const segment({0.0, 1.0, 2.0}, {3.0, 2.1, 2.5});
    std::vector<double> const zPoints = {0.0, 1.0, 2.0, 0.5, 1.5, -1.0, 3.0};
    std::vector<double> const expectedAreas = {3.0, 2.1, 2.5, 2.55, 2.3, 3.0, 2.5};
    std::vector<double> expectedRadii;
    for (double area : expectedAreas) {
        expectedRadii.push_back(std::sqrt(area / M_PI));
    }
    
    std::vector<double> const actualAreas = getInterpolatedAreas(segment, zPoints);
    std::vector<double> const actualRadii = getInterpolatedRadii(segment, zPoints);
    EXPECT_THAT(actualAreas, ElementsAreEqual(expectedAreas, 1e-14));
    EXPECT_THAT(actualRadii, ElementsAreEqual(expectedRadii, 1e-14));
}

