/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdhallen <jdhallen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/13 10:20:01 by jdhallen          #+#    #+#             */
/*   Updated: 2025/06/23 16:57:17 by jdhallen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Channel.hpp"

Channel::Channel()
	: name("#channel"), clients_list(){
	std::cout << "New channel " << name << " created" << std::endl;
}

Channel::Channel(std::string _name)
	: name(_name), clients_list(){
	std::cout << "New channel " << name << " created" << std::endl;
}

Channel::~Channel(){
	
}

Channel::Channel(const Channel &cpy){
	*this = cpy;
}

Channel Channel::operator=(const Channel &src){
	if (this != &src)
	{
		this->name = src.name;
		this->clients_list = src.clients_list;
	}
	return *this;
}

const std::string	Channel::getName(void) const{
	return name;
}

const std::map<int, Client>	Channel::getClients(void) const{
	return clients_list;
}

void	Channel::addClient(int client_fd, Client new_client){
	clients_list[client_fd] = new_client;
}

void	Channel::removeClient(std::string _client_name){
	for (std::map<int, Client>::iterator it = clients_list.begin(); it != clients_list.end(); it++){
		if (it->second.getNickname() == _client_name){
			clients_list.erase(it);
			break ;
		}
	}
}

void	Channel::removeClient(int fd){
	for (std::map<int, Client>::iterator it = clients_list.begin(); it != clients_list.end(); it++){
		if (it->second.getClientfd() == fd){
			clients_list.erase(it);
			break ;
		}
	}
}