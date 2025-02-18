#include <gtest/gtest.h>
#include <fstream>
#include <string>

#include "cvOneDOptions.h"
#include "cvOneDOptionsLegacySerializer.h"

#include "OptionsTestHelpers.hpp"

cvOneD::options simpleArteryOptions() {
    cvOneD::options options;

    // MODEL
    options.modelName = "simpleArtery_Res_";

    // NODE 
    options.nodeName = {"0", "1"};
    options.nodeXcoord = {0.0, 0.0};
    options.nodeYcoord = {0.0, 0.0};
    options.nodeZcoord = {0.0, -20.0};

    // JOINT
    options.jointName = {};
    options.jointNode = {};
    options.jointInletName = {};
    options.jointOutletName = {};

    // JOINTINLET AND JOINTOUTLET 
    options.jointInletListNames = {};
    options.jointInletListNumber = {};
    options.jointInletList = {};
    options.jointOutletListNames = {};
    options.jointOutletListNumber = {};
    options.jointOutletList = {};

    // SEGMENT 
    options.segmentName = {"ARTERY"};
    options.segmentID = {0};
    options.segmentLength = {20.0};
    options.segmentTotEls = {50};
    options.segmentInNode = {0};
    options.segmentOutNode = {1};
    options.segmentInInletArea = {2.0};
    options.segmentInOutletArea = {2.0};
    options.segmentInFlow = {0.0};
    options.segmentMatName = {"MAT1"};
    options.segmentLossType = {"NONE"};
    options.segmentBranchAngle = {0.0};
    options.segmentUpstreamSegment = {0};
    options.segmentBranchSegment = {0};
    options.segmentBoundType = {"RESISTANCE"};
    options.segmentDataTableName = {"RESTABLE"};

    // DATATABLE 
    options.dataTableName = {"RESTABLE", "INLETDATA"};
    options.dataTableType = {"LIST", "LIST"};
    options.dataTableVals = {
        {0.0, 100.0},  // RESTABLE
        {0.0, 200.0, 10.0, 200.0}  // INLETDATA
    };

    // MATERIAL 
    options.materialName = {"MAT1"};
    options.materialType = {"OLUFSEN"};
    options.materialDensity = {1.06};
    options.materialViscosity = {0.04};
    options.materialPRef = {113324.0};
    options.materialExponent = {1.0};
    options.materialParam1 = {2.0e7};
    options.materialParam2 = {-22.5267};
    options.materialParam3 = {8.65e5};

    // SOLVER OPTIONS
    options.timeStep = 0.01;
    options.stepSize = 10;
    options.maxStep = 1000;
    options.quadPoints = 4;
    options.inletDataTableName = "INLETDATA";
    options.boundaryType = "FLOW";
    options.convergenceTolerance = 1.0e-6;
    options.useIV = 1;
    options.useStab = 1;
    options.outputType = "TEXT";

    return options;
}

cvOneD::options bifurcationOptions() {
    cvOneD::options options;

    // MODEL
    options.modelName = "results_bifurcation_R_";

    // NODE 
    options.nodeName = {"0", "1", "2", "3"};
    options.nodeXcoord = {0.0, 0.0, 0.0, 0.0};
    options.nodeYcoord = {0.0, 0.0, -3.25280917510326, 3.25280917510326};
    options.nodeZcoord = {0.0, -8.6, -7.85297602634594, -7.85297602634594};

    // JOINT DATA
    options.jointName = {"JOINT1"};
    options.jointNode = {"1"};  
  
    // Inlet and Outlet Connections for the Joint
    options.jointInletName = {"INSEGS"}; 
    options.jointOutletName = {"OUTSEGS"};

    // Inlet and Outlet Lists
    options.jointInletListNames = {"INSEGS"};
    options.jointInletListNumber = {1};  
    options.jointInletList = {{0}}; 

    options.jointOutletListNames = {"OUTSEGS"};
    options.jointOutletListNumber = {2}; 
    options.jointOutletList = {{1, 2}};

    // SEGMENT 
    options.segmentName = {"seg0", "seg1", "seg2"};
    options.segmentID = {0, 1, 2};
    options.segmentLength = {8.6, 8.5, 8.5};
    options.segmentTotEls = {50, 50, 50};
    options.segmentInNode = {0, 1, 1};
    options.segmentOutNode = {1, 3, 2};
    options.segmentInInletArea = {2.32352192659501, 1.13097335529233, 1.13097335529233};
    options.segmentInOutletArea = {2.32352192659501, 1.13097335529233, 1.13097335529233};
    options.segmentInFlow = {0.0, 0.0, 0.0};
    options.segmentMatName = {"MAT1", "MAT1", "MAT1"};
    options.segmentLossType = {"NONE", "NONE", "NONE"};
    options.segmentBranchAngle = {0.0, 0.0, 0.0};
    options.segmentUpstreamSegment = {0, 0, 0};
    options.segmentBranchSegment = {0, 0, 0};
    options.segmentBoundType = {"NOBOUND", "RESISTANCE", "RESISTANCE"};
    options.segmentDataTableName = {"NONE", "R_VALS", "R_VALS"};

    // DATATABLE 
    options.dataTableName = {"R_VALS", "STEADY_FLOW", "PULS_FLOW"};
    options.dataTableType = {"LIST", "LIST", "LIST"};
    options.dataTableVals = {
        {0.0, 991.36},  // R_VALS
        {0.0, 7.985, 1.0, 7.985},  // STEADY_FLOW
        {
            0.0, 0.0,
            0.019668108360095, -4.11549971450822,
            0.055247073448669, -7.16517105402019,
            0.089913757381125, -1.16130916560675,
            0.113633067440174, 9.27967911020849,
            0.133703252874754, 20.4428655623545,
            0.150124313684865, 32.8883708080991,
            0.162896249870507, 43.9606301469767,
            0.173843623743914, 54.6495650354764,
            0.186615559929556, 67.3593157640345,
            0.204861183051901, 79.6320314281343,
            0.234784004972547, 86.0097422945109,
            0.26872086398011, 78.7468811589053,
            0.294264736351393, 64.7319679994526,
            0.314334921785973, 52.023318573387,
            0.32893142028385, 41.4040199668672,
            0.34535248109396, 29.7287601931208,
            0.363598104216306, 17.3938305987427,
            0.380019165026417, 6.42196693062957,
            0.396440225836527, -5.11194051566898,
            0.422389556499419, -18.901261909892,
            0.455347523345814, -23.8602212809087,
            0.496182965572016, -18.2411583475954,
            0.533485128399922, -10.4792705793446,
            0.575247332435512, -4.00466780439001,
            0.640931575675956, -2.79835885098868,
            0.682440368279291, -5.30400645008189,
            0.726077816913567, -8.52809746163143,
            0.762721110017611, -9.03919130533637,
            0.802405340308712, -7.30944473123762,
            0.846498929521047, -5.55578067364513,
            0.885944229033165, -5.87317544184486,
            0.925563296384544, -7.14939949266443,
            0.968258054490832, -6.10169723438909,
            1.00949316274733, -4.10721983893183,
            1.05237037708484, -3.31554531122748,
            1.087, -1.77083891076532
        }  // PULS_FLOW
    };

    // MATERIAL 
    options.materialName = {"MAT1"};
    options.materialType = {"OLUFSEN"};
    options.materialDensity = {1.06};
    options.materialViscosity = {0.04};
    options.materialPRef = {0};
    options.materialExponent = {2.0};
    options.materialParam1 = {1.0e15};
    options.materialParam2 = {-20};
    options.materialParam3 = {1e9};

    // SOLVER OPTIONS
    options.timeStep = 0.001087;
    options.stepSize = 50;
    options.maxStep = 1000;
    options.quadPoints = 2;
    options.inletDataTableName = "STEADY_FLOW";
    options.boundaryType = "FLOW";
    options.convergenceTolerance = 1.0e-6;
    options.useIV = 1;
    options.useStab = 1;
    options.outputType = "TEXT";

    return options;
}

struct LegacySerializerTestParams {
    std::string filePath;
    cvOneD::options expOptions;
};

class LegacySerializerTest : public ::testing::TestWithParam<LegacySerializerTestParams> {};

INSTANTIATE_TEST_SUITE_P(
    TestReadOptionsLegacyFormat,
    LegacySerializerTest,
    ::testing::Values(
        LegacySerializerTestParams{"TestFiles/BifurcationR.in", bifurcationOptions()},
        LegacySerializerTestParams{"TestFiles/SimpleArtery.in", simpleArteryOptions()}
    )
);

TEST_P(LegacySerializerTest, ParseSimpleFile) {
    // Test parameters
    const auto& params = GetParam();

    // Execute and verify
    cvOneD::options actOptions;
    cvOneD::readOptionsLegacyFormat(params.filePath, &actOptions);
    expectEqOptions(actOptions, params.expOptions);
}
