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
	std::istringstream stream(buffer);
	std::string line;
	Server& 	server = client.getServer();


	if (!stream)
	{
		std::cout << "Erreur : le flux n'a pas pu être créé." << std::endl;
		return ;
	}
	std::string method;
	std::string path;
	std::string version;
	std::string finalPath;
	std::string name;
	std::string email;
	std::string message;
	while (std::getline(stream, line))
	{
		size_t pos1 = line.find("DELETE");
		size_t pos2 = line.find("HTTP");
		
		if (pos1 != std::string::npos && pos2 != std::string::npos)
		{
			method = line.substr(pos1, 7);
			path = line.substr(pos1 + 7, pos2 - pos1 - 8);
			version = line.substr(pos2);
			finalPath = "." + server.getRoot() + path;
		}
	}
	if (!finalPath.empty())
	{
		std::cout << "finalPath = |" << finalPath << "|" << std::endl;
		std::ifstream infile(finalPath.c_str());
		if (infile.is_open())
		{
			std::cout << "Le fichier existe et est accessible pour lecture." << std::endl;
			std::string file_content = readFile(finalPath);
			std::string reponse = httpHeaderResponse("200 Ok", "text/html", file_content);
			send(client.getSocket(), reponse.c_str(), reponse.size(), 0);
			infile.close();
			if (deleteFile(finalPath))
				std::cout << "Fichier supprimé après lecture." << std::endl;
			else
				std::cerr << "Échec de la suppression du fichier." << std::endl;
		}
		else
		{
			std::cerr << "Le fichier n'existe pas ou ne peut pas être ouvert." << std::endl;
			generate_html_page_error(client, "404");
		}
	}
}