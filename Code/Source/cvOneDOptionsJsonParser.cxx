#include <iostream>
#include <fstream>
#include <sstream>

#include <nlohmann/json.hpp>

#include "cvOneDOptionsJsonSerializer.h"

using json = nlohmann::ordered_json;

namespace cvOneD{

namespace {

std::string readFileContents(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filePath);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// Helper function for error handling
template <typename Func>
void wrapParsingErrors(const std::string& sectionName, Func&& func) {
    try {
        func();
    } catch (const std::exception& e) {
        throw std::runtime_error("Error parsing '" + sectionName + "': " + std::string(e.what()));
    }
}

void parseNodeData(const nlohmann::ordered_json& jsonData, options& opts) try {
    // Ensure "nodes" exists and is an array
    if (!jsonData.contains("nodes") || !jsonData["nodes"].is_array()) {
        throw std::invalid_argument("Expected 'nodes' to be an array in JSON input.");
    }

    const auto& nodes = jsonData["nodes"];

    for (const auto& node : nodes) {
        if (!node.is_object()) {
            throw std::invalid_argument("Each entry in 'nodes' must be a JSON object.");
        }

        // Ensure required fields exist
        opts.nodeName.push_back(node.at("name").get<std::string>());
        opts.nodeXcoord.push_back(node.at("x").get<double>());
        opts.nodeYcoord.push_back(node.at("y").get<double>());
        opts.nodeZcoord.push_back(node.at("z").get<double>());
    }
} catch (const std::exception& e) {
    throw std::runtime_error("Error parsing 'nodes': " + std::string(e.what()));
}

void parseJointData(const nlohmann::ordered_json& jsonData, options& opts) try {
    // Ensure "joints" exists and is an array
    if (!jsonData.contains("joints") || !jsonData["joints"].is_array()) {
        throw std::invalid_argument("Expected 'joints' to be an array in JSON input.");
    }

    const auto& joints = jsonData["joints"];

    for (const auto& jointEntry : joints) {
        if (!jointEntry.is_object()) {
            throw std::invalid_argument("Each entry in 'joints' must be a JSON object.");
        }

        // Parse JOINT data
        const auto& joint = jointEntry.at("joint"); // Ensures "joint" exists
        opts.jointName.push_back(joint.at("id").get<std::string>());
        opts.jointNode.push_back(joint.at("associatedNode").get<std::string>());
        opts.jointInletName.push_back(joint.at("inletName").get<std::string>());
        opts.jointOutletName.push_back(joint.at("outletName").get<std::string>());

        // Parse JOINTINLET data
        const auto& jointInlet = jointEntry.at("jointInlet"); // Ensures "jointInlet" exists
        opts.jointInletListNames.push_back(jointInlet.at("name").get<std::string>());
        opts.jointInletListNumber.push_back(jointInlet.at("totalSegments").get<int>());
        opts.jointInletList.emplace_back(jointInlet.at("segments").get<std::vector<long>>());

        // Parse JOINTOUTLET data
        const auto& jointOutlet = jointEntry.at("jointOutlet"); // Ensures "jointOutlet" exists
        opts.jointOutletListNames.push_back(jointOutlet.at("name").get<std::string>());
        opts.jointOutletListNumber.push_back(jointOutlet.at("totalSegments").get<int>());
        opts.jointOutletList.emplace_back(jointOutlet.at("segments").get<std::vector<long>>());
    }

} catch (const std::exception& e) {
    throw std::runtime_error("Error parsing 'joints': " + std::string(e.what()));
}

void parse_material_data(const nlohmann::json& json, options& opts) try {
    if (!json.contains("materials") || !json["materials"].is_array()) {
        throw std::invalid_argument("Expected 'materials' to be an array in JSON input.");
    }

    const auto& materials = json["materials"];
    
    for (const auto& material : materials) {
        if (!material.is_object()) {
            throw std::invalid_argument("Each material in 'materials' must be a JSON object.");
        }

        opts.materialName.push_back(material.at("name").get<std::string>());
        opts.materialType.push_back(material.at("type").get<std::string>());
        opts.materialDensity.push_back(material.at("density").get<double>());
        opts.materialViscosity.push_back(material.at("viscosity").get<double>());
        opts.materialPRef.push_back(material.at("pRef").get<double>());
        opts.materialExponent.push_back(material.at("exponent").get<double>());
        opts.materialParam1.push_back(material.at("param1").get<double>());
        opts.materialParam2.push_back(material.at("param2").get<double>());
        opts.materialParam3.push_back(material.at("param3").get<double>());
    }

} catch (const std::exception& e) {
    throw std::runtime_error("Error parsing material data: " + std::string(e.what()));
}

void parseDataTables(const nlohmann::ordered_json& jsonData, options& opts) try {
    if (!jsonData.contains("dataTables") || !jsonData["dataTables"].is_array()) {
        throw std::invalid_argument("Expected 'dataTables' to be an array in JSON input.");
    }

    const auto& dataTables = jsonData["dataTables"];

    // Parse each data table entry
    for (const auto& table : dataTables) {
        if (!table.is_object()) {
            throw std::invalid_argument("Each entry in 'dataTables' must be a JSON object.");
        }

        // Ensure required fields exist and have the correct types
        const auto& name = table.at("name").get<std::string>();
        const auto& type = table.at("type").get<std::string>();

        // Parse "values" array
        if (!table.contains("values") || !table["values"].is_array()) {
            throw std::invalid_argument("Expected 'values' to be an array in each data table.");
        }

        cvDoubleVec values;
        for (const auto& val : table["values"]) {
            if (!val.is_number()) {
                throw std::invalid_argument("Each 'values' entry must be a number.");
            }
            values.push_back(val.get<double>());
        }

        // Append parsed data to the options object
        opts.dataTableName.push_back(name);
        opts.dataTableType.push_back(type);
        opts.dataTableVals.push_back(values);
    }
} catch (const std::exception& e) {
    throw std::runtime_error("Error parsing 'dataTables': " + std::string(e.what()));
}

void parseSegmentData(const nlohmann::ordered_json& jsonData, options& opts) try {
    const auto& segments = jsonData.at("segments"); // Throws if "segments" does not exist
    if (!segments.is_array()) {
        throw std::invalid_argument("'segments' must be an array.");
    }

    for (const auto& segment : segments) {
        if (!segment.is_object()) {
            throw std::invalid_argument("Each entry in 'segments' must be a JSON object.");
        }

        // Parse required fields without defaults; throw if missing
        opts.segmentName.push_back(segment.at("name").get<std::string>());
        opts.segmentID.push_back(segment.at("id").get<int>());
        opts.segmentLength.push_back(segment.at("length").get<double>());
        opts.segmentTotEls.push_back(segment.at("totalElements").get<int>());
        opts.segmentInNode.push_back(segment.at("inNode").get<int>());
        opts.segmentOutNode.push_back(segment.at("outNode").get<int>());
        opts.segmentInInletArea.push_back(segment.at("inletArea").get<double>());
        opts.segmentInOutletArea.push_back(segment.at("outletArea").get<double>());
        opts.segmentInFlow.push_back(segment.at("flow").get<double>());
        opts.segmentMatName.push_back(segment.at("materialName").get<std::string>());
        opts.segmentLossType.push_back(segment.at("lossType").get<std::string>());
        opts.segmentBranchAngle.push_back(segment.at("branchAngle").get<double>());
        opts.segmentUpstreamSegment.push_back(segment.at("upstreamSegment").get<int>());
        opts.segmentBranchSegment.push_back(segment.at("branchSegment").get<int>());
        opts.segmentBoundType.push_back(segment.at("boundaryType").get<std::string>());
        opts.segmentDataTableName.push_back(segment.at("dataTableName").get<std::string>());
    }
} catch (const std::exception& e) {
    throw std::runtime_error("Error parsing segment data: " + std::string(e.what()));
}

void parseSolverOptions(const nlohmann::ordered_json& jsonData, options& opts) try {
    // Ensure "solverOptions" section exists
    const auto& solverOptions = jsonData.at("solverOptions");

    opts.timeStep = solverOptions.at("timeStep").get<double>();
    opts.stepSize = solverOptions.at("stepSize").get<long>();
    opts.maxStep = solverOptions.at("maxStep").get<long>();
    opts.quadPoints = solverOptions.at("quadPoints").get<long>();
    opts.inletDataTableName = solverOptions.at("inletDataTableName").get<std::string>();
    opts.boundaryType = solverOptions.at("boundaryType").get<std::string>();
    opts.convergenceTolerance = solverOptions.at("convergenceTolerance").get<double>();
    opts.useIV = solverOptions.at("useIV").get<int>();
    opts.useStab = solverOptions.at("useStab").get<int>();

    // Doesn't seem like this is used...but we'll provide a default anyway
    opts.outputType = solverOptions.value("outputType", "None");    

} catch (const std::exception& e) {
    throw std::runtime_error("Error parsing 'solverOptions': " + std::string(e.what()));
}

options parseJsonOptions(const std::string& jsonString) {
    json jsonData;
    options opts;

    try{
        jsonData = json::parse(jsonString);
    } catch (const std::exception& e) {
        throw std::runtime_error("Error parsing json file: " + std::string(e.what()));
    }

    opts.modelName = jsonData.value("modelName", "");
    parseNodeData(jsonData,opts);
    parseJointData(jsonData,opts);
    parse_material_data(jsonData,opts);
    parseDataTables(jsonData,opts);
    parseSegmentData(jsonData, opts);
    parseSolverOptions(jsonData, opts);

    return opts;
}

} // namespace

options readJsonOptions(std::string const& inputFile){
    auto const jsonStr = readFileContents(inputFile);

    return parseJsonOptions(jsonStr);
}

} // namespace cvOneD


