#include <gtest/gtest.h>
#include <fstream>
#include <string>

#include "cvOneDOptions.h"
#include "cvOneDOptionsLegacySerializer.h"

// Test case for the add function
TEST(LegacySerializer, ParseSimpleFile) {
    // Path to the file in the TestFiles directory
    const std::string filePath = "TestFiles/SimpleArtery.in"; 

    std::ifstream file(filePath);

    file.close();
}
