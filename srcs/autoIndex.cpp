#include "Webserv.hpp"
#include "include.hpp"
#include <dirent.h>
#include <sys/stat.h>


bool isDirectory(const std::string& path)
{
	struct stat statbuf;
	std::string full = "./www/" + path;
	if (stat(full.c_str(), &statbuf) != 0)
	{
		return false;
	}
	std::cout << "true " << path << " and full " << full << std::endl;
	return S_ISDIR(statbuf.st_mode);
}


std::vector<std::string> listDirectory(const std::string& directory)
{
	std::vector<std::string> files;
	DIR* dir = opendir(directory.c_str());
	if (dir != NULL)
	{
		struct dirent* entry;
		while ((entry = readdir(dir)) != NULL)
			files.push_back(entry->d_name);
		closedir(dir);
	}
	return files;
}

std::string create_page(std::string html, std::string directory)
{
	html = "<!DOCTYPE html>\n";
    html += "<html lang=\"fr\">\n";
    html += "<head>\n";
    html += "\t<meta charset=\"UTF-8\">\n";
    html += "\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
    html += "\t<title>Index of " + directory + "</title>\n";
    html += "\t<style>\n";
    html += "\t\tbody {\n";
    html += "\t\t\tbackground-color: rgb(54, 54, 54);\n";
    html += "\t\t\tcolor: white;\n";
    html += "\t\t\tfont-family: Arial, sans-serif;\n";
    html += "\t\t\tmargin: 0;\n";
    html += "\t\t\tpadding: 0;\n";
    html += "\t\t}\n";
    html += "\t\th1 {\n";
    html += "\t\t\ttext-align: center;\n";
    html += "\t\t\tcolor: rgb(243, 0, 0);\n";
    html += "\t\t\ttext-shadow: 2px 2px 5px rgba(0, 0, 0, 0.8);\n";
    html += "\t\t\tfont-size: 4vw;\n";
    html += "\t\t\tmargin-top: 20px;\n";
    html += "\t\t}\n";
    html += "\t\tul {\n";
    html += "\t\t\tlist-style: none;\n";
    html += "\t\t\tpadding: 0;\n";
    html += "\t\t\tmargin: 20px;\n";
    html += "\t\t}\n";
    html += "\t\tli {\n";
    html += "\t\t\tmargin-bottom: 10px;\n";
    html += "\t\t}\n";
    html += "\t\tbutton {\n";
    html += "\t\t\tfont-size: 1.2vw;\n";
    html += "\t\t\tpadding: 10px 20px;\n";
    html += "\t\t\tbackground-color: black;\n";
    html += "\t\t\tcolor: white;\n";
    html += "\t\t\tborder: none;\n";
    html += "\t\t\tborder-radius: 5px;\n";
    html += "\t\t\tcursor: pointer;\n";
    html += "\t\t\ttransition: background-color 0.3s;\n";
    html += "\t\t\ttext-align: left;\n";
    html += "\t\t\twidth: 200px;\n";
    html += "\t\t}\n";
    html += "\t\tbutton:hover {\n";
    html += "\t\t\tbackground-color: #333;\n";
    html += "\t\t}\n";
    html += "\t</style>\n";
    html += "</head>\n";
    html += "<body>\n";
    // html += "\t<h1>Index of " + directory + "</h1>\n";
    html += "\t<h1>AutoIndex_Page</h1>\n";

    html += "\t<ul>\n";
	return html;
}


std::string generateAutoIndexPage(Config &conf, const std::string& directory, const std::vector<std::string>& files, bool islocation)
{
	std::cout << "directory-------------------------------------- = " << directory << std::endl;
	std::string html;
	html = create_page(html, directory);
	for (std::vector<std::string>::const_iterator it = files.begin(); it != files.end(); ++it)
	{
		if (*it == "." || *it == "..")
			continue;
		std::string relativePath = directory;


        (void) islocation;
        if (conf.getIsLocation() == true)
        {
            std::string rootLoc = conf.getServer()[0].getLocation()[0].getRoot();
            std::cout << "relativePath: " << relativePath << std::endl;
            std::cout << "rootLoc: " << rootLoc << std::endl;

            std::string rootLocRelative = rootLoc.substr(1);  // Supprime le premier '/' pour rendre `rootLoc` relatif
            std::cout << "rootLocRelative: " << rootLocRelative << std::endl;

            // Supprime './' du début de relativePath si présent
            if (relativePath.find("./") == 0) {
                relativePath = relativePath.substr(2);  // Retire './' du début
            }

            std::cout << "relativePath après nettoyage: " << relativePath << std::endl;

            if (relativePath.find(rootLocRelative) == 0) {
                relativePath = relativePath.substr(rootLocRelative.size());
                std::cout << "relativePath modifié: " << relativePath << std::endl;
            }
        }

        else {
            std::string root = conf.getServer()[0].getRoot();
            std::cout << "relativePath: " << relativePath << std::endl;
            std::cout << "root: " << root << std::endl;

            std::string rootRelative = root.substr(1);  // Supprime le premier '/' pour rendre `root` relatif
            std::cout << "rootRelative: " << rootRelative << std::endl;

            // Supprime './' du début de relativePath si présent
            if (relativePath.find("./") == 0) {
                relativePath = relativePath.substr(2);  // Retire './' du début
            }

            std::cout << "relativePath après nettoyage: " << relativePath << std::endl;

            if (relativePath.find(rootRelative) == 0) {
                relativePath = relativePath.substr(rootRelative.size());
                std::cout << "relativePath modifié: " << relativePath << std::endl;
            }
        }






		if (!relativePath.empty() && relativePath[relativePath.size() - 1] != '/')
			relativePath += "/";
		relativePath += *it;
		size_t pos;
		while ((pos = relativePath.find("//")) != std::string::npos)
		{
			relativePath.replace(pos, 2, "/");
		}
		std::cout << "relativePath----------------------------------- = " << relativePath << std::endl;
		std::string fullPath = relativePath;
		bool isDir = isDirectory(fullPath);

		html += "\t\t<li><a href=\"" + relativePath + "\"><button>" + *it + (isDir ? "/" : "") + "</button></a></li>\n";
	}
	html += "\t</ul>\n";
	html += "</body>\n";
	html += "</html>";
	return html;
}
