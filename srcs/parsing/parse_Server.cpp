#include "Webserv.hpp"


static bool check_format_port(std::string port)
{
	for (size_t i = 0; i < port.size(); i++)
	{
		if (!std::isdigit(port[i]))
		{
			std::cerr << "Error: Invalid port" << std::endl;
			return false;
		}
	}
	return true;
}

static bool check_format_host(std::string host)
{
	for (size_t i = 0; i < host.size(); i++)
	{
		if (!std::isdigit(host[i]) && host[i] != '.' && host[i] != ':')
		{
			std::cerr << "Error: Invalid host" << std::endl;
			return false;
		}
	}
	return true;
}

bool parse_listen(std::string line, Server &serv)
{
	size_t pos = line.find_first_not_of(" \t");
	if (pos != std::string::npos && line.find("listen", pos) == pos)
	{
		size_t start = pos + 6;
		while (start < line.size() && std::isspace(line[start]))
			start++;
		size_t end = line.find_first_of("\t;", start);
		std::string port = line.substr(start, end - start);
		if (port.size() > 4 && port.size() == 14)
		{
			if (check_format_host(port) == false)
				return false;
			serv.setHost(port);
			port = port.substr(port.find(':') + 1);
			serv.setPort(port);
			return true;
		}
		else if (port.size() == 4)
		{
			if (check_format_port(port) == false)
				return false;
			serv.setPort(port);
			return true;
		}
		else 
		{
			std::cerr << "Error: Invalid port" << std::endl;
			return false;
		}
	}
	return true;
}

bool parse_server_name(std::string line, Server &serv)
{
	size_t pos = line.find_first_not_of(" \t");
	if (pos != std::string::npos && line.find("server_name", pos) == pos)
	{
		size_t start = pos + 11;
		while (start < line.size() && std::isspace(line[start]))
			start++;
		size_t end = line.find_first_of(" \t;", start);
		std::string server_name = line.substr(start, end - start);
		if (check_format_host(server_name) == false)
		{
			return false;
		}
		serv.setServerName(server_name);
	}
	return true;
}

void parse_index(std::string line, Server &serv)
{
	size_t pos = line.find_first_not_of(" \t");
	if (pos != std::string::npos && line.find("index", pos) == pos)
	{
		size_t start = pos + 5;
		while (start < line.size() && std::isspace(line[start]))
			start++;
		size_t end = line.find_first_of(" \t;", start);
		std::string index = line.substr(start, end - start);
		serv.setIndex(index);
	}
}

void parse_root(std::string line, Server &serv)
{
	size_t pos = line.find_first_not_of(" \t;");
	if (pos != std::string::npos && line.find("root", pos) == pos)
	{
		size_t start = pos + 4;
		while (start < line.size() && std::isspace(line[start]))
			start++;
		size_t end = line.find_first_of(" \t;", start);
		std::string root = line.substr(start, end - start);
		serv.setRoot(root);
	}
}

void parse_error_page(std::string line, Server &serv)
{
	size_t pos = line.find_first_not_of(" \t;");
	if (pos != std::string::npos && line.find("error_page", pos) == pos)
	{
		std::istringstream iss(line);
		std::string word;
		iss >> word;
		std::string error_code;
		iss >> error_code;
		std::string error_file;
		iss >> error_file;
		serv.setErrorPage(error_code, error_file);
	}
}

bool isdigit(std::string str)
{
	for (size_t i = 0; i < str.size(); i++)
	{
		if (!std::isdigit(str[i]))
			return false;
	}
	return true;
}
static int convertToBytes(const std::string &size_str)
{
	std::string unit;
	int size = 0;

	size_t i = 0;
	while (i < size_str.size() && std::isdigit(size_str[i]))
	{
		size = size * 10 + (size_str[i] - '0');
		i++;
	}
	if (i < size_str.size())
		unit = size_str.substr(i);
	if (unit == "KB" || unit == "kb")
		return size * 1024;
	else if (unit == "MB" || unit == "mb")
		return size * 1024 * 1024;
	else if (unit == "GB" || unit == "gb")
		return size * 1024 * 1024 * 1024;
	return size;
}

void parse_max_body_size(const std::string &line, Server &serv)
{
	size_t pos = line.find_first_not_of(" \t");
	
	if (pos != std::string::npos && line.find("max_body_size", pos) == pos)
	{
		size_t start = pos + 13;
		while (start < line.size() && std::isspace(line[start]))
			start++;
		size_t end = line.find_first_of(" \t", start);
		std::string max_body_size_str = line.substr(start, end - start);
		int max_body_size = convertToBytes(max_body_size_str);
		serv.setMaxBodySize(max_body_size);
	}
}

void parse_auto_index(std::string line, Server &serv)
{
	size_t pos = line.find_first_not_of(" \t");
	if (pos != std::string::npos && line.find("auto_index", pos) == pos)
	{
		size_t start = pos + 10;
		while (start < line.size() && std::isspace(line[start]))
			start++;
		size_t end = line.find_first_of(" \t", start);
		std::string auto_index = line.substr(start, end - start);
		if (auto_index != "on" && auto_index != "off")
			return;
		serv.setAutoIndex(auto_index);
	}
}

void parse_cgi_path(std::string& line, Server &server)
{
	size_t pos = 0;

	while ((pos = line.find(".", pos)) != std::string::npos)
	{
		size_t extention = line.find(":", pos);
		if (extention == std::string::npos)
			break;

		std::string cgi_extension = line.substr(pos, extention - pos);

		size_t start = extention + 1;
		size_t end = line.find_first_of(" \t;", start);
		std::string extracted_path;
		if (end != std::string::npos)
			extracted_path = line.substr(start, end - start);
		else
			extracted_path = line.substr(start);
		server.addCgis(cgi_extension, extracted_path);
		if (end != std::string::npos)
			pos = end;
		else
			pos = std::string::npos;
	}
}