/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_utils.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdhallen <jdhallen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/02 11:22:34 by jdhallen          #+#    #+#             */
/*   Updated: 2025/07/04 16:04:21 by jdhallen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Global.hpp"

std::vector<std::string> parsing_channel(std::string arg)
{
	std::vector<std::string> channel_list;
	size_t start = 0;
	while (start < arg.size())
	{
		size_t pos = arg.find(',', start);
		if (pos == std::string::npos)
		{
			channel_list.push_back(arg.substr(start));
			break;
		}
		if (pos > start)
			channel_list.push_back(arg.substr(start, pos - start));
		start = pos + 1;
	}
	return channel_list;
}

std::vector<std::string> Server::check_channel_name(int fd, std::string arg, std::string cmd_name)
{
	if (arg.size() > 50)
	{
		sendRPL(fd, "irc.local", "479", clients[fd]->getNickname().c_str(),
				arg.c_str(), ":Channel name is too long", NULL);
		throw std::runtime_error(info(std::string("Client " + to_string(fd) + " tried to create a wrong Channel").c_str()));
	}
	if (arg.size() <= 1)
	{
		sendRPL(fd, "irc.local", "461", clients[fd]->getNickname().c_str(),
				cmd_name.c_str(), ":Not enough parameters", NULL);
		throw std::runtime_error(info(std::string("Client " + to_string(fd) + " tried to create a wrong Channel").c_str()));
	}
	if (arg[0] != '#')
	{
		sendRPL(fd, "irc.local", "476", clients[fd]->getNickname().c_str(),
				arg.c_str(), ":Bad Channel Mask", NULL);
		throw std::runtime_error(info(std::string("Client " + to_string(fd) + " tried to create a wrong Channel").c_str()));
	}
	for (size_t i = 0; i < arg.size(); i++)
	{
		if (arg[i] <= 32 || arg[i] == ':')
		{
			sendRPL(fd, "irc.local", "479", clients[fd]->getNickname().c_str(),
					arg.c_str(), ":Channel name is invalid", NULL);
			throw std::runtime_error(info(std::string("Client " + to_string(fd) + " tried to create a wrong Channel").c_str()));
		}
	}
	std::vector<std::string> channels = parsing_channel(arg);
	for (size_t i = 0; i < channels.size(); i++)
	{
		if (channels[i][0] != '#')
		{
			sendRPL(fd, "irc.local", "476", clients[fd]->getNickname().c_str(),
					arg.c_str(), ":Bad Channel Mask", NULL);
			throw std::runtime_error(info(std::string("Client " + to_string(fd) + " tried to create a wrong Channel").c_str()));
		}
	}
	return channels;
}

void Server::client_name_parsing(int fd, std::string arg)
{
	if (arg.size() > 50)
	{
		sendRPL(fd, "irc.local", "432",
				arg.c_str(), ":Erroneous nickname", NULL);
		throw std::runtime_error(info(std::string("Client " + to_string(fd) + " tried to create a wrong Channel").c_str()));
	}
	for (size_t i = 0; i < arg.size(); i++)
	{
		if (arg[i] <= 32 || arg[i] == ':')
		{
			sendRPL(fd, "irc.local", "432",
					arg.c_str(), ":Erroneous nickname", NULL);
			throw std::runtime_error(info(std::string("Client " + to_string(fd) + " tried to create a wrong Channel").c_str()));
		}
	}
	return;
}

std::vector<std::string> Server::check_client_name(int fd, std::string arg)
{
	try
	{
		client_name_parsing(fd, arg);
	}
	catch (std::runtime_error &e)
	{
		throw e;
	}
	std::vector<std::string> clients = parsing_channel(arg);
	return clients;
}

std::vector<std::string> Server::check_key_string(int fd, std::string arg)
{
	if (arg.size() > 23)
	{
		sendRPL(fd, "irc.local", "475", clients[fd]->getNickname().c_str(),
				arg.c_str(), ":Cannot join channel (+k)", NULL);
		throw std::runtime_error(info(std::string("Client " + to_string(fd) + " tried to create a wrong Channel").c_str()));
	}
	for (size_t i = 0; i < arg.size(); i++)
	{
		if (arg[i] <= 32 || arg[i] == ':')
		{
			sendRPL(fd, "irc.local", "475", clients[fd]->getNickname().c_str(),
					arg.c_str(), ":Cannot join channel (+k)", NULL);
			throw std::runtime_error(info(std::string("Client " + to_string(fd) + " tried to create a wrong Channel").c_str()));
		}
	}
	return parsing_channel(arg);
}

void Server::ValidateMsgContent(int fd, std::string arg)
{
	if (arg.size() > 510)
	{
		sendRPL(fd, "irc.local", "417", clients[fd]->getNickname().c_str(),
				":Input line was too long", NULL);
		throw std::runtime_error(info(std::string("Client " + to_string(fd) + " tried to create a wrong Channel").c_str()));
	}
	if (arg.size() <= 1)
	{
		sendRPL(fd, "irc.local", "412", clients[fd]->getNickname().c_str(),
				":No text to send", NULL);
		throw std::runtime_error(info(std::string("Client " + to_string(fd) + " tried to create a wrong Channel").c_str()));
	}
	if (arg[0] != ':')
	{
		sendRPL(fd, "irc.local", "412", clients[fd]->getNickname().c_str(),
				":No text to send", NULL);
		throw std::runtime_error(info(std::string("Client " + to_string(fd) + " tried to create a wrong Msg").c_str()));
	}
	for (size_t i = 0; i < arg.size(); i++)
	{
		if (arg[i] <= 31 && arg[i] != 9)
		{
			sendRPL(fd, "irc.local", "412", clients[fd]->getNickname().c_str(),
					":No text to send", NULL);
			throw std::runtime_error(info(std::string("Client " + to_string(fd) + " tried to create a wrong Channel").c_str()));
		}
	}
}

void Server::sendWelcomeChannelMsg(const Client *client, std::string channel_name)
{
	if (!channels.at(channel_name)->getTopic().empty())
	{
		sendRPL(client->getClientfd(), "irc.local", "332", client->getNickname().c_str(),
				channels.at(channel_name)->getOGName().c_str(), std::string(":" + channels.at(channel_name)->getTopic()).c_str(), NULL);
	}
	else
	{
		sendRPL(client->getClientfd(), "irc.local", "331", client->getNickname().c_str(),
				channels.at(channel_name)->getOGName().c_str(), ":No topic is set", NULL);
	}
	sendRPL(client->getClientfd(), "irc.local", "324", client->getNickname().c_str(),
			channels.at(channel_name)->getOGName().c_str(), channels.at(channel_name)->getModeListAndKey().c_str(), NULL);
	sendRPL(client->getClientfd(), "irc.local", "353", client->getNickname().c_str(),
			"=", channels.at(channel_name)->getOGName().c_str(), channels.at(channel_name)->getListClientByType().c_str(), NULL);
	sendRPL(client->getClientfd(), "irc.local", "366", client->getNickname().c_str(),
			channels.at(channel_name)->getOGName().c_str(), ":End of /NAMES list.", NULL);
	std::string rpl = std::string(":" + client->getNickname() + "!" +
								  client->getUsername() + "@" + client->getHostname() + " JOIN " +
								  channels.at(channel_name)->getOGName() + "\n");
	sendRPL_Channel(true, client->getClientfd(), channel_name, rpl);
}

void Server::sendRPL_Channel(bool self_display, int fd, std::string channel_name, std::string msg)
{
	for (std::map<int, std::pair<Client *, bool> >::const_iterator client = channels.at(channel_name)->getClients().begin(); client != channels.at(channel_name)->getClients().end(); client++)
	{
		if (!self_display)
		{
			if (client->first != fd)
				sendToClient(client->first, msg);
		}
		else
			sendToClient(client->first, msg);
	}
	if (DEBUG)
		std::cout << std::flush << info(std::string("A message is send in channel ") + channel_name + std::string(" : ")) << WHITE << msg;
}

// Enter fd and any const char * to create the rpl msg
void Server::sendRPL(int fd, ...)
{
	va_list args;
	va_start(args, fd);
	std::string rpl = ":";
	const char *msg;
	while ((msg = va_arg(args, const char *)) != NULL)
		rpl += msg + std::string(" ");
	rpl += "\r\n";
	sendToClient(fd, rpl);
	if (DEBUG)
		std::cout << std::flush << WHITE << rpl;
	va_end(args);
}

void Server::sendToClient(int fd, const std::string &msg)
{
	if (clients[fd]->getSendBuffer().empty())
	{
		ssize_t sent = send(fd, msg.c_str(), msg.size(), 0);
		if (sent < 0)
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK)
			{
				clients[fd]->getSendBuffer() += msg;
				enablePOLLOUT(fd);
			}
		}
		else if ((size_t)sent < msg.size())
		{
			clients[fd]->getSendBuffer() += msg.substr(sent);
			enablePOLLOUT(fd);
		}
	}
	else
	{
		clients[fd]->getSendBuffer() += msg;
		enablePOLLOUT(fd);
	}
}

void Server::flushSendBuffer(int fd)
{
	std::string &buf = clients[fd]->getSendBuffer();
	if (buf.empty())
		return;
	ssize_t sent = send(fd, buf.c_str(), buf.size(), 0);
	if (sent > 0)
	{
		buf = buf.substr(sent);
		if (buf.empty())
			disablePOLLOUT(fd);
	}
}

void Server::enablePOLLOUT(int fd)
{
	if (!clients[fd]->getSendBuffer().empty())
		pollfds[fd].events |= POLLOUT;
	else
		pollfds[fd].events &= ~POLLOUT;
}

void Server::disablePOLLOUT(int fd)
{
	pollfds[fd].events &= ~POLLOUT;
}