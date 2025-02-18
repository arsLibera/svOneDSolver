#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>

#include "cvOneDOptionsJsonSerializer.h"

using json = nlohmann::ordered_json;

namespace cvOneD{

namespace {

// Serialize cvStringVec into a JSON array (as a plain array)
nlohmann::ordered_json to_json(const cvStringVec& vec) {
    nlohmann::ordered_json j = nlohmann::ordered_json::array();
    for (const auto& str : vec) {
        j.push_back(str);
    }
    return j;
}

// Serialize cvLongVec into a JSON array (as a plain array)
nlohmann::ordered_json to_json(const cvLongVec& vec) {
    nlohmann::ordered_json j = nlohmann::ordered_json::array();
    for (const auto& num : vec) {
        j.push_back(num);
    }
    return j;
}

// Serialize cvDoubleVec into a JSON array (as a plain array)
nlohmann::ordered_json to_json(const cvDoubleVec& vec) {
    nlohmann::ordered_json j = nlohmann::ordered_json::array();
    for (const auto& num : vec) {
        j.push_back(num);
    }
    return j;
}

// Serialize cvLongMat into a JSON array of arrays (matrix)
nlohmann::ordered_json to_json(const cvLongMat& mat) {
    nlohmann::ordered_json j = nlohmann::ordered_json::array();
    for (const auto& row : mat) {
        nlohmann::ordered_json jRow = nlohmann::ordered_json::array();
        for (const auto& val : row) {
            jRow.push_back(val);
        }
        j.push_back(jRow);
    }
    return j;
}

template <typename... Containers>
void check_consistent_size(const std::string& dataName, const Containers&... containers) {
    auto size = [](const auto& container) { return container.size(); };
    if (!((size(containers) == size(std::get<0>(std::tie(containers...)))) && ...)) {
        throw std::runtime_error(dataName + ": All containers must have the same size.");
    }
}

// Serialize Node data into a JSON array, returning only the array
nlohmann::ordered_json serialize_node_data(const cvStringVec& nodeName, const cvDoubleVec& nodeXcoord, const cvDoubleVec& nodeYcoord, const cvDoubleVec& nodeZcoord) {
    nlohmann::ordered_json nodes = nlohmann::ordered_json::array();
    
    check_consistent_size("nodes", nodeName, nodeXcoord, nodeYcoord, nodeZcoord);

    size_t n = nodeName.size();
    for (size_t i = 0; i < n; ++i) {
        nlohmann::ordered_json node;
        node["name"] = nodeName.at(i);
        node["x"] = nodeXcoord.at(i);
        node["y"] = nodeYcoord.at(i);
        node["z"] = nodeZcoord.at(i);
        nodes.push_back(node);
    }
    
    return nodes;  // Just return the array of nodes
}

nlohmann::ordered_json serialize_joint_data(const options& opts) {
    nlohmann::ordered_json j = nlohmann::ordered_json::array();

    // Check consistency of the joint inlet and outlet vectors
    check_consistent_size("joints", opts.jointName,
                          opts.jointInletListNames, opts.jointInletListNumber,
                          opts.jointOutletListNames, opts.jointOutletListNumber,
                          opts.jointInletList, opts.jointOutletList);

    size_t jointCount = opts.jointName.size();

    // There must be enough nodes for us to make associations. This is 
    // an implicit assumption made in the current joint creation code.
    if( opts.nodeName.size() < jointCount ){
        throw std::runtime_error("The implicit joint-node association requires that there be at least as many nodes as there are joints.");
    }

    for (size_t i = 0; i < jointCount; ++i) {
        nlohmann::ordered_json joint;

        // Add JOINT data
        // We're adding the node based on the index so we
        // can start incorporating the explicit association
        // from the implicit one currently used in the code.
        joint["joint"] = {
            {"id", "J" + std::to_string(i + 1)},
            {"associatedNode", opts.nodeName.at(i)},
            {"inletName", opts.jointInletListNames.at(i)},
            {"outletName", opts.jointOutletListNames.at(i)}
        };

        // Add JOINTINLET data
        joint["jointInlet"] = {
            {"name", opts.jointInletListNames.at(i)},
            {"totalSegments", opts.jointInletListNumber.at(i)},
            {"segments", to_json(opts.jointInletList.at(i))}
        };

        // Add JOINTOUTLET data
        joint["jointOutlet"] = {
            {"name", opts.jointOutletListNames.at(i)},
            {"totalSegments", opts.jointOutletListNumber.at(i)},
            {"segments", to_json(opts.jointOutletList.at(i))}
        };

        j.push_back(joint);
    }

    return j;
}

nlohmann::ordered_json serialize_material_data(const options& opts) {
    nlohmann::ordered_json materials = nlohmann::ordered_json::array();

    // Ensure all material-related vectors are the same size
    check_consistent_size("materials",
                          opts.materialName, opts.materialType, opts.materialDensity, 
                          opts.materialViscosity, opts.materialPRef, opts.materialExponent, 
                          opts.materialParam1, opts.materialParam2, opts.materialParam3);

    size_t n = opts.materialName.size();
    for (size_t i = 0; i < n; ++i) {
        nlohmann::ordered_json material;
        material["name"] = opts.materialName.at(i);
        material["type"] = opts.materialType.at(i);
        material["density"] = opts.materialDensity.at(i);
        material["viscosity"] = opts.materialViscosity.at(i);
        material["pRef"] = opts.materialPRef.at(i);
        material["exponent"] = opts.materialExponent.at(i);
        material["param1"] = opts.materialParam1.at(i);
        material["param2"] = opts.materialParam2.at(i);
        material["param3"] = opts.materialParam3.at(i);
        materials.push_back(material);
    }

    return materials;  // Just return the array of materials
}

// Serialize data tables into a JSON array
nlohmann::ordered_json serialize_data_tables(const cvStringVec& dataTableName, 
                                             const cvStringVec& dataTableType, 
                                             const std::vector<cvDoubleVec>& dataTableVals) {
    nlohmann::ordered_json dataTables = nlohmann::ordered_json::array();

    check_consistent_size("data tables", dataTableName, dataTableType, dataTableVals);

    size_t n = dataTableName.size();
    for (size_t i = 0; i < n; ++i) {
        nlohmann::ordered_json table;
        table["name"] = dataTableName.at(i);
        table["type"] = dataTableType.at(i);
        table["values"] = to_json(dataTableVals.at(i));  // Use the existing `to_json` for cvDoubleVec
        dataTables.push_back(table);
    }

    return dataTables;  // Return the array of data tables
}

nlohmann::ordered_json serializeSegmentData(const options& opts) {
    nlohmann::ordered_json segments = nlohmann::ordered_json::array();

    // Ensure all segment-related vectors are the same size
    check_consistent_size("segments", opts.segmentName, opts.segmentID, opts.segmentLength, 
                          opts.segmentTotEls, opts.segmentInNode, opts.segmentOutNode, 
                          opts.segmentInInletArea, opts.segmentInOutletArea, opts.segmentInFlow, 
                          opts.segmentMatName, opts.segmentLossType, opts.segmentBranchAngle, 
                          opts.segmentUpstreamSegment, opts.segmentBranchSegment, 
                          opts.segmentBoundType, opts.segmentDataTableName);

    size_t n = opts.segmentName.size();
    for (size_t i = 0; i < n; ++i) {
        nlohmann::ordered_json segment;

        segment["name"] = opts.segmentName.at(i);
        segment["id"] = opts.segmentID.at(i);
        segment["length"] = opts.segmentLength.at(i);
        segment["totalElements"] = opts.segmentTotEls.at(i);
        segment["inNode"] = opts.segmentInNode.at(i);
        segment["outNode"] = opts.segmentOutNode.at(i);
        segment["inletArea"] = opts.segmentInInletArea.at(i);
        segment["outletArea"] = opts.segmentInOutletArea.at(i);
        segment["flow"] = opts.segmentInFlow.at(i);
        segment["materialName"] = opts.segmentMatName.at(i);
        segment["lossType"] = opts.segmentLossType.at(i);
        segment["branchAngle"] = opts.segmentBranchAngle.at(i);
        segment["upstreamSegment"] = opts.segmentUpstreamSegment.at(i);
        segment["branchSegment"] = opts.segmentBranchSegment.at(i);
        segment["boundaryType"] = opts.segmentBoundType.at(i);
        segment["dataTableName"] = opts.segmentDataTableName.at(i);

        segments.push_back(segment);
    }

    return segments;
}

nlohmann::ordered_json serializeSolverOptions(options const& opts) {
    nlohmann::ordered_json solverOptions;

    solverOptions["timeStep"] = opts.timeStep;
    solverOptions["stepSize"] = opts.stepSize;
    solverOptions["maxStep"] = opts.maxStep;
    solverOptions["quadPoints"] = opts.quadPoints;
    solverOptions["inletDataTableName"] = opts.inletDataTableName;
    solverOptions["boundaryType"] = opts.boundaryType;
    solverOptions["convergenceTolerance"] = opts.convergenceTolerance;
    solverOptions["useIV"] = opts.useIV;
    solverOptions["useStab"] = opts.useStab;
    solverOptions["outputType"] = opts.outputType;

    return solverOptions;
}

// Serialize all options into a single JSON object
nlohmann::ordered_json serialize_options(const options& opts) {
    nlohmann::ordered_json j;
    
    // Serialize and add each part of the options data
    j["modelName"] = opts.modelName;  

    j["solverOptions"] = serializeSolverOptions(opts);

    j["materials"] = serialize_material_data(opts);

    j["nodes"] = serialize_node_data(opts.nodeName, opts.nodeXcoord, opts.nodeYcoord, opts.nodeZcoord); 

    j["joints"] = serialize_joint_data(opts); 

    j["segments"] = serializeSegmentData(opts);

    j["dataTables"] = serialize_data_tables(opts.dataTableName, opts.dataTableType, opts.dataTableVals);

    return j;
}

} // namespace

void writeJsonOptions(options const& opts, std::string const& fileName){
    // Serialize options into JSON
    nlohmann::ordered_json jsonData = cvOneD::serialize_options(opts);

    // Write JSON to the specified file
    std::ofstream outFile(fileName);
    if (!outFile) {
        throw std::runtime_error("Unable to open file for writing: " + fileName);
    }
    outFile << jsonData.dump(2); // Write with 2-space indentation
    outFile.close();
}

} // namespace cvOneD


