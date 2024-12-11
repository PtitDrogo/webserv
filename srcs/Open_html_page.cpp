#include "Webserv.hpp"
#include "config.hpp"
#include "server.hpp"
#include <fstream>
#include "sys/stat.h"

// std::string display_meme(std::string response)
// {
//     std::string html;
//     if(1)
//     {
//         if(1)
//         {
//             std::cout << "AAAAAAAAAAAAAAH" << std::endl;
//             html =
//                 "HELP ME HELP ME"
//                 "\t<div class='user-info'>\n"
//                 "\t\t<h2>User Connection Details</h2>\n"
//                 "\t\t<p><strong>Email:</strong> " "Wesh la team" " </p>\n"
//                 "\t\t<p><strong>Password:</strong> " "Wesh la team" " </p>\n"
//                 "\t</div>\n"
//                 "</body>\n</html>";

//                 response += html;
//         }
//     }
//     return (response);
// }


std::string readFile(std::string &path)
{
    std::cout << "DEBUG: path = " << path << std::endl;

    // if (!isRegularFile(path))
    // {
    //     std::cerr << "Error: " << path << " is not a regular file." << std::endl;
    //     return "";
    // }

    std::ifstream file(path.c_str(), std::ios::binary);
    if (!file.is_open())
    {
        std::cerr << "Error: could not open file" << std::endl;
        return "";
    }
    return std::string(
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>()
    );
}


std::string httpHeaderResponse(std::string code, std::string contentType, std::string content)
{
	return ("HTTP/1.1 " + code + "\r\n"
			"Content-Type: " + contentType + "\r\n"
			"Content-Length: " + to_string(content.size()) + "\r\n"
			"Connection: close\r\n"
			"\r\n" + content); //IT REALLY SHOULD BE CLOSE OR KEEP ALIVE DEPENDING ON CONTEXT;
}

void generate_default_error_page(std::string error_code, int client_socket)
{
    std::string path;
    if (error_code == "400")
        path = "./config/default_error_pages/400.html";
    else if (error_code == "403")
        path = "./config/default_error_pages/403.html";
    else if (error_code == "404")
        path = "./config/default_error_pages/404.html";
    else if (error_code == "413")
        path = "./config/default_error_pages/413.html";
    else
        path = "./config/default_error_pages/404.html";
    std::string file_content = readFile(path);
    std::string reponse = httpHeaderResponse(error_code, "text/html", file_content);
    send(client_socket, reponse.c_str(), reponse.size(), 0);
}

void generate_html_page_error(const Client& client, std::string error_code)
{
    std::cout << "DEBUG: JE SUIS DEDANS" << std::endl;
    const Server& server = client.getServer();

    // On récupère la map de l'erreur pour l'utiliser ensuite
    std::map<std::string, std::string> errorPageMap = server.getErrorPage();
    //conf.getServer()[server_index].getRoot()

    // Recherche de l'erreur dans la map
    std::map<std::string, std::string>::iterator it = errorPageMap.find(error_code);
    std::string path;
    if (it != errorPageMap.end())
    {
        path = "." + it->second;
    }
    else
    {
        std::cerr << "Error page for code " << error_code << " not found." << std::endl;
        generate_default_error_page(error_code, client.getSocket());
    }

    std::string file_content = readFile(path);

    std::string reponse = httpHeaderResponse(error_code, "text/html", file_content);

    // Envoi de la réponse
    //DEBUG POUR LE BRO BASTIEN
    // reponse = display_meme(reponse);
    // reponse += "AAAAAAH LET ME IN LET ME IN LET ME IN";
    // std::cout << RED << reponse << RESET << std::endl;
    std::cout << RED << reponse.c_str() << RESET << std::endl;
    send(client.getSocket(), reponse.c_str(), reponse.size(), 0); //TODO CHECK ALL SEND
}
