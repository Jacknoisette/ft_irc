/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdhallen <jdhallen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/11 17:09:16 by jdhallen          #+#    #+#             */
/*   Updated: 2025/06/13 15:44:29 by jdhallen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Server.hpp"

Server::Server()
	: port(6667), password(""), channels(), clients(), server(){
	server.fd = -1;
}

Server::Server(int _port, std::string _password)
	: port(_port), password(_password), channels(), clients(), server(){
	server.fd = -1;
}

Server::~Server(){
	if (server.fd != -1)
		close(server.fd);
	std::cout << "ircserv closed" << std::endl;
}

Server::Server(const Server &cpy){
	*this = cpy;
}

Server &Server::operator=(const Server &src){
	if (this != &src)
	{
		this->port = src.port;
		this->password = src.password;
		this->channels = src.channels;
		this->clients = src.clients;
	}
	return *this;
}

const std::string Server::getPassword(void) const{
	return password;
}

int Server::getPort(void) const{
	return port;
}

const std::vector<Channel>	Server::getChannels(void) const{
	return channels;
}

const std::vector<Client>	Server::getClients(void) const{
	return clients;
}

const Channel	&Server::getChannel(std::string _name) const{
	for (size_t i = 0; i < channels.size(); i++)
		if (channels[i].getName() == _name)
			return channels[i];
	throw std::exception();
}

const Client	&Server::getClient(std::string _nickname) const{
	for (size_t i = 0; i < clients.size(); i++)
		if (clients[i].getNickname() == _nickname)
			return clients[i];
	throw std::exception();
}

std::vector<struct pollfd>	Server::getClientPollfds(void){
	std::vector<struct pollfd> pollfds;
	for (size_t i = 0; i < clients.size(); ++i) {
		pollfds.push_back(clients[i].getPollfd());
	}
	return pollfds;
}

void Server::setPassword(std::string _password){
	password = _password;
}

void Server::setPort(int _port){
	port = _port;
}

void Server::addChannel(Channel &_new_channel){
	channels.push_back(_new_channel);
}

void Server::addClient(Client &_new_client){
	clients.push_back(_new_client);
}

void Server::removeChannel(std::string _channel_name){
	for (std::vector<Channel>::iterator it = channels.begin(); it != channels.end(); it++){
		if (it->getName() == _channel_name)
			channels.erase(it);
	}
}

void Server::removeClient(std::string _client_name){
	for (std::vector<Client>::iterator it = clients.begin(); it != clients.end(); it++){
		if (it->getNickname() == _client_name)
			clients.erase(it);
	}
}

void Server::removeClient(size_t pos){
	size_t i = 0;
	for (std::vector<Client>::iterator it = clients.begin(); it != clients.end(); it++){
		if (i == pos){
			clients.erase(it);
			return ;
		}
		i++;
	}
}

void Server::parsing(char **argv){
	std::string arg1(argv[1]);
	if (!std::isdigit(arg1[0]))
		throw std::runtime_error(RED + std::string("Error in parsing\nPresence of non digit char") + RESET);
	try {
		port = strToNbr<int>(arg1);
	} catch (const std::invalid_argument &e){
		throw std::runtime_error(RED + std::string(e.what()) + RESET);
	}
	if (port <= 0)
		throw std::runtime_error(RED + std::string("Error with <port>\nNon valid") + RESET);
	else if (port <= 1023)
		throw std::runtime_error(YELLOW + std::string("Warning with <port>\nThis port may be reserved") + RESET);
	else if (port >= 49152 && port <= 65535)
		throw std::runtime_error(YELLOW + std::string("Warning with <port>\nThis port is reserved for temporary usage") + RESET);
	else if (port > 65535)
		throw std::runtime_error(RED + std::string("Error with <port>\nValue to big") + RESET);
	std::cout << GREEN << "IRC : Valid Port " << port << RESET << std::endl;
	password = argv[2];
	std::cout << GREEN << "IRC : Valid Password" << RESET << std::endl;
}

void Server::server_creation(){
	server.fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server.fd == -1)
		throw std::runtime_error(RED + std::string("Error socket creation\nServer creation failure") + RESET);
	
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(port);
	int mod = 1;
	if(setsockopt(server.fd, SOL_SOCKET, SO_REUSEADDR, &mod, sizeof(mod)) == -1)
		throw std::runtime_error(RED + std::string("Error setsockopt\nsetsockopt failed") + RESET);
	 if (fcntl(server.fd, F_SETFL, O_NONBLOCK) == -1)
		throw std::runtime_error(RED + std::string("Error fnctl\nSocket failed to be non-blocking") + RESET);
	if (bind(server.fd,(struct sockaddr*)&addr, sizeof(addr)) < 0) {
		throw std::runtime_error(RED + std::string("Error socket binding\nSocket binding to <port> failed") + RESET);
	}
	if (listen(server.fd, SOMAXCONN) < 0) {
		throw std::runtime_error(RED + std::string("Error socket listening\nSocket listen function failed") + RESET);
	}
	server.events = POLLIN;
	server.revents = 0;
}

void Server::server_iteration(){
	std::cout << clients.size() << std::endl;
	int server_event_nbr = poll(&server, 1, -1);
	if (server_event_nbr > 0){
		if (server.revents & POLLIN){
			int client_fd = accept(server.fd, NULL, NULL);
			if (client_fd == -1)
				throw std::runtime_error(RED + std::string("Error socket creation\nClient creation failure") + RESET);
			struct pollfd new_client_pollfd;
			new_client_pollfd.fd = client_fd;
			new_client_pollfd.events = POLLIN;
			new_client_pollfd.revents = 0;
			
			Client new_client(client_fd, new_client_pollfd);
			
			addClient(new_client);
			std::cout << GREY << "A new client join the irc server !" << RESET << std::endl;
		}
	}
	int client_event_nbr = poll(getClientPollfds().data(), clients.size(), -1);
	if (client_event_nbr > 0){
		for (size_t i = 0; i < clients.size(); i++)
		{
			if (clients[i].getPollfd().revents & POLLIN){\
				char line[BUFFER];
				int read_size;
				read_size = recv(clients[i].getPollfd().fd, line, BUFFER, 0);
				if (read_size <= 0){
					close(clients[i].getPollfd().fd);
					clients[i].setPollfdFd(-1);
					removeClient(i);
					--i;
					std::cout << GREY << "A client leaved the irc server !" << RESET << std::endl;
					continue ;
				}
				if (line[0] != '\0')
				{
					std::vector<std::string> group = line_split(line);
					for (size_t i = 0; i < group.size(); i++)
					{
						std::vector<std::string> cmd = cmd_parsing(const_cast<char *>(group[i].c_str()));
						if (cmd[0] == "NICK" && cmd.size() == 2)
							clients[i].setNickname(cmd[1]);
						if (cmd[0] == "USER" && cmd.size() == 5){
							clients[i].setUsername(cmd[1]);
							clients[i].setHostname(cmd[2]);
						}
					}
					std::cout << "name" << clients[i].getNickname() << std::endl;
					std::cout << "name" << clients[i].getUsername() << std::endl;
					std::cout << "name" << clients[i].getHostname() << std::endl;
					
					if (!clients[i].getAuthenticated()
						&& clients[i].getNickname() != ""
						&& clients[i].getUsername() != ""
						&& clients[i].getHostname() != ""){
						sendRPL(clients[i].getPollfd().fd, "irc.local", "001", clients[i].getNickname(), "Welcome to IRC " + clients[i].getNickname());
						sendRPL(clients[i].getPollfd().fd, "irc.local", "002", clients[i].getNickname(), "Your host is irc.local version 1.0");
						sendRPL(clients[i].getPollfd().fd, "irc.local", "003", clients[i].getNickname(), "Server creation in June 2025");
						sendRPL(clients[i].getPollfd().fd, "irc.local", "004", clients[i].getNickname(), "irc.local", "1.0", "o", "itkol");
						clients[i].setAuthenticated(true);
					}
				}
				std::cout << "name" << clients[i].getNickname() << std::endl;
			}
		}
	}
}

void Server::enter_command(void){
	int client_event_nbr = poll(getClientPollfds().data(), clients.size(), -1);
	if (client_event_nbr > 0){
		for (size_t i = 0; i < clients.size(); i++)
		{
			if (clients[i].getPollfd().revents & POLLIN){
				char line[BUFFER];
				int read_size;
				read_size = recv(clients[i].getPollfd().fd, line, BUFFER, 0);
				if (read_size <= 0){
					close(clients[i].getPollfd().fd);
					clients[i].setPollfdFd(-1);
					removeClient(i);
					--i;
					std::cout << GREY << "A client leaved the irc server !" << RESET << std::endl;
					continue ;
				}
				for (size_t j = 0; j < clients.size(); j++){
					if (j != i)
						send(clients[j].getPollfd().fd, line, read_size, 0);
				}
				std::cout << "Message reçu de " << clients[i].getPollfd().fd << " : "
						<< std::string(line, read_size) << std::endl;
			} 
		}
	}
}

void Server::channel_update(void){
	
}