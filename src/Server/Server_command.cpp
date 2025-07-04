/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_command.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdhallen <jdhallen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/23 17:08:49 by jdhallen          #+#    #+#             */
/*   Updated: 2025/07/04 13:06:15 by jdhallen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Global.hpp"
#include "Server.hpp"
#include <cctype>

void	Server::join(int fd, std::vector<std::string> arg){
	if (arg.size() < 2){
		sendRPL(fd, "irc.local", "461", clients[fd].getNickname().c_str(),
				"JOIN", ":Not enough parameters", NULL);
		return ;
	}

	// TODO: Si channel +i --> 473 ERR_INVITEONLYCHAN <channel> :Cannot join channel (+i)
	// TODO: Si banni --> 474 ERR_BANNEDFROMCHAN <channel> :Cannot join channel (+b)
	
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
	// (void)key_list;
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
				if (channels_pos->second.getClients().size() >= channels_pos->second.getChannelLimit() && channels_pos->second.getChannelLimit() > 0){
					sendRPL(fd, "irc.local", "471",
						original->c_str(), ":Cannot join channel (+l)", NULL);
					continue ;
				}
				size_t key_pos = std::distance(channel_list_lower.begin(), it);
				const std::string& channel_key = channels_pos->second.getPassword();
				bool key_needed = !channel_key.empty();
				std::cout << "channel key :" << channel_key << std::endl;
				std::cout << "key needed :" << key_needed << std::endl;
				if (key_needed) {
					if (key_pos >= key_list.size() || channel_key != key_list[key_pos]){
						sendRPL(fd, "irc.local", "475",
							original->c_str(), ":Cannot join channel (+k)", NULL);
						continue;
					}
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

void Server::mode(int fd, std::vector<std::string> arg)
{
	if (arg.size() < 3 || arg.at(1).at(0) != '#')
	{
		sendRPL(fd, "irc.local", "461", clients[fd].getNickname().c_str(),
			"MODE", ": Not enough parameters", NULL);
		return;
	}

	if (channels.find(toLowerString(arg.at(1))) == channels.end())
	{
		sendRPL(fd, "irc.local", "403", clients[fd].getNickname().c_str(),
            arg.at(1).c_str(), ":No such channel", NULL);
		return;
	}

	std::map<std::string, std::pair<Channel, size_t> >& channelMap = clients[fd].getChannels();
	std::map<std::string, std::pair<Channel, size_t> >::iterator currentChannel = 
		channelMap.find(toLowerString(arg.at(1)));

	if (currentChannel == channelMap.end())
	{
		sendRPL(fd, "irc.local", "442", clients[fd].getNickname().c_str(),
			arg.at(1).c_str(), ":You're not on that channel", NULL);
		return;
	}

	std::map<int, std::pair<Client, bool> >& clientMap = currentChannel->second.first.getClients();
	if (clientMap.find(fd) == clientMap.end() || !clientMap.find(fd)->second.second)
	{
  	  	sendRPL(fd, "irc.local", "482", clients[fd].getNickname().c_str(),
			arg.at(1).c_str(), ":You're not channel operator", NULL);
		return;
	}

	std::map<std::string, std::pair<Client, bool> >& strClientMap = 
		currentChannel->second.first.getstrClientMap();

	std::string modeStr = arg.at(2);
	if (modeStr.length() < 2) return;

	bool adding = (modeStr[0] == '+');
	char mode = modeStr[1];

	switch (mode)
	{
		case 'i':
     		currentChannel->second.first.setIsOnInvite(adding);
			sendRPL_Channel(true, fd, toLowerString(arg.at(1)), std::string(clients[fd].getNickname()
		                 + " MODE " + arg.at(1) + " " + arg.at(2) + " \n").c_str());
    	  break;

   		case 't':
    		currentChannel->second.first.setIsTopicRestricted(adding);
			sendRPL_Channel(true, fd, toLowerString(arg.at(1)), std::string(clients[fd].getNickname()
		                 + " MODE " + arg.at(1) + " " + arg.at(2) + " \n").c_str());
     	 break;

    	case 'k':
     		if (adding)
     		{
				if (arg.size() < 4)
				{
					sendRPL(fd, "irc.local", "461", clients[fd].getNickname().c_str(),
							"MODE", ":Not enough parameters", NULL);
					return;
				}
			currentChannel->second.first.setPassword(arg.at(3));
			}
			else
			{
				if (arg.size() < 4)
				{
					sendRPL(fd, "irc.local", "461", clients[fd].getNickname().c_str(),
							"MODE", ":Not enough parameters", NULL);
					return;
				}
				if (currentChannel->second.first.getPassword() == arg.at(3))
					currentChannel->second.first.setPassword("");
				else
				{
					sendRPL(fd, "irc.local", "467", clients[fd].getNickname().c_str(),
							arg.at(1).c_str(), ":Channel key incorrect", NULL);
					return;
				}
     		}
			sendRPL_Channel(true, fd, toLowerString(arg.at(1)), std::string(clients[fd].getNickname()
				+ " MODE " + arg.at(1) + " " + arg.at(2) + " " + arg.at(3) + " \n").c_str());
    		break;

		case 'o':
			if (arg.size() < 4)
			{
				sendRPL(fd, "irc.local", "461", clients[fd].getNickname().c_str(),
						"MODE", ":Not enough parameters", NULL);
				return;
			}
			if (strClients.find(toLowerString(arg.at(3))) == strClients.end())
			{
				sendRPL(fd, "irc.local", "401", clients[fd].getNickname().c_str(),
						arg.at(3).c_str(), ":No such nick", NULL);
				return;
			}
			if (strClientMap.find(toLowerString(arg.at(3))) == strClientMap.end())
			{
				sendRPL(fd, "irc.local", "441", clients[fd].getNickname().c_str(),
						arg.at(3).c_str(), ":They aren't on that channel", NULL);
				return;
			}
			strClientMap.find(toLowerString(arg.at(3)))->second.second = adding;
			sendRPL_Channel(true, fd, toLowerString(arg.at(1)), std::string(clients[fd].getNickname()
						+ " MODE " + arg.at(1) + " " + arg.at(2) + " " + arg.at(3) + " \n").c_str());
			break;

		case 'l':
			if (adding)
			{
				if (arg.size() < 4)
				{
					sendRPL(fd, "irc.local", "461", clients[fd].getNickname().c_str(),
							"MODE", ":Not enough parameters", NULL);
					return;
				}

				unsigned long limit = strToNbr<unsigned long>(arg.at(3));
				if (limit == 0)
				{
					sendRPL(fd, "irc.local", "461", clients[fd].getNickname().c_str(),
							"MODE", ":Invalid limit value", NULL);
					return;
				}
				currentChannel->second.first.setChannelLimit(limit);
				sendRPL_Channel(true, fd, toLowerString(arg.at(1)), std::string(clients[fd].getNickname()
					+ " MODE " + arg.at(1) + " " + arg.at(2) + " " + arg.at(3) + " \n").c_str());
			}
			else
			{
				currentChannel->second.first.setChannelLimit(0);
				sendRPL_Channel(true, fd, toLowerString(arg.at(1)), std::string(clients[fd].getNickname()
					+ " MODE " + arg.at(1) + " " + arg.at(2) + " \n").c_str());
			}
			break;

		default:
			sendRPL(fd, "irc.local", "472", clients[fd].getNickname().c_str(),
				std::string(1, mode).c_str(), ":is unknown mode char to me", NULL);
			return;
	}
}


// void	Server::topic(int fd, std::vector<std::string> arg){
// 	if (arg.size() < 1){
// 		sendRPL(fd, "irc.local", "461", clients[fd].getNickname().c_str(),
// 				"PART", ":Not enough parameters", NULL);
// 		return ;
// 	}
// 	std::vector<std::string> channel_list;
// 	std::string msg_on_leave;
// 	try{
// 		if (arg.size() >= 2)
// 			channel_list = check_channel_name(fd, arg[1], "PART");
// 		if (arg.size() >= 3){
// 			ValidateMsgContent(fd, arg[2]);
// 			msg_on_leave = arg[2];
// 		}
// 	}
// 	catch (std::runtime_error & e){
// 		std::cout << e.what() << std::endl;
// 		if (DEBUG)
// 			std::cout << info(std::string("Client " + to_string(fd) + " tried to leave a wrong Channel").c_str()) << std::endl;
// 		return ;
// 	}
// }
