/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_iteration.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdhallen <jdhallen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/19 14:19:42 by jdhallen          #+#    #+#             */
/*   Updated: 2025/07/03 15:08:57 by jdhallen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Global.hpp"

std::string Server::make_client_nonblock(int client_fd, sockaddr_in &client_addr){
	char client_ip[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(client_addr.sin_addr), client_ip, INET_ADDRSTRLEN);
	
	if (DEBUG)
	{
		std::cerr << "make_client_nonblock called\n\n";
		std::cout << "New connexion from " << client_ip 
			<< ":" << ntohs(client_addr.sin_port) 
			<< " (fd=" << client_fd << ")" << std::endl;
	}
	
	int flags = fcntl(client_fd, F_GETFL, 0);
	if (flags == -1) {
		close(client_fd);
		throw std::runtime_error(error("Fcntl", "F_GETFL failed"));
	} 
		
	if (fcntl(client_fd, F_SETFL, flags | O_NONBLOCK) == -1) {
		close(client_fd);
		throw std::runtime_error(error("Fnctl", "Socket failed to be non-block"));
	}
	
	return std::string(client_ip);
}

void Server::add_new_client()
{
	if (DEBUG)
		std::cerr << "add_new_client called\n";
	struct sockaddr_in client_addr;
	socklen_t client_addr_len = sizeof(client_addr);

	int client_fd = accept(pollfds[0].fd, (struct sockaddr*)&client_addr, &client_addr_len);
	if (client_fd == -1) {
		std::cout << error("Accept", "Incorrect fd") << std::endl;
		return ;
	}
	
	std::string client_ip;
	try {
		client_ip = make_client_nonblock(client_fd, client_addr);
	}
	catch (std::runtime_error & e){
		throw e;
	}
		
	struct pollfd new_client_pollfd;
	new_client_pollfd.fd = client_fd;
	new_client_pollfd.events = POLLIN;
	new_client_pollfd.revents = 0;
	
	Client new_client(client_fd);
	pollfds.push_back(new_client_pollfd);
	addClient(new_client);
	std::cout << info(std::string("A new client join the irc server ! fd=") + to_string(client_fd)
		+ std::string(" from ") + std::string(client_ip) + std::string(":") + to_string(ntohs(client_addr.sin_port))) << std::endl;
}

std::string Server::analyse_line(int client_fd, std::vector<pollfd>::iterator &it)
{
	if (DEBUG)
		std::cerr << "analyse_line called\n";
	char line[BUFFER] = {0};
	int read_size;
	read_size = recv(client_fd, line, BUFFER - 1, 0);
	if (read_size <= 0){
		if (errno == EAGAIN || errno == EWOULDBLOCK) {
			++it;
			throw std::runtime_error(warning("Read", std::string("reading from client ") + to_string(client_fd) + std::string(": ")));
		}
		std::pair<int, std::string> client_leave;
		client_leave.first = it->fd;
		client_leave.second = "Client Quit";
		garbage.push_back(client_leave);
		if (read_size == 0)
			return "";
		else if (read_size < 0)
			throw std::runtime_error(warning("Read", std::string("reading from client ") + to_string(client_fd) + std::string(": ")));
	}
	if (DEBUG)
		std::cout << "line :" << line << std::endl;
	std::string std_line(line);
	return std_line;
}

void Server::detect_client_input()
{
	for (std::vector<pollfd>::iterator it = pollfds.begin() + 1; it != pollfds.end(); it++)
	{
		if (it->revents & (POLLERR | POLLHUP | POLLNVAL)) {
			std::cout << "Error event detected on client " << it->fd << std::endl;
			std::pair<int, std::string> client_leave;
			client_leave.first = it->fd;
			client_leave.second = "Client Quit";
			garbage.push_back(client_leave);
			continue;
		}
		
		if (it->revents & POLLIN){
			int client_fd = it->fd;
			
			std::string line;
			try {
				line = analyse_line(client_fd, it);
				if (line == "")
					continue ;
			}
			catch (const std::runtime_error & e){
				std::cout << e.what() << std::endl;
				continue ;
			}
			std::vector<std::string> group;
			try {
				group = line_split(line);
			} catch (std::runtime_error & e){
				sendRPL(it->fd, "irc.local", "417", clients[it->fd].getNickname().c_str(),
					":Input line was too long", NULL);
				if (DEBUG)
					std::cout << e.what() << std::endl;
			}
			std::vector<std::vector<std::string> > cmd_group;
			for (size_t j = 0; j < group.size(); j++){
				cmd_group.push_back(cmd_parsing(group[j]));
			}
			command_debug(client_fd, cmd_group);
      		client_command(client_fd, cmd_group);
			it->revents = 0;
		}
	}
}

void Server::detect_client_output()
{
	for (std::vector<pollfd>::iterator it = pollfds.begin() + 1; it != pollfds.end(); it++)
	{
		if (it->revents & (POLLERR | POLLHUP | POLLNVAL)) {
			std::cout << "Error event detected on client " << it->fd << std::endl;
			std::pair<int, std::string> client_leave;
			client_leave.first = it->fd;
			client_leave.second = "Client Quit";
			garbage.push_back(client_leave);
			continue;
		}
		
		if (it->revents & POLLOUT){
			flushSendBuffer(it->fd);
			if (DEBUG)
				std::cout << warning("POLLOUT", "Someone is spamming");
			it->revents = 0;
		}
	}
}

void	Server::take_out_the_trash(){
	for (size_t i = 0; i < garbage.size(); i++){
		for (std::map<std::string, std::pair<Channel, size_t> >::iterator it = clients[garbage[i].first].getChannels().begin();
				it != clients[garbage[i].first].getChannels().end(); it++){
			it->second.first.removeClient(garbage[i].first);
		}
		std::cout << info(std::string("A client leaved the irc server ! with fd " + to_string(garbage[i].first)).c_str()) << std::endl;
		removeClient(garbage[i].first);
		for (std::vector<pollfd>::iterator it = pollfds.begin() + 1; it != pollfds.end(); it++){
			if (it->fd == garbage[i].first){
				it = pollfds.erase(it);
				break ;
			}
		}
		close(garbage[i].first);
	}
	garbage.clear();
}

void	Server::server_iteration()
{
	struct sigaction sa;
	sa.sa_flags = 0;
	sa.sa_handler = signalHandler;
	sigemptyset(&sa.sa_mask);
	if (sigaction(SIGINT, &sa, NULL) == -1 || sigaction(SIGQUIT, &sa, NULL) == -1)
		throw (std::runtime_error("handler installation failed"));
	for (size_t i = 0; i < pollfds.size(); ++i)
		pollfds[i].revents = 0;
	int client_event_nbr = poll(&this->pollfds[0], pollfds.size(), -1);
	if (DEBUG)
		std::cout << client_event_nbr << std::endl;
	if (shutdownRequested)
		throw (std::runtime_error(std::string("\r") + confirm("stopping server...")));
	if (client_event_nbr < 0){
		std::cout << error("Socket creation", "Server crash") << std::endl;
		return ;
	}

	if (DEBUG)
		socket_debug();
	
	if (pollfds[0].revents & POLLIN){
		add_new_client();
	}
	detect_client_input();
	detect_client_output();
	take_out_the_trash();
}
