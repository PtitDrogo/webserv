#include "Webserv.hpp"
#include "include.hpp"
#include "httpRequest.hpp"
#include "config.hpp"
#include "server.hpp"
#include "location.hpp"

bool isExtension(std::string path)
{
	//return false if there is not extension
	if(path.find(".html") == std::string::npos &&
		path.find(".css") == std::string::npos &&
		path.find(".js") == std::string::npos &&
		path.find(".png") == std::string::npos &&
		path.find(".jpg") == std::string::npos &&
		path.find(".gif") == std::string::npos &&
		path.find(".ico") == std::string::npos &&
		path.find(".py") == std::string::npos)
		return (false);
	return (true);
}

void autoIndex(std::string path, Config &conf, int client_socket)
{
	std::string finalPath;
	std::string reponse;
	std::string file_content;

	finalPath = "." + conf.getServer()[0].getRoot() + path;
	std::vector<std::string> files = listDirectory(finalPath);
	file_content = generateAutoIndexPage(finalPath, files);
	reponse = httpHeaderResponse("200 Ok", "text/html", file_content);
	std::cout << "reponse = " << reponse << std::endl;
	reponse = httpHeaderResponse("200 Ok", "text/html", file_content);
	send(client_socket, reponse.c_str(), reponse.size(), 0);
}


std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    size_t end = str.find_last_not_of(" \t\r\n");

    if (start == std::string::npos || end == std::string::npos) {
        return "";
    }

    return str.substr(start, end - start + 1);
}

void printVectorrr(std::vector<std::string> vec)
{
	for (size_t i = 0; i < vec.size(); i++)
	{
		std::cout << "vec[" << i << "] = " << vec[i] << std::endl;
	}
}

void parse_allow_method(Config &conf, HttpRequest &req)
{
    // Récupérer les méthodes autorisées
    std::string allow_methods = conf.getServer()[0].getLocation()[0].getAllowMethod();
    std::vector<std::string> methods;

    // Découper la chaîne des méthodes autorisées
    std::istringstream stream(allow_methods);
    std::string method;
    while (stream >> method) {
        methods.push_back(method);
    }

    // Afficher les méthodes autorisées
    std::cout << "DEBUG: getAllowMethod() = '" << allow_methods << "'" << std::endl;
    for (size_t i = 0; i < methods.size(); ++i) {
        std::cout << "vec[" << i << "] = " << methods[i] << std::endl;
    }

    // Récupérer la méthode de la requête
    std::string request_method = req.getMetode();
    request_method.erase(request_method.find_last_not_of(" \t\r\n") + 1); // Nettoyage
    std::cout << "DEBUG: req.getMetode() (trimmed) = '" << request_method << "'" << std::endl;

    // Vérifier si la méthode est dans la liste des méthodes autorisées
    if (std::find(methods.begin(), methods.end(), request_method) == methods.end()) {
        // La méthode n'est pas autorisée -> Erreur 400
        generate_html_page_error(conf, 1, "400");
    }
}

std::string CheckLocation(const std::string& path, Config& conf, const std::vector<location>& locationPath, bool& locationMatched, HttpRequest &req)
{
    // Nettoyer le path en supprimant les espaces avant et après

	(void) conf;
	parse_allow_method(conf, req);
    std::string cleanedPath = trim(path);

    // Affiche les valeurs pour vérification
    std::cout << "cleanedPath = |" << cleanedPath << "|" << std::endl;

    // Parcours toutes les locations
    for (size_t i = 0; i < locationPath.size(); ++i) {
        std::string locationStr = locationPath[i].getPath();

        // Nettoyer la location en supprimant les espaces avant et après
        locationStr = trim(locationStr);

        std::cout << "locationStr = |" << locationStr << "|" << std::endl;

        // Vérifie si le path commence par locationStr
        if (cleanedPath.find(locationStr) == 0) {
            std::cout << "je rentre ici pour location = " << locationStr << std::endl;

            // Si le chemin est plus court que locationStr, cela signifie qu'on n'a rien à extraire
            if (cleanedPath.size() <= locationStr.size()) {
				locationMatched = true;
                return "." + locationPath[i].getRoot();
            }

            // Sinon, on extrait la sous-chaîne relative après le path de la location
            std::string relativePath = cleanedPath.substr(locationStr.size());

            // Si le chemin relatif est vide ou juste un "/", retourner l'index ou la racine
            if (relativePath.empty() || relativePath == "/") {
                if (!locationPath[i].getIndex().empty()) {
					locationMatched = true;
                    return "." + locationPath[i].getRoot() + locationPath[i].getIndex();
                }
				locationMatched = true;
                return "." + locationPath[i].getRoot(); // Retourne le root si pas d'index
            }
			locationMatched = true;

            // Retourne le chemin complet avec la partie relative
            return "." + locationPath[i].getRoot() + relativePath;
        }
    }

    return ""; // Si aucun match n'est trouvé
}


bool check_host(std::string line, Config &conf)
{
	size_t pos = line.find("Host: ");
	if (pos != std::string::npos)
	{
		std::string host = line.substr(pos + 6);
		// Nettoyage de la chaîne pour éviter les caractères superflus
		host = host.substr(0, host.find_first_of("\r\n"));
		host.erase(std::remove_if(host.begin(), host.end(), ::isspace), host.end());

		std::cout << "-------------host = |" << host << "|" << std::endl;

		std::string expected_host = conf.getServer()[0].getServerName() + ":" + conf.getServer()[0].getPort();
		if (host != expected_host)
		{
			std::cout << "host.conf = |" << expected_host << "|" << std::endl;
			std::cout << "Error: Host not found" << std::endl;
			return false;
		}
	}
	return true;
}


void	parse_buffer_get(std::string buffer, Config &conf , int client_socket, HttpRequest &req)
{
	std::istringstream stream(buffer);
	std::string line;
	std::string method;
	std::string path;
	std::string version;
	std::string finalPath;
	std::string reponse;
	std::string file_content;
	std::vector<location> locationPath = conf.getServer()[0].getLocation();
	if (!stream)
	{
		std::cout << "Erreur : le flux n'a pas pu être créé." << std::endl;
		return ;
	}
	while (std::getline(stream, line))
	{
		size_t pos1 = line.find("GET");
		size_t pos2 = line.find("HTTP");
		if (pos1 != std::string::npos && pos2 != std::string::npos)
		{
			method = line.substr(pos1, 4);
			path = line.substr(pos1 + 4, pos2 - pos1 - 5);
			version = line.substr(pos2);
			std::cout << "path-----------------------------------" << path << std::endl;
			bool locationMatched = false;
			finalPath = CheckLocation(path, conf, locationPath, locationMatched, req);
			std::cout << "finalPath-----------------------------------" << finalPath << std::endl;
			if (!locationMatched)
			{
				if (path == "/")
				{
					if (!conf.getServer()[0].getIndex().empty())
						finalPath = "." + conf.getServer()[0].getRoot() + conf.getServer()[0].getIndex();
					else if (conf.getServer()[0].getAutoIndex() == "on")
					{
						autoIndex(path, conf, client_socket);
						return ;
					}
					else
						generate_html_page_error(conf, client_socket, "404");
				}
				else if (conf.getServer()[0].getAutoIndex() == "on" && conf.getServer()[0].getIndex().empty())
				{
					finalPath = "." + conf.getServer()[0].getRoot() + path;
					if (!isExtension(finalPath))
					{
						std::vector<std::string> files = listDirectory(finalPath);
						file_content = generateAutoIndexPage(finalPath, files);
					}
					else if (isExtension(finalPath))
						file_content = readFile(finalPath);
					reponse = httpHeaderResponse("200 Ok", "text/html", file_content);
					std::cout << "reponse = " << reponse << std::endl;
					reponse = httpHeaderResponse("200 Ok", "text/html", file_content);
					send(client_socket, reponse.c_str(), reponse.size(), 0);
					return ;
				}
				else
					finalPath = "." + conf.getServer()[0].getRoot() + path;
			}
		}
		if (check_host(line, conf) == false)
		{
			generate_html_page_error(conf, client_socket, "400");
			return ;
		}
	}
	std::cout << "------------voici le path------------|" << finalPath << "|" << std::endl;
	file_content = readFile(finalPath);
	if (file_content.empty())
		generate_html_page_error(conf, client_socket, "404");
	reponse = httpHeaderResponse("200 Ok", "text/html", file_content);
	send(client_socket, reponse.c_str(), reponse.size(), 0);
}

void parse_buffer_post(std::string buffer , int client_socket, Config &conf)
{
	std::istringstream stream(buffer);
	std::string line;

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
	std::string filename;
	while (std::getline(stream, line))
	{
		size_t pos1 = line.find("FileName=");
		size_t pos3 = line.find("name=");
		size_t pos4 = line.find("email=");
		size_t pos5 = line.find("message=");
		if (pos3 != std::string::npos && pos4 != std::string::npos && pos5 != std::string::npos)
		{
			size_t start_pos = pos1 + 9;
			size_t end_pos = line.find('&', start_pos);
			filename = line.substr(start_pos, end_pos - start_pos);
			start_pos = pos3 + 5;
			end_pos = line.find('&', start_pos);
			name = line.substr(start_pos, end_pos - start_pos);
			start_pos = pos4 + 6;
			end_pos = line.find('&', start_pos);
			email = line.substr(start_pos, end_pos - start_pos);
			start_pos = pos5 + 8;
			end_pos = line.find('\0', start_pos);
			message = line.substr(start_pos, end_pos - start_pos);
		}
	}
	// std::cout << message << std::endl;
	
	if (!filename.empty())
	{
		filename = "./config/base_donnees/" + filename + ".txt";
		
		std::ifstream infile(filename.c_str());
		if (infile.is_open())
		{
			path = "./config/page/error_page_exist.html";
			std::string file_content = readFile(path);
			std::string reponse = httpHeaderResponse("200 Ok", "text/html", file_content);
			send(client_socket, reponse.c_str(), reponse.size(), 0);
			infile.close();
		}
		else
		{
			std::ofstream outfile(filename.c_str(), std::ios::app);
			if (outfile.is_open())
			{
				outfile << "name : " << name << std::endl;
				outfile << "email : " << email << std::endl;
				outfile << "content : " << message << std::endl;
				outfile << "--------------------------------------" << std::endl;
				outfile.flush();
				outfile.close();

				std::string path = "." + conf.getServer()[0].getRoot() + conf.getServer()[0].getIndex();
				std::string file_content = readFile(path);
				std::string reponse = httpHeaderResponse("200 Ok", "text/html", file_content);
				send(client_socket, reponse.c_str(), reponse.size(), 0);
			}
		}
	}
	else
		std::cout << "Unable to open file" << std::endl;
	filename.clear();
	name.clear();
	email.clear();
	message.clear();
}