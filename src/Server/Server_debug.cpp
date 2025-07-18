/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_debug.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdhallen <jdhallen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/23 16:02:28 by jdhallen          #+#    #+#             */
/*   Updated: 2025/07/04 16:04:15 by jdhallen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Server.hpp"

void	Server::socket_debug(void){
	std::cout << "Socket events status:" << std::endl;
	for (size_t i = 0; i < pollfds.size(); ++i) {
		std::cout << std::flush << "Socket " << pollfds[i].fd << " revents=" << pollfds[i].revents;
		if (pollfds[i].revents & POLLIN) std::cout << std::flush << " (POLLIN)";
		if (pollfds[i].revents & POLLOUT) std::cout << std::flush << " (POLLOUT)";
		if (pollfds[i].revents & POLLERR) std::cout << std::flush << " (POLLERR)";
		if (pollfds[i].revents & POLLHUP) std::cout << std::flush << " (POLLHUP)";
		if (pollfds[i].revents & POLLNVAL) std::cout << std::flush << " (POLLNVAL)";
		std::cout << std::endl;
	}
}

void Server::command_debug(int client_fd, const std::vector<std::vector<std::string> > &cmdGroup, const std::string& where)
{
	if (DEBUG)
	{
			std::cerr << "=== DEBUG COMMAND ===" << std::endl;
			std::cerr << "Client: " << clients[client_fd]->getNickname() << " (fd:" << client_fd << ")" << std::endl;
			std::cerr << "Called by: " << where << std::endl;
			std::cerr << "Number of commands: " << cmdGroup.size() << std::endl;

			for (size_t cmdIndex = 0; cmdIndex < cmdGroup.size(); cmdIndex++)
			{
				std::cerr << "--- Command " << cmdIndex << " ---" << std::endl;
				if (cmdGroup[cmdIndex].empty())
				{
					std::cerr << "  (empty command)" << std::endl;
					continue;
				}
				std::cerr << "  Command: " << cmdGroup[cmdIndex][0] << std::endl;
				for (size_t argIndex = 1; argIndex < cmdGroup[cmdIndex].size(); argIndex++)
					std::cerr << "  Arg " << (argIndex-1) << ": " << cmdGroup[cmdIndex][argIndex] << std::endl;
				std::cerr << "  Total args: " << (cmdGroup[cmdIndex].size() - 1) << std::endl;
			}
			std::cerr << "====================" << std::endl;
  }
}

void	Server::ultimateDebug(){
	if (!DEBUG)
		return;
	std::cout << RESET << "_________________________" << std::endl;
	{
		std::cout << BIG << ORANGE << "Channels :" << RESET << ORANGE << std::endl;
		for (std::map<std::string, Channel*>::iterator	channels_it = channels.begin(); channels_it != channels.end(); channels_it++){
			std::cout << std::flush << "|";
			std::cout << std::flush << "channel : " << channels_it->first << " | ";
			std::cout << std::flush << "adr : " << std::showbase << std::hex << reinterpret_cast<uintptr_t>(channels_it->second) << std::dec << " | ";
			std::cout << std::flush << "mode : +";
			if (channels_it->second->getIsOnInvite())
				std::cout << std::flush << "i";
			if (channels_it->second->getIsTopicRestricted())
				std::cout << std::flush << "t";
			if (channels_it->second->getPassword() != "")
				std::cout << std::flush << "k";
			if (channels_it->second->getChannelLimit() > 0)
				std::cout << std::flush << "l";
			std::cout << std::flush << " | ";
			std::cout << std::flush << "limit : ";
			if (channels_it->second->getChannelLimit() == 0)
				std::cout << std::flush << "None";
			else
				std::cout << std::flush << to_string(channels_it->second->getChannelLimit());
			std::cout << std::flush << " | pass : ";
			if (channels_it->second->getPassword().empty())
				std::cout << std::flush << "None";
			else
				std::cout << std::flush << channels_it->second->getPassword();
			std::cout << "\033[4m" << std::endl;
			
			for (std::map<int, std::pair<Client*, bool> >::iterator clients_it = channels_it->second->getClientsListFd().begin(); clients_it != channels_it->second->getClientsListFd().end(); clients_it++){
				if (clients_it->second.second)
					std::cout << std::flush << "@";
				std::cout << std::flush << "client : " << clients_it->second.first->getNickname();
				std::cout << std::flush << " fd : " << clients_it->first;
				std::cout << std::flush << " adr :" << std::showbase << std::hex << reinterpret_cast<uintptr_t>(clients_it->second.first) << std::dec;
				std::cout << std::flush << ",";
			}
			std::cout << RESET << ORANGE << std::endl;
		}
	}
	std::cout << std::endl;
	{
		std::cout << RESET << BIG << WHITE << "Client :" << RESET << WHITE << std::endl;
		for (std::map<int, Client*>::iterator	clients_it = clients.begin(); clients_it != clients.end(); clients_it++){
			std::cout << std::flush << "|";
			std::cout << std::flush << "fd : " << to_string(clients_it->first) << " | ";
			std::cout << std::flush << "nick : " << clients_it->second->getNickname() << " | ";
			std::cout << std::flush << "user : " << clients_it->second->getUsername() << " | ";
			std::cout << std::flush << "host : " << clients_it->second->getHostname() << " | ";
			std::cout << std::flush << "adr : " << std::showbase << std::hex << reinterpret_cast<uintptr_t>(clients_it->second) << std::dec << " | ";
			std::cout << "\033[4m" << std::endl;
			for (std::map<std::string, std::pair<Channel*, size_t> >::iterator channels_it = clients_it->second->getChannels().begin(); channels_it != clients_it->second->getChannels().end(); channels_it++){
				if (channels_it->first == clients_it->second->getCurrentChannel())
					std::cout << std::flush << "*";
				std::cout << std::flush << "channel : " << channels_it->first;
				std::cout << std::flush << " adr :" << std::showbase << std::hex << reinterpret_cast<uintptr_t>(channels_it->second.first) << std::dec;
				std::cout << std::flush << ",";
			}
			std::cout << RESET << WHITE << std::endl;
		}
	}
	std::cout << RESET << "_________________________" << std::endl;
}
