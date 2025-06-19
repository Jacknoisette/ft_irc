/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_checker.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdhallen <jdhallen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/19 14:22:36 by jdhallen          #+#    #+#             */
/*   Updated: 2025/06/19 15:49:16 by jdhallen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Server.hpp"

void Server::check_Auth(int fd) {
	if (!clients[fd].getAuthenticated()
		&& clients[fd].getNickname() != ""
		&& clients[fd].getUsername() != ""
		&& clients[fd].getHostname() != ""){
		if (DEBUG)
			std::cout << "Sending welcome messages to client " << fd << std::endl;
		sendRPL(fd, "irc.local", "001", clients[fd].getNickname(), "Welcome to IRC " + clients[fd].getNickname());
		sendRPL(fd, "irc.local", "002", clients[fd].getNickname(), "Your host is irc.local version 1.0");
		sendRPL(fd, "irc.local", "003", clients[fd].getNickname(), "Server creation in June 2025");
		sendRPL(fd, "irc.local", "004", clients[fd].getNickname(), "irc.local", "1.0", "o", "itkol");
		clients[fd].setAuthenticated(true);
	}
}