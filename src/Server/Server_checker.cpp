/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_checker.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdhallen <jdhallen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/19 14:22:36 by jdhallen          #+#    #+#             */
/*   Updated: 2025/07/04 18:38:17 by jdhallen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Global.hpp"
#include "Server.hpp"

void Server::client_command(int client_fd, std::vector<std::vector<std::string> > &cmdGroup)
{
	if (DEBUG)
		command_debug(client_fd, cmdGroup, "client_command");
	checkAuth(client_fd, cmdGroup);
	if (!clients[client_fd]->getAuthenticated())
		return ;
	// if (DEBUG)
	// 	command_debug(client_fd, cmdGroup, "client_command, before check_BaseCmd");
	check_BaseCmd(client_fd, cmdGroup);
}

void Server::check_BaseCmd(int fd, std::vector<std::vector<std::string> > &cmd_group){
	for (std::vector<std::vector<std::string> >::const_iterator cmd = cmd_group.begin(); cmd != cmd_group.end(); cmd++){
		if (cmd->empty())
			continue ;
		std::map<std::string, void (Server::*)(int, std::vector<std::string>)>::iterator it = cmd_func_list.find((*cmd)[0]);
		if (it != cmd_func_list.end())
			(this->*(it->second))(fd, *cmd);
		else
			sendRPL(fd, "irc.local", "421", clients[fd]->getNickname().c_str()
			        , (*cmd)[0].c_str(), ":Unknown command", NULL);
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

void Server::checkAuth(int fd, std::vector<std::vector<std::string> >& cmd_group)
{
	if (clients[fd]->getAuthenticated())
		return;
	else if (cmd_group.empty() || cmd_group.at(0).empty())
		return;
	for (std::vector<std::vector<std::string> >::iterator cmdIt = cmd_group.begin(); 
		 cmdIt != cmd_group.end(); )
	{
		if (cmdIt->empty())
			return ;
		if (!clients[fd]->getUsername().empty() && !clients[fd]->getNormalizedNick().empty())
			break;
		if (password.empty())
			clients[fd]->setPasswordMatch(true);
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
				clients[fd]->setPasswordMatch(true);
			else
			{
				sendRPL(fd, "irc.local", "464", (*cmdIt)[1].c_str(), ":Password incorrect", NULL);
				return;
			}
		}
		else if ((*cmdIt)[0] == "NICK")
			nick(fd, *cmdIt);
		else if ((*cmdIt)[0] == "USER")
		{
			if (cmdIt->size() < 5 || (*cmdIt)[4][0] != ':')
			{
				sendRPL(fd, "irc.local", "461", (*cmdIt)[0].c_str(), ":Not enough parameters", NULL);
				return;
			}
			if (!clients[fd]->getPasswordMatch() && !password.empty())
			{
				sendRPL(fd, "irc.local", "464", "", ":Password incorrect", NULL);
				return;
			}
			clients[fd]->setUsername((*cmdIt)[1]);
			clients[fd]->setHostname((*cmdIt)[2]);
		}
		else if ((*cmdIt)[0] == "QUIT" && cmdIt->size() >= 1){
			quit(fd, *cmdIt);
		}
		else
		{
			sendRPL(fd, "irc.local", "451", "*", "You have not registered", NULL);
		}
		cmdIt = cmd_group.erase(cmdIt);
	}
	if (!clients[fd]->getUsername().empty() && !clients[fd]->getNormalizedNick().empty())
	{
		sendRPL(fd, "irc.local", "001", clients[fd]->getNickname().c_str(),
				std::string("Welcome to the Internet Relay Network " + clients[fd]->getNickname()).c_str(), NULL);
		sendRPL(fd, "irc.local", "002", clients[fd]->getNickname().c_str(),
				"Your host is irc.local, running version 1.0", NULL);
		sendRPL(fd, "irc.local", "003", clients[fd]->getNickname().c_str(),
				"This server was created Mon Jun 1 2025", NULL);
		sendRPL(fd, "irc.local", "004", clients[fd]->getNickname().c_str(),
				"irc.local 1.0 o o", NULL);
		clients[fd]->setAuthenticated(true);
		if (DEBUG)
			ultimateDebug();
	}
}

