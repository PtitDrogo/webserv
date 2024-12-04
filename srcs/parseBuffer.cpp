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

void autoIndex(std::string path, Config &conf, int client_socket, bool islocation)
{
	std::string finalPath;
	std::string reponse;
	std::string file_content;
	(void) conf;
	int	server_index = conf.getIndexOfClientServer(client_socket);

	(void) islocation;
	if (conf.getIsLocation() == true)
	{
		std::cout << "haaaaaaaaaaaaaaaaaaaaaaaaaaaaa----d--d-d-d-d-d-d--d" << std::endl;
		finalPath = path;
	}
	else 
		finalPath = "." + conf.getServer()[server_index].getRoot() + path;
	std::cout << "---------------------------------------finalPath = |" << finalPath << "|" << std::endl;
	std::vector<std::string> files = listDirectory(finalPath);
	file_content = generateAutoIndexPage(conf, finalPath, files, conf.getIsLocation());
	reponse = httpHeaderResponse("200 Ok", "text/html", file_content);
	std::cout << "reponse = |" << reponse << "|" << std::endl;
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

std::string CheckLocation(const std::string& path, Config& conf, const std::vector<location>& locationPath, bool& locationMatched, HttpRequest &req)
{
	(void) conf;
	(void) req;
	std::cout << "path = |" << path << "|" << std::endl;
	std::string cleanedPath = trim(path);

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


bool check_host(std::string line, Config &conf, int server_index)
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
		if (conf.getServer()[server_index].getHost().empty())
		{
			my_host = conf.getServer()[server_index].getServerName() + ":" + conf.getServer()[server_index].getPort();
		}
		else
			my_host = conf.getServer()[server_index].getHost();
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
	send(client_socket, response.c_str(), response.size(), 0);
	close(client_socket);
}

std::string parse_no_location(std::string path, Config &conf, int server_index, std::string finalPath, int client_socket, bool islocation)
{
	std::string reponse;
	std::string file_content;

	(void) islocation;
	if (path == "/")
	{
		if (!conf.getServer()[server_index].getIndex().empty())
			finalPath = "." + conf.getServer()[server_index].getRoot() + conf.getServer()[server_index].getIndex();
		else if (conf.getServer()[server_index].getAutoIndex() == "on")
		{
			autoIndex(path, conf, client_socket, conf.getIsLocation());
			return "";
		}
		else
			generate_html_page_error(conf, client_socket, "404");
	}
	else if (conf.getServer()[server_index].getAutoIndex() == "on" && conf.getServer()[server_index].getIndex().empty())
	{
		finalPath = "." + conf.getServer()[server_index].getRoot() + path;
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
		finalPath = "." + conf.getServer()[server_index].getRoot() + path;
	return finalPath;
}


bool isMethodAllowed(const std::string& allowedMethods, const std::string& reqMethod, Config &conf) {
    // Afficher les méthodes autorisées et la méthode demandée
    std::cout << "allowedMethods = |" << allowedMethods << "|" << std::endl;
    std::cout << "reqMethod = |" << reqMethod << "|" << std::endl;

	(void)	conf;
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


std::string parse_with_location(Config &conf, int client_socket, std::string finalPath, bool islocation, HttpRequest &req)
{
	// std::cout << "je suis laaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" << std::endl;
	std::cout << "allow method " << conf.getServer()[0].getLocation()[0].getAllowMethod() << std::endl;
	islocation = true;
	conf.setIsLocation(islocation);

	std::cout << req.getMethod() << std::endl;
	std::cout << "llowMethod = |" << conf.getServer()[0].getLocation()[0].getAllowMethod() << "|" << std::endl;
	if (isMethodAllowed(conf.getServer()[0].getLocation()[0].getAllowMethod(), req.getMethod(), conf) == false)
		std::cout << "errrreeeeeusssssssssssr" << std::endl;
	if (!conf.getServer()[0].getLocation()[0].getAllowMethod().empty())
		std::cout << "----------------ALLOW METHOD EMPTY" << std::endl;
	if (conf.getServer()[0].getLocation()[0].getAllowMethod().empty()) {
		std::cout << "Error: No allowed methods defined for location." << std::endl;
	}
	if (isMethodAllowed(conf.getServer()[0].getLocation()[0].getAllowMethod(), req.getMethod(), conf) == false && conf.getServer()[0].getLocation()[0].getAllowMethod().empty() == false)
	{
		std::cout << "errrreeeeeur" << std::endl;
		generate_html_page_error(conf, client_socket, "404");
		return "";
	}

	std::cout << "index =" << conf.getServer()[0].getLocation()[0].getIndex() << std::endl;
	if (conf.getServer()[0].getLocation()[0].getIndex().empty() == false)
	{
		finalPath = "." + conf.getServer()[0].getLocation()[0].getRoot() + conf.getServer()[0].getLocation()[0].getIndex();
		return finalPath;
	}
	else if (conf.getServer()[0].getLocation()[0].getAutoIndex() == "on")
	{
		if (conf.getServer()[0].getLocation()[0].getAutoIndex() == "on")
		{
			autoIndex(finalPath, conf, client_socket, conf.getIsLocation());
		}
		else
		{
			generate_html_page_error(conf, client_socket, "404");
			return "";
		}
	}
	std::map<std::string, std::string> redirMap = conf.getServer()[0].getLocation()[0].getRedir();
	for (std::map<std::string, std::string>::iterator it = redirMap.begin(); it != redirMap.end(); ++it)
	{
		std::string errorCode = it->first;
		std::string path = it->second;
		if (errorCode == "301" && path != "")
			sendRedirection(client_socket, path);
	}
	return finalPath;
}




void	parse_buffer_get(std::string buffer, Config &conf , int client_socket, HttpRequest &req)
{
	std::istringstream stream(buffer);
	std::string line;
	int	server_index = conf.getIndexOfClientServer(client_socket);
	std::string method;
	std::string path;
	std::string version;
	std::string finalPath;
	std::string pathLoc;
	std::string reponse;
	std::string file_content;
	std::vector<location> locationPath = conf.getServer()[server_index].getLocation();
	bool islocation = false;
	conf.setIsLocation(islocation);

	std::cout << "setttttttttttttttttttttttttttttttttttttlocation == " << conf.getIsLocation() << std::endl;
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
			std::cout << "pathloc = |" << pathLoc << "|" << std::endl;
			if (!locationMatched)	
				finalPath = parse_no_location(path, conf, server_index, pathLoc, client_socket, conf.getIsLocation());
			else
				finalPath = parse_with_location(conf, client_socket, pathLoc,  conf.getIsLocation(), req);
			// if (finalPath.empty() || finalPath == pathLoc)
			// 	return ;
			std::cout << "finalPath = |" << finalPath << "|" << std::endl;
		}
		if (check_host(line, conf, server_index) == false)
		{
			generate_html_page_error(conf, client_socket, "400");
			return ;
		}
	}
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
	int	server_index = conf.getIndexOfClientServer(client_socket);

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
	if (conf.getServer()[server_index].getLocation()[0].getAllowMethod().find("POST") == std::string::npos && conf.getServer()[server_index].getLocation()[0].getAllowMethod().empty() == false)
	{
		generate_html_page_error(conf, client_socket, "404");
		return ;
	}
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

				std::string path = "." + conf.getServer()[server_index].getRoot() + conf.getServer()[server_index].getIndex();
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









bool preparePostParse(int fd, char *buffer, Config &conf, int recv_value)
{
	std::string initial_data(buffer, recv_value);
	int	server_index = conf.getIndexOfClientServer(fd);
	size_t content_length_pos = initial_data.find("Content-Length: ");
	if (content_length_pos == std::string::npos)
	{
		generate_html_page_error(conf, fd, "400");
		return false;
	}

	size_t length_start = content_length_pos + 16;
	size_t length_end = initial_data.find("\r\n", length_start);
	int content_length = 0;
	std::istringstream(initial_data.substr(length_start, length_end - length_start)) >> content_length;

	int content_length_size_t = content_length;
	if (content_length_size_t > conf.getServer()[server_index].getMaxBodySize())
	{
		generate_html_page_error(conf, fd, "400");
		return false;
	}

	std::string body = initial_data;
	int total_received = body.size();
	while (total_received < content_length_size_t)
	{
		recv_value = recv(fd, buffer, sizeof(buffer), 0);
		if (recv_value <= 0)
		{
			std::cerr << "Erreur : données POST incomplètes." << std::endl;
			generate_html_page_error(conf, fd, "400");
			return false;
		}

		body.append(buffer, recv_value);
		total_received += recv_value;
	}
	parse_buffer_post(body, fd, conf);
	return (true);
}