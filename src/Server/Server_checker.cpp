/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_checker.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdhallen <jdhallen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/19 14:22:36 by jdhallen          #+#    #+#             */
/*   Updated: 2025/07/04 13:34:20 by jdhallen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Global.hpp"
#include "Server.hpp"
#include <cctype>

void Server::client_command(int client_fd, const std::vector<std::vector<std::string> > &cmdGroup)
{
	int vecVecStrCpt = 0;
	int vecStrCpt = 0;
	if (DEBUG)
	{
		std::cerr << "client_command called\n";
		for (std::vector<std::vector<std::string> >::const_iterator vecVecStrIt = cmdGroup.begin(); vecVecStrIt != cmdGroup.end(); vecVecStrIt++)
		{
			std::cerr << "command: " << vecVecStrCpt << std::endl;
			for (std::vector<std::string>::const_iterator vecStrIt = vecVecStrIt->begin(); vecStrIt != vecVecStrIt->end(); vecStrIt++)
			{
				std::cerr << "arg: " << vecStrCpt << " " << *vecStrIt << std::endl;
				vecStrCpt++;
			}
			vecVecStrCpt++;
		}
	}
	checkAuth(client_fd, cmdGroup);
	if (!clients[client_fd].getAuthenticated())
		return ;
	check_BaseCmd(client_fd, cmdGroup);
}

void Server::check_BaseCmd(int fd, const std::vector<std::vector<std::string> > &cmd_group){
	for (std::vector<std::vector<std::string> >::const_iterator cmd = cmd_group.begin(); cmd != cmd_group.end(); cmd++){
		if (cmd->empty())
			continue ;
		std::map<std::string, void (Server::*)(int, std::vector<std::string>)>::iterator it = cmd_func_list.find((*cmd)[0]);
    if (it != cmd_func_list.end())
      (this->*(it->second))(fd, *cmd);
    else if (DEBUG)
    	std::cerr << "error: this command doesn't exist\n";
	}
}

bool Server::isValidNickname(const std::string& nick)
{
	if (nick.empty() || nick.size() > 9)
		return (false);
	if (isdigit(nick[0]) || nick[0] == '-' || nick[0] == '#')
		return (false);
	for (size_t i = 0; i < nick.size(); i++)
	{
		char c = nick.at(i);
		if (std::isalnum(c) || c == '_' || c == '-' || c == '[' ||
			c == ']' || c == '{' || c == '}' || c == '\\' || c == '|')
			continue;
		else
			return (false);
	}
	return (true);
}

void Server::checkAuth(int fd, const std::vector<std::vector<std::string> >& cmdGroupIt)
{
	if (clients[fd].getAuthenticated())
		return;
	for (std::vector<std::vector<std::string> >::const_iterator cmdIt = cmdGroupIt.begin(); 
		 cmdIt != cmdGroupIt.end(); cmdIt++)
	{
		if (password == "")
			clients[fd].setPasswordMatch(true);
		if ((*cmdIt)[0] == "PASS")
		{
			if (cmdIt->size() < 2)
			{
				sendRPL(fd, "irc.local", "461", (*cmdIt)[0].c_str(), ":Not enough parameters", NULL);
				return;
			}
			if (password.empty())
				break;
			if ((*cmdIt)[1] == password)
				clients[fd].setPasswordMatch(true);
			else
			{
				sendRPL(fd, "irc.local", "464", (*cmdIt)[1].c_str(), ":Password incorrect", NULL);
				quit(fd, *cmdIt);
				return;
			}
		}
		else if ((*cmdIt)[0] == "NICK")
			nick(fd, *cmdIt);
		else if ((*cmdIt)[0] == "USER")
		{
			if (cmdIt->size() < 5)
			{
				sendRPL(fd, "irc.local", "461", (*cmdIt)[0].c_str(), ":Not enough parameters", NULL);
				return;
			}
			if (!clients[fd].getPasswordMatch() && !password.empty())
		  {
				sendRPL(fd, "irc.local", "464", "", ":Password incorrect", NULL);
				return;
			}
			clients[fd].setUsername((*cmdIt)[1]);
			clients[fd].setHostname((*cmdIt)[2]);
		}
		else if ((*cmdIt)[0] == "QUIT" && cmdIt->size() >= 1)
			quit(fd, *cmdIt);
	}

	if (!clients[fd].getUsername().empty() && !clients[fd].getNormalizedNick().empty())
	{
		sendRPL(fd, "irc.local", "001", clients[fd].getNickname().c_str(),
				std::string("Welcome to the Internet Relay Network " + clients[fd].getNickname()).c_str(), NULL);
		sendRPL(fd, "irc.local", "002", clients[fd].getNickname().c_str(),
				"Your host is irc.local, running version 1.0", NULL);
		sendRPL(fd, "irc.local", "003", clients[fd].getNickname().c_str(),
				"This server was created Mon Jun 1 2025", NULL);
		sendRPL(fd, "irc.local", "004", clients[fd].getNickname().c_str(),
				"irc.local 1.0 o o", NULL);
		clients[fd].setAuthenticated(true);
	}
}

