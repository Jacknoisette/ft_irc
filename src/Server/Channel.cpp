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
	, clients_list()
	, ClientMapOp()
	, isOnInvite(false)
	, isTopicRestricted(false)
	, password("")
	, channelLimit(0)
{
}

Channel::Channel(std::string _name)
	: og_name(_name), name(toLowerString(_name)), topic("")
	, clients_list()
	, ClientMapOp()
	, isOnInvite(false)
	, isTopicRestricted(false)
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
		this->clients_list = src.clients_list;
		this->ClientMapOp = src.ClientMapOp;
		this->isOnInvite = src.isOnInvite;
		this->isTopicRestricted = src.isTopicRestricted;
		this->password = src.password;
		this->channelLimit = src.channelLimit;
	}
	return *this;
}

const std::string	&Channel::getOGName(void) const{
	return og_name;
}

const std::string	&Channel::getName(void) const{
	return name;
}

std::map<int, std::pair<Client*, bool> >	&Channel::getClients(void){
	return clients_list;
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

std::map<std::string, std::pair<Client*, bool> >& Channel::getClientMapOp(void)
{
	return (ClientMapOp);
}

std::map<std::string, std::pair<Client*, bool> >& Channel::getClientMapInvite(void)
{
	return (ClientMapInvite);
}

void	Channel::addClient(int client_fd, Client *new_client, bool is_op){
	if (clients_list.find(client_fd) == clients_list.end()){
		std::pair<Client*, bool> client_def;
		client_def.first = new_client;
		client_def.second = is_op;
		clients_list[client_fd] = client_def;
	}
	if (ClientMapOp.find(new_client->getNickname()) == ClientMapOp.end())
	{
		std::pair<Client*, bool> clientPair;
		clientPair.first = new_client;
		clientPair.second = is_op;
		ClientMapOp[new_client->getNickname()] = clientPair;
	}
	if (ClientMapInvite.find(new_client->getNickname()) == ClientMapInvite.end())
	{
		std::pair<Client*, bool> clientPair;
		clientPair.first = new_client;
		ClientMapInvite[new_client->getNickname()] = clientPair;
	}
}

void	Channel::removeClient(int fd)
{
	std::string clientName = clients_list.find(fd)->second.first->getNickname();
	if (ClientMapOp.find(clientName) != ClientMapOp.end())
		ClientMapOp.erase(clientName);
	if (ClientMapInvite.find(clientName) != ClientMapInvite.end())
		ClientMapInvite.erase(clientName);
	if (clients_list.find(fd) != clients_list.end())
		clients_list.erase(fd);
	if (DEBUG)
		std::cout << info(std::string("A client with fd " + to_string(fd) + ", leaved the channel " + name +  "!").c_str())<< std::endl;
}

const std::string Channel::getListClientByType(void) const{
	std::string list = ":";
	std::string listop;
	std::string listclient;
	for (std::map<int, std::pair<Client*, bool> >::const_iterator client = clients_list.begin(); client != clients_list.end(); client++){
		std::string name = client->second.first->getNickname();
		if (client->second.second == true)
			listop += ((client != clients_list.begin()) ? "@" : " @") + name;
		else
			listclient += ((client != clients_list.begin()) ? "" : " ") + name;
	}
	list += listop + listclient;
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
