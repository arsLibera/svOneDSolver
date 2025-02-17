#include "cvOneDOptions.h"

// Compares two sets of options using gtest macros
void expectEqOptions(const cvOneD::options& actual, const cvOneD::options& expected);

// Read the file contents to a string
std::string readFileContents(const std::string& filePath);