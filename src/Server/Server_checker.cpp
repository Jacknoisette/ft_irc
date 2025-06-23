/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_checker.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdhallen <jdhallen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/19 14:22:36 by jdhallen          #+#    #+#             */
/*   Updated: 2025/06/23 16:38:46 by jdhallen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Server.hpp"

void Server::client_command(int client_fd, const std::vector<std::vector<std::string> > &cmd_group){
	check_Auth(client_fd, cmd_group);
	if (!clients[client_fd].getAuthenticated())
		return ;
	// if (!clients[client_fd].getIsInChannel())
}


void Server::check_Auth(int fd, const std::vector<std::vector<std::string> > &cmd_group){
	if (!clients[fd].getAuthenticated()){
		for (std::vector<std::vector<std::string> >::const_iterator cmd = cmd_group.begin(); cmd != cmd_group.end(); cmd++){
			if ((*cmd)[0] == "NICK" && (*cmd).size() >= 2)
				clients[fd].setNickname((*cmd)[1]);
			if ((*cmd)[0] == "USER" && (*cmd).size() >= 5){
				clients[fd].setUsername((*cmd)[1]);
				clients[fd].setHostname((*cmd)[2]);
			}
		}
		
		if (DEBUG){
            std::cout << "nickname" << clients[fd].getNickname() << std::endl;
			std::cout << "username" << clients[fd].getUsername() << std::endl;
			std::cout << "hostname" << clients[fd].getHostname() << std::endl;  
    	}
		if (clients[fd].getNickname() != ""
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
}