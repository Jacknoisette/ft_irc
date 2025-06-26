/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdhallen <jdhallen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/19 14:16:43 by jdhallen          #+#    #+#             */
/*   Updated: 2025/06/26 11:16:17 by jdhallen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Server.hpp"

std::map<std::string, void (Server::*)(int, std::vector<std::string>)> Server::cmd_func_list;

Server::Server()
	: port(6667), password(""), channels(), clients(), pollfds(), garbage(){
}

Server::Server(int _port, std::string _password)
	: port(_port), password(_password), channels(), clients(), pollfds(), garbage(){
}

Server::~Server(){
	for (size_t i = 0; i < pollfds.size(); i++)
		if (pollfds[i].fd != -1)
			close(pollfds[i].fd);
	std::cout << confirm("ircserv closed") << std::endl;
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
		this->garbage = src.garbage;
	}
	return *this;
}

void Server::configure(void){
	cmd_func_list["JOIN"] = &Server::join;
	cmd_func_list["QUIT"] = &Server::quit;
	cmd_func_list["PING"] = &Server::ping;
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

const std::map<int, Client>	Server::getClients(void) const{
	return clients;
}

const Channel	&Server::getChannel(std::string _name) const{
	for (size_t i = 0; i < channels.size(); i++)
		if (channels[i].getName() == _name)
			return channels[i];
	throw std::exception();
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
	clients[_new_client.getClientfd()] = _new_client;
}

void Server::removeChannel(std::string _channel_name){
	for (std::vector<Channel>::iterator it = channels.begin(); it != channels.end(); it++){
		if (it->getName() == _channel_name)
			channels.erase(it);
	}
}

void Server::removeClient(std::string _client_name){
	for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); it++){
		if (it->second.getNickname() == _client_name){
			clients.erase(it);
			break ;
		}
	}
}

void Server::removeClient(int fd){
	for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); it++){
		if (it->second.getClientfd() == fd){
			clients.erase(it);
			break ;
		}
	}
}

void Server::parsing(char **argv){
	std::string arg1(argv[1]);
	if (!std::isdigit(arg1[0]))
		throw std::runtime_error(error("Parsing","Presence of non digit char"));
	try {
		port = strToNbr<int>(arg1);
	} catch (const std::invalid_argument &e){
		throw std::runtime_error(error("Parsing", std::string(e.what())));
	}
	if (port <= 0)
		throw std::runtime_error(error("<port>","Non valid"));
	else if (port <= 1023)
		throw std::runtime_error(warning("<port>","This port may be reserved"));
	else if (port >= 49152 && port <= 65535)
		throw std::runtime_error(warning("<port>","This port is reserved for temporary usage"));
	else if (port > 65535)
		throw std::runtime_error(error("<port>","Value to big"));
	std::cout << confirm(std::string("Valid Port " + to_string(port))) << std::endl;
	password = argv[2];
	std::cout << confirm("Valid Password") << std::endl;
}

void Server::server_creation(){
	struct pollfd server;
	server.fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server.fd == -1)
		throw std::runtime_error(error("Socket creation","Server creation failure"));
	
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(port);
	int mod = 1;
	if (setsockopt(server.fd, SOL_SOCKET, SO_REUSEADDR, &mod, sizeof(mod)) == -1)
		throw std::runtime_error(error("setsockopt","setsockopt failed"));
	if (fcntl(server.fd, F_SETFL, O_NONBLOCK) == -1)
		throw std::runtime_error(error("Fnctl","Socket failed to be non-blocking"));
	if (bind(server.fd,(struct sockaddr*)&addr, sizeof(addr)) < 0)
		throw std::runtime_error(error("Socket binding","Socket binding to <port> failed"));
	if (listen(server.fd, SOMAXCONN) < 0)
		throw std::runtime_error(error("Socket listening","Socket listen function failed"));
	server.events = POLLIN;
	server.revents = 0;
	pollfds.push_back(server);
}

// void Server::enter_command(void){
// 	updatePollfds();
// 	int client_event_nbr = poll(pollfds.data(), clients.size(), -1);
// 	updateClientPollfds();
// 	if (client_event_nbr > 0){
// 		for (size_t i = 0; i < clients.size(); i++)
// 		{
// 			if (clients[f].getPollfd().revents & POLLIN){
// 				char line[BUFFER];
// 				int read_size;
// 				read_size = recv(clients[f].getPollfd().fd, line, BUFFER, 0);
// 				if (read_size <= 0){
// 					close(clients[f].getPollfd().fd);
// 					clients[f].setPollfdFd(-1);
// 					removeClient(i);	// if (client_fd != -1)
	// 	close(client_fd);
// 					--i;
// 					std::cout << GREY << "A client leaved the irc server !" << RESET << std::endl;
// 					continue ;
// 				}
// 				for (size_t j = 0; j < clients.size(); j++){
// 					if (j != i)
// 						send(clients[j].getPollfd().fd, line, read_size, 0);
// 				}
// 				std::cout << "Message reçu de " << clients[f].getPollfd().fd << " : "
// 						<< std::string(line, read_size) << std::endl;
// 			} 
// 		}
// 	}
// }
