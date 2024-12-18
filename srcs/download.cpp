#include "Webserv.hpp"

bool				download(Client& client);
static std::string 	readFile_http(std::string filePath);

bool	download(Client& client) 
{
	std::string filename = client.getRequest().substr(client.getRequest().find(client.getServer().getRoot() + "base_donnees/") + client.getServer().getRoot().size() + 13);

	if (filename.find("?fileName=") != std::string::npos)
		filename = filename.substr(filename.find("?fileName=") + 10, filename.find(" ") - filename.find("?fileName=") - 10);
	else
		filename = filename.substr(0, filename.find(" "));

	std::string filePath = "." + client.getServer().getRoot() + "base_donnees/" + filename;
	if (file_exists_parsebuffer(filePath.c_str()) == false)
	{
		generate_html_page_error(client, "404");
		return false;
	}

	std::string fileContent = readFile_http(filePath);
	if (fileContent.empty()) {
		generate_html_page_error(client, "500");
		return false;
	}

	std::stringstream rep;
	rep << "HTTP/1.1 200 OK\r\n";
	rep << "Content-Type: application/octet-stream\r\n";
	rep << "content-length: " << fileContent.size() << "\r\n";
	rep << "Content-Disposition: attachment; filename=\"" << filename << "\"\r\n";
	rep << "Connection: close\r\n";
	rep << "\r\n";
	rep << fileContent;

	if (send(client.getSocket(), rep.str().c_str(), rep.str().size(), 0) == -1)
		return false;
	return true;
}

static std::string	readFile_http(std::string filePath) {
	std::ifstream inputFile(filePath.c_str(), std::ios::binary);

	std::stringstream buffer;
	buffer << inputFile.rdbuf(); 
	return (buffer.str());
}
