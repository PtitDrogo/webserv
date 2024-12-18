#include "Webserv.hpp"

bool upload(Client& client);

bool upload(Client& client)
{
	const Server& 	server = client.getServer();
	if (client.extractBoundary() == false)
	{
		// std::cout << "1" << std::endl;
		generate_html_page_error(client, "400");
		return false;
	}

	if (client.extractFileName() == false)
	{
		generate_html_page_error(client, "400");
		return false;
	}

	if (client.extractContentType() == false)
	{
		generate_html_page_error(client, "400");
		return false;
	}
	std::string path = "." + server.getRoot() + server.getIndex();
	std::string file_content = readFile(path);
	std::string response = httpHeaderResponse("200 Ok", "text/html", file_content);
	send(client.getSocket(), response.c_str(), response.size(), 0);
	return true;
}

