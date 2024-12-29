
#include "testJson.h"

#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>

// Test function to demonstrate the serialization
void test_json(){
    cvOneD::options opts;

    opts.modelName = "MyModel";
    opts.nodeName = {"Node1", "Node2", "Node3"};
    opts.nodeXcoord = {1.1, 2.2, 3.3};
    opts.nodeYcoord = {4.4, 5.5, 6.6};
    opts.nodeZcoord = {7.7, 8.8, 9.9};

    // Joint Data
    opts.jointName = {"Joint1", "Joint2", "Joint3"};
    // We would also like to include "jointNode" here
    // once it is properly integrated in all existing
    // input files.
    opts.jointInletName = {"Inlet1", "Inlet2", "Inlet3"};
    opts.jointOutletName = {"Outlet1", "Outlet2", "Outlet3"};

    opts.jointInletListNames = {"IN1", "IN2", "IN3"};
    opts.jointInletListNumber = {1, 1, 1};
    opts.jointInletList = {{0}, {1}, {2}};

    opts.jointOutletListNames = {"OUT1", "OUT2", "OUT3"};
    opts.jointOutletListNumber = {1, 1, 1};
    opts.jointOutletList = {{1}, {2}, {3}};
    
    // Material Data
    opts.materialName = {"Material1", "Material2"};
    opts.materialType = {"Type1", "Type2"};
    opts.materialDensity = {1000.0, 1200.0};
    opts.materialViscosity = {0.3, 0.4};
    opts.materialPRef = {1.0, 1.2};
    opts.materialExponent = {0.5, 0.6};
    opts.materialParam1 = {10.0, 20.0};
    opts.materialParam2 = {30.0, 40.0};
    opts.materialParam3 = {50.0, 60.0};

    // Data tables
    opts.dataTableName = {"R_VALS", "STEADY_FLOW", "PULS_FLOW"};
    opts.dataTableType = {"LIST", "LIST", "LIST"};
    opts.dataTableVals = {
        {0.0, 991.36},
        {0.0, 7.985, 1.0, 7.985},
        {0.0, 0.0, 0.019668108360095, -4.11549971450822, 0.055247073448669, -7.16517105402019}
    };

    // Segment Data
    opts.segmentName = {"Aorta", "iliacR", "iliacL"};
    opts.segmentID = {0, 1, 2};
    opts.segmentLength = {17.670671, 12.997461, 12.997461};
    opts.segmentTotEls = {50, 50, 50};
    opts.segmentInNode = {0, 1, 1};
    opts.segmentOutNode = {1, 3, 2};
    opts.segmentInInletArea = {5.027254990390394, 1.55, 1.55};
    opts.segmentInOutletArea = {1.6068894493599328, 0.3525652531134944, 0.3525652531134944};
    opts.segmentInFlow = {0.0, 0.0, 0.0};
    opts.segmentMatName = {"MAT1", "MAT1", "MAT1"};
    opts.segmentLossType = {"NONE", "NONE", "NONE"};
    opts.segmentBranchAngle = {0.0, 0.0, 0.0};
    opts.segmentUpstreamSegment = {0, 0, 0};
    opts.segmentBranchSegment = {0, 0, 0};
    opts.segmentBoundType = {"NOBOUND", "RCR", "RCR"};
    opts.segmentDataTableName = {"NONE", "RCR_VALS", "RCR_VALS"};

    // Solver Options
    opts.timeStep = 0.001087;
    opts.stepSize = 10;
    opts.maxStep = 100;
    opts.quadPoints = 2;
    opts.inletDataTableName = "STEADY_FLOW";
    opts.boundaryType = "FLOW";
    opts.convergenceTolerance = 1.0e-6;
    opts.useIV = 1;
    opts.useStab = 0;
    opts.outputType = "NONE";

    // Serialize the options into a JSON object
    cvOneD::writeJsonOptions(opts,"out.json");
    
    cvOneD::readJsonOptions("out.json");

}