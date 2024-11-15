#include <iostream>
#include <fstream>



std::string fileToString(const char *filePath)
{
    std::ifstream inputFile(filePath);
    std::string   result;
    std::string   line;
    if (!inputFile.is_open()) {
        std::cerr << "Failed to open the file." << std::endl;
        return (result); //Idk how exactly I would return the error, throw an exception ?
    }
    while (std::getline(inputFile, line))
    {
        result += line;
    }
    inputFile.close();
    return (result);
}

