/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdhallen <jdhallen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/11 17:09:16 by jdhallen          #+#    #+#             */
/*   Updated: 2025/06/23 12:54:05 by jdhallen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Client.hpp"

Client::Client()
	: client_fd(-1), nickname(""), username(""), hostname(""), authenticated(false),
		is_in_channel(false), channel(""){
			
}

Client::Client(int _client_fd)
	: client_fd(_client_fd), nickname(""), username(""), hostname(""), authenticated(false),
		is_in_channel(false), channel(""){
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
		this->is_in_channel = src.is_in_channel;
		this->channel = src.channel;
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

bool				Client::getIsInChannel(void) const{
	return is_in_channel;
}

const std::string	Client::getChannel(void) const{
	return channel;
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

void Client::setIsInChannel(bool _is_in_channel){
	is_in_channel = _is_in_channel;
}

void Client::setChannel(std::string _channel){
	channel = _channel;
}
