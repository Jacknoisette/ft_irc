/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_command.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdhallen <jdhallen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/23 17:08:49 by jdhallen          #+#    #+#             */
/*   Updated: 2025/06/27 13:57:42 by jdhallen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void	Server::join(int fd, std::vector<std::string> arg){
	if (arg.size() < 2){
		sendRPL(fd, "You need a channel name to join", NULL);
		return ;
	}
	std::vector<std::string> channel_list;
	std::vector<std::string> key_list;
	try{
		channel_list = check_channel_name(fd, arg[1]);
		if (arg.size() >= 3)
			key_list = check_key_string(fd, arg[2]);
	}
	catch (std::runtime_error & e){
		if (DEBUG)
			std::cout << info(std::string("Client " + to_string(fd) + " tried to create a wrong Channel").c_str()) << std::endl;
		return ;
	}
	(void)key_list;
	for (std::vector<std::string>::iterator it = channel_list.begin(); it != channel_list.end(); it++)
	{
		std::map<std::string, Channel>::iterator channels_pos = channels.find(*it);
		if (channels_pos != channels.end()){
			channels_pos->second.addClient(fd, clients[fd]);
			clients[fd].addChannel(channels[*it]);
		}
		else{
			Channel new_channel(*it);
			new_channel.addClient(fd, clients[fd]);
			channels[*it] = new_channel;
			clients[fd].addChannel(channels[*it]);
			std::cout << info(std::string("New channel " + *it + " created").c_str()) << std::endl;
		}
	}
	std::cout << info("Someone wants to join") << std::endl;
}

void	Server::leave(int fd, std::vector<std::string> arg){
	(void)fd;
	(void)arg;
	std::cout << info("Someone wants to leave") << std::endl;
}

void	Server::quit(int fd, std::vector<std::string> arg){
	(void)arg;
	garbage.push_back(fd);
	std::cout << info("Someone wants to quit") << std::endl;
}

void	Server::ping(int fd, std::vector<std::string> arg){
	std::string token;
    if (arg.size() > 1)
        token = arg[1];
    else {
        token = " ";
	}
	sendRPL(fd, "PONG", " :", token.c_str());
}
