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

#include "cvOneDCreateAndRunModel.h"
#include "cvOneDInputFlagParser.h"
#include "cvOneDOptionsJsonSerializer.h"
#include "cvOneDOptionsLegacySerializer.h"

using namespace std;

namespace {

void WriteHeader(){
  printf("---------------------------------\n");
  printf(" oneDSolver \n");
  printf(" 1D Finite Element Hemodynamics  \n");
  printf("---------------------------------\n");
}

void echoOptions(const cvOneD::options& opts){
  // Print Input Data Echo
  string fileName("echo.out");
  cvOneD::printToLegacyFile(opts,fileName);

  // For now, we'll just duplicate the printing of
  // the echo of the options to JSON as well
  string jsonFilename("echo.json");
  cvOneD::writeJsonOptions(opts, jsonFilename);
}

} // namespace

// =============
// MAIN FUNCTION
// =============
int main(int argc, char** argv){

  // Write Program Header
  WriteHeader();

  try{

    auto const simOpts = cvOneD::parseArgsAndHandleOptions(argc,argv);
    
    if(simOpts){
      cvOneD::validateOptions(*simOpts);
      echoOptions(*simOpts);

      // The simulation options were defined so we can run the simulation
      cvOneD::runOneDSolver(*simOpts);
    } else{
      // The user could just want to convert legacy input *.in -> *.json 
      // so we don't error but we notify the user that no simulation
      // is run.
      std::cout << "The simulation was not run because" 
                   " no input file was provided." << std::endl;
    }

  }catch(exception& e){
    // Print Exception Message
    printf("%s\n",e.what());
    // Execution Terminated
    printf("Terminated.\n");
    return 1;
  }
  printf("Completed!\n");
  return 0;

}
