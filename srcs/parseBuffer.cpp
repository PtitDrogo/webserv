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

void autoIndex(std::string path, Config &conf, Client& client, bool islocation)
{
	std::string finalPath;
	std::string reponse;
	std::string file_content;
	(void) conf;
	Server &server = client.getServer();

	(void) islocation;
	if (conf.getIsLocation() == true)
	{
		std::cout << "haaaaaaaaaaaaaaaaaaaaaaaaaaaaa----d--d-d-d-d-d-d--d" << std::endl;
		finalPath = path;
	}
	else 
		finalPath = "." + server.getRoot() + path;
	std::cout << "---------------------------------------finalPath = |" << finalPath << "|" << std::endl;
	std::vector<std::string> files = listDirectory(finalPath);
	file_content = generateAutoIndexPage(conf, finalPath, files, conf.getIsLocation());
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

std::string CheckLocation(const std::string& path, Config& conf, const std::vector<location>& locationPath, bool& locationMatched, HttpRequest &req)
{
	(void) conf;
	(void) req;
	std::cout << "path = |" << path << "|" << std::endl;
	std::string cleanedPath = trim(path);
	std::cout << "cleanedPath = |" << cleanedPath << "|" << std::endl;
	for (size_t i = 0; i < locationPath.size(); ++i)
	{
		std::string locationStr = locationPath[i].getPath();
		locationStr = trim(locationStr);
		if (cleanedPath.find(locationStr) == 0)
		{
			std::cout << "locationStr = |" << locationStr << "|" << std::endl;
			if (cleanedPath.size() <= locationStr.size())
			{
				locationMatched = true;
				return "." + locationPath[i].getRoot();
			}
			std::string relativePath = cleanedPath.substr(locationStr.size());
			if (relativePath.empty() || relativePath == "/")
			{
				if (!locationPath[i].getIndex().empty())
				{
					locationMatched = true;
					return "." + locationPath[i].getRoot() + locationPath[i].getIndex();
				}
				locationMatched = true;
				return "." + locationPath[i].getRoot();
			}
			locationMatched = true;
			return "." + locationPath[i].getRoot() + relativePath;
		}
	}
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

		std::cout << "-------------host = |" << host << "|" << std::endl;
		std::string my_host;
		if (Server.getHost().empty())
		{
			my_host = Server.getServerName() + ":" + Server.getPort();
		}
		else
			my_host = Server.getHost();
		std::cout << "-------------my_host = |" << my_host << "|" << std::endl;
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
	std::cout << RED "response = |" << response << "|" << RESET << std::endl;
	send(client_socket, response.c_str(), response.size(), 0);
}


std::string parse_no_location(std::string path, Config &conf, Client &client, std::string finalPath, int client_socket, bool islocation)
{
	std::string reponse;
	std::string file_content;
	Server& server = client.getServer();

	(void) islocation;
	if (path == "/")
	{
		if (!server.getIndex().empty())
		{
			finalPath = "." + server.getRoot() + server.getIndex();
			return finalPath;
		}
		if (server.getAutoIndex() == "on")
		{
			autoIndex(path, conf, client, conf.getIsLocation());
			return "";
		}
		else
		{
			generate_html_page_error(client, "404");
		}
	}
	else if (server.getAutoIndex() == "on" && server.getIndex().empty())
	{
		finalPath = "." + server.getRoot() + path;
		if (!isExtension(finalPath))
		{
			std::vector<std::string> files = listDirectory(finalPath);
			file_content = generateAutoIndexPage(conf, finalPath, files, conf.getIsLocation());
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


bool isMethodAllowed(const std::string& allowedMethods, const std::string& reqMethod, Config &conf)
{
	(void)	conf;
	std::string trimmedReqMethod = reqMethod;
	trimmedReqMethod.erase(0, trimmedReqMethod.find_first_not_of(" \t"));
	trimmedReqMethod.erase(trimmedReqMethod.find_last_not_of(" \t") + 1);

	std::stringstream ss(allowedMethods);
	std::string method;

	while (std::getline(ss, method, ' '))
	{
		method.erase(0, method.find_first_not_of(" \t"));
		method.erase(method.find_last_not_of(" \t") + 1);
		if (method == trimmedReqMethod)
			return true;
	}
	return false;
}


std::string parse_with_location(Config &conf, Client &client, std::string finalPath, bool islocation, HttpRequest &req)
{
	Server& server = client.getServer();
	islocation = true;
	conf.setIsLocation(islocation);

	std::cout << req.getMethod() << std::endl;
	if (isMethodAllowed(server.getLocation()[0].getAllowMethod(), req.getMethod(), conf) == false && server.getLocation()[0].getAllowMethod().empty() == false)
	{
		generate_html_page_error(client, "404");
		return "";
	}
	if (server.getLocation()[0].getRedir().empty() == false)
	{
		std::map<std::string, std::string> redirMap = server.getLocation()[0].getRedir();
		for (std::map<std::string, std::string>::iterator it = redirMap.begin(); it != redirMap.end(); ++it)
		{
			std::string errorCode = it->first;
			std::string path = it->second;
			if (errorCode == "301" && path != "")
				sendRedirection(client.getSocket(), path);
			return "";
		}
	}
	if (server.getLocation()[0].getIndex().empty() == false)
	{
		finalPath = "." + server.getLocation()[0].getRoot() + server.getLocation()[0].getIndex();
		return finalPath;
	}
	else if (server.getLocation()[0].getAutoIndex() == "on")
	{
		if (server.getLocation()[0].getAutoIndex() == "on")
			autoIndex(finalPath, conf, client, conf.getIsLocation());
		else
		{
			generate_html_page_error(client, "404");
			return "";
		}
	}
	return finalPath;
}




void	parse_buffer_get(std::string buffer, Config &conf , Client &client, HttpRequest &req)
{
	Server& 	server = client.getServer();
	int 		client_socket = client.getSocket();
	std::istringstream stream(buffer);
	std::string line;
	std::string method;
	std::string path;
	std::string version;
	std::string finalPath;
	std::string pathLoc;
	std::string reponse;
	std::string file_content;
	std::vector<location> locationPath = server.getLocation();
	bool islocation = false;
	conf.setIsLocation(islocation);

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
			bool locationMatched = false;
			pathLoc = CheckLocation(path, conf, locationPath, locationMatched, req);
			if (!locationMatched)	
				finalPath = parse_no_location(path, conf, client, pathLoc, client_socket, conf.getIsLocation());
			else
				finalPath = parse_with_location(conf, client, pathLoc,  conf.getIsLocation(), req);
			if (finalPath.empty() || finalPath == pathLoc)
				return ;
		}
		if (check_host(line, server) == false)
		{
			generate_html_page_error(client, "400");
			return ;
		}
	}
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


std::string handle_connexion(std::string username, std::string password, Cookies &cook)
{
	std::map<std::string, std::string> users;
	users[username] = password;
	bool isConnect = false;
	

	std::map<std::string, std::string> cookies = cook.getCookies();
	if (cookies.find("session_token") != cookies.end())
	{
		std::cout << "Utilisateur déjà connecté" << std::endl;
		std::string path = "./config/page/dejaConnecter.html";
		std::string file_content = readFile(path);
		std::string response = httpHeaderResponseForCookies("200 Ok", "text/html", file_content, cook);
		return response;
	}

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
			cook.setCookies("session_token", session_token);

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
	
	if (cookies.find("session_token") != cookies.end())
	{
		std::cout << "Déconnexion réussie" << std::endl;
		std::string path = "./config/page/isDeconnected.html";
		cook.deleteCookie("session_token");
		std::string file_content = readFile(path);
		std::string response = httpHeaderResponse("200 Ok", "text/html", file_content);
		std::cout << "response = |" << response << "|" << std::endl;
		return response;
	}
	else
	{
		std::string path = "./config/page/PasConnecter.html";
		std::cout << "Aucun utilisateur connecté" << std::endl;
		std::string file_content = readFile(path);
		std::string response = httpHeaderResponse("200 Ok", "text/html", file_content);
		std::cout << "response = |" << response << "|" << std::endl;
		return response;
	}
}


void parse_buffer_post(const Client& client, std::string buffer, Cookies &cook)
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
	std::string filename;
	std::string username;
	std::string password;
	if (server.getLocation()[0].getAllowMethod().find("POST") == std::string::npos && server.getLocation()[0].getAllowMethod().empty() == false)
	{
		generate_html_page_error(client, "404");
		return ;
	}
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
			size_t start_pos = pos6 + 9;
			size_t end_pos = line.find('&', start_pos);
			username = line.substr(start_pos, end_pos - start_pos);
			start_pos = pos7 + 9;
			end_pos = line.find('\0', start_pos);
			password = line.substr(start_pos, end_pos - start_pos);
		}

	}
	if (password.empty() == false && username.empty() == false)
	{
		std::string response;
		response = handle_connexion(username, password, cook);
		send(client.getSocket(), response.c_str(), response.size(), 0);
		return ;
	}
	if (cook.getIsConnect() == true)
	{
		std::string response2 = handle_deconnexion(cook);
		send(client.getSocket(), response2.c_str(), response2.size(), 0);
		return ;
	}
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
			return ;
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
				return ;
			}
			else
				std::cout << "Unable to open file" << std::endl;
		}
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

bool preparePostParse(const Client& client, std::string buffer, Cookies &cook)
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

	std::string body = buffer.substr(buffer.find("\r\n\r\n") + 4);
	std::string boundary = body.substr(0, body.find("\r\n"));
	std::string numericBoundary;
    for (size_t i = 0; i < boundary.size(); ++i)
	{
        if (std::isdigit(boundary[i]))
            numericBoundary += boundary[i];
    }
    boundary = numericBoundary;

	if (client.getRequest().find("Content-Type: multipart/form-data") != std::string::npos)
	{
		std::cout << MAGENTA << "Extract data form request" << RESET << std::endl;
		std::string body = client.getRequest().substr(client.getHeadEnd());

		const std::string key = "filename=\"";
		size_t fileNamePos = body.find(key);

		if (fileNamePos == std::string::npos) {
			std::cerr << "Error: Filename not found." << std::endl;
			return false;
		}

		size_t endPos = body.find("\"\r\n", fileNamePos);
		if (endPos == std::string::npos) {
			std::cerr << "Error: Invalid filename format." << std::endl;
			return false;
		}

		std::string fileName = body.substr(fileNamePos + key.length(), endPos - (fileNamePos + key.length()));

		// // Validation du nom de fichier pour éviter les chemins traversants
		// if (fileName.find("/") != std::string::npos || fileName.find("..") != std::string::npos) {
		// 	std::cerr << "Error: Invalid filename." << std::endl;
		// 	return false;
		// }

		std::cout << MAGENTA << "fileName: \"" << fileName << "\"" << RESET << std::endl;

		size_t contentTypePos = body.find("Content-Type:", body.find("--" + boundary));
		if (contentTypePos == std::string::npos) {
			std::cerr << "Error: Content-Type not found." << std::endl;
			return false;
		}

		size_t contentTypeEnd = body.find("\r\n", contentTypePos);
		if (contentTypeEnd == std::string::npos) {
			std::cerr << "Error: Malformed Content-Type header." << std::endl;
			return false;
		}

		std::string contentType = body.substr(contentTypePos + std::string("Content-Type: ").length(), contentTypeEnd - (contentTypePos + std::string("Content-Type: ").length()));

		std::cout << MAGENTA << "contentType: \"" << contentType << "\"" << RESET << std::endl;

		size_t contentStart = body.find("\r\n\r\n", contentTypeEnd);
		if (contentStart == std::string::npos) {
			std::cerr << "Error: Content start not found." << std::endl;
			return false;
		}
		contentStart += 4;

		// Pour Firefox, on soustrait 30 pour gérer correctement la fin du contenu
		size_t contentEnd = body.find("--" + boundary, contentStart) - 30; 
		if (contentEnd == std::string::npos) {
			std::cerr << "Error: Content end not found." << std::endl;
			return false;
		}

		std::string fileContent = body.substr(contentStart, contentEnd - contentStart);

		// std::cout << fileContent << std::endl;

		fileName = "./config/base_donnees/" + fileName;

		std::ofstream outFile(fileName.c_str(), std::ios::binary);
		if (!outFile) {
			std::cerr << "Error: Unable to create file: " << fileName << std::endl;
			return false;
		}
		outFile.write(fileContent.data(), fileContent.size());
		outFile.close();

		std::cout << MAGENTA << "File saved successfully: " << fileName << RESET << std::endl;

		std::string path = "." + server.getRoot() + server.getIndex();
		std::string file_content = readFile(path);
		std::string reponse = httpHeaderResponse("200 Ok", "text/html", file_content);
		send(client.getSocket(), reponse.c_str(), reponse.size(), 0);
	}
	else
		parse_buffer_post(client, body, cook);
	return true;
}