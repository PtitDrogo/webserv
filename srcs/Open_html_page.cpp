#include "Webserv.hpp"

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

void generate_html_page_error(Config &conf, int client_socket, std::string error_code)
{
    std::cout << "DEBUG: JE SUIS DEDANS" << std::endl;
    int	server_index = conf.getIndexOfClientServer(client_socket);
    
    // On récupère la map de l'erreur pour l'utiliser ensuite
    std::map<std::string, std::string> errorPageMap = conf.getServer()[server_index].getErrorPage();
    //conf.getServer()[server_index].getRoot()

    // Recherche de l'erreur dans la map
    std::map<std::string, std::string>::iterator it = errorPageMap.find(error_code);

    std::string path;
    if (it != errorPageMap.end()) {
        // Si la clé existe, on utilise le chemin associé
        path = "." + it->second;
    } else {
        // Si la clé n'existe pas, on utilise une page d'erreur par défaut
        std::cerr << "Error page for code " << error_code << " not found." << std::endl;
        path = "./error_default.html"; // Page d'erreur par défaut
    }

    // Lecture du contenu du fichier
    std::string file_content = readFile(path);

    // Création de la réponse HTTP
    std::string reponse = httpHeaderResponse(error_code, "text/html", file_content);

    // Envoi de la réponse
    send(client_socket, reponse.c_str(), reponse.size(), 0);
}
