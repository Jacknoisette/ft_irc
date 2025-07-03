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

#include "Global.hpp"
#include "Server.hpp"
#include <cctype>

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
		ValidateMsgContent(fd, arg[2]);
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
			channels[toLowerString(arg.at(1))].sendRPL_Channel(true, fd, std::string(clients[fd].getNickname()
		                 + " MODE " + arg.at(1) + " " + arg.at(2) + " \n").c_str());
      break;

    case 't':
      currentChannel->second.first.setIsTopicRestricted(adding);
			channels[toLowerString(arg.at(1))].sendRPL_Channel(true, fd, std::string(clients[fd].getNickname()
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
			channels[toLowerString(arg.at(1))].sendRPL_Channel(true, fd, std::string(clients[fd].getNickname()
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
			channels[toLowerString(arg.at(1))].sendRPL_Channel(true, fd, std::string(clients[fd].getNickname()
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
				channels[toLowerString(arg.at(1))].sendRPL_Channel(true, fd, std::string(clients[fd].getNickname()
		                 + " MODE " + arg.at(1) + " " + arg.at(2) + " " + arg.at(3) + " \n").c_str());
      }
      else
      {
        currentChannel->second.first.setChannelLimit(0);
				channels[toLowerString(arg.at(1))].sendRPL_Channel(true, fd, std::string(clients[fd].getNickname()
		                 + " MODE " + arg.at(1) + " " + arg.at(2) + " \n").c_str());
      }
      break;

    default:
      sendRPL(fd, "irc.local", "472", clients[fd].getNickname().c_str(),
              std::string(1, mode).c_str(), ":is unknown mode char to me", NULL);
      return;
	}
}
