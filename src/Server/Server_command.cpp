/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_command.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdhallen <jdhallen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/23 17:08:49 by jdhallen          #+#    #+#             */
/*   Updated: 2025/06/23 17:36:04 by jdhallen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Server.hpp"

void	Server::join(int fd, std::vector<std::string> arg){
	(void)fd;
	(void)arg;
	std::cout << info("Someone wants to join") << std::endl;
}

void	Server::quit(int fd, std::vector<std::string> arg){
	(void)fd;
	(void)arg;
	std::cout << info("Someone wants to quit") << std::endl;
}

void	Server::ping(int fd, std::vector<std::string> arg){
	(void)fd;
	(void)arg;
	std::cout << info("Someone wants to ping") << std::endl;
}