#include "Webserv.hpp"

bool parse_location(std::string line, Server &serv, std::ifstream &file)
{
	size_t pos5 = line.find_first_not_of(" \t");
	if (pos5 != std::string::npos && line.find("location", pos5) == pos5)
	{
		location loc;
		size_t start = pos5 + 8;
		while (start < line.size() && std::isspace(line[start])) {
			start++;
		}
		size_t end = line.find_first_of(" {", start);
		if (end == std::string::npos) {
			std::cerr << "Error: location block not opened" << std::endl;
			return false;
		}
		std::string path = line.substr(start, end - start) + ' ';
		std::cout << "path = |" << path << "|" << std::endl;
		if (path == " " || path == "/ " || path.empty())
		{
			std::cerr << "Error: location has no path" << std::endl;
			return false;
		}
		loc.setPath(path);

		size_t braceOpenPos = line.find('{', end);
		if (braceOpenPos == std::string::npos) {
			std::cerr << "Error: location block not opened" << std::endl;
			return false;
		}
		std::string sectionContent;

		if (braceOpenPos != std::string::npos) {
			sectionContent += line.substr(braceOpenPos + 1) + "\n";
		}
		std::istream& input = file;
		std::string nextLine;
		while (std::getline(input, nextLine)) {
			size_t braceClosePos = nextLine.find('}');
			if (braceClosePos != std::string::npos) {
				sectionContent += nextLine.substr(0, braceClosePos);
				break;
			} else {
				sectionContent += nextLine + "\n";
			}
		}

		std::istringstream sectionStream(sectionContent);
		std::string subLine;
		while (std::getline(sectionStream, subLine)) {
			size_t indexPos = subLine.find("index");
			if (indexPos != std::string::npos) {
				size_t startIndex = subLine.find_first_not_of(" \t", indexPos + 5);
				if (startIndex == std::string::npos)
					return true;
				size_t endIndex = subLine.find_first_of(" \t;", startIndex);
				std::string index = subLine.substr(startIndex, endIndex - startIndex);
				loc.setIndex(index);
			}
			size_t rootPos = subLine.find("root");
			if (rootPos != std::string::npos) {
				size_t startRoot = subLine.find_first_not_of(" \t", rootPos + 4);
				if (startRoot == std::string::npos)
					return true;
				size_t endRoot = subLine.find_first_of(" \t;", startRoot);
				std::string root = subLine.substr(startRoot, endRoot - startRoot);
				loc.setRoot(root);
			}
			size_t autoIndexPos = subLine.find("auto");
			if (autoIndexPos != std::string::npos) {
				size_t startAutoIndex = subLine.find_first_not_of(" \t", autoIndexPos + 5);
				if (startAutoIndex == std::string::npos)
					return true;
				size_t endAutoIndex = subLine.find_first_of(" \t;", startAutoIndex);
				std::string autoIndex = subLine.substr(startAutoIndex, endAutoIndex - startAutoIndex);
				if (autoIndex != "on" && autoIndex != "off") {
					std::cerr << "Error: invalid value for auto_index" << std::endl;
					return false;
				}
				loc.setAutoIndex(autoIndex);
			}
			size_t allowMethodPos = subLine.find("allow_method");
			if (allowMethodPos != std::string::npos) {
				size_t startAllowMethod = subLine.find_first_not_of(" \t", allowMethodPos + 13); // 12 = length of "allow_method"
				if (startAllowMethod == std::string::npos) {
					return true;
				}
				size_t endAllowMethod = subLine.find_first_of("\n", startAllowMethod); 
				std::string allowMethod = subLine.substr(startAllowMethod, endAllowMethod - startAllowMethod);
				loc.setAllowMethod(allowMethod);
			}
			size_t cgiPathPos = subLine.find("cgi_path");
			if (cgiPathPos != std::string::npos) {
				size_t startCgiPath = subLine.find_first_not_of(" \t", cgiPathPos + 8);
				if (startCgiPath == std::string::npos)
					return true;
				size_t endCgiPath = subLine.find_first_of(";", startCgiPath);
				std::string cgiPath = subLine.substr(startCgiPath, endCgiPath - startCgiPath);
				loc.setCgiPath(cgiPath);
			}
			size_t redirPos = subLine.find("return");
			if (redirPos != std::string::npos)
			{
				size_t startRedir = subLine.find_first_not_of(" \t", redirPos + 6);
				if (startRedir == std::string::npos)
					return true;
				size_t endRedir = subLine.find_first_of(" \t;", startRedir);
				std::string error_code = subLine.substr(startRedir, endRedir - startRedir);
				startRedir = endRedir;
				while (startRedir < subLine.size() && std::isspace(subLine[startRedir]))
					startRedir++;
				endRedir = subLine.find_first_of(" \t;", startRedir);
				std::string path = subLine.substr(startRedir, endRedir - startRedir);
				loc.setRedir(error_code, path);
			}
		}
		serv.setLocation(loc);
	}
	return true;
}