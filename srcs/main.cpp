/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ilbendib <ilbendib@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/06 13:08:48 by ilbendib          #+#    #+#             */
/*   Updated: 2024/11/06 16:57:21 by ilbendib         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/config.hpp"

int main(int argc, char const *argv[])
{
	(void)argc;
	(void)argv;
	
	Config conf;

	conf.parse_config_file();
    // Création du socket
    // int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    
    // Configuration de l'adresse du serveur
    sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(conf.get_ports()[0]);  // Exemple de port
    server_address.sin_addr.s_addr = INADDR_ANY;

    std::cout << "Server socket created" << std::endl;

    // Affichage du port, converti en format lisible
    std::cout << "Server port: " << ntohs(server_address.sin_port) << std::endl;

    return 0;
}