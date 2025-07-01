/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_command.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdhallen <jdhallen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/23 17:08:49 by jdhallen          #+#    #+#             */
/*   Updated: 2025/07/01 14:21:57 by jdhallen         ###   ########.fr       */
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
		std::cout << e.what() << std::endl;
		if (DEBUG)
			std::cout << info(std::string("Client " + to_string(fd) + " tried to create a wrong Channel").c_str()) << std::endl;
		return ;
	}
	(void)key_list;
	std::vector<std::pair<std::string, std::string> > channel_list_lower = toLowerVector(channel_list);
	for (std::vector<std::pair<std::string, std::string> >::iterator it = channel_list_lower.begin(); it != channel_list_lower.end(); it++)
	{
		std::string *lower = &it->first;
		std::string *original = &it->second;
		std::map<std::string, Channel>::iterator channels_pos = channels.find(*lower);
		if (channels_pos != channels.end()){
			if (clients[fd].getChannels().find(*lower) == clients[fd].getChannels().end()){
				channels_pos->second.addClient(fd, clients[fd], false);
				clients[fd].addChannel(channels[*lower]);
				clients[fd].setCurrentChannel(*lower);
				channels_pos->second.sendRPL_Channel(true, fd, std::string("Client " + clients[fd].getNickname() +
					" joined channel " + *original + " \n").c_str());
			} else {
				clients[fd].setCurrentChannel(*lower);
				if (DEBUG)
					std::cout << info(std::string("Client " + clients[fd].getNickname() + " switched to channel " + *original + " \n").c_str()) << std::endl;
			}
		}
		else{
			Channel new_channel(*original);
			new_channel.addClient(fd, clients[fd], true);
			channels[*lower] = new_channel;
			clients[fd].addChannel(channels[*lower]);
			clients[fd].setCurrentChannel(*lower);
			std::cout << info(std::string("New channel " + *original + " created").c_str()) << std::endl;
			channels[*lower].sendRPL_Channel(true, fd, std::string("Client " + clients[fd].getNickname() + " joined channel " + *original + " \n").c_str());
		}
	}
	if (DEBUG)
		std::cout << info("Someone wants to join") << std::endl;
}

void	Server::part(int fd, std::vector<std::string> arg){
	if (arg.size() < 1){
		sendRPL(fd, "Error in leave command", (const char)NULL);
		return ;
	}
	std::vector<std::string> channel_list;
	std::string msg_on_leave;
	try{
		if (arg.size() >= 2)
			channel_list = check_channel_name(fd, arg[1]);
		if (arg.size() >= 3){
			check_msg_string(fd, arg[2]);
			msg_on_leave = arg[2];
		}
	}
	catch (std::runtime_error & e){
		std::cout << e.what() << std::endl;
		if (DEBUG)
			std::cout << info(std::string("Client " + to_string(fd) + " tried to leave a wrong Channel").c_str()) << std::endl;
		return ;
	}
	if (clients[fd].getChannels().empty())
		return ;
	if (channel_list.size() == 0){
		Channel *actual_channel = &channels[clients[fd].getCurrentChannel()];
		actual_channel->removeClient(fd);
		actual_channel->sendRPL_Channel(true, fd, std::string("Client " + clients[fd].getNickname() + " leaved channel " +
			clients[fd].getCurrentChannel() + " for reason : " + msg_on_leave + "\n").c_str());
		clients[fd].removeChannel(clients[fd].getCurrentChannel());
		if (actual_channel->getClients().size() == 0)
			removeChannel(clients[fd].getCurrentChannel());
		clients[fd].updateCurrentChannel();
	}
	else {
		std::vector<std::pair<std::string, std::string> > channel_list_lower = toLowerVector(channel_list);
		for (std::vector<std::pair<std::string, std::string> >::iterator it = channel_list_lower.begin(); it != channel_list_lower.end(); it++)
		{
			std::string *lower = &it->first;
			std::string *original = &it->second;
			std::map<std::string, Channel>::iterator channels_pos = channels.find(*lower);
			if (channels_pos != channels.end() &&
					clients[fd].getChannels().find(*lower) != clients[fd].getChannels().end()){
				std::string client_nickname = clients[fd].getNickname();
				channels_pos->second.removeClient(fd);
				channels_pos->second.sendRPL_Channel(true, fd, std::string("Client " +
					client_nickname + " leaved channel " + *original + " for reason : " + msg_on_leave + "\n").c_str());
				clients[fd].removeChannel(*lower);
				if (channels[*lower].getClients().size() == 0)
					removeChannel(*lower);
				clients[fd].updateCurrentChannel();
			}
		}
	}
	if (DEBUG)
		std::cout << info("Someone wants to leave") << std::endl;
}

void	Server::quit(int fd, std::vector<std::string> arg){
	(void)arg;
	garbage.push_back(fd);
	if (DEBUG)
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

void	Server::privmsg(int fd, std::vector<std::string> arg){
	if (arg.size() < 3){
		sendRPL(fd, "Error in privmsg command", (const char)NULL);
		return ;
	}
	std::vector<std::string> channel_list;
	std::string msg;
	try{
		channel_list = check_channel_name(fd, arg[1]);
		check_msg_string(fd, arg[2]);
		msg = arg[2];
	}
	catch (std::runtime_error & e){
		std::cout << e.what() << std::endl;
		if (DEBUG)
			std::cout << info(std::string("Client " + to_string(fd) + " tried to send a Wrong msg or in a wrong Channel").c_str()) << std::endl;
		return ;
	}
	if (clients[fd].getChannels().empty())
		return ;
	std::vector<std::pair<std::string, std::string> > channel_list_lower = toLowerVector(channel_list);
	for (std::vector<std::pair<std::string, std::string> >::iterator it = channel_list_lower.begin(); it != channel_list_lower.end(); it++){
		std::string *lower = &it->first;
		std::map<std::string, Channel>::iterator channels_pos = channels.find(*lower);
		if (channels_pos != channels.end() &&
				clients[fd].getChannels().find(*lower) != clients[fd].getChannels().end()){
			std::string rpl = std::string(":" + clients[fd].getNickname() + "!" +
				clients[fd].getUsername() + "@" + clients[fd].getHostname() + " PRIVMSG " +
				channels_pos->second.getOGName() + " :" + msg + "\n");
			channels_pos->second.sendRPL_Channel(true, fd, rpl);
		}
	}
}