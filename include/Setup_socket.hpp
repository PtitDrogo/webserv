/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Setup_socket.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ilbendib <ilbendib@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/11 18:09:51 by ilbendib          #+#    #+#             */
/*   Updated: 2024/11/11 18:11:44 by ilbendib         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SETUP_SOCKET_HPP
# define SETUP_SOCKET_HPP

#include "server.hpp"

int SetupSocket(Server serv);
int SetupClientAddress(int server_socket);

#endif