/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_command.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdhallen <jdhallen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/23 17:08:49 by jdhallen          #+#    #+#             */
/*   Updated: 2025/06/26 15:30:23 by jdhallen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Server.hpp"

void	Server::join(int fd, std::vector<std::string> arg){
	if (arg.size() < 2)
		sendRPL(fd, "You need a channel name to join", NULL);
	if (!check_channel_name(fd, arg[1])){
		if (DEBUG)
			std::cout << info(std::string("Client " + to_string(fd) + " tried to create a wrong Channel").c_str()) << std::endl;
	}
	bool	channel_exist = false;
	for (size_t i = 0; i < channels.size(); i++)
	{
		if (channels[i].getName() == arg[1]){
			channels[i].addClient(fd, clients[fd]);
			channel_exist = true;
			break ;
		}
	}
	if (!channel_exist){
		Channel new_channel(arg[1]);
		new_channel.addClient(fd, clients[fd]);
		channels.push_back(new_channel);
	}
	std::cout << info("Someone wants to join") << std::endl;
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