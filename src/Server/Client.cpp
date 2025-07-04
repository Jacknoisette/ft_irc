/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdhallen <jdhallen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/11 17:09:16 by jdhallen          #+#    #+#             */
/*   Updated: 2025/07/04 13:29:09 by jdhallen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client()
	: client_fd(-1), nickname(""), normalizedNick(""), username(""), hostname(""), authenticated(false)
		,passwordMatch(false), in_channels(), sendBuffer(){
}

Client::Client(int _client_fd)
	: client_fd(_client_fd), nickname(""), normalizedNick(""), username(""), hostname(""), authenticated(false)
		,passwordMatch(false), in_channels(), sendBuffer(){
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
		this->normalizedNick = src.normalizedNick;
		this->username = src.username;
		this->hostname = src.hostname;
		this->authenticated = src.authenticated;
		this->passwordMatch = src.passwordMatch;
		this->in_channels = src.in_channels;
		this->sendBuffer = src.sendBuffer;
	}
	return *this;
}

int	Client::getClientfd(void) const{
	return client_fd;
}

const std::string	Client::getNickname(void) const{
	return nickname;
}

const std::string Client::getNormalizedNick(void) const
{
	return (normalizedNick);
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

std::map<std::string, std::pair<Channel, size_t> >	&Client::getChannels(void){
	return in_channels;
}

bool Client::getPasswordMatch(void) const
{
	return (passwordMatch);
}

void Client::setPasswordMatch(bool match)
{
	passwordMatch = match;
}

const std::string &Client::getCurrentChannel(void) const{
	return current_channel;
}

std::string &Client::getSendBuffer(void){
	return sendBuffer;
}

void Client::setClientfd(int _client_fd){
	client_fd = _client_fd;
}

void Client::setNickname(std::string _nickname){
	nickname = _nickname;
}

void Client::setNormalizedNick(const std::string& nickname)
{
	normalizedNick = nickname;
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

void Client::setCurrentChannel(std::string _current_channel){
	current_channel = _current_channel;
}

void Client::setSendBuffer(std::string _sendBuffer){
	sendBuffer = _sendBuffer;
}

void Client::addChannel(Channel &_new_channel){
	if (in_channels.find(_new_channel.getName()) == in_channels.end()){
		std::pair<Channel, size_t> channel_def;
		channel_def.first = _new_channel;
		size_t maxVal = findMaxVal();
		channel_def.second = maxVal + 1;
		in_channels[_new_channel.getName()] = channel_def;
	}
}

void	Client::removeChannel(std::string _channel_name){
	if (in_channels.find(_channel_name) != in_channels.end())
		in_channels.erase(_channel_name);
}

size_t	Client::findMaxVal(void){
	size_t maxVal = 0;
	for (std::map<std::string, std::pair<Channel, size_t> >::const_iterator it = in_channels.begin(); it != in_channels.end(); ++it) {
		if (it->second.second > maxVal) {
			maxVal = it->second.second;
		}
	}
	return maxVal;
}

void	Client::updateCurrentChannel(void){
	if (!getChannels().empty()) {
		std::map<std::string, std::pair<Channel, size_t> >& channels = getChannels();
		size_t maxVal = findMaxVal();
		for (std::map<std::string, std::pair<Channel, size_t> >::const_iterator it = channels.begin(); it != channels.end(); ++it) {
			if (it->second.second == maxVal) {
				setCurrentChannel(it->first);
				break;
			}
		}
	} else {
		setCurrentChannel("");
	}
}
