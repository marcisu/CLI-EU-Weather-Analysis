#pragma once

#include "Candlestick.h"
#include "CSVReader.h"
#include <string>
#include <vector>
#include <map>

class WeatherData
{
    public:
        WeatherData(std::string filename);
        std::vector<std::string> getCountryList() const;
        std::vector<std::string> getYearsList() const;
        std::vector<std::vector<double>> getTemperatures(int countryIndex) const;

        std::vector<Candlestick> computeCandlestick(const std::vector<std::vector<double>>& temperatures, 
                                                    int startYearIndex) const;  

        std::vector<std::vector<double>> filterDateRange(   const std::vector<std::vector<double>>& temperatures, 
                                                            const int startyear,
                                                            const int endyear) const;

        std::vector<Candlestick> predictTemperatures(const std::vector<Candlestick>& data, int years, int startYear) const;
    
    private:
        std::vector<std::vector<std::string>> dataset;
};