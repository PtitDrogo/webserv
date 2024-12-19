#include "server.hpp"

bool deleteFile(const std::string& path)
{
	if (remove(path.c_str()) == 0)
		return true;
	else
		return false;
}

bool parse_buffer_delete(Client& client, HttpRequest& req)
{
	Server& 	server = client.getServer();

	std::string location_path = CheckLocation(req.getPath(), client.getServer().getLocation(), client);
	if (client.getLocation() != NULL && client.getLocation()->getAllowMethod().find("DELETE") == std::string::npos && client.getLocation()->getAllowMethod().empty() == false)
	{
		return (generate_html_page_error(client, "405"));
	}
	if (client.getLocation() != NULL)
	{
		size_t pos;
		while ((pos = location_path.find("//")) != std::string::npos)
            location_path.replace(pos, 2, "/");
		if (file_exists_parsebuffer(location_path.c_str()) == false)
			return (generate_html_page_error(client, "404"));
		if (deleteFile(location_path) == false) 
			return(generate_html_page_error(client, "404"));
		std::string path = "." + server.getRoot() + server.getIndex();
		std::string file_content = readFile(path);
		std::string reponse = httpHeaderResponse("200 Ok", "text/html", file_content);
		if (send(client.getSocket(), reponse.c_str(), reponse.size(), 0) <= 0)
			return false;
		return true;
	}
	if (client.getRequest().find("DELETE " + client.getServer().getRoot() + "base_donnees/") != std::string::npos) {

        std::string filename = client.getRequest().substr(client.getRequest().find(client.getServer().getRoot() + "base_donnees/") + client.getServer().getRoot().size() + 13);

        if (filename.find("?fileName=") != std::string::npos)
            filename = filename.substr(filename.find("?fileName=") + 10, filename.find(" ") - filename.find("?fileName=") - 10);
		else
            filename = filename.substr(0, filename.find(" "));
		
        std::string filePath = "." + client.getServer().getRoot() + "base_donnees/" + filename;
		if (file_exists_parsebuffer(filePath.c_str()) == false) 
			return (generate_html_page_error(client, "404"));

		if (deleteFile(filePath) == false) 
			return(generate_html_page_error(client, "404"));
		std::string path = "." + server.getRoot() + server.getIndex();
		std::string file_content = readFile(path);
		std::string reponse = httpHeaderResponse("200 Ok", "text/html", file_content);
		if (send(client.getSocket(), reponse.c_str(), reponse.size(), 0) <= 0)
			return false;
	}
	return true;
}