#include "config.hpp"
#include "server.hpp"

std::string readFile(std::string &path)
{
	std::ifstream file(path.c_str(), std::ios::binary);
	if(!file.is_open())
	{
		std::cerr << "Error: could not open file" << std::endl;
		return ("");
	}
	return std::string(
		std::istreambuf_iterator<char>(file),
		std::istreambuf_iterator<char>()
	);
}

std::string httpHeaderResponse(std::string code, std::string contentType, std::string content)
{
	return ("HTTP/1.1 " + code + "\r\n"
			"Content-Type: " + contentType + "\r\n"
			"Content-Length: " + to_string(content.size()) + "\r\n"
			"Connection: close\r\n"
			"\r\n" + content);
}

void generate_html_page_error(Server &serv, int client_socket, std::string error_code)
{
	std::cout << "DEBUG: JE SUIS DEDANS" << std::endl;
	std::string path = "." + serv.getErrorPage(error_code);
	std::string file_content = readFile(path);
	std::string reponse = httpHeaderResponse("200 Ok", "text/html", file_content);
	send(client_socket, reponse.c_str(), reponse.size(), 0);
}
