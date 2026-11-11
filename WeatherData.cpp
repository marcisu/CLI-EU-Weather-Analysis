#include "WeatherData.h"
#include "CSVReader.h"
#include <iostream>
#include <iomanip>
#include <numeric> 
#include <map>
#include <stdexcept>
#include <set>
#include <functional>

////// CODE ADAPTED FROM MERKEL MAIN APPLICATION //////

WeatherData::WeatherData(std::string filename)
{
    dataset = CSVReader::readCSV(filename);
}


////// MY CODE STARTS BELOW //////

/** Return vector of all countries codes in the dataset */
std::vector<std::string> WeatherData::getCountryList() const
{
    std::vector<std::string> countries(dataset[0].begin() + 1, dataset[0].end());
    return countries;
}

/** Returns vector containing strings for each unique year in the dataset */
std::vector<std::string> WeatherData::getYearsList() const
{
    std::vector<std::string> years;
    std::set<std::string> uniqueYears;

    if (dataset.empty() || dataset[0].empty()) {
        std::cerr << "Dataset is empty or invalid." << std::endl;
        return {};
    }

    // Iterate through the dataset starting from the second row (first row is headers)
    for (size_t i = 1; i < dataset.size(); ++i) {
        try {
            // Extract the year from the timestamp (assuming first column is timestamp in format "YYYY-MM-DDTHH:MM:SSZ")
            std::string timestamp = dataset[i][0];
            std::string year = timestamp.substr(0, 4);

            // Check if the year is unique
            if (uniqueYears.find(year) == uniqueYears.end()) {
                uniqueYears.insert(year);
                years.push_back(year);
            }

        } catch (const std::exception& e) {
            std::cerr << "Error parsing year from timestamp: " << e.what() << std::endl;
        }
    }
    return years;
}

// TASK 1
/** Retrieve temperatures for a specific country grouped by year */
std::vector<std::vector<double>> WeatherData::getTemperatures(int countryIndex) const
{
    std::vector<std::vector<double>> temperaturesByYear;

    if (dataset.empty() || dataset[0].empty()) {
        std::cerr << "Dataset is empty or incorrectly formated." << std::endl;
        return temperaturesByYear;
    }

    // Retrieve the list of countries
    auto countries = getCountryList();

    // Validate the country index
    if (countryIndex < 0 || countryIndex > countries.size()) {
        std::cerr << "Country index is out of bounds." << std::endl;
        return temperaturesByYear;
    }

    // Get the country name based on the index
    std::string country = countries[countryIndex];

    try {
        // Find the index of the country in the dataset header
        size_t datasetIndex = std::distance(dataset[0].begin(), std::find(dataset[0].begin(), dataset[0].end(), country));
        
        if (datasetIndex == dataset[0].size()) {
            throw std::invalid_argument("Country not found in dataset.");
        }

        // Map to collect temperatures by year
        std::map<int, std::vector<double>> yearToTemperatures;

        // Iterate through dataset to group temperatures by year for the specified country
        for (size_t i = 1; i < dataset.size(); ++i) {
            std::string timestamp = dataset[i][0];
            int year = std::stoi(timestamp.substr(0, 4));
            double temperature = std::stod(dataset[i][datasetIndex]);

            yearToTemperatures[year].push_back(temperature);
        }

        // Transfer the map contents to a 2D vector
        for (const auto& yearTempsPair : yearToTemperatures) {
            temperaturesByYear.push_back(yearTempsPair.second);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error retrieving temperatures: " << e.what() << std::endl;
    }

    return temperaturesByYear;
}

// TASK 1
/** Return vector of Candlesticks for each countries */
std::vector<Candlestick> WeatherData::computeCandlestick(const std::vector<std::vector<double>>& countryTemperatures, 
                                                         int startYearIndex) const 
{
    std::vector<Candlestick> candlesticks;

    // // Get the list of years as strings
    auto years = getYearsList();

    double previousYearMean = 0.0;

    // Iterate through all years
    for (size_t i = 0; i < countryTemperatures.size(); ++i) {
        const std::vector<double>& yearlyTemperatures = countryTemperatures[i];

        if (yearlyTemperatures.empty()) continue;

        // Calculate high, low, open, and close temperatures
        double high = *std::max_element(yearlyTemperatures.begin(), yearlyTemperatures.end());
        double low = *std::min_element(yearlyTemperatures.begin(), yearlyTemperatures.end());
        double close = std::accumulate(yearlyTemperatures.begin(), yearlyTemperatures.end(), 0.0) / yearlyTemperatures.size();
        // For first year, make open = same year close, else, open = last year close
        double open = (i == 0) ? close : previousYearMean;

        // Create a Candlestick object and add it to the list
        candlesticks.emplace_back(years[startYearIndex + i], open, high, low, close);

        // Update the previous year mean temperature
        previousYearMean = close;
    }
    return candlesticks;
}    


// TASK 3
/** Filter data by country and year */
std::vector<std::vector<double>> WeatherData::filterDateRange(  const std::vector<std::vector<double>>& temperatures, 
                                                                const int startYear,
                                                                const int endYear) const
{
    std::vector<std::vector<double>> filteredData;
    
    // Create a map to store year-to-index mapping
    std::vector<std::string> years = getYearsList();
    std::map<int, size_t> yearToIndex;
    for (size_t i = 0; i < years.size(); ++i) {
        int year = std::stoi(years[i]);
        yearToIndex[year] = i;
    }

    // Find start index and end index in temperatures vector
    auto startFilter = yearToIndex.find(startYear);
    auto endFilter = yearToIndex.find(endYear);

    size_t startIndex = startFilter->second;
    size_t endIndex = endFilter->second;

    // Iterate through temperatures vector and collect filtered data
    for (size_t i = startIndex; i <= endIndex; ++i) {
        filteredData.push_back(temperatures[i]);
    }

    return filteredData;
}

// TASK 4
/** Predict Candlestick data for specified number of yearts using Exponential Moving Average */
std::vector<Candlestick> WeatherData::predictTemperatures(  const std::vector<Candlestick>& countryData, 
                                                            int years, 
                                                            int startYear) const
{
    std::vector<Candlestick> predictions;
    if (countryData.empty()) {
        std::cerr << "No data available for predictions." << std::endl;
        return predictions;
    }

    // // Define the number of periods for EMA
    int N = countryData.size();
    // Define alpha (Smoothing factor) as:
    double alpha = 2.0 / (N + 1);

    // Initialize the EMAs for high, low, and close
    double ema_high = countryData[0].high;
    double ema_low = countryData[0].low;
    double ema_close = countryData[0].close;

    // Calculate EMA for each candlestick
    for (const auto& candlestick : countryData) {
        ema_high = alpha * candlestick.high + (1 - alpha) * ema_high;
        ema_low = alpha * candlestick.low + (1 - alpha) * ema_low;
        ema_close = alpha * candlestick.close + (1 - alpha) * ema_close;
    }

    // Store the last observed EMAs
    double lastEmaHigh = ema_high;
    double lastEmaLow = ema_low;
    double lastEmaClose = ema_close;

    // Predict EMAs for each year
    for (int i = 0; i < years; ++i) {
        double nextEmaHigh = alpha * countryData.back().high + (1 - alpha) * lastEmaHigh;
        double nextEmaLow = alpha * countryData.back().low + (1 - alpha) * lastEmaLow;
        double nextEmaClose = alpha * countryData.back().close + (1 - alpha) * lastEmaClose;
        std::string startYearStr = std::to_string(startYear + i);

        predictions.push_back({startYearStr, lastEmaClose, nextEmaHigh, nextEmaLow, nextEmaClose});

        // Update last observed EMAs for the next iteration
        lastEmaHigh = nextEmaHigh;
        lastEmaLow = nextEmaLow;
        lastEmaClose = nextEmaClose;
    }

    return predictions;
}

////// END OF MY CODE //////
