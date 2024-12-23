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
    char buffer[PIPE_BUFFER];
    ssize_t bytesRead;

    while (true) 
	{
        bzero(&buffer, sizeof(buffer));
        bytesRead = read(pipefd, buffer, sizeof(buffer));
        if (bytesRead < 0) 
		{
			return "";
        }
        if (bytesRead == 0)
            break;
        result.append(buffer, bytesRead);
    }
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

bool	checkFailedExecve(Client &client)
{
	int status;

	if (client.getCgiCaller() == NULL)
		return true;
	waitpid(client.getCgiCaller()->getCgiPID(), &status, WNOHANG);
	if (WIFEXITED(status)) 
	{
		int exit_code = WEXITSTATUS(status);
		if (exit_code != SUCCESS)
            return (generate_html_page_error(*client.getCgiCaller(), "500"));
	}
    return true;
}