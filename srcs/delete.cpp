#include "server.hpp"

bool deleteFile(const std::string& path)
{
	if (remove(path.c_str()) == 0)
		return true;
	else
		return false;
}

void parse_buffer_delete(std::string buffer, Client& client)
{
	(void)buffer;
	Server& 	server = client.getServer();

	if (client.getRequest().find("DELETE /config/base_donnees/") != std::string::npos) {

		std::cout << YELLOW << "DELETE in process..." << RESET << std::endl; // print debug
        std::string filename = client.getRequest().substr(client.getRequest().find("/config/base_donnees/") + 21);

        if (filename.find("?fileName=") != std::string::npos)
            filename = filename.substr(filename.find("?fileName=") + 10, filename.find(" ") - filename.find("?fileName=") - 10);
		else
            filename = filename.substr(0, filename.find(" "));
		
        std::string filePath = "./config/base_donnees/" + filename;
		if (file_exists_parsebuffer(filePath.c_str()) == false) {
			std::cout << RED << "DELETE Fail" << RESET << std::endl;
			generate_html_page_error(client, "404");
			return ;
		}

		if (deleteFile(filePath) == false) {
			std::cout << RED << "DELETE Fail" << RESET << std::endl;
			generate_html_page_error(client, "404");
			return ;
		}

		std::string path = "." + server.getRoot() + server.getIndex();
		std::string file_content = readFile(path);
		std::string reponse = httpHeaderResponse("200 Ok", "text/html", file_content);
		send(client.getSocket(), reponse.c_str(), reponse.size(), 0);
		std::cout << GREEN << "DETELE Successful" << RESET << std::endl;
	}
}