#include "Webserv.hpp"

bool upload(Client& client);

bool upload(Client& client) {
	std::cout << YELLOW << "UPLOAD in process..." << RESET << std::endl; // debug extra content type
	const Server& 	server = client.getServer();
	if (client.extractBoundary() == false) {
		std::cerr << "Error: Unable to extract boundary." << std::endl;
		generate_html_page_error(client, "400");
		return false;
	}

	if (client.extractFileName() == false) {
		std::cerr << "Error: Unable to extract filename." << std::endl;
		generate_html_page_error(client, "400");
		return false;
	}

	if (client.extractContentType() == false) {
		std::cerr << "Error: Unable to extract content type." << std::endl;
		generate_html_page_error(client, "400");
		return false;
	}
	
	std::string path = "." + server.getRoot() + server.getIndex();
	std::string file_content = readFile(path);
	std::string response = httpHeaderResponse("200 Ok", "text/html", file_content);
	send(client.getSocket(), response.c_str(), response.size(), 0);
	std::cout << MAGENTA << "UPLOAD Successful and saved: " << client.getFileName() << RESET << std::endl; // debug extra content type
	return true;
}