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

#include <string>
#include <iostream>

#include "cvOneDInputFlagParser.h"

#include "cvOneDOptionsJsonParser.h"
#include "cvOneDOptionsJsonSerializer.h"
#include "cvOneDOptionsLegacySerializer.h"

using namespace std;

namespace cvOneD{

namespace {

void convertLegacyToJsonOptions(const std::string& legacyFilename, const std::string& jsonFilename){
  // Convert legacy format to JSON and print it to file
  cvOneD::options opts{};
  cvOneD::readOptionsLegacyFormat(legacyFilename,&opts);

  cvOneD::writeJsonOptions(opts, jsonFilename);

  std::cout << "Converted legacy file " << legacyFilename <<
               "to json file " << jsonFilename << std::endl;
}

struct ArgOptions{
  std::optional<std::string> jsonInput = std::nullopt;

  std::optional<std::string> legacyConversionInput = std::nullopt;
  std::optional<std::string> jsonConversionOutput = std::nullopt;
};

std::string removeQuotesIfPresent(const std::string& str) {
    std::string result = str;
    if (result.front() == '"' && result.back() == '"') {
        result = result.substr(1, result.length() - 2); 
    }
    return result;
}

ArgOptions parseInputArgs(int argc, char** argv) {
    // Right now, we don't check for bad arguments but we could
    // do that in here if we want more coherent behavior.
    ArgOptions options; 

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];

        if (arg == "-jsonInput" && i + 1 < argc) {
            options.jsonInput = removeQuotesIfPresent(argv[i + 1]);
            i++;
        }

        if (arg == "-legacyToJson" && i + 2 < argc) {
            options.legacyConversionInput = removeQuotesIfPresent(argv[i + 1]);
            options.jsonConversionOutput = removeQuotesIfPresent(argv[i + 2]);
            i++;
        }

    }

    return options;
}

cvOneD::options readLegacyOptions(std::string const& inputFile){
  cvOneD::options opts{};
  cvOneD::readOptionsLegacyFormat(inputFile,&opts);
  return opts;
}

} // namespace

// Parse the incoming arguments and read the options file.
// Also performs option file conversions if requested by user.
//
// Updated behavior: 
//  parse input arg pairs as
//    "-argName argValue -anotherArg anotherValue"
//  Current options:
//    * Run simulation with JSON input file:
//       -jsonInput inputFilename
//    * Convert legacy input to JSON input:
//       -legacyToJson legacyInput.in jsonInput.json
//
// Preserved legacy behavior: 
//   Single input that is a legacy input file, e.g.:
//     ./svOneDSolver inputFilename.in
//
// Legacy behavior is used only if exactly one input is provided. 
//
std::optional<cvOneD::options> parseArgsAndHandleOptions(int argc, char** argv){
  
  // Legacy behavior
  if(argc == 2){
    string inputFile{removeQuotesIfPresent(argv[1])};
    return readLegacyOptions(inputFile);
  }

  // Default behavior
  auto const argOptions = parseInputArgs(argc,argv);

  // Conversion
  if(argOptions.legacyConversionInput && argOptions.jsonConversionOutput){
    convertLegacyToJsonOptions(*argOptions.legacyConversionInput, 
                   *argOptions.jsonConversionOutput);
  }

  // Only return the input args if the user provided them
  if(argOptions.jsonInput){
    return cvOneD::readJsonOptions(*argOptions.jsonInput);
  }

  return std::nullopt;
}

} // namespace cvOneD