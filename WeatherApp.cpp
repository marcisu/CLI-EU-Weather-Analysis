#include "Candlestick.h"
#include "CSVReader.h"
#include "WeatherData.h"
#include "WeatherApp.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include <map>
#include <functional>

////// CODE ADAPTED FROM MERKEL MAIN APPLICATION //////

WeatherApp::WeatherApp()
{

}

/** Initalize application */
void WeatherApp::init()
{
    std::cout << "Welcome to My Weather App! " << std::endl;
    int input;
    // Get user menu option and process it
    while(true)
    {
        printMenu();
        input = getUserInputOption("Type a number from 1-5");
        processUserOption(input);
    }
}

/** Print menu */
void WeatherApp::printMenu()
{
    // 1 Return a summary of candlestick data for a country
    std::cout << "1: Print Candlestick Data as Table" << std::endl;
    // 2 Plot candlestick for a given country
    std::cout << "2: Plot CandleStick Data" << std::endl;
    // 3 Predict weather for a given country
    std::cout << "3: Weather Predictions" << std::endl;
    // 4 Print help menu
    std::cout << "4: Help" << std::endl;
    // 5 Exit the application
    std::cout << "5: Exit" << std::endl;
    std::cout << "=============== " << std::endl;
}

/** Print help menu */
void WeatherApp::printHelp()
{
    std::cout << "Help Menu: " <<std::endl;
    std::cout << "Type 1 to Print Table of Candlestick Data by Country " <<std::endl;
    std::cout << "Type 2 to Plot Candlestick Data by Country " <<std::endl;
    std::cout << "Type 3 to See Weather Predicitons by Country " <<std::endl;
    std::cout << "=============== " << std::endl;
}

/** Process User Input Option (Inspired by MerkelMain app) */
void WeatherApp::processUserOption(int userOption) 
{
    switch(userOption)
    {
        case 1:
            printCandlestickData();
            break;
        case 2:
            plotCandlestickData();
            break;
        case 3:
            predictWeather();
            break;
        case 4:
            printHelp();
            break;
        case 5:
            std::cout << "Exiting Weather App. Goodbye!" << std::endl;
            exit(0); // Exit the program with status 0 (success)
            break;
        default:
            std::cout << "Invalid input. Please type 1-5." << std::endl;
            break;
    }
}

/** Get all user input, execute simple exception handling */
int WeatherApp::getUserInputOption(const std::string& prompt)
{
    int userOption = 0;
    std::string line;
    std::cout << prompt << std::endl;
    std::getline(std::cin, line);
    try {
        userOption = std::stoi(line);
    } catch(const std::exception& e) {
        std::cerr << "Invalid input. Please try again." << std::endl;
        return userOption;
    }
    return userOption;
}


////// MY CODE STARTS BELOW //////

/** Map the years to index values for filtering */
std::map<int, size_t> WeatherApp::yearToIndex()
{
    // Get a list of years and map it to an index value
    std::vector<std::string> years = weatherData.getYearsList();
    // Create a map to store year-to-index mapping
    std::map<int, size_t> yearToIndex;
    for (size_t i = 0; i < years.size(); ++i) {
        int year = std::stoi(years[i]);
        yearToIndex[year] = i;
    }
    return yearToIndex;
}

/** Validate input range when filtering by date range */
bool WeatherApp::validateDateRange(int startYear, int endYear)
{
    // Check start date is before end date
    if (startYear > endYear) {
        std::cerr << "Start Year cannot be greater than End Year." << std::endl;
        return false;
    }

    auto yearToIndex = WeatherApp::yearToIndex();

    // Find correct start index and end index in temperatures vector
    auto startFilter = yearToIndex.find(startYear);
    auto endFilter = yearToIndex.find(endYear);

    // Check if startYear or endYear were not found in yearToIndex map / dataset
    if (startFilter == yearToIndex.end() || endFilter == yearToIndex.end()) {
        std::cerr << "Start year or end year not found in dataset." << std::endl;
        return false;
    } 
    return true;
}

/** Store user input for date range as a pair of related integers */
std::pair<int, int> WeatherApp::getDateRange()
{
    int startYear = getUserInputOption("Enter start year (YYYY): ");
    int endYear = getUserInputOption("Enter end year (YYYY): ");
    return std::make_pair(startYear, endYear);
}

/** Prints a list of countries with a country code */
void WeatherApp::printCountryList() 
{
    std::vector<std::string> countries = weatherData.getCountryList();

    // Print the list of countries with their codes
    std::cout << "CODE :  COUNTRY" << std::endl;
    for (size_t i = 0; i < countries.size(); ++i) 
    {
        std::cout << (i + 1) << "    :  " << countries[i] << std::endl;
    }
}

// TASK 1 / 3 
/** Retrieve candlestick data for selected country and filtered dates */
std::vector<Candlestick> WeatherApp::getCandlestickData()
{
    // Initialize an empty vector
    std::vector<Candlestick> candlesticks {};
    // Map to associate years with indices in the data
    auto yearToIndex = WeatherApp::yearToIndex();

    printCountryList();
    // Get user input for the country selection and validate 
    int countryInput = getUserInputOption("Type Country Number (CODE)");
    while (countryInput <= 0 || countryInput > weatherData.getCountryList().size()) {
        std::cerr << "Invalid input. Please select a valid country code." << std::endl;
        countryInput = getUserInputOption("Type Country Number (CODE)");
    }
    
    // Ask user for date range preference and validate
    int dateRangeOption = getUserInputOption("Select date range option:\n1. Show data for entire date range\n2. Filter by date range");
    while (dateRangeOption != 1 && dateRangeOption != 2) {
        std::cerr << "Invalid input for date range option. Please select 1 or 2." << std::endl;
        dateRangeOption = getUserInputOption("Select date range option:\n1. Show data for entire date range\n2. Filter by date range");
    }

    // If user chooses to filter by date range, get inputs and validate
    std::pair<int, int> dateRange;
    if (dateRangeOption == 2) {
        // Get date range
        dateRange = getDateRange();
        // Validate date range
        while (!validateDateRange(dateRange.first, dateRange.second)) {
            std::cerr << "Invalid date range. Please enter valid start and end years." << std::endl;
            dateRange = getDateRange();
        }
    }

    // Get temperatures filtered by country (Adjust for 0 indexing)
    std::vector<std::vector<double>> countryTemps = weatherData.getTemperatures(countryInput - 1);
    // Initialise vector for temperatures filtered by date
    std::vector<std::vector<double>> filteredTemps;

    int startYearIndex = 0;
    // Option 2: If user chose to filter by date range, filter the temperatures
    if (dateRangeOption == 2) {
        // Check whether first year is same as first year in dataset (For determining first "Open" value)
        if (dateRange.first == yearToIndex.begin()->first)
        {
            filteredTemps = weatherData.filterDateRange(countryTemps, dateRange.first, dateRange.second);
            startYearIndex = yearToIndex[dateRange.first];
        }
        else // If it is not, add a year before to correctly compute open value
        {
            filteredTemps = weatherData.filterDateRange(countryTemps, dateRange.first - 1, dateRange.second);
            startYearIndex = yearToIndex[dateRange.first - 1];
        }
    // Option 1: Show data for all date ranges
    } else { 
        filteredTemps = countryTemps; // Show all data
    }

    // Get Candlestick Data
    candlesticks = weatherData.computeCandlestick(filteredTemps, startYearIndex);
    std::cout << "Candlestick data for " << weatherData.getCountryList()[countryInput - 1] << std::endl;
    // Check if first year of data is first year in dataset (To make sure to print correct range)
    if ((dateRange.first == yearToIndex.begin()->first) || (dateRange.first == 0))
    {
        return candlesticks;
    }
    else { // Remove first candlestick as we dont want to plot or print it
        candlesticks.erase(candlesticks.begin());
        return candlesticks;
    }
}

// TASK 1
/** Print candlestick data as a table */
void WeatherApp::printTable(const std::vector<Candlestick>& tableData) const
{
    // Print table headers 
    std::cout   << "+-----------+-----------+-----------+-----------+-----------+" << std::endl;
    std::cout   << "| " << std::left << std::setw(10) << "Year"
                << "| " << std::setw(10) << "Open"
                << "| " << std::setw(10) << "High"
                << "| " << std::setw(10) << "Low"
                << "| " << std::setw(10) << "Close"
                << "|" << std::endl;
    std::cout   << "+-----------+-----------+-----------+-----------+-----------+" << std::endl;

    // Print table rows in correct format
    for (const auto& cs : tableData) {
        std::cout   << "| " << std::left << std::setw(10) << cs.year 
                    << "| " << std::fixed << std::setprecision(3)
                    << std::setw(10) << cs.open
                    << "| " << std::setw(10) << cs.high
                    << "| " << std::setw(10) << cs.low
                    << "| " << std::setw(10) << cs.close
                    << "|"  << std::endl;
        std::cout   << "+-----------+-----------+-----------+-----------+-----------+" << std::endl;
    }
}

// TASK 2
/** Print candlestick data as a plot */
void WeatherApp::plot(const std::vector<Candlestick>& plotData) const 
{
    const int height = 20;  // height of the graph in text rows
    const int width = 6;   // width of each candlestick box in characters
    const int padding = 3;  // padding between candlestick boxes
   
   // Number of items to display at a time
    const int items_per_page = 10;
    int total_pages = (plotData.size() + items_per_page - 1) / items_per_page;
    int current_page = 0;

    while (true) {
        // Get subset of Candlesticks to plot
        int start_index = current_page * items_per_page;
        int end_index = std::min(start_index + items_per_page, (int)plotData.size());
        std::vector<Candlestick> pageData(plotData.begin() + start_index, plotData.begin() + end_index);

        // Determine the min and max temperatures for scaling the graph
        double min_temp = pageData[0].low;
        double max_temp = pageData[0].high;

        // Find global min and max temperatures
        for (const auto& cs : pageData) {
            if (cs.low < min_temp) min_temp = cs.low;
            if (cs.high > max_temp) max_temp = cs.high;
        }

        // Scale factor to fit the temperatures within the graph height
        double scale = height / (max_temp - min_temp);

        // Print the graph (Working our way down the graph height)
        for (int i = height; i >= 0; --i) {
            // Print Y axis
            double temp = min_temp + i / scale;
            std::cout << std::setw(6) << std::fixed << std::setprecision(1) << temp << " | ";

            // Scale all values to graph height
            for (const auto& cs : pageData) {
                int lowPos = static_cast<int>((cs.low - min_temp) * scale);
                int highPos = static_cast<int>((cs.high - min_temp) * scale);
                int openPos = static_cast<int>((cs.open - min_temp) * scale);
                int closePos = static_cast<int>((cs.close - min_temp) * scale);

                // Change output depending on case
                if (i >= lowPos && i <= highPos) {
                    if (i == openPos && i == closePos) {
                        std::cout << "\033[32m[" << std::string(width - 2, '=') << "]\033[0m"; // Green when openPos and closePos the same
                    } else if (i == openPos) {
                        std::cout << "\033[34m[" << std::string(width - 2, '-') << "]\033[0m"; // Blue for openPos
                    } else if (i == closePos) {
                        std::cout << "\033[31m[" << std::string(width - 2, '-') << "]\033[0m"; // Red for closePos
                    } else {
                        // Print double '||' in the middle of the candlestick body
                        std::cout << std::string((width / 2) - 1, ' ') << "||" << std::string((width / 2) - 1, ' ');
                    }
                } else {
                    std::cout << std::string(width, ' ');
                }
                std::cout << std::string(padding, ' ');
            }
            std::cout << std::endl;
        }

        // Print the x-axis with years ensuring correct spacing
        std::cout << "       +";
        for (size_t i = 0; i < pageData.size(); ++i) {
            std::cout << std::string(width + padding, '-');
        }
        std::cout << std::endl;
        std::cout << std::string(width + padding + 1, ' ');
        // Print years
        for (const auto& cs : pageData) {
            std::cout << std::setw(width + padding) << std::left << cs.year;
        }
        std::cout << std::endl;

        // Print color legend
        std::cout << "Legend: ";
        std::cout << "\033[32m[=]\033[0m Same Open and Close  ";
        std::cout << "\033[34m[-]\033[0m Open ";
        std::cout << "\033[31m[-]\033[0m Close ";
        std::cout << std::endl;
        std::cout << "=============== " << std::endl;

        // Pagination control
        std::cout << "Page " << (current_page + 1) << " of " << total_pages << std::endl;
        std::cout << "Press 'n' for next page, 'p' for previous page, 'q' to quit: ";
        
        char input;
        std::cin >> input;
        // Clear input buffer (https://www.geeksforgeeks.org/clearing-the-input-buffer-in-cc/)
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        // Handle pagination input
        // n for next page
        if (input == 'n') {
            if (current_page < total_pages - 1) {
                current_page++;
            } else {
                std::cout << "Already on the last page." << std::endl;
            }
        // p for previous page
        } else if (input == 'p') {
            if (current_page > 0) {
                current_page--;
            } else {
                std::cout << "Already on the first page." << std::endl;
            }
        // q for quit
        } else if (input == 'q') {
            break;
        } else {
            std::cout << "Invalid input, please try again." << std::endl;
        }
    }
}

// TASK 1 OUTPUT
void WeatherApp::printCandlestickData()
{
    // Print candlestick data as a Table
    std::vector<Candlestick> printData = getCandlestickData();
    printTable(printData);
}

// TASK 2 OUTPUT
void WeatherApp::plotCandlestickData()
{
    // Plot candlestick data
    std::vector<Candlestick> plotData = getCandlestickData();
    plot(plotData);
}

// TASK 4 OUTPUT
/** Print projected Candlestick Data for selected number of years using EMA method (see report) */
void WeatherApp::predictWeather()
{
    // Map to associate years with indices in the data
    auto yearToIndex = WeatherApp::yearToIndex();

    printCountryList();
    // Get user input for the country selection and validate
    int countryInput = getUserInputOption("Type Country Number");
    while (countryInput <= 0 || countryInput > weatherData.getCountryList().size()) {
        std::cerr << "Invalid input. Please select a valid country code." << std::endl;
        countryInput = getUserInputOption("Type Country Number");
    }

   // Get all data for the selected country (Adjust for 0 indexing)
    std::vector<std::vector<double>> countryTemps = weatherData.getTemperatures(countryInput - 1);
    std::vector<Candlestick> candlesticks = weatherData.computeCandlestick(countryTemps, 0);
    
    // Ask user for date range preference and validate
    int predictionRange = getUserInputOption("Type the number of years for which to predict");

    // Start the prediction from the year following the latest available year
    int predictionStartYear = yearToIndex.rbegin()->first + 1;
    std::vector<Candlestick> predictions = weatherData.predictTemperatures(candlesticks, predictionRange, predictionStartYear);

    // Ask user whether to visualize the prediction data as a table or a plot, validates input
    int tableOrPlot = getUserInputOption("Type 1 to visualise as a table OR Type 2 to visualise as a plot");
    while (tableOrPlot != 1 && tableOrPlot != 2) {
        std::cerr << "Invalid input for date range option. Please select 1 or 2." << std::endl;
        tableOrPlot = getUserInputOption("Type 1 to visualise as a table OR Type 2 to visualise as a plot");
    }
 
    // Print title
    std::cout << weatherData.getCountryList()[countryInput - 1] << std::endl;
    std::cout << "Predicted temperatures for the next " << predictionRange << " years: " << std::endl;

    // If option 1, display prediction data as a table
    if (tableOrPlot == 1){

        printTable(predictions);
    }
    // If option 2, display prediciton data as a plot
    else if (tableOrPlot == 2)
    {
        plot(predictions);
    }
}

////// END OF MY CODE //////
