/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdhallen <jdhallen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/13 10:20:01 by jdhallen          #+#    #+#             */
/*   Updated: 2025/06/13 11:14:50 by jdhallen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Channel.hpp"

Channel::Channel()
	: name("#channel"), client_list(){
	std::cout << "New channel " << name << " created" << std::endl;
}

Channel::Channel(std::string _name)
	: name(_name), client_list(){
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
		this->client_list = src.client_list;
	}
	return *this;
}

const std::string	Channel::getName(void) const{
	return name;
}

const std::vector<Client>	Channel::getClients(void) const{
	return client_list;
}