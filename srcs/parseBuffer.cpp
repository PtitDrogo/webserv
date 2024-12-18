#include "Webserv.hpp"

bool	parse_buffer_get(Client &client, Cookies& cook, HttpRequest &req)
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

	if (!stream)
		return false;
	while (std::getline(stream, line))
	{
		size_t pos1 = line.find("GET");
		size_t pos2 = line.find("HTTP");
		size_t pos8 = line.find("session_token=");
		if (pos1 != std::string::npos && pos2 != std::string::npos)
		{
			method = line.substr(pos1, 4);
			path = line.substr(pos1 + 4, pos2 - pos1 - 5);
			version = line.substr(pos2);

			pathLoc = CheckLocation(path, locationPath, client);
			if (client.getLocation() == NULL)
				finalPath = parse_no_location(path, client, pathLoc, client_socket);
			else
				finalPath = parse_with_location(client, pathLoc, req);
			if (finalPath.empty())
				return true;
		}

		if (check_host(line, server) == false)
		{
			return (generate_html_page_error(client, "400"));
		}
		if (pos8 != std::string::npos)
		{

			std::string cookies = line.substr(pos8 + strlen("session_token="));
			cookies = cookies.substr(0, cookies.find_first_of("\r\n"));
			req.setCookies(cookies);
		}
	}
	file_content = readFile(finalPath);
	if (file_content.empty())
		return (generate_html_page_error(client, "404"));
	if (!req.getCookies().empty())
	{
		std::map<std::string, Cookie> &cookies_map = cook.getCookies();
		std::map<std::string, Cookie>::iterator it = cookies_map.find(req.getCookies());
		if (it != cookies_map.end())
			file_content = injectUserHtml(file_content, it->second.username);
	}
	reponse = httpHeaderResponse("200 Ok", "text/html", file_content);
	if (send(client_socket, reponse.c_str(), reponse.size(), 0) == -1)
		return false;
	return true;
}



bool parse_buffer_post(Client& client, Cookies &cook, HttpRequest &req)
{
	std::istringstream stream(client.getRequest());
	std::string line;
	Server& 	server = client.getServer();

	if (!stream)
		return false;
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
		return (generate_html_page_error(client, "404"));
	}
	while (std::getline(stream, line))
	{
		size_t pos1 = line.find("FileName=");
		size_t pos3 = line.find("name=");
		size_t pos4 = line.find("email=");
		size_t pos5 = line.find("message=");
		size_t pos6 = line.find("username=");
		size_t pos7 = line.find("password=");
		size_t pos8 = line.find("session_token=");
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
		if (pos8 != std::string::npos)
		{
			std::string cookies = line.substr(pos8 + strlen("session_token="));
			cookies = cookies.substr(0, cookies.find_first_of("\r\n"));
			req.setCookies(cookies);
		}

	}
	if (!filename.empty())
	{
		filename = "." + client.getServer().getRoot() + "base_donnees/" + filename + ".txt";
		std::ifstream infile(filename.c_str());
		if (infile.is_open())
		{
			path = "." + client.getServer().getRoot() + "page/error_page_exist.html";
			std::string file_content = readFile(path);
			std::string reponse = httpHeaderResponse("200 Ok", "text/html", file_content);
			if (send(client.getSocket(), reponse.c_str(), reponse.size(), 0) == -1)
			{	
				infile.close();
				return false;
			}
			infile.close();
			return true;
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
				if (send(client.getSocket(), reponse.c_str(), reponse.size(), 0) == -1)
					return false;
				return true;
			}
			else
			{
				return (generate_html_page_error(client, "404"));
			}
		}
	}
	if (req.getPath() == "/login")
	{
		std::string response;
		response = handle_connexion(username, password, cook, req.getCookies(), client);
		if (send(client.getSocket(), response.c_str(), response.size(), 0) == -1)
			return false;
		return true;
	}
	else if (req.getPath() == "/Unlog")
	{
		std::string response2 = handle_deconnexion(cook, req.getCookies(), client);
		if (send(client.getSocket(), response2.c_str(), response2.size(), 0) == -1)
			return false;
		return true;
	}
	else
		return (generate_html_page_error(client, "404"));
	filename.clear();
	name.clear();
	email.clear();
	message.clear();
	username.clear();
	password.clear();
}

bool file_exists_parsebuffer(const char *path)
{
	struct stat st;
	
	if (stat(path, &st) != 0)
		return false;
	if (!S_ISREG(st.st_mode)) 
		return false;
	return true;
}

bool preparePostParse(Client& client, Cookies &cook, HttpRequest &req)
{
	const Server& 	server = client.getServer();

	if (client.getContentLength() == std::string::npos) 
		return (generate_html_page_error(client, "400"));
	if (server.getMaxBodySize() != -1 && client.getContentLength() > (size_t)server.getMaxBodySize())
		return (generate_html_page_error(client, "413"));
	if (client.getRequest().find("Content-Type: multipart/form-data") != std::string::npos)
		return (upload(client));
	else
		return (parse_buffer_post(client, cook, req));
}

bool prepareGetParse(Client& client, Cookies& cook, HttpRequest &req)
{
	if (client.getRequest().find("GET " + client.getServer().getRoot() + "base_donnees/") != std::string::npos)
	{
		if (download(client) == false)
			return false;
	}
	else
	{
		if (parse_buffer_get(client, cook, req) == false)
			return false;
	}
	return true;
}




