/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_debug.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdhallen <jdhallen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/23 16:02:28 by jdhallen          #+#    #+#             */
/*   Updated: 2025/06/25 14:06:37 by jdhallen         ###   ########.fr       */
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

void Server::command_debug(int client_fd, const std::vector<std::vector<std::string> > &cmd_group){
    if (DEBUG){	
		std::cout << PURPLE << "Cmd arrival from client fd " << client_fd << " :" << std::endl;
		for (size_t group = 0; group < cmd_group.size(); group++)
		{
			std::cout << std::flush << "Cmd " << group << " :";
			for (size_t cmd = 0; cmd < cmd_group[group].size(); cmd++){
				std::cout << std::flush << cmd_group[group][cmd] << " ";
			}	
			std::cout << std::endl;
		}
		std::cout << RESET << std::endl;
	}
}