#include "Webserv.hpp"
#include <sstream>

// magic code here, do not touch
std::string fileToString(const char *filePath)
{
    std::ifstream inputFile(filePath);

    if (!inputFile.is_open())
    {
        throw std::runtime_error("Failed to open the file: " + std::string(filePath));
    } // love

    std::stringstream buffer;
    buffer << inputFile.rdbuf(); //gets all content of the file and puts it into buffer;
    return (buffer.str()); // sex
}


std::string intToString(int value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}


std::string readFromPipeFd(int pipefd) 
{
    std::string result;
    char buffer[4096];  // Buffer for reading
    ssize_t bytesRead;

    while (true) 
	{
        bytesRead = read(pipefd, buffer, sizeof(buffer));
        if (bytesRead < 0) 
		{
            std::cout << "Error here, down the line, this should be a 500 error" << std::endl;
			break;
        }
        if (bytesRead == 0)
            break;
        result.append(buffer, bytesRead);
    }

    return result;
}