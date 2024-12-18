#include "Webserv.hpp"
#include "include.hpp"
#include <dirent.h>
#include <sys/stat.h>

bool isDirectory(const std::string &path)
{
    struct stat statbuf;
    std::string full = "./www/" + path;
    if (stat(full.c_str(), &statbuf) != 0)
        return false;
    return S_ISDIR(statbuf.st_mode);
}

std::vector<std::string> listDirectory(const std::string &directory)
{
    std::vector<std::string> files;
    DIR *dir = opendir(directory.c_str());
    if (dir != NULL)
    {
        struct dirent *entry;
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
    html += "\t<h1>AutoIndex_Page</h1>\n";

    html += "\t<ul>\n";
    return html;
}

std::string generateAutoIndexPage(const std::string &directory, const std::vector<std::string> &files, Client &client)
{
    std::string html;
    html = create_page(html, directory);
    for (std::vector<std::string>::const_iterator it = files.begin(); it != files.end(); ++it)
    {
        if (*it == "." || *it == "..")
            continue;
        std::string relativePath = directory;

        if (client.getLocation() != NULL)
        {
            std::string rootLoc = client.getLocation()->getRoot();

            std::string rootLocRelative = rootLoc.substr(1);
            if (relativePath.find("./") == 0)
                relativePath = relativePath.substr(2);
            if (relativePath.find(rootLocRelative) == 0)
                relativePath = relativePath.substr(rootLocRelative.size());
        }

        else
        {
            std::string root = client.getServer().getRoot();

            std::string rootRelative = root.substr(1);
            if (relativePath.find("./") == 0)
                relativePath = relativePath.substr(2);
            if (relativePath.find(rootRelative) == 0)
                relativePath = relativePath.substr(rootRelative.size());
        }
        if (!relativePath.empty() && relativePath[relativePath.size() - 1] != '/')
            relativePath += "/";
        relativePath += *it;
        size_t pos;
        while ((pos = relativePath.find("//")) != std::string::npos)
            relativePath.replace(pos, 2, "/");
        std::string fullPath = relativePath;
        bool isDir = isDirectory(fullPath);

        // std::cout << client.getLocation()->getPath() + "/" << std::endl;
        std::string locationpath = client.getLocation()->getPath();
        std::string pitier = locationpath.substr(0, locationpath.size() - 1);
        std::cout << pitier << std::endl;
        html += "\t\t<li><a href=\"" + pitier + "/" + relativePath + "\"><button>" + *it + (isDir ? "/" : "") + "</button></a></li>\n";
    }
    html += "\t</ul>\n";
    html += "</body>\n";
    html += "</html>";
    return html;
}

bool autoIndex(std::string path, Client &client)
{
    std::string finalPath;
    std::string reponse;
    std::string file_content;
    Server &server = client.getServer();

    if (client.getLocation() != NULL)
        finalPath = path;
    else
        finalPath = "." + server.getRoot() + path;
    std::vector<std::string> files = listDirectory(finalPath);
    file_content = generateAutoIndexPage(finalPath, files, client);
    reponse = httpHeaderResponse("200 Ok", "text/html", file_content);
    if (send(client.getSocket(), reponse.c_str(), reponse.size(), 0) == -1)
        return false;
    return true;
}