/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_command.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdhallen <jdhallen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/23 17:08:49 by jdhallen          #+#    #+#             */
/*   Updated: 2025/07/03 13:59:14 by jdhallen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Global.hpp"
#include "Server.hpp"

void	Server::join(int fd, std::vector<std::string> arg){
	if (arg.size() < 2){
		sendRPL(fd, "irc.local", "461", clients[fd].getNickname().c_str(),
				"JOIN", ":Not enough parameters", NULL);
		return ;
	}

	// TODO: Si channel plein --> 471 ERR_CHANNELISFULL <channel> :Cannot join channel (+l)
	// TODO: Si channel +i --> 473 ERR_INVITEONLYCHAN <channel> :Cannot join channel (+i)
	// TODO: Si banni --> 474 ERR_BANNEDFROMCHAN <channel> :Cannot join channel (+b)
	// TODO: Si mauvais key --> 475 ERR_BADCHANNELKEY <channel> :Cannot join channel (+k)
	
	std::vector<std::string> channel_list;
	std::vector<std::string> key_list;
	try{
		channel_list = check_channel_name(fd, arg[1], "JOIN");
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
				if (clients[fd].getChannels().size() >= 20){
					sendRPL(fd, "irc.local", "405", clients[fd].getNickname().c_str(),
						original->c_str(), ":You have joined too many channels", NULL);
					continue ;
				}
				channels_pos->second.addClient(fd, clients[fd], false);
				clients[fd].addChannel(channels[*lower]);
				clients[fd].setCurrentChannel(*lower);
				sendWelcomeChannelMsg(clients[fd], *lower);
			} else {
				clients[fd].setCurrentChannel(*lower);
				if (DEBUG)
					std::cout << info(std::string("Client " + clients[fd].getNickname() + " switched to channel " + *original + " \n").c_str()) << std::endl;
			}
		}
		else{
			if (clients[fd].getChannels().size() >= 20){
				sendRPL(fd, "irc.local", "405", clients[fd].getNickname().c_str(),
					original->c_str(), ":You have joined too many channels", NULL);
				continue ;
			}
			Channel new_channel(*original);
			new_channel.addClient(fd, clients[fd], true);
			channels[*lower] = new_channel;
			clients[fd].addChannel(channels[*lower]);
			clients[fd].setCurrentChannel(*lower);
			std::cout << info(std::string("New channel " + *original + " created").c_str()) << std::endl;
			sendWelcomeChannelMsg(clients[fd], *lower);
		}
	}
	if (DEBUG)
		std::cout << info("Someone wants to join") << std::endl;
}

void	Server::part(int fd, std::vector<std::string> arg){
	if (arg.size() < 1){
		sendRPL(fd, "irc.local", "461", clients[fd].getNickname().c_str(),
				"PART", ":Not enough parameters", NULL);
		return ;
	}
	std::vector<std::string> channel_list;
	std::string msg_on_leave;
	try{
		if (arg.size() >= 2)
			channel_list = check_channel_name(fd, arg[1], "PART");
		if (arg.size() >= 3){
			ValidateMsgContent(fd, arg[2]);
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
		std::string rpl = std::string(":" + clients[fd].getNickname() + "!" +
			clients[fd].getUsername() + "@" + clients[fd].getHostname() + " PART " +
			actual_channel->getOGName() + " " + msg_on_leave + "\n");
		sendRPL_Channel(true, fd, actual_channel->getName(), rpl);
		actual_channel->removeClient(fd);
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
			if (channels_pos != channels.end()){
				if (clients[fd].getChannels().find(*lower) != clients[fd].getChannels().end()){
					std::string client_nickname = clients[fd].getNickname();
					std::string rpl = std::string(":" + clients[fd].getNickname() + "!" +
						clients[fd].getUsername() + "@" + clients[fd].getHostname() + " PART " +
						channels_pos->second.getOGName() + " " + msg_on_leave + "\n");
					sendRPL_Channel(true, fd, channels_pos->first, rpl);
					channels_pos->second.removeClient(fd);
					clients[fd].removeChannel(*lower);
					if (channels[*lower].getClients().size() == 0)
						removeChannel(*lower);
					clients[fd].updateCurrentChannel();
				}
				else
					sendRPL(fd, "irc.local", "442", clients[fd].getNickname().c_str(),
						original->c_str(), ":You're not on that channel", NULL);
			}
			else
				sendRPL(fd, "irc.local", "403", clients[fd].getNickname().c_str(),
					original->c_str(), ":No such channel", NULL);
		}
	}
	if (DEBUG)
		std::cout << info("Someone wants to leave") << std::endl;
}

void	Server::quit(int fd, std::vector<std::string> arg){
	if (arg.size() < 1){
		sendRPL(fd, "irc.local", "461", clients[fd].getNickname().c_str(),
				"QUITs", ":Not enough parameters", NULL);
		return ;
	}
	std::string msg;
	try{
		if (arg.size() >= 2){
			ValidateMsgContent(fd, arg[1]);
			msg = arg[1];
		}	
	}
	catch (std::runtime_error & e){
		std::cout << e.what() << std::endl;
		if (DEBUG)
			std::cout << info(std::string("Client " + to_string(fd) + " tried to send a Wrong msg in QUIT").c_str()) << std::endl;
		return ;
	}
	std::pair<int, std::string> client_leave;
	client_leave.first = fd;
	client_leave.second = msg;
	garbage.push_back(client_leave);
	if (DEBUG)
		std::cout << info("Someone wants to quit") << std::endl;
}

void	Server::ping(int fd, std::vector<std::string> arg){
	std::string token;
    if (arg.size() > 1)
        token = arg[1];
    else {
		sendRPL(fd, "irc.local", "461", clients[fd].getNickname().c_str(),
				"PING", ":Not enough parameters", NULL);
		return ;
	}
	sendRPL(fd, "irc.local", "PONG", " :", token.c_str(), NULL);
}

void	Server::privmsg(int fd, std::vector<std::string> arg){
	if (arg.size() < 2){
		sendRPL(fd, "irc.local", "411", clients[fd].getNickname().c_str(),
				"PRIVMSG", ":No recipient given", NULL);
		return ;
	}
	if (arg.size() < 3){
		sendRPL(fd, "irc.local", "412", clients[fd].getNickname().c_str(),
				"PRIVMSG", ":No text to send", NULL);
		return ;
	}
	std::vector<std::string> channel_list;
	std::vector<std::string> client_list;
	std::string msg;
	try{
		if (arg[1][0] == '#')
			channel_list = check_channel_name(fd, arg[1], "PRIVMSG");
		else
			client_list = check_client_name(fd, arg[1]);
		ValidateMsgContent(fd, arg[2]);
		msg = arg[2];
	}
	catch (std::runtime_error & e){
		std::cout << e.what() << std::endl;
		if (DEBUG)
			std::cout << info(std::string("Client " + to_string(fd) + " tried to send a Wrong msg or in a wrong Channel").c_str()) << std::endl;
		return ;
	}
	if (!channel_list.empty()){
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
					channels_pos->second.getOGName() + " " + msg + "\n");
				sendRPL_Channel(false, fd, channels_pos->first, rpl);
			}
		}
	}
	else if (!client_list.empty()){
		for (std::vector<std::string>::iterator it = client_list.begin(); it != client_list.end(); it++){
			for (std::map<int, Client>::iterator clients_pos = clients.begin(); clients_pos != clients.end(); clients_pos++){
				if (toLowerString(clients_pos->second.getNickname()) == toLowerString(*it)){
					if (clients_pos->first == fd)
						continue ;
					std::string rpl = std::string(clients[fd].getNickname() + "!" +
						clients[fd].getUsername() + "@" + clients[fd].getHostname() + " PRIVMSG " +
						clients_pos->second.getNickname() + " " + msg + "\n");
					sendRPL(clients_pos->first, rpl.c_str(), NULL);
				}
			}
		}
	}
}

