/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_utils.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdhallen <jdhallen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/02 11:22:34 by jdhallen          #+#    #+#             */
/*   Updated: 2025/07/02 16:38:00 by jdhallen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Global.hpp"

std::vector<std::string>	parsing_channel(std::string arg){
	std::vector<std::string>	channel_list;
	size_t start = 0;
	while (start < arg.size())
	{
		size_t pos = arg.find(',', start);
        if (pos == std::string::npos) {
            channel_list.push_back(arg.substr(start));
            break;
        }
        if (pos > start)
            channel_list.push_back(arg.substr(start, pos-start));
        start = pos + 1;
	}
	return channel_list;
}

std::vector<std::string> Server::check_channel_name(int fd, std::string arg, std::string cmd_name){
	if (arg.size() > 50){
		sendRPL(fd, "irc.local", "479", clients[fd].getNickname().c_str(),
				arg.c_str(), ":Channel name is too long", NULL);
		throw std::runtime_error(info(std::string("Client " + to_string(fd) + " tried to create a wrong Channel").c_str()));
	}
	if (arg.size() <= 1){
		sendRPL(fd, "irc.local", "461", clients[fd].getNickname().c_str(),
				cmd_name.c_str(), ":Not enough parameters", NULL);
		throw std::runtime_error(info(std::string("Client " + to_string(fd) + " tried to create a wrong Channel").c_str()));
	}
	if (arg[0] != '#')
	{
		sendRPL(fd, "irc.local", "476", clients[fd].getNickname().c_str(),
				arg.c_str(), ":Bad Channel Mask", NULL);
		throw std::runtime_error(info(std::string("Client " + to_string(fd) + " tried to create a wrong Channel").c_str()));
	}
	for (size_t i = 0; i < arg.size(); i++){
		if (arg[i] <= 32 || arg[i] == ':'){
			sendRPL(fd, "irc.local", "479", clients[fd].getNickname().c_str(),
					arg.c_str(), ":Channel name is invalid", NULL);
			throw std::runtime_error(info(std::string("Client " + to_string(fd) + " tried to create a wrong Channel").c_str()));
		}
	}
	std::vector<std::string> channels = parsing_channel(arg);
	for (size_t i = 0; i < channels.size(); i++){
		if (channels[i][0] != '#'){
			sendRPL(fd, "irc.local", "476", clients[fd].getNickname().c_str(),
					arg.c_str(), ":Bad Channel Mask", NULL);
			throw std::runtime_error(info(std::string("Client " + to_string(fd) + " tried to create a wrong Channel").c_str()));
		}		
	}
	return channels;
}

void Server::client_name_parsing(int fd, std::string arg){
	if (arg.size() > 50){
		sendRPL(fd, "irc.local", "432",
					arg.c_str(), ":Erroneous nickname", NULL);
		throw std::runtime_error(info(std::string("Client " + to_string(fd) + " tried to create a wrong Channel").c_str()));
	}
	for (size_t i = 0; i < arg.size(); i++){
		if (arg[i] <= 32 || arg[i] == ':'){
			sendRPL(fd, "irc.local", "432",
					arg.c_str(), ":Erroneous nickname", NULL);
			throw std::runtime_error(info(std::string("Client " + to_string(fd) + " tried to create a wrong Channel").c_str()));
		}
	}
	return ;
}


std::vector<std::string> Server::check_client_name(int fd, std::string arg){
	try{
		client_name_parsing(fd, arg);
	}catch (std::runtime_error & e){
		throw e;
	}
	std::vector<std::string> clients = parsing_channel(arg);
	return clients;
}

std::vector<std::string>	Server::check_key_string(int fd, std::string arg){
	if (arg.size() > 23){
		sendRPL(fd, "irc.local", "475", clients[fd].getNickname().c_str(),
					arg.c_str(), ":Cannot join channel (+k)", NULL);
		throw std::runtime_error(info(std::string("Client " + to_string(fd) + " tried to create a wrong Channel").c_str()));
	}
	for (size_t i = 0; i < arg.size(); i++){
		if (arg[i] <= 32 || arg[i] == ':'){
			sendRPL(fd, "irc.local", "475", clients[fd].getNickname().c_str(),
					arg.c_str(), ":Cannot join channel (+k)", NULL);
			throw std::runtime_error(info(std::string("Client " + to_string(fd) + " tried to create a wrong Channel").c_str()));
		}
	}
	return parsing_channel(arg);
}

void	Server::ValidateMsgContent(int fd, std::string arg)
{
	if (arg.size() > 510)
	{
		sendRPL(fd, "irc.local", "417", clients[fd].getNickname().c_str(),
					":Input line was too long", NULL);
		throw std::runtime_error(info(std::string("Client " + to_string(fd)
		                 + " tried to create a wrong Channel").c_str()));
	}
	if (arg.size() <= 1)
	{
		sendRPL(fd, "irc.local", "412", clients[fd].getNickname().c_str(),
					":No text to send", NULL);
		throw std::runtime_error(info(std::string("Client " + to_string(fd)
		                  + " tried to create a wrong Channel").c_str()));
	}
	if (arg[0] != ':'){
		sendRPL(fd, "irc.local", "412", clients[fd].getNickname().c_str(),
					":No text to send", NULL);
		throw std::runtime_error(info(std::string("Client " + to_string(fd) + " tried to create a wrong Msg").c_str()));
	}
	for (size_t i = 0; i < arg.size(); i++){
		if (arg[i] <= 31 && arg[i] != 9 ){
			sendRPL(fd, "irc.local", "412", clients[fd].getNickname().c_str(),
					":No text to send", NULL);
			throw std::runtime_error(info(std::string("Client " + to_string(fd) + " tried to create a wrong Channel").c_str()));
		}
	}
}