#pragma once

#include "WeatherData.h"
#include <vector>
#include <string>
#include <functional>

class CSVReader
{
    public:
        CSVReader();
        static std::vector<std::vector<std::string>> readCSV(const std::string csvFilename);

    private:
        static std::vector<std::string> tokenise(const std::string& csvLine, char seperator);
        static void showLoadingBar(int current, int total);
};