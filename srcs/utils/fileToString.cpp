#include "Webserv.hpp"

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

std::string readFromPipeFd(int pipefd) 
{
    std::string result;
    char buffer[65535];
    ssize_t bytesRead;


    while (true) 
	{
        bzero(&buffer, sizeof(buffer));
        bytesRead = read(pipefd, buffer, sizeof(buffer));
        if (bytesRead < 0) 
		{
            std::cout << "Error here, down the line, this should be a 500 error" << std::endl;
			break;
        }
        if (bytesRead == 0)
            break;
        // std::cout << "result is : |" << result << "|" << std::endl;
        result.append(buffer, bytesRead);
        // std::cout << "AFTER result is : |" << result << "|" << std::endl;
        // break ; //Plus rien a foutre
    }

    // std::cerr << "je suis sorti" << std::endl;
    // printf("Je suis sorti\n");
    return result;   
}

bool isRegularFile(const std::string& path) 
{
    struct stat buffer;

    if (stat(path.c_str(), &buffer) != 0)
        return false;
    return S_ISREG(buffer.st_mode);
}

std::string injectUserHtml(const std::string& fileContent, const std::string& username) 
{
    std::stringstream updatedContent;

    updatedContent << fileContent;

    // Add a simple HTML snippet with the username
    updatedContent << "\n<!-- Injected User Information -->\n";
    updatedContent << "<div style=\"position: absolute; border: 1px solid #000; padding: 10px; margin-top: 20px; width: 20%; top: 2%; right: 5%;\">\n";
    updatedContent << "    <p style=\"text-shadow: 2px 2px 5px rgba(255, 0, 0); text-align: center;\"><strong>PROFILE</strong></p>\n";
    updatedContent << "    <p> <strong>Logged in as :<strong> <span style='text-shadow: 2px 2px 5px rgba(255, 0, 0);'>" << username << "</span></p>\n";
    updatedContent << "</div>\n";

    return updatedContent.str();
}