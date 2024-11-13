/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Open_html_page.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ilbendib <ilbendib@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/11 18:11:20 by ilbendib          #+#    #+#             */
/*   Updated: 2024/11/12 18:35:23 by ilbendib         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef OPEN_HTML_PAGE_HPP
# define OPEN_HTML_PAGE_HPP

#include "server.hpp"

std::string readFile(std::string &path);
std::string httpHeaderResponse(std::string code, std::string contentType, std::string content);
void generate_html_page_error(Server &serv, int client_socket, std::string error_code);



#endif