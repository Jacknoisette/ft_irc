/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_checker.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdhallen <jdhallen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/19 14:22:36 by jdhallen          #+#    #+#             */
/*   Updated: 2025/07/02 16:54:34 by jdhallen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Global.hpp"
#include "Server.hpp"

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

void Server::checkAuth(int fd, const std::vector<std::vector<std::string> >& cmdGroupIt)
{
	if (clients[fd].getAuthenticated())
		return ;
	for (std::vector<std::vector<std::string> >::const_iterator cmdIt = cmdGroupIt.begin(); cmdIt != cmdGroupIt.end(); cmdIt++)
	{
		if ((*cmdIt)[0] == "PASS" && cmdIt->size() >= 2 && (*cmdIt)[1] == password)
			clients[fd].setPasswordMatch(true);
		else if ((*cmdIt)[0] == "NICK" && cmdIt->size() >= 2 && clients[fd].getPasswordMatch())
		{
			bool nickUsed = false;
			if (clients.find(fd) != clients.end() && clients[fd].getNickname() == (*cmdIt)[1])
				nickUsed = true;
			if (!nickUsed)
				clients[fd].setNickname((*cmdIt)[1]);
			else
				sendRPL(fd, "Nickname already used", " : ", (*cmdIt)[1].c_str(), NULL);
		}
		else if ((*cmdIt)[0] == "USER" && (*cmdIt).size() >= 5 && clients[fd].getNickname() != "")
		{
			clients[fd].setUsername((*cmdIt)[1]);
			clients[fd].setHostname((*cmdIt)[2]);
		}
		else if ((*cmdIt)[0] == "QUIT" && cmdIt->size() >= 1)
			quit(fd, *cmdIt);
		else if (DEBUG)
			std::cerr << "command order not respected\n";
	}
	if (clients[fd].getUsername() != "")
	{
		sendRPL(fd, "irc.local", "001", clients[fd].getNickname().c_str()
		        , std::string("Welcome to IRC " + clients[fd].getNickname()).c_str(), NULL);
		sendRPL(fd, "irc.local", "002", clients[fd].getNickname().c_str()
		        , "Your host is irc.local version 1.0", NULL);
		sendRPL(fd, "irc.local", "003", clients[fd].getNickname().c_str()
		        , "Server creation in June 2025", NULL);
		sendRPL(fd, "irc.local", "004", clients[fd].getNickname().c_str()
		        , "irc.local", "1.0", "operator autorised", "Channel may contain invite mod or password", NULL);
		clients[fd].setAuthenticated(true);
	}
}

// void Server::check_Auth(int fd, const std::vector<std::vector<std::string> > &cmd_group)
// {
// 	if (!clients[fd].getAuthenticated())
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
// 					clients[fd].setNickname((*cmd)[1]);
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
// 				clients[fd].setUsername((*cmd)[1]);
// 				clients[fd].setHostname((*cmd)[2]);
// 			}
// 			if ((*cmd)[0] == "QUIT" && (*cmd).size() >= 1)
// 				quit(fd, (*cmd));
// 		}
// 		if (clients[fd].getNickname() != "" && clients[fd].getUsername() != ""
// 		     && clients[fd].getHostname() != "")
// 		{
// 			if (DEBUG)
// 				std::cout << "Sending welcome messages to client " << fd << std::endl;
// 			sendRPL(fd, "irc.local", "001", clients[fd].getNickname().c_str(), std::string("Welcome to IRC " + clients[fd].getNickname()).c_str(), NULL);
// 			sendRPL(fd, "irc.local", "002", clients[fd].getNickname().c_str(), "Your host is irc.local version 1.0", NULL);
// 			sendRPL(fd, "irc.local", "003", clients[fd].getNickname().c_str(), "Server creation in June 2025", NULL);
// 			sendRPL(fd, "irc.local", "004", clients[fd].getNickname().c_str(), "irc.local", "1.0", "operator autorised", "Channel may contain invite mod or password", NULL);
// 			clients[fd].setAuthenticated(true);
// 			if (DEBUG)
// 			{
//       	std::cout << "nickname : " << clients[fd].getNickname() << std::endl;
// 				std::cout << "username : " << clients[fd].getUsername() << std::endl;
// 				std::cout << "hostname : " << clients[fd].getHostname() << std::endl;  
//   		}
// 		}
// 	}
// }
