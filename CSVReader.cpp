#include "CSVReader.h"
#include <iostream>
#include <fstream>
#include <functional>

////// CODE ADAPTED FROM MERKEL MAIN APPLICATION //////

CSVReader::CSVReader()
{

}

/** CSV Reading and parsing (modified for this project) */
std::vector<std::vector<std::string>> CSVReader::readCSV(const std::string csvFilename)
{
    std::cout << "Application Loading: Please wait... " << std::endl;
    std::vector<std::vector<std::string>> entries;

    // opens file in read mode, (default) good practice to close file
    std::ifstream csvFile{csvFilename};
    std::string line;
    int totalLines = 0;
    int currentLine = 0;

    if (csvFile.is_open())
    {
        while(std::getline(csvFile, line))
        {
            totalLines++;
        }
        // Reset the read pointer to the beginning of the file
        csvFile.clear();
        csvFile.seekg(0, std::ios::beg);

        while(std::getline(csvFile, line))
        {
            try {
                std::vector<std::string> entry = tokenise(line, ',');
                entries.push_back(entry);
            } catch (const std::exception& e)
            {
                std::cout << "CSVReader::readCSV bad data " << std::endl;
            }
            currentLine++;
            showLoadingBar(currentLine, totalLines);
        } 
        std::cout << std::endl;
    }

    // Clear terminal (Hide progress bar when finished) 
    // https://stackoverflow.com/questions/228617/how-do-i-clear-the-console-in-both-windows-and-linux-using-c
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif

    std::cout << "CSVReader::readCSV successfully read " << entries.size() << " entries" << std::endl;
    return entries;
}

/** Tokenise algorithm */
std::vector<std::string> CSVReader::tokenise(const std::string& csvLine, char seperator)
{
    // string vector tokens ## stores the tokens
    std::vector<std::string> tokens;

    // int start, end ## used to delineate the position of the tokens
    signed int start, end;
    std::string token;

    // start = csvLine.find_first_not_of(seperator)
    start = csvLine.find_first_not_of(seperator, 0);

    do{
        // end = next seperator after start
        end = csvLine.find_first_of(seperator, start);

        // if start == csvLine.length or start == end ## nothing more to find so break
        if (start == csvLine.length() || start == end) break;

        // if end >= 0 ## we found the seperator, create the token for that substring
        if (end >= 0) token = csvLine.substr(start, end - start);

        // else ## end if invalid, get substring until the end of the line
        else token = csvLine.substr(start, csvLine.length() - start);

        // Push back token to vector
        tokens.push_back(token);

    // Move start past current token
    start = end + 1;

    // Continue loop condition until there are no more tokens
    } while (end != std::string::npos);

return tokens;
}

////// MY CODE STARTS BELOW //////

// Inspired by code here: (https://stackoverflow.com/questions/14539867/how-to-display-a-progress-indicator-in-pure-c-c-cout-printf)
/** Print out loading bar to improve User Experience  */
void CSVReader::showLoadingBar(int current, int total) {
    int barWidth = 50;
    float progress = static_cast<float>(current) / total;
    int pos = static_cast<int>(barWidth * progress);

    std::cout << "[";
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] " << std::setw(3) << static_cast<int>(progress * 100.0) << " %\r";
    std::cout.flush();
}

////// END OF MY CODE//////
