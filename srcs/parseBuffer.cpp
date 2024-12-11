#include "Webserv.hpp"

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

void autoIndex(std::string path, const Client& client)
{
	std::string finalPath;
	std::string reponse;
	std::string file_content;
	const Server& server = client.getServer();

	finalPath = "." + server.getRoot() + path;
	std::vector<std::string> files = listDirectory(finalPath);
	file_content = generateAutoIndexPage(finalPath, files);
	reponse = httpHeaderResponse("200 Ok", "text/html", file_content);
	std::cout << "reponse = " << reponse << std::endl;
	reponse = httpHeaderResponse("200 Ok", "text/html", file_content);
	send(client.getSocket(), reponse.c_str(), reponse.size(), 0);
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

void parse_allow_method(Client& client, HttpRequest &req)
{
	// Récupérer les méthodes autorisées
	Server& server = client.getServer();
    std::string allow_methods = server.getLocation()[0].getAllowMethod(); //This shouldnt be 0 !
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
    std::string request_method = req.getMethod();
    request_method.erase(request_method.find_last_not_of(" \t\r\n") + 1); // Nettoyage
    std::cout << "DEBUG: req.getMetode() (trimmed) = '" << request_method << "'" << std::endl;

    // Vérifier si la méthode est dans la liste des méthodes autorisées
    if (std::find(methods.begin(), methods.end(), request_method) == methods.end()) {
        // La méthode n'est pas autorisée -> Erreur 400
		// std::cout << "Allow methods" << allow_methods << "request_method" << request_method << std::endl;
        generate_html_page_error(client, "400");
    }
}

std::string CheckLocation(const std::string& path, Client& client, const std::vector<location>& locationPath, bool& locationMatched, HttpRequest &req)
{
    // Nettoyer le path en supprimant les espaces avant et après

	parse_allow_method(client, req);
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


bool check_host(std::string line, const Server& Server)
{
	size_t pos = line.find("Host: ");
	if (pos != std::string::npos)
	{
		std::string host = line.substr(pos + 6);
		// Nettoyage de la chaîne pour éviter les caractères superflus
		host = host.substr(0, host.find_first_of("\r\n"));
		host.erase(std::remove_if(host.begin(), host.end(), ::isspace), host.end());

		std::cout << "-------------host = |" << host << "|" << std::endl;

		std::string expected_host = Server.getServerName() + ":" + Server.getPort();
		if (host != expected_host)
		{
			std::cout << "host.conf = |" << expected_host << "|" << std::endl;
			std::cout << "Error: Host not found" << std::endl;
			return false;
		}
	}
	return true;
}


void	parse_buffer_get(Client& client, std::string buffer, HttpRequest &req)
{
	Server& 	server = client.getServer();
	int 		client_socket = client.getSocket();
	std::istringstream stream(buffer);
	std::string line;
	std::string method;
	std::string path;
	std::string version;
	std::string finalPath;
	std::string reponse;
	std::string file_content;
	std::vector<location> locationPath = server.getLocation();
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
			// std::cout << "path-----------------------------------" << path << std::endl;
			bool locationMatched = false;
			finalPath = CheckLocation(path, client, locationPath, locationMatched, req);
			// std::cout << "finalPath-----------------------------------" << finalPath << std::endl;
			if (!locationMatched)
			{
				if (path == "/")
				{
					if (!server.getIndex().empty())
						finalPath = "." + server.getRoot() + server.getIndex();
					else if (server.getAutoIndex() == "on")
					{
						autoIndex(path, client);
						return ;
					}
					else
						generate_html_page_error(client, "404");
				}
				else if (server.getAutoIndex() == "on" && server.getIndex().empty())
				{
					finalPath = "." + server.getRoot() + path;
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
					finalPath = "." + server.getRoot() + path;
			}
		}
		if (check_host(line, server) == false)
		{
			generate_html_page_error(client, "400");
			return ;
		}
	}
	// std::cout << "------------voici le path------------|" << finalPath << "|" << std::endl;
	file_content = readFile(finalPath);
	if (file_content.empty())
		generate_html_page_error(client, "404");
	reponse = httpHeaderResponse("200 Ok", "text/html", file_content);
	send(client_socket, reponse.c_str(), reponse.size(), 0);
}

void parse_buffer_post(const Client& client, std::string buffer)
{
	std::istringstream stream(buffer);
	std::string line;
	const Server& 	server = client.getServer();

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
			send(client.getSocket(), reponse.c_str(), reponse.size(), 0);
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

				std::string path = "." + server.getRoot() + server.getIndex();
				std::string file_content = readFile(path);
				std::string reponse = httpHeaderResponse("200 Ok", "text/html", file_content);
				send(client.getSocket(), reponse.c_str(), reponse.size(), 0);
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

bool preparePostParse(Client& client)
{
	const Server& 	server = client.getServer();

	if (client.getContentLength() == std::string::npos)
	{
		generate_html_page_error(client, "400");
		return false;
	}

	if (client.getContentLength() > (size_t)server.getMaxBodySize())
	{
		generate_html_page_error(client, "413");
		return false;
	}


	if (client.getRequest().find("Content-Type: multipart/form-data") != std::string::npos) {


		// foutre tout sa dans une fonction de la class client/////////////////////////////////

		size_t boundaryPos = client.getRequest().find("boundary=");
		if (boundaryPos != std::string::npos) {
			// extraire tout ce qui vient apres "boundary="
			std::string boundary = client.getRequest().substr(boundaryPos + 9); // 9 = longueur de "boundary="
			
			// trouver la premiere position ou les tirets s'arretent
			std::size_t nonDashPos = boundary.find_first_not_of('-');
			if (nonDashPos != std::string::npos) {
				// extraire tout ce qui vient apres les tirets
				std::string result = boundary.substr(nonDashPos, boundary.find("\r\n", nonDashPos) - nonDashPos);
				client.setBoundary(result);
			}
			else {
				std::cout << "Aucun contenu après les tirets." << std::endl;
				//generate a html page error !!!
				// return ;
			}
		}
		else {
			std::cout << "La clé 'boundary=' est introuvable." << std::endl;
			//generate a html page error !!!
			// return ;
		}

		//vas chercher de lq dernier boundary de la requete
		size_t lastBoundaryPos = client.getRequest().find("--" + client.getBoundary() + "--");
		lastBoundaryPos += 6 + client.getBoundary().size(); // 6 = "--" + "--" + "\r\n"
		// lastBoundaryPos -= client.getHeadEnd();

		std::cout << "lastBoundaryPos = " << lastBoundaryPos << std::endl;
		client.stebodyEnd(lastBoundaryPos);

		////////////////////////////////////////////////////////////////////////////////////////


		std::cout << MAGENTA << "Extract data form request" << RESET << std::endl;

		client.extractFileName();

		std::cout << MAGENTA << "fileName: \"" << client.getFileName() << "\"" << RESET << std::endl; // debug filename
		
		client.extractContentType();

		std::cout << MAGENTA << "File saved successfully: " << client.getFileName() << RESET << std::endl; // debug extra content type

		// redirections vers la page home
		std::string path = "." + server.getRoot() + server.getIndex();
		std::string file_content = readFile(path);
		std::string response = httpHeaderResponse("200 Ok", "text/html", file_content);
		send(client.getSocket(), response.c_str(), response.size(), 0);
	}
	else
		parse_buffer_post(client, client.getBody()); // MODIF DE LLITOT j'envoie _body avec la class client je ne sais pas si c'est bon ?? au lieu de body tout cours qui etqit calculer dans la fonction parse_buffer_post
	return true;
}


bool prepareGetParse(Client& client, HttpRequest &req) {

	// const Server& 	server = client.getServer();

	if (client.getRequest().find("GET /config/base_donnees/") != std::string::npos)
	{
		// Extraire le nom du fichier depuis l'URL de la requête
        std::string filename = client.getRequest().substr(client.getRequest().find("/config/base_donnees/") + 21);

        if (filename.find("?fileName=") != std::string::npos) {
            filename = filename.substr(filename.find("?fileName=") + 10, filename.find(" ") - filename.find("?fileName=") - 10);
        }
		// else
        //     filename = filename.substr(filename.find("/config/base_donnees/"));

        std::string filePath = "./config/base_donnees/" + filename;

		std::cout << MAGENTA << "filePath: \"" << filePath << "\"" << RESET << std::endl; // debug filename


        std::string fileContent = readFile(filePath);
		std::cout << "fileContent = " << fileContent << std::endl;

        if (fileContent.empty()) {
            // Si le fichier n'est pas trouvé, envoyer une réponse d'erreur 404
            std::string response = "HTTP/1.1 404 Not Found\r\n";
            response += "Content-Type: text/plain\r\n\r\n";
            response += "File not found.\r\n";
            send(client.getSocket(), response.c_str(), response.size(), 0);
            return false;
        }

        // Préparer la réponse HTTP avec les en-têtes appropriés pour un téléchargement de fichier
        std::stringstream rep;
        rep << "HTTP/1.1 200 OK\r\n";
        rep << "Content-Type: application/octet-stream\r\n";
		rep << "content-length: " << fileContent.size() << "\r\n";
        rep << "Content-Disposition: attachment; filename=\"" << filename << "\"\r\n";
		rep << "Connection: close\r\n";
		rep << "\r\n";
		rep << fileContent;


		std::cout << "rep = " << rep.str() << std::endl;

        // Envoyer les en-têtes HTTP
        send(client.getSocket(), rep.str().c_str(), rep.str().size(), 0);
	}
	else
		parse_buffer_get(client, client.getRequest(), req);
	return true;
}
