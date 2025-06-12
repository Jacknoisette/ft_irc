/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdhallen <jdhallen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/11 17:09:16 by jdhallen          #+#    #+#             */
/*   Updated: 2025/06/12 15:29:12 by jdhallen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Server.hpp"

Server::Server()
	: port(6667), password(""), pollfds(), server_fd(-1){
	
}

Server::Server(int _port, std::string _password)
	: port(_port), password(_password), pollfds(), server_fd(-1){
	
}

Server::~Server(){
	if (server_fd != -1)
		close(server_fd);
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
		this->pollfds = src.pollfds;
	}
	return *this;
}

const std::string Server::getPassword(void) const{
	return password;
}

int Server::getPort(void) const{
	return port;
}

std::vector<struct pollfd> Server::getPollfds(void) const{
	return pollfds;
}

void Server::setPassword(std::string _password){
	password = _password;
}

void Server::setPort(int _port){
	port = _port;
}

void Server::addPollfds(struct pollfd _pollfd){
	pollfds.push_back(_pollfd);
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
	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd == -1)
		throw std::runtime_error(RED + std::string("Error socket creation\nServer creation failure") + RESET);
	
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(port);
	int mod = 1;
	if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &mod, sizeof(mod)) == -1)
		throw std::runtime_error(RED + std::string("Error setsockopt\nsetsockopt failed") + RESET);
	 if (fcntl(server_fd, F_SETFL, O_NONBLOCK) == -1)
		throw std::runtime_error(RED + std::string("Error fnctl\nSocket failed to be non-blocking") + RESET);
	if (bind(server_fd,(struct sockaddr*)&addr, sizeof(addr)) < 0) {
		throw std::runtime_error(RED + std::string("Error socket binding\nSocket binding to <port> failed") + RESET);
	}
	if (listen(server_fd, SOMAXCONN) < 0) {
		throw std::runtime_error(RED + std::string("Error socket listening\nSocket listen function failed") + RESET);
	}
	struct pollfd new_server;
	new_server.fd = server_fd;
	new_server.events = POLLIN;
	new_server.revents = 0;
	addPollfds(new_server);
}

void Server::server_iteration(){
	int event_nbr = poll(pollfds.data(), pollfds.size(), -1);
	if (event_nbr > 0){
		if (pollfds[0].revents & POLLIN){
			int client_fd = accept(pollfds[0].fd, NULL, NULL);
			if (client_fd == -1)
				throw std::runtime_error(RED + std::string("Error socket creation\nClient creation failure") + RESET);
			struct pollfd new_client;
			new_client.fd = client_fd;
			new_client.events = POLLIN;
			new_client.revents = 0;
			addPollfds(new_client);
			std::cout << GREY << "A new client join the irc server !" << RESET << std::endl;
		}
		for (size_t i = 1; i < pollfds.size(); i++)
		{
			if (pollfds[i].revents & POLLIN){
				char line[BUFFER];
				int read_size;
				while (1)
				{
					read_size = recv(pollfds[i].fd, line, BUFFER, 0);
					if (read_size <= 0)
						break ;
					for (size_t j = 1; j < pollfds.size(); j++){
						if (j != i)
							send(pollfds[j].fd, line, read_size, 0);
					}
				}	
			} 
		}
	}
}