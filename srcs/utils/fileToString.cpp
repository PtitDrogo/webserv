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
    char buffer[4096];  //Buffer for reading -- Idee de genie de Lchapard, un buffer de 65 000;
    ssize_t bytesRead;


    while (true) 
	{
        printf("DEBUT BOUCLE\n");
        bzero(&buffer, sizeof(buffer));
        bytesRead = read(pipefd, buffer, sizeof(buffer));
        printf("Je suis la %li\n", bytesRead);
        if (bytesRead < 0) 
		{
            std::cout << "Error here, down the line, this should be a 500 error" << std::endl;
			break;
        }
        if (bytesRead == 0)
        {    
            printf("LET ME OUT\n");
            break;
        }
        std::cout << "result is : |" << result << "|" << std::endl;
        result.append(buffer, bytesRead);
        std::cout << "AFTER result is : |" << result << "|" << std::endl;
        printf("Je suis la\n");
        // break ; //Plus rien a foutre
    }

    std::cerr << "je suis sorti" << std::endl;
    printf("Je suis sorti\n");
    return result;   
}