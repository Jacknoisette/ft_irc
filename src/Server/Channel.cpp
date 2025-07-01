/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdhallen <jdhallen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/13 10:20:01 by jdhallen          #+#    #+#             */
/*   Updated: 2025/06/30 17:53:25 by jdhallen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

Channel::Channel()
	: og_name("#channel"), name("#channel"), clients_list(){
}

Channel::Channel(std::string _name)
	: og_name(_name), name(toLowerString(_name)), clients_list(){
}

Channel::~Channel(){
	
}

Channel::Channel(const Channel &cpy){
	*this = cpy;
}

Channel &Channel::operator=(const Channel &src){
	if (this != &src)
	{
		this->og_name = src.og_name;
		this->name = src.name;
		this->clients_list = src.clients_list;
	}
	return *this;
}

const std::string	Channel::getOGName(void) const{
	return og_name;
}

const std::string	Channel::getName(void) const{
	return name;
}

const std::map<int, std::pair<Client, bool> >	Channel::getClients(void) const{
	return clients_list;
}

void	Channel::addClient(int client_fd, Client new_client, bool is_op){
	if (clients_list.find(client_fd) == clients_list.end()){
		std::pair<Client, bool> client_def;
		client_def.first = new_client;
		client_def.second = is_op;
		clients_list[client_fd] = client_def;
	}	
}

void	Channel::removeClient(int fd){
	if (clients_list.find(fd) != clients_list.end())
		clients_list.erase(fd);
	if (DEBUG)
		std::cout << info(std::string("A client with fd " + to_string(fd) + ", leaved the channel " + name +  "!").c_str())<< std::endl;
}

void	Channel::sendRPL_Channel(bool self_display, int fd, std::string msg) {
	(void)self_display;
	for (std::map<int, std::pair<Client, bool> >::iterator client = clients_list.begin(); client != clients_list.end(); client++){
		if (!self_display){
			if (client->first != fd)
				send(client->first, msg.c_str(), msg.size(), 0);
		}
		else
			send(client->first, msg.c_str(), msg.size(), 0);
	}
	if (DEBUG)
		std::cout << std::flush << info(std::string("A message is send in channel ") + name + std::string(" : ")) << WHITE << msg;
}