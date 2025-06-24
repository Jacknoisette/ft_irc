/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_command.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdhallen <jdhallen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/23 17:08:49 by jdhallen          #+#    #+#             */
/*   Updated: 2025/06/23 17:36:04 by jdhallen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Server.hpp"

void	Server::join(int fd, std::vector<std::string> arg){
	(void)fd;
	(void)arg;
	std::cout << info("Someone wants to join") << std::endl;
}

void	Server::quit(int fd, std::vector<std::string> arg){
	(void)arg;
	(void)fd;
	// removeClient(fd);
	// close(fd);
	// it = pollfds.erase(it);
	std::cout << info("Someone wants to quit") << std::endl;
}

void	Server::ping(int fd, std::vector<std::string> arg){
	std::string token;
    if (arg.size() > 1)
        token = arg[1];
    else
        token = "";
	sendRPL(fd, "PONG", " :", token.c_str(), NULL);
}