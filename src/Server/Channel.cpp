/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdhallen <jdhallen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/13 10:20:01 by jdhallen          #+#    #+#             */
/*   Updated: 2025/06/27 13:59:58 by jdhallen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Channel.hpp"

Channel::Channel()
	: name("#channel"), clients_list(){
}

Channel::Channel(std::string _name)
	: name(_name), clients_list(){
}

Channel::~Channel(){
	
}

Channel::Channel(const Channel &cpy){
	*this = cpy;
}

Channel &Channel::operator=(const Channel &src){
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

const std::map<int, std::pair<Client, bool> >	Channel::getClients(void) const{
	return clients_list;
}

void	Channel::addClient(int client_fd, Client new_client){
	std::pair<Client, bool> client_def;
	client_def.first = new_client;
	client_def.second = false;
	clients_list[client_fd] = client_def;
}

void	Channel::removeClient(int fd){
	if (clients_list.find(fd) != clients_list.end())
		clients_list.erase(fd);
	if (DEBUG)
		std::cout << info(std::string("A client with fd " + to_string(fd) + ", leaved the channel " + name +  "!").c_str())<< std::endl;
}