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

void autoIndex(std::string path, Client& client)
{
	std::string finalPath;
	std::string reponse;
	std::string file_content;
	Server &server = client.getServer();

	// if (conf.getIsLocation() == true)
	if (client.getLocation() != NULL)
	{
		std::cout << "haaaaaaaaaaaaaaaaaaaaaaaaaaaaa----d--d-d-d-d-d-d--d" << std::endl;
		finalPath = path;
	}
	else 
		finalPath = "." + server.getRoot() + path;
	std::cout << "---------------------------------------finalPath = |" << finalPath << "|" << std::endl;
	std::vector<std::string> files = listDirectory(finalPath);
	file_content = generateAutoIndexPage(finalPath, files, client);
	reponse = httpHeaderResponse("200 Ok", "text/html", file_content);
	std::cout << "reponse = |" << reponse << "|" << std::endl;
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

std::string CheckLocation(const std::string& path, std::vector<location>& locationPath, Client& client)
{

	// std::cout << "path = |" << path << "|" << std::endl;
	std::string cleanedPath = trim(path);
	// std::cout << "cleanedPath = |" << cleanedPath << "|" << std::endl;
	// std::cout << "Location vector size is " << locationPath.size() << std::endl;
	for (size_t i = 0; i < locationPath.size(); ++i)
	{
		std::string locationStr = locationPath[i].getPath();
		locationStr = trim(locationStr);
		// std::cout << "IN STRING :" << cleanedPath << ", We are trying to find" << locationStr << std::endl;
		if (cleanedPath == locationStr) //This used to be find, it caused some bugs, but maybe was useful for other reasons.
		{
			// std::cout << "locationStr = |" << locationStr << "|" << std::endl;
			if (cleanedPath.size() <= locationStr.size())
			{
				client.setLocation(&locationPath[i]);
				return "." + locationPath[i].getRoot();
			}
			std::string relativePath = cleanedPath.substr(locationStr.size());
			if (relativePath.empty() || relativePath == "/")
			{
				if (!locationPath[i].getIndex().empty())
				{
					client.setLocation(&locationPath[i]);
					return "." + locationPath[i].getRoot() + locationPath[i].getIndex();
				}
				client.setLocation(&locationPath[i]);
				return "." + locationPath[i].getRoot();
			}
			client.setLocation(&locationPath[i]);
			return "." + locationPath[i].getRoot() + relativePath;
		}
	}
	// std::cout << std::endl << "NO LOCATION ASSIGNED" << std::endl;
	return "";
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

		// std::cout << "-------------host = |" << host << "|" << std::endl;
		std::string my_host;
		if (Server.getHost().empty())
		{
			my_host = Server.getServerName() + ":" + Server.getPort();
		}
		else
			my_host = Server.getHost();
		// std::cout << "-------------my_host = |" << my_host << "|" << std::endl;
		if (host != my_host)
		{
			std::cout << "host.conf = |" << my_host << "|" << std::endl;
			return false;
		}
	}
	return true;
}


void sendRedirection(int client_socket, const std::string& path)
{
	std::ostringstream responseStream;
	std::cout << "---------------------------------------------path = " << path << std::endl;
	responseStream << "HTTP/1.1 301 Moved Permanently\r\n"
				<< "Location: " << path << "\r\n"
				<< "Content-Type: text/html\r\n"
				<< "Content-Length: 0\r\n"
				<< "Connection: close\r\n"
				<< "\r\n";
	std::string response = responseStream.str();
	std::cout << GREEN "response = |" << response << "|" << RESET << std::endl;
	send(client_socket, response.c_str(), response.size(), 0);
	// close(client_socket); //Derriere il est tj dans la liste de pollfd de poll !, Il va toujours etre dans ma map de clients;
}

void check_password_username()
{
	std::string pathWord;
	std::string UserName;

	std::cout << "je rentre dans le password --------------------------------------" << std::endl;
}

std::string parse_no_location(std::string path, Client &client, std::string finalPath, int client_socket)
{
	std::string reponse;
	std::string file_content;
	Server& server = client.getServer();

	if (path == "/")
	{
		if (!server.getIndex().empty())
			finalPath = "." + server.getRoot() + server.getIndex();
		else if (server.getAutoIndex() == "on")
		{
			autoIndex(path, client);
			return "";
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
			file_content = generateAutoIndexPage(finalPath, files, client);
		}
		else if (isExtension(finalPath))
			file_content = readFile(finalPath);
		reponse = httpHeaderResponse("200 Ok", "text/html", file_content);
		std::cout << "reponse = " << reponse << std::endl;
		reponse = httpHeaderResponse("200 Ok", "text/html", file_content);
		send(client_socket, reponse.c_str(), reponse.size(), 0);
		return "";
	}
	else
		finalPath = "." + server.getRoot() + path;
	return finalPath;
}


bool isMethodAllowed(const std::string& allowedMethods, const std::string& reqMethod) {
    // Afficher les méthodes autorisées et la méthode demandée
    std::cout << "allowedMethods = |" << allowedMethods << "|" << std::endl;
    std::cout << "reqMethod = |" << reqMethod << "|" << std::endl;

    // Trimmer la méthode demandée
    std::string trimmedReqMethod = reqMethod;
    trimmedReqMethod.erase(0, trimmedReqMethod.find_first_not_of(" \t"));
    trimmedReqMethod.erase(trimmedReqMethod.find_last_not_of(" \t") + 1);
    
    std::stringstream ss(allowedMethods);
    std::string method;

    // Parcourir les méthodes autorisées
    while (std::getline(ss, method, ' ')) {
        // Trimmer chaque méthode autorisée
        method.erase(0, method.find_first_not_of(" \t"));
        method.erase(method.find_last_not_of(" \t") + 1);

        // Comparer avec la méthode demandée
        if (method == trimmedReqMethod)
		{
            return true;  // Match trouvé
        }
    }
    return false;  // Aucun match trouvé
}


std::string parse_with_location(Client &client, std::string finalPath, HttpRequest &req)
{
	std::cout << "je suis laaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" << std::endl;
	if (client.getLocation() != NULL)
	{	
		
		std::cout << "C'est tfreydie, location treated is :" << client.getLocation()->getPath() << std::endl;
		std::cout << "allow method " << client.getLocation()->getAllowMethod() << std::endl;
	}
	location location = *(client.getLocation());

	std::cout << req.getMethod() << std::endl;
	if (isMethodAllowed(location.getAllowMethod(), req.getMethod()) == false)
		std::cout << "isMethodAllowed = false" << std::endl;
	if (location.getAllowMethod().empty() == false)
		std::cout << "location.getAllowMethod().empty() = false" << std::endl;
	if (isMethodAllowed(location.getAllowMethod(), req.getMethod()) == false && location.getAllowMethod().empty() == false)
	{
		std::cout << "errrreeeeeur" << std::endl;
		generate_html_page_error(client, "404");
		return "";
	}
	if (location.getRedir().empty() == false)
	{
		std::cout << "je rentre la ------------------------------------------------------" << std::endl;
		std::map<std::string, std::string> redirMap = location.getRedir();
		for (std::map<std::string, std::string>::iterator it = redirMap.begin(); it != redirMap.end(); ++it)
		{
			std::string errorCode = it->first;
			std::string path = it->second;
			if (errorCode == "301" && path != "")
				sendRedirection(client.getSocket(), path);
			return "";
		}
	}
	std::cout << "index =" << location.getIndex() << std::endl;
	if (location.getIndex().empty() == false)
	{
		finalPath = "." + location.getRoot() + location.getIndex();
		return finalPath;
	}
	else if (location.getAutoIndex() == "on")
	{
		if (location.getAutoIndex() == "on")
		{
			autoIndex(finalPath, client);
		}
		else
		{
			generate_html_page_error(client, "404");
			return "";
		}
	}
	return finalPath;
}




void	parse_buffer_get(Client &client, HttpRequest &req)
{
	Server& 	server = client.getServer();
	int 		client_socket = client.getSocket();
	std::istringstream stream(client.getRequest());
	std::string line;
	std::string method;
	std::string path;
	std::string version;
	std::string finalPath;
	std::string pathLoc;
	std::string reponse;
	std::string file_content;
	std::vector<location> locationPath = server.getLocation();

	if (client.getLocation() != NULL)
		std::cout << "content of location is : " << client.getLocation()->getPath() << std::endl;
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
			// std::cout << "path = |" << path << "|" << std::endl;
			// std::cout << "locationPath.size() = " << locationPath.size() << std::endl;
			// std::cout << "locationPath[0].getPath() = |" << locationPath[0].getPath() << "|" << std::endl;
			pathLoc = CheckLocation(path, locationPath, client);
			// std::cout << "pathloc3 = |" << pathLoc << "|" << std::endl;
			if (client.getLocation() == NULL)	
			{
				finalPath = parse_no_location(path, client, pathLoc, client_socket);
			}
			else
			{
				finalPath = parse_with_location(client, pathLoc, req);
			}
			if (finalPath.empty() || finalPath == pathLoc)
			{
				return ;
			}
			// std::cout << "finalPathhhhhhhhhhh = |" << finalPath << "|" << std::endl;
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





std::string httpHeaderResponseForCookies(std::string code, std::string contentType, std::string content, Cookies &cook)
{
    std::string response = "HTTP/1.1 " + code + "\r\n";
    response += "Content-Type: " + contentType + "\r\n";
    
    // Utilisation de stringstream pour la conversion
    std::stringstream ss;
    ss << content.size();
    response += "Content-Length: " + ss.str() + "\r\n";
    
    response += "Connection: close\r\n";

    std::map<std::string, std::string> cookies = cook.getCookies();
    for (std::map<std::string, std::string>::iterator it = cookies.begin(); it != cookies.end(); ++it)
    {
        response += "Set-Cookie: " + it->first + "=" + it->second + "; Path=/; HttpOnly; Secure\r\n";
    }

    response += "\r\n" + content;

    return response;
}





std::string generate_token()
{
	std::string token;
	std::string charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	for (size_t i = 0; i < 16; i++)
	{
		token += charset[rand() % charset.size()];
	}
	return token;
}

std::string handle_connexion(std::string username, std::string password, Cookies &cook)
{
    std::map<std::string, std::string> users;
    users[username] = password;
	bool isConnect = false;

    // Vérifie si le cookie "session_token" existe, ce qui signifie que l'utilisateur est déjà connecté
    std::map<std::string, std::string> cookies = cook.getCookies();
    if (cookies.find("session_token") != cookies.end())
	{
        std::cout << "Utilisateur déjà connecté" << std::endl;
        // Répond avec un message indiquant que l'utilisateur est déjà connecté
		std::string path = "./config/page/dejaConnecter.html";
		std::string file_content = readFile(path);
		std::string response = httpHeaderResponseForCookies("200 Ok", "text/html", file_content, cook);
		return response;
    }

    // Vérifie que le nom d'utilisateur et le mot de passe ne sont pas vides
    if (password.empty() == false && username.empty() == false)
    {
        std::string response;
        std::string path = "./config/page/IsConnected.html";
        std::map<std::string, std::string>::iterator it = users.find(username);
        if (it != users.end() && it->second == password)
        {
			isConnect = true;
			cook.setIsConnect(isConnect);
            std::cout << "Connexion réussie" << std::endl;
            std::string session_token = "token_" + username;
            cook.setCookies("session_token", session_token);  // Set the session token cookie

            std::string file_content = readFile(path);
            response = httpHeaderResponseForCookies("200 Ok", "text/html", file_content, cook);
        }
        else
        {
			isConnect = false;
			cook.setIsConnect(isConnect);
            std::cout << "Connexion échouée" << std::endl;
            response = "HTTP/1.1 401 Unauthorized\r\n";
            response += "Content-Type: text/html\r\n\r\n";
            response += "<h1>Identifiants incorrects</h1>";
        }

        return response;
    }

    return "HTTP/1.1 400 Bad Request\r\nContent-Type: text/html\r\n\r\n<h1>Veuillez fournir un nom d'utilisateur et un mot de passe</h1>";
}


std::string handle_deconnexion(Cookies &cook)
{
	std::map<std::string, std::string> cookies = cook.getCookies();
	
	// Vérifie si un cookie de session existe, ce qui signifie que l'utilisateur est connecté
	if (cookies.find("session_token") != cookies.end()) {
		std::cout << "Déconnexion réussie" << std::endl;
		std::string path = "./config/page/isDeconnected.html";
		// Supprime le cookie "session_token"
		cook.deleteCookie("session_token");

		// Redirige vers la page d'accueil ou affiche un message de déconnexion
		std::string file_content = readFile(path);
		std::string response = httpHeaderResponse("200 Ok", "text/html", file_content);
		std::cout << "response = |" << response << "|" << std::endl;
		return response;
	}
	else
	{
		std::string path = "./config/page/PasConnecter.html";
		std::cout << "Aucun utilisateur connecté" << std::endl;
		// L'utilisateur n'est pas connecté
		std::string file_content = readFile(path);
		std::string response = httpHeaderResponse("200 Ok", "text/html", file_content);
		std::cout << "response = |" << response << "|" << std::endl;

		return response;
	}
}


void parse_buffer_post(Client& client, Cookies &cook)
{
	std::istringstream stream(client.getRequest());
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
	std::string filename;
	std::string username;
	std::string password;
	if (client.getLocation() != NULL && client.getLocation()->getAllowMethod().find("POST") == std::string::npos && client.getLocation()->getAllowMethod().empty() == false)
	{
		generate_html_page_error(client, "404");
		return ;
	}
	std::cout << "path ======================= |" << path << "|" << std::endl;
	while (std::getline(stream, line))
	{
		size_t pos1 = line.find("FileName=");
		size_t pos3 = line.find("name=");
		size_t pos4 = line.find("email=");
		size_t pos5 = line.find("message=");
		size_t pos6 = line.find("username=");
		size_t pos7 = line.find("password=");
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
		if (pos6 != std::string::npos && pos7 != std::string::npos)
		{
			std::cout << "je rentre dans le login --------------------------------------" << std::endl;
			size_t start_pos = pos6 + 9;
			size_t end_pos = line.find('&', start_pos);
			username = line.substr(start_pos, end_pos - start_pos);
			start_pos = pos7 + 9;
			end_pos = line.find('\0', start_pos);
			password = line.substr(start_pos, end_pos - start_pos);
		}

	}
	// std::cout << message << std::endl;
	std::cout << "username = |" << username << "|" << std::endl;
	std::cout << "password = |" << password << "|" << std::endl;
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

	if (password.empty() == false && username.empty() == false)
	{
		std::string response;
		response = handle_connexion(username, password, cook);
		send(client.getSocket(), response.c_str(), response.size(), 0);
		return ;
	}
	if (cook.getIsConnect() == false || cook.getIsConnect() == true)
	{
		std::cout << "je rentre dans le Unlog --------------------------------------" << std::endl;
		std::string response2 = handle_deconnexion(cook);
		send(client.getSocket(), response2.c_str(), response2.size(), 0);
		return ;
	}
	else
		std::cout << "Unable to open file" << std::endl;



	filename.clear();
	name.clear();
	email.clear();
	message.clear();
	username.clear();
	password.clear();
}



























bool preparePostParse(Client& client, Cookies &cook)
{
	const Server& 	server = client.getServer();

	if (client.getContentLength() == std::string::npos)
	{
		generate_html_page_error(client, "400");
		return false;
	}

	if (server.getMaxBodySize() != -1 && client.getContentLength() > (size_t)server.getMaxBodySize())
	{
		std::cout << "MAXBODYSIZE IS :" << server.getMaxBodySize() << std::endl;
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
		client.setbodyEnd(lastBoundaryPos);

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
		parse_buffer_post(client, cook);
	return true;
}


static bool file_exists_parsebuffer(const char *path)
{
	struct stat st;
    
    if (stat(path, &st) != 0)
		return false;
	if (!S_ISREG(st.st_mode)) 
        return false;
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
		if (file_exists_parsebuffer(filePath.c_str()) == false)
		{
			generate_html_page_error(client, "404");
			return false;
		}
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
		parse_buffer_get(client, req);
	return true;
}
