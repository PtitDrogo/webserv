/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Open_html_page.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tfreydie <tfreydie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/11 18:11:20 by ilbendib          #+#    #+#             */
/*   Updated: 2024/11/11 20:02:21 by tfreydie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef OPEN_HTML_PAGE_HPP
# define OPEN_HTML_PAGE_HPP

#include "server.hpp"

std::string readFile(std::string &path);
std::string httpHeaderResponse(std::string code, std::string contentType, std::string content);
void generate_html_page404(Server &serv, int client_socket);
void generate_html_page_without_location(Server &serv, int client_socket);
void generate_html_page_with_location(Server &serv, location &loc, int client_socket);
void handleRecvValue(int valread, size_t &i, std::vector<struct pollfd> &fds);
void addPollFD(int client_socket, std::vector<struct pollfd> &fds);
void disconnectClient(std::vector<struct pollfd> &fds, size_t &i);

#endif