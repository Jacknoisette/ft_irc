/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdhallen <jdhallen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/13 10:20:01 by jdhallen          #+#    #+#             */
/*   Updated: 2025/07/04 16:04:19 by jdhallen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

Channel::Channel()
	: og_name("#channel"), name("#channel"), topic("")
	, clientsListFd()
	, clientsListStr()
	, clientInvite()
	, isOnInvite(false)
	, isTopicRestricted(true)
	, password("")
	, channelLimit(0)
{
}

Channel::Channel(std::string _name)
	: og_name(_name), name(toLowerString(_name)), topic("")
	, clientsListFd()
	, clientsListStr()
	, clientInvite()
	, isOnInvite(false)
	, isTopicRestricted(true)
	, password("")
	, channelLimit(0)
{
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
		this->topic = src.topic;
		this->clientsListFd = src.clientsListFd;
		this->clientsListStr = src.clientsListStr;
		this->isOnInvite = src.isOnInvite;
		this->isTopicRestricted = src.isTopicRestricted;
		this->password = src.password;
		this->channelLimit = src.channelLimit;
		this->clientInvite = src.clientInvite;
	}
	return *this;
}

const std::string	&Channel::getOGName(void) const{
	return og_name;
}

const std::string	&Channel::getName(void) const{
	return name;
}

std::map<int, std::pair<Client*, bool> >	&Channel::getClientsListFd(void){
	return clientsListFd;
}

bool Channel::getIsOnInvite(void) const
{
	return (isOnInvite);
}

void Channel::setIsOnInvite(bool isOnInvite)
{
	this->isOnInvite = isOnInvite;
}

bool Channel::getIsTopicRestricted(void) const
{
	return (isTopicRestricted);
}

void Channel::setIsTopicRestricted(bool isRestricted)
{
	isTopicRestricted = isRestricted;
}

const std::string& Channel::getTopic(void) const
{
	return (topic);
}

void Channel::setTopic(const std::string& topicName)
{
	topic = topicName;
}

const std::string& Channel::getPassword(void) const
{
	return (password);
}

void Channel::setPassword(const std::string& password)
{
	this->password = password;
}

void Channel::setChannelLimit(size_t channelLimit)
{
	this->channelLimit = channelLimit;
}

size_t Channel::getChannelLimit() const
{
	return (channelLimit);
}

std::map<std::string, std::pair<Client*, bool> >& Channel::getClientsListStr(void)
{
	return (clientsListStr);
}

std::vector<int>& Channel::getClientInvite(void)
{
	return (clientInvite);
}

void	Channel::addClient(int client_fd, Client *new_client, bool is_op){
	if (clientsListFd.find(client_fd) == clientsListFd.end()){
		std::pair<Client*, bool> client_def;
		client_def.first = new_client;
		client_def.second = is_op;
		clientsListFd[client_fd] = client_def;
	}
	if (clientsListStr.find(new_client->getNickname()) == clientsListStr.end())
	{
		std::pair<Client*, bool> clientPair;
		clientPair.first = new_client;
		clientPair.second = is_op;
		clientsListStr[new_client->getNickname()] = clientPair;
	}
}

void	Channel::removeClient(int fd)
{
	std::string clientName = clientsListFd.find(fd)->second.first->getNickname();
	if (clientsListStr.find(clientName) != clientsListStr.end())
		clientsListStr.erase(clientName);
	if (clientsListFd.find(fd) != clientsListFd.end())
		clientsListFd.erase(fd);
	if (DEBUG)
		std::cout << info(std::string("A client with fd " + to_string(fd) + ", leaved the channel " + name +  "!").c_str())<< std::endl;
}

const std::string Channel::getListClientByType(void) const {
	std::string list = ":";
	std::vector<std::string> nicks;

	for (std::map<int, std::pair<Client*, bool> >::const_iterator it = clientsListFd.begin(); it != clientsListFd.end(); ++it) {
		if (it->second.second)
			nicks.push_back("@" + it->second.first->getNickname());
	}
	for (std::map<int, std::pair<Client*, bool> >::const_iterator it = clientsListFd.begin(); it != clientsListFd.end(); ++it) {
		if (!it->second.second)
			nicks.push_back(it->second.first->getNickname());
	}

	for (size_t i = 0; i < nicks.size(); ++i) {
		if (i != 0)
			list += " ";
		list += nicks[i];
	}

	return list;
}

const std::string Channel::getModeListAndKey(void) const{
	std::string list = "+";
	if (isOnInvite)
		list += "i";
	if (isTopicRestricted)
		list += "t";
	if (password != "")
		list += "k";
	if (channelLimit > 0)
		list += "l";
	if (list != "+" && channelLimit > 0)
		list += " " + to_string(channelLimit);
	if (list != "+" && password != "")
		list += " " + password;
	std::cout << list << std::endl;
	return list;
}
