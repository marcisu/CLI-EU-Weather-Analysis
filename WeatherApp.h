#pragma once

#include <vector>
#include "WeatherData.h"

class WeatherApp
{
    public:
        WeatherApp();
        void init();
    
    private:
        // Functions for user interaction, menu and data validation ALL TASKS
        void printMenu();
        void printHelp();
        void processUserOption(int userOption);
        int getUserInputOption(const std::string& prompt);
        bool validateDateRange(int startYear, int endYear);
        std::pair<int, int> getDateRange();
        std::map<int, size_t> yearToIndex();

        // Functions for getting Candlestick Data and printing as a table or a plot
        std::vector<Candlestick> getCandlestickData(); 
        void printCountryList(); 
        void printTable(const std::vector<Candlestick>& tableData) const; // TASK 1
        void plot(const std::vector<Candlestick>& plotData) const; // TASK 2

        // Main functions executed
        void printCandlestickData(); // TASK 1
        void plotCandlestickData(); // TASK 2
        void predictWeather(); // TASK 4

        WeatherData weatherData{"dataset1980-2019.csv"};
};