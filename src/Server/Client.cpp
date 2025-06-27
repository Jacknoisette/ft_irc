/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdhallen <jdhallen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/11 17:09:16 by jdhallen          #+#    #+#             */
/*   Updated: 2025/06/27 12:20:20 by jdhallen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client()
	: client_fd(-1), nickname(""), username(""), hostname(""), authenticated(false),
		in_channels(){
			
}

Client::Client(int _client_fd)
	: client_fd(_client_fd), nickname(""), username(""), hostname(""), authenticated(false),
		in_channels(){
}

Client::~Client(){
}

Client::Client(const Client &cpy){
	*this = cpy;
}

Client &Client::operator=(const Client &src){
	if (this != &src)
	{
		this->client_fd = src.client_fd;
		this->nickname = src.nickname;
		this->username = src.username;
		this->hostname = src.hostname;
		this->authenticated = src.authenticated;
		this->in_channels = src.in_channels;
	}
	return *this;
}

int	Client::getClientfd(void) const{
	return client_fd;
}

const std::string	Client::getNickname(void) const{
	return nickname;
}

const std::string	Client::getUsername(void) const{
	return username;
}

const std::string	Client::getHostname(void) const{
	return hostname;
}

bool				Client::getAuthenticated(void) const{
	return authenticated;
}

std::map<std::string, Channel>	&Client::getChannels(void){
	return in_channels;
}

void Client::setClientfd(int _client_fd){
	client_fd = _client_fd;
}

void Client::setNickname(std::string _nickname){
	nickname = _nickname;
}

void Client::setUsername(std::string _username){
	username = _username;
}

void Client::setHostname(std::string _hostname){
	hostname = _hostname;
}

void Client::setAuthenticated(bool _authenticated){
	authenticated = _authenticated;
}

void Client::addChannel(Channel &_new_channel){
	in_channels[_new_channel.getName()] = _new_channel;
}

void	Client::removeChannel(std::string _channel_name){
	if (in_channels.find(_channel_name) != in_channels.end())
		in_channels.erase(_channel_name);
}
