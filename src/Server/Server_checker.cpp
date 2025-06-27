/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_checker.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdhallen <jdhallen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/19 14:22:36 by jdhallen          #+#    #+#             */
/*   Updated: 2025/06/26 15:14:57 by jdhallen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void Server::client_command(int client_fd, const std::vector<std::vector<std::string> > &cmd_group){
	check_Auth(client_fd, cmd_group);
	if (!clients[client_fd].getAuthenticated())
		return ;
	check_BaseCmd(client_fd, cmd_group);
}

void Server::check_BaseCmd(int fd, const std::vector<std::vector<std::string> > &cmd_group){
	for (std::vector<std::vector<std::string> >::const_iterator cmd = cmd_group.begin(); cmd != cmd_group.end(); cmd++){
		std::map<std::string, void (Server::*)(int, std::vector<std::string>)>::iterator it = cmd_func_list.find((*cmd)[0]);
        if (it != cmd_func_list.end()) {
            (this->*(it->second))(fd, *cmd);
        }
	}
}

void Server::check_Auth(int fd, const std::vector<std::vector<std::string> > &cmd_group){
	if (!clients[fd].getAuthenticated()){
		for (std::vector<std::vector<std::string> >::const_iterator cmd = cmd_group.begin(); cmd != cmd_group.end(); cmd++){
			if ((*cmd)[0] == "NICK" && (*cmd).size() >= 2){
				bool nickUsed = false;
				for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it){
					if (it->first != fd && it->second.getNickname() == (*cmd)[1]) {
						nickUsed = true;
						break;
					}
				}
				if (!nickUsed)
					clients[fd].setNickname((*cmd)[1]);
				else
					sendRPL(fd, "Nickame already used", " : ",(*cmd)[1].c_str(), NULL);
			}
			if ((*cmd)[0] == "USER" && (*cmd).size() >= 5){
				clients[fd].setUsername((*cmd)[1]);
				clients[fd].setHostname((*cmd)[2]);
			}
			if ((*cmd)[0] == "QUIT" && (*cmd).size() >= 1)
				quit(fd, (*cmd));
		}
		
		if (clients[fd].getNickname() != ""
			&& clients[fd].getUsername() != ""
			&& clients[fd].getHostname() != ""){
			if (DEBUG)
				std::cout << "Sending welcome messages to client " << fd << std::endl;
			sendRPL(fd, "irc.local", "001", clients[fd].getNickname().c_str(), std::string("Welcome to IRC " + clients[fd].getNickname()).c_str(), NULL);
			sendRPL(fd, "irc.local", "002", clients[fd].getNickname().c_str(), "Your host is irc.local version 1.0", NULL);
			sendRPL(fd, "irc.local", "003", clients[fd].getNickname().c_str(), "Server creation in June 2025", NULL);
			sendRPL(fd, "irc.local", "004", clients[fd].getNickname().c_str(), "irc.local", "1.0", "operator autorised", "Channel may contain invite mod or password", NULL);
					
			clients[fd].setAuthenticated(true);
			if (DEBUG){
            	std::cout << "nickname : " << clients[fd].getNickname() << std::endl;
				std::cout << "username : " << clients[fd].getUsername() << std::endl;
				std::cout << "hostname : " << clients[fd].getHostname() << std::endl;  
    		}
		}		
	}
}
