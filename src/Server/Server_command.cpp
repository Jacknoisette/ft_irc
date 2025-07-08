/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_command.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdhallen <jdhallen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/23 17:08:49 by jdhallen          #+#    #+#             */
/*   Updated: 2025/07/04 16:04:16 by jdhallen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Global.hpp"
#include "Server.hpp"
#include <cctype>

void	Server::join(int fd, std::vector<std::string> arg){
	if (arg.size() < 2){
		sendRPL(fd, "irc.local", "461", clients[fd]->getNickname().c_str(),
				"JOIN", ":Not enough parameters", NULL);
		return ;
	}
	
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
	std::vector<std::pair<std::string, std::string> > channel_list_lower = toLowerVector(channel_list);
	for (std::vector<std::pair<std::string, std::string> >::iterator it = channel_list_lower.begin(); it != channel_list_lower.end(); it++)
	{
		std::string *lower = &it->first;
		std::string *original = &it->second;
		if ((*lower)[0] != '#')
		{
			sendRPL(fd, "irc.local", "476", clients[fd]->getNickname().c_str(),
					lower->c_str(), ":Bad Channel Mask", NULL);
			continue ;
		}
		std::map<std::string, Channel*>::iterator channels_pos = channels.find(*lower);
		if (channels_pos != channels.end()){
			if (clients[fd]->getChannels().find(*lower) == clients[fd]->getChannels().end()){
				if (clients[fd]->getChannels().size() >= 20){
					sendRPL(fd, "irc.local", "405", clients[fd]->getNickname().c_str(),
						original->c_str(), ":You have joined too many channels", NULL);
					continue ;
				}
				if (channels_pos->second->getIsOnInvite()){
					std::vector<int> &clientInvite = channels_pos->second->getClientInvite();
					std::vector<int>::iterator itClientInvite = std::find(clientInvite.begin(), clientInvite.end(), fd);
					if (itClientInvite == clientInvite.end()){
						sendRPL(fd, "irc.local", "473", clients[fd]->getNickname().c_str(),
							original->c_str(), ":Cannot join channel (+i)", NULL);
						continue ;
					}
					clientInvite.erase(itClientInvite);
				}
				if (channels_pos->second->getClientsListFd().size() >= channels_pos->second->getChannelLimit() && channels_pos->second->getChannelLimit() > 0){
					sendRPL(fd, "irc.local", "471", clients[fd]->getNickname().c_str(),
						original->c_str(), ":Cannot join channel (+l)", NULL);
					continue ;
				}
				size_t key_pos = std::distance(channel_list_lower.begin(), it);
				const std::string& channel_key = channels_pos->second->getPassword();
				bool key_needed = !channel_key.empty();
				if (key_needed) {
					if (key_pos >= key_list.size() || channel_key != key_list[key_pos]){
						sendRPL(fd, "irc.local", "475", clients[fd]->getNickname().c_str(),
							original->c_str(), ":Cannot join channel (+k)", NULL);
						continue;
					}
				}
				channels_pos->second->addClient(fd, clients[fd], false);
				clients[fd]->addChannel(channels[*lower]);
				clients[fd]->setCurrentChannel(*lower);
				sendWelcomeChannelMsg(clients[fd], *lower);
			} else {
				clients[fd]->setCurrentChannel(*lower);
				if (DEBUG)
					std::cout << info(std::string("Client " + clients[fd]->getNickname() + " switched to channel " + *original + " \n").c_str()) << std::endl;
			}
		}
		else{
			if (clients[fd]->getChannels().size() >= 20){
				sendRPL(fd, "irc.local", "405", clients[fd]->getNickname().c_str(),
					original->c_str(), ":You have joined too many channels", NULL);
				continue ;
			}
			Channel *new_channel = getOrCreateChannel(*original);
			new_channel->addClient(fd, clients[fd], true);
			clients[fd]->addChannel(channels[*lower]);
			clients[fd]->setCurrentChannel(*lower);
			std::cout << info(std::string("New channel " + *original + " created").c_str()) << std::endl;
			sendWelcomeChannelMsg(clients[fd], *lower);
		}
	}
	if (DEBUG)
		std::cout << info("Someone wants to join") << std::endl;
	if (DEBUG)
		ultimateDebug();
}

void	Server::part(int fd, std::vector<std::string> arg){
	if (arg.size() < 1){
		sendRPL(fd, "irc.local", "461", clients[fd]->getNickname().c_str(),
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
	if (clients[fd]->getChannels().empty())
		return ;
	if (channel_list.size() == 0){
		Channel *actual_channel = channels[clients[fd]->getCurrentChannel()];
		std::string rpl = std::string(":" + clients[fd]->getNickname() + "!" +
			clients[fd]->getUsername() + "@" + clients[fd]->getHostname() + " PART " +
			actual_channel->getOGName() + " " + msg_on_leave + "\n");
		sendRPL_Channel(true, fd, actual_channel->getName(), rpl);
		actual_channel->removeClient(fd);
		clients[fd]->removeChannel(clients[fd]->getCurrentChannel());
		if (actual_channel->getClientsListFd().size() == 0)
			removeChannel(clients[fd]->getCurrentChannel());
		clients[fd]->updateCurrentChannel();
	}
	else {
		std::vector<std::pair<std::string, std::string> > channel_list_lower = toLowerVector(channel_list);
		for (std::vector<std::pair<std::string, std::string> >::iterator it = channel_list_lower.begin(); it != channel_list_lower.end(); it++)
		{
			std::string *lower = &it->first;
			std::string *original = &it->second;
			if ((*lower)[0] != '#')
			{
				sendRPL(fd, "irc.local", "476", clients[fd]->getNickname().c_str(),
						lower->c_str(), ":Bad Channel Mask", NULL);
				continue ;
			}
			std::map<std::string, Channel*>::iterator channels_pos = channels.find(*lower);
			if (channels_pos != channels.end()){
				if (clients[fd]->getChannels().find(*lower) != clients[fd]->getChannels().end()){
					std::string client_nickname = clients[fd]->getNickname();
					std::string rpl = std::string(":" + clients[fd]->getNickname() + "!" +
						clients[fd]->getUsername() + "@" + clients[fd]->getHostname() + " PART " +
						channels_pos->second->getOGName() + " " + msg_on_leave + "\n");
					sendRPL_Channel(true, fd, channels_pos->first, rpl);
					channels_pos->second->removeClient(fd);
					clients[fd]->removeChannel(*lower);
					if (channels[*lower]->getClientsListFd().size() == 0)
						removeChannel(*lower);
					clients[fd]->updateCurrentChannel();
				}
				else
					sendRPL(fd, "irc.local", "442", clients[fd]->getNickname().c_str(),
						original->c_str(), ":You're not on that channel", NULL);
			}
			else
				sendRPL(fd, "irc.local", "403", clients[fd]->getNickname().c_str(),
					original->c_str(), ":No such channel", NULL);
		}
	}
	if (DEBUG)
		std::cout << info("Someone wants to leave") << std::endl;
	if (DEBUG)
		ultimateDebug();
}

void	Server::quit(int fd, std::vector<std::string> arg){
	if (arg.size() < 1){
		sendRPL(fd, "irc.local", "461", clients[fd]->getNickname().c_str(),
				"QUIT", ":Not enough parameters", NULL);
		return ;
	}
	std::string msg;
	try{
		if (arg.size() >= 2){
			ValidateMsgContent(fd, arg[1]);
			msg = arg[1];
		}
		else {
			msg = ":Client Quit";
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
	if (DEBUG)
		ultimateDebug();
}

void	Server::ping(int fd, std::vector<std::string> arg){
	std::string token;
    if (arg.size() > 1)
        token = arg[1];
    else {
		sendRPL(fd, "irc.local", "461", clients[fd]->getNickname().c_str(),
				"PING", ":Not enough parameters", NULL);
		return ;
	}
	sendRPL(fd, "irc.local", "PONG", " :", token.c_str(), NULL);
}

void	Server::privmsg(int fd, std::vector<std::string> arg){
	if (arg.size() < 2){
		sendRPL(fd, "irc.local", "411", clients[fd]->getNickname().c_str(),
				"PRIVMSG", ":No recipient given", NULL);
		return ;
	}
	if (arg.size() < 3){
		sendRPL(fd, "irc.local", "412", clients[fd]->getNickname().c_str(),
				"PRIVMSG", ":No text to send", NULL);
		return ;
	}
	std::vector<std::string> target_list;
	std::string msg;
	try{
		target_list = check_channel_name(fd, arg[1], "PRIVMSG");
		ValidateMsgContent(fd, arg[2]);
		msg = arg[2];
	}
	catch (std::runtime_error & e){
		std::cout << e.what() << std::endl;
		if (DEBUG)
			std::cout << info(std::string("Client " + to_string(fd) + " tried to send a Wrong msg or in a wrong Channel").c_str()) << std::endl;
		return ;
	}
	std::vector<std::string> already_sent;
	if (!target_list.empty()){
		std::vector<std::pair<std::string, std::string> > target_list_lower = toLowerVector(target_list);
		for (std::vector<std::pair<std::string, std::string> >::iterator it = target_list_lower.begin(); it != target_list_lower.end(); it++){
			std::string *lower = &it->first;
			std::string *original = &it->second;
			std::vector<std::string>::iterator italready_sent = std::find(already_sent.begin(), already_sent.end(), (*lower));
			if (italready_sent != already_sent.end()){
				continue ;
			}
			already_sent.push_back((*lower));
			if ((*lower)[0] == '#')
			{
				std::map<std::string, Channel*>::iterator channels_pos = channels.find(*lower);
				if (channels_pos == channels.end()){
					sendRPL(fd, "irc.local", "403", clients[fd]->getNickname().c_str(),
							original->c_str(), ":No such channel", NULL);
					continue ;
				}
				if (clients[fd]->getChannels().find(*lower) == clients[fd]->getChannels().end()){
					sendRPL(fd, "irc.local", "442", clients[fd]->getNickname().c_str(),
							original->c_str(), ":You're not on that channel", NULL);
					continue ;
				}
				std::string rpl = std::string(":" + clients[fd]->getNickname() + "!" +
						clients[fd]->getUsername() + "@" + clients[fd]->getHostname() + " PRIVMSG " +
						channels_pos->second->getOGName() + " " + msg + "\n");
				sendRPL_Channel(false, fd, channels_pos->first, rpl);
			} else {
				bool found_nick = false;
				for (std::map<int, Client*>::iterator clients_pos = clients.begin(); clients_pos != clients.end(); clients_pos++){
					if (toLowerString(clients_pos->second->getNickname()) == (*lower)){
						if (clients_pos->first == fd)
							break ;
						std::string rpl = std::string(clients[fd]->getNickname() + "!" +
							clients[fd]->getUsername() + "@" + clients[fd]->getHostname() + " PRIVMSG " +
							clients_pos->second->getNickname() + " " + msg);
						sendRPL(clients_pos->first, rpl.c_str(), NULL);
						found_nick = true;
						break ;
					}
				}
				if (!found_nick ){
					sendRPL(fd, "irc.local", "401", clients[fd]->getNickname().c_str(),
							original->c_str(), ":No such nick", NULL);
				}
			}
		}
	}
}

void Server::mode(int fd, std::vector<std::string> arg)
{
	if (arg.size() < 3)
	{
		sendRPL(fd, "irc.local", "461", clients[fd]->getNickname().c_str(),
			"MODE", ": Not enough parameters", NULL);
		return;
	}

	if (channels.find(toLowerString(arg.at(1))) == channels.end())
	{
		sendRPL(fd, "irc.local", "403", clients[fd]->getNickname().c_str(),
            arg.at(1).c_str(), ":No such channel", NULL);
		return;
	}

	std::map<std::string, std::pair<Channel*, size_t> >& channelMap = clients[fd]->getChannels();
	std::map<std::string, std::pair<Channel*, size_t> >::iterator currentChannel = 
		channelMap.find(toLowerString(arg.at(1)));

	if (currentChannel == channelMap.end())
	{
		sendRPL(fd, "irc.local", "442", clients[fd]->getNickname().c_str(),
			arg.at(1).c_str(), ":You're not on that channel", NULL);
		return;
	}

	std::map<int, std::pair<Client*, bool> >& clientMap = currentChannel->second.first->getClientsListFd();
	if (clientMap.find(fd) == clientMap.end() || !clientMap.find(fd)->second.second)
	{
  	  	sendRPL(fd, "irc.local", "482", clients[fd]->getNickname().c_str(),
			arg.at(1).c_str(), ":You're not channel operator", NULL);
		return;
	}

	std::map<std::string, std::pair<Client*, bool> >& strClientMap = 
		currentChannel->second.first->getClientsListStr();

	std::string modeStr = arg.at(2);
	if (modeStr.length() < 2) return;

	bool adding = (modeStr[0] == '+');
	char mode = modeStr[1];

	switch (mode)
	{
		case 'i':
     		currentChannel->second.first->setIsOnInvite(adding);
			sendRPL_Channel(true, fd, toLowerString(arg.at(1)), std::string(clients[fd]->getNickname()
		                 + " MODE " + arg.at(1) + " " + arg.at(2) + " \n").c_str());
    	  break;

   		case 't':
    		currentChannel->second.first->setIsTopicRestricted(adding);
			sendRPL_Channel(true, fd, toLowerString(arg.at(1)), std::string(clients[fd]->getNickname()
		                 + " MODE " + arg.at(1) + " " + arg.at(2) + " \n").c_str());
     	 break;

    	case 'k':
   		if (adding)
   		{
				if (arg.size() < 4)
				{
					sendRPL(fd, "irc.local", "461", clients[fd]->getNickname().c_str(),
							"MODE", ":Not enough parameters", NULL);
					return;
				}
				currentChannel->second.first->setPassword(arg.at(3));
			}
			else
			{
				if (arg.size() < 4)
				{
					sendRPL(fd, "irc.local", "461", clients[fd]->getNickname().c_str(),
							"MODE", ":Not enough parameters", NULL);
					return;
				}
				if (currentChannel->second.first->getPassword() == arg.at(3))
					currentChannel->second.first->setPassword("");
				else
				{
					sendRPL(fd, "irc.local", "467", clients[fd]->getNickname().c_str(),
							arg.at(1).c_str(), ":Channel key incorrect", NULL);
					return;
				}
   		}
			sendRPL_Channel(true, fd, toLowerString(arg.at(1)), std::string(clients[fd]->getNickname()
				+ " MODE " + arg.at(1) + " " + arg.at(2) + " " + arg.at(3) + " \n").c_str());
    		break;

		case 'o':
		{
			if (arg.size() < 4)
			{
				sendRPL(fd, "irc.local", "461", clients[fd]->getNickname().c_str(),
						"MODE", ":Not enough parameters", NULL);
				return;
			}
			if (strClients.find(toLowerString(arg.at(3))) == strClients.end())
			{
				sendRPL(fd, "irc.local", "401", clients[fd]->getNickname().c_str(),
						arg.at(3).c_str(), ":No such nick", NULL);
				return;
			}
			std::string targetNick = toLowerString(arg.at(3));
			std::map<std::string, std::pair<Client*, bool> >::iterator nickIt = strClientMap.find(targetNick);
			if (nickIt == strClientMap.end())
			{
				sendRPL(fd, "irc.local", "441", clients[fd]->getNickname().c_str(),
						arg.at(3).c_str(), ":They aren't on that channel", NULL);
				return;
			}
			int targetFd = nickIt->second.first->getClientfd();
			std::map<int, std::pair<Client*, bool> >::iterator fdIt = clientMap.find(targetFd);
			if (fdIt != clientMap.end())
				fdIt->second.second = adding;
			sendRPL_Channel(true, fd, toLowerString(arg.at(1)), std::string(clients[fd]->getNickname()
						+ " MODE " + arg.at(1) + " " + arg.at(2) + " " + arg.at(3) + " \n").c_str());
			break;
		}

		case 'l':
			if (adding)
			{
				if (arg.size() < 4)
				{
					sendRPL(fd, "irc.local", "461", clients[fd]->getNickname().c_str(),
							"MODE", ":Not enough parameters", NULL);
					return;
				}

				unsigned long limit = strToNbr<unsigned long>(arg.at(3));
				if (limit == 0)
				{
					sendRPL(fd, "irc.local", "461", clients[fd]->getNickname().c_str(),
							"MODE", ":Invalid limit value", NULL);
					return;
				}
				currentChannel->second.first->setChannelLimit(limit);
				sendRPL_Channel(true, fd, toLowerString(arg.at(1)), std::string(clients[fd]->getNickname()
					+ " MODE " + arg.at(1) + " " + arg.at(2) + " " + arg.at(3) + " \n").c_str());
			}
			else
			{
				currentChannel->second.first->setChannelLimit(0);
				sendRPL_Channel(true, fd, toLowerString(arg.at(1)), std::string(clients[fd]->getNickname()
					+ " MODE " + arg.at(1) + " " + arg.at(2) + " \n").c_str());
			}
			break;

		default:
			sendRPL(fd, "irc.local", "472", clients[fd]->getNickname().c_str(),
				std::string(1, mode).c_str(), ":is unknown mode char to me", NULL);
			return;
	}
	if (DEBUG)
		ultimateDebug();
}

void	Server::topic(int fd, std::vector<std::string> arg){
	if (arg.size() < 2){
		sendRPL(fd, "irc.local", "461", clients[fd]->getNickname().c_str(),
				"TOPIC", ":Not enough parameters", NULL);
		return ;
	}
	std::vector<std::string> channel_list;
	std::string channel;
	std::string new_topic;
	try{
		channel_list = check_channel_name(fd, arg[1], "TOPIC");
		if (channel_list.size() > 1){
			sendRPL(fd, "irc.local", "407", clients[fd]->getNickname().c_str(),
				arg[1].c_str(), ":Too many targets", NULL);
			throw std::runtime_error(info(std::string("Client " + to_string(fd) + " tried to TOPIC too many channels").c_str()));
		}
		channel = channel_list[0];
		if (arg.size() >= 3){
			ValidateMsgContent(fd, arg[2]);
			new_topic = arg[2];
		}
	}
	catch (std::runtime_error & e){
		std::cout << e.what() << std::endl;
		if (DEBUG)
			std::cout << info(std::string("Client " + to_string(fd) + " tried to topic a wrong Channel").c_str()) << std::endl;
		return ;
	}
	std::string channel_lower = toLowerString(channel);
	std::map<std::string, Channel*>::iterator channel_pos = channels.find(channel_lower);
	if (channel_pos != channels.end()){
		if (clients[fd]->getChannels().find(channel_lower) != clients[fd]->getChannels().end()){
			if (arg.size() == 2)
			{
				if (!channels.at(channel_lower)->getTopic().empty()){
					sendRPL(fd, "irc.local", "332", clients[fd]->getNickname().c_str(),
							channel.c_str(), std::string(":" + channels.at(channel_lower)->getTopic()).c_str(), NULL);
				}else{
					sendRPL(fd, "irc.local", "331", clients[fd]->getNickname().c_str(),
							channel.c_str(), ":No topic is set", NULL);
				}
			} else {
				std::map<int, std::pair<Client *, bool> >::iterator client_pos = channel_pos->second->getClientsListFd().find(fd);
				if (client_pos->second.second || !channel_pos->second->getIsTopicRestricted()){
					new_topic = new_topic.substr(1);
					channel_pos->second->setTopic(new_topic);
					std::string rpl = std::string(":" + clients[fd]->getNickname() + "!" +
						clients[fd]->getUsername() + "@" + clients[fd]->getHostname() + " TOPIC " +
						channel + " :" + new_topic + "\n");
					sendRPL_Channel(true, fd, channel_lower, rpl);
				}
				else {
					sendRPL(fd, "irc.local", "482", clients[fd]->getNickname().c_str(),
							channel.c_str(), ":You're not channel operator", NULL);
				}
			}
		}
		else {
			sendRPL(fd, "irc.local", "442", clients[fd]->getNickname().c_str(),
				channel.c_str(), ":You're not on that channel", NULL);
		}
	}
	else {
		sendRPL(fd, "irc.local", "403", clients[fd]->getNickname().c_str(),
				channel.c_str(), ":No such channel", NULL);
	}
	
	if (DEBUG)
		std::cout << info("Someone wants to topic") << std::endl;
}

void	Server::nick(int fd, const std::vector<std::string> arg)
{
	if (arg.size() < 2)
	{
		sendRPL(fd, "irc.local", "431", "", ":No nickname given", NULL);
		return;
	}

	if (!clients[fd]->getPasswordMatch() && !password.empty())
	{
		sendRPL(fd, "irc.local", "464", "", ":Password incorrect", NULL);
		return;
	}

	if (!isValidNickname(arg[1]))
	{
		sendRPL(fd, "irc.local", "432", arg[1].c_str(), ":Erroneous nickname", NULL);
		return;
	}

	if (strClients.find(toLowerString(arg[1])) != strClients.end())
	{
		sendRPL(fd, "irc.local", "433", arg[1].c_str(), ":Nickname is already in use", NULL);
		return;
	}

	clients[fd]->setNickname(arg[1]);
	clients[fd]->setNormalizedNick(toLowerString(arg[1]));
	strClients[toLowerString(arg[1])] = clients[fd];
	if (DEBUG)
		ultimateDebug();
}

void Server::invite(int fd, std::vector<std::string> arg)
{
	if (arg.size() < 3){
		sendRPL(fd, "irc.local", "461", clients[fd]->getNickname().c_str(),
			"INVITE", ": Not enough parameters", NULL);
		return;
	}
	std::string channelName = toLowerString(arg.at(2));
	std::string guestName = toLowerString(arg.at(1));

	if (channels.find(channelName) == channels.end()){
		sendRPL(fd, "irc.local", "403", clients[fd]->getNickname().c_str(),
			channelName.c_str(), ":No such channel", NULL);
		return;
	}

	std::map<std::string, std::pair<Channel*, size_t> >& client_channels = clients[fd]->getChannels();
	std::map<std::string, std::pair<Channel*, size_t> >::iterator itInviteInChannel = 
		client_channels.find(channelName);

	if (itInviteInChannel == client_channels.end()){
		sendRPL(fd, "irc.local", "442", clients[fd]->getNickname().c_str(),
			channelName.c_str(), ":You're not on that channel", NULL);
		return;
	}

	std::map<int, std::pair<Client*, bool> >& clientMap = itInviteInChannel->second.first->getClientsListFd();
	if (clientMap.find(fd) == clientMap.end() || !clientMap.find(fd)->second.second)
	{
  		sendRPL(fd, "irc.local", "482", clients[fd]->getNickname().c_str(),
			channelName.c_str(), ":You're not channel operator", NULL);
		return;
	}
	
	if (strClients.find(guestName) == strClients.end()){
		sendRPL(fd, "irc.local", "401", clients[fd]->getNickname().c_str(),
			arg.at(1).c_str(), ":No such nick", NULL);
		return ;
	}

	std::map<std::string, std::pair<Client*, bool> >& channelListOfClientStr
    		= itInviteInChannel->second.first->getClientsListStr();
	if (channelListOfClientStr.find(guestName) != channelListOfClientStr.end()){
		sendRPL(fd, "irc.local", "443", clients[fd]->getNickname().c_str(),
				guestName.c_str(), channelName.c_str(), ":Is already on channel", NULL);
		return;
	}

	std::vector<int> &clientInvite = itInviteInChannel->second.first->getClientInvite();
	Client *guestClient = strClients.find(guestName)->second;
	int 	guestClientFd = guestClient->getClientfd();

	std::vector<int>::iterator it = std::find(clientInvite.begin(), clientInvite.end(), guestClientFd);
    if (it != clientInvite.end())
		return ;
	clientInvite.push_back(guestClientFd);
	sendRPL(fd, "irc.local", "341", clients[fd]->getNickname().c_str(),
			guestClient->getNickname().c_str(), channelName.c_str(), NULL);

	std::string rpl = std::string(clients[fd]->getNickname() + "!" +
		clients[fd]->getUsername() + "@" + clients[fd]->getHostname() + " INVITE " +
			guestClient->getNickname() + " :" + channelName.c_str() + "\n");
	sendRPL(guestClientFd, rpl.c_str(), NULL);
}

void	Server::applykick(int fd, int kick_client_fd, std::string &channel_lower, std::string &kick_msg){
	std::string rpl = std::string(":" + clients[fd]->getNickname() + "!" +
		clients[fd]->getUsername() + "@" + clients[fd]->getHostname() + " KICK " +
		channels[channel_lower]->getOGName() + " " + clients[kick_client_fd]->getNickname() + " " + kick_msg + "\n");
	sendRPL_Channel(true, fd, channel_lower, rpl);
	channels[channel_lower]->removeClient(kick_client_fd);
	clients[kick_client_fd]->removeChannel(channel_lower);
	if (channels[channel_lower]->getClientsListFd().size() == 0)
		removeChannel(channel_lower);
	clients[kick_client_fd]->updateCurrentChannel();
}

void	Server::kick(int fd, std::vector<std::string> arg){
	if (arg.size() < 3){
		sendRPL(fd, "irc.local", "461", clients[fd]->getNickname().c_str(),
				"KICK", ":Not enough parameters", NULL);
		return ;
	}
	std::vector<std::string> channel_list;
	std::vector<std::string> client_list;
	std::string kick_msg = "";
	try{
		channel_list = check_channel_name(fd, arg[1], "PRIVMSG");
		client_list = check_client_name(fd, arg[2]);
		if (arg.size() >= 4) {
			if (arg[3].empty() || arg[3] == ":")
				kick_msg = ":" + clients[fd]->getNickname();
			else
				kick_msg = arg[3];
		} else {
			kick_msg = ":" + clients[fd]->getNickname();
		}
	}
	catch (std::runtime_error & e){
		std::cout << e.what() << std::endl;
		if (DEBUG)
			std::cout << info(std::string("Client " + to_string(fd) + " tried to send a Wrong msg or in a wrong Channel").c_str()) << std::endl;
		return ;
	}
	if (channel_list.empty() && !client_list.empty())
		return ;
	if (clients[fd]->getChannels().empty())
		return ;
	if (channel_list.size() > client_list.size() && client_list.size() != 1) {
		sendRPL(fd, "irc.local", "407", clients[fd]->getNickname().c_str(),
			arg[1].c_str(), ":Too many targets", NULL);
		return ;
	} else if (channel_list.size() < client_list.size() && channel_list.size() != 1) {
		sendRPL(fd, "irc.local", "407", clients[fd]->getNickname().c_str(),
			arg[2].c_str(), ":Too many targets", NULL);
		return ;
	}
	std::vector<std::pair<std::string, std::string> > channel_list_lower = toLowerVector(channel_list);
	std::vector<std::pair<std::string, std::string> > client_list_lower = toLowerVector(client_list);
	size_t	max_size = 0;
	if (channel_list_lower.size() >= client_list_lower.size())
		max_size = channel_list_lower.size();
	else
		max_size = client_list_lower.size();
	for (size_t i = 0; i < max_size; i++){
		std::string *channel_lower, *channel_original, *client_lower, *client_original = NULL;
		if (channel_list_lower.size() == client_list_lower.size()) {
			channel_lower = &channel_list_lower[i].first;
			channel_original = &channel_list_lower[i].second;
			client_lower = &client_list_lower[i].first;
			client_original = &client_list_lower[i].second;
		} else if (channel_list_lower.size() == 1) {
			channel_lower = &channel_list_lower[0].first;
			channel_original = &channel_list_lower[0].second;
			client_lower = &client_list_lower[i].first;
			client_original = &client_list_lower[i].second;
		} else if (client_list_lower.size() == 1) {
			channel_lower = &channel_list_lower[i].first;
			channel_original = &channel_list_lower[i].second;
			client_lower = &client_list_lower[0].first;
			client_original = &client_list_lower[0].second;
		}
		if (channels.find(*channel_lower) == channels.end()){
			sendRPL(fd, "irc.local", "403", clients[fd]->getNickname().c_str(),
				channel_original->c_str(), ":No such channel", NULL);
			continue ;
		}
		if (clients[fd]->getChannels().find(*channel_lower) == clients[fd]->getChannels().end()){
			sendRPL(fd, "irc.local", "442", clients[fd]->getNickname().c_str(),
				channel_original->c_str(), ":You're not on that channel", NULL);
			continue ;
		}
		std::map<std::string, std::pair<Client *, bool> >::iterator client_to_kick = channels[*channel_lower]->getClientsListStr().find(*client_lower);
		if (client_to_kick == channels[*channel_lower]->getClientsListStr().end()){
			sendRPL(fd, "irc.local", "441", clients[fd]->getNickname().c_str(),
					client_original->c_str(), channel_original->c_str(), ":They aren't on that channel", NULL);
			continue ;
		}
		std::map<int, std::pair<Client *, bool> >::iterator client_pos = channels[*channel_lower]->getClientsListFd().find(fd);
		if (!client_pos->second.second){
			sendRPL(fd, "irc.local", "482", clients[fd]->getNickname().c_str(),
						channel_original->c_str(), ":You're not channel operator", NULL);
			continue ;
		}
		applykick(fd, client_to_kick->second.first->getClientfd(), *channel_lower, kick_msg);
	}
	if (DEBUG)
		std::cout << info("Someone wants to kick") << std::endl;
	if (DEBUG)
		ultimateDebug();
}
