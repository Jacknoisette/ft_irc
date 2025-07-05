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
	command_debug(client_fd, cmdGroup, "client_command");
	checkAuth(client_fd, cmdGroup);
	if (!clients[client_fd]->getAuthenticated())
		return ;
	command_debug(client_fd, cmdGroup, "client_command, before check_BaseCmd");
	check_BaseCmd(client_fd, cmdGroup);
}

void Server::check_BaseCmd(int fd, const std::vector<std::vector<std::string> > &cmd_group){
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
	for (std::vector<std::vector<std::string> >::iterator cmdIt = cmd_group.begin(); 
		 cmdIt != cmd_group.end(); )
	{
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
	}
}

// void Server::check_Auth(int fd, const std::vector<std::vector<std::string> > &cmd_group)
// {
// 	if (!clients[fd]->getAuthenticated())
// 	{
// 		for (std::vector<std::vector<std::string> >::const_iterator cmd = cmd_group.begin(); cmd != cmd_group.end(); cmd++)
// 		{
// 			if ((*cmd)[0] == "NICK" && (*cmd).size() >= 2)
// 			{
// 				bool nickUsed = false;
// 				for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it)
// 				{
// 					if (it->first != fd && it->second.getNickname() == (*cmd)[1])
// 					{
// 						nickUsed = true;
// 						break;
// 					}
// 				}
// 				if (!nickUsed)
// 				{
// 					clients[fd]->setNickname((*cmd)[1]);
// 					if (DEBUG)
// 						std::cerr << "Nickname set to:" << (*cmd)[1] << std::endl;
// 				}
// 				else
// 					sendRPL(fd, "Nickname already used", " : ",(*cmd)[1].c_str(), NULL);
// 			}
// 			if ((*cmd)[0] == "USER" && (*cmd).size() >= 5)
// 			{
// 				if (DEBUG)
// 					std::cerr << "cmd is: " << (*cmd)[1] << ", args are: " << (*cmd)[2] << std::endl;
// 				clients[fd]->setUsername((*cmd)[1]);
// 				clients[fd]->setHostname((*cmd)[2]);
// 			}
// 			if ((*cmd)[0] == "QUIT" && (*cmd).size() >= 1)
// 				quit(fd, (*cmd));
// 		}
// 		if (clients[fd]->getNickname() != "" && clients[fd]->getUsername() != ""
// 		     && clients[fd]->getHostname() != "")
// 		{
// 			if (DEBUG)
// 				std::cout << "Sending welcome messages to client " << fd << std::endl;
// 			sendRPL(fd, "irc.local", "001", clients[fd]->getNickname().c_str(), std::string("Welcome to IRC " + clients[fd]->getNickname()).c_str(), NULL);
// 			sendRPL(fd, "irc.local", "002", clients[fd]->getNickname().c_str(), "Your host is irc.local version 1.0", NULL);
// 			sendRPL(fd, "irc.local", "003", clients[fd]->getNickname().c_str(), "Server creation in June 2025", NULL);
// 			sendRPL(fd, "irc.local", "004", clients[fd]->getNickname().c_str(), "irc.local", "1.0", "operator autorised", "Channel may contain invite mod or password", NULL);
// 			clients[fd]->setAuthenticated(true);
// 			if (DEBUG)
// 			{
//       	std::cout << "nickname : " << clients[fd]->getNickname() << std::endl;
// 				std::cout << "username : " << clients[fd]->getUsername() << std::endl;
// 				std::cout << "hostname : " << clients[fd]->getHostname() << std::endl;  
//   		}
// 		}
// 	}
// }
