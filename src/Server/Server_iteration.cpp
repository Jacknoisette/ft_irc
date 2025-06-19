/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_iteration.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdhallen <jdhallen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/19 14:19:42 by jdhallen          #+#    #+#             */
/*   Updated: 2025/06/19 15:50:32 by jdhallen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Server.hpp"


void Server::add_new_client(){
	struct sockaddr_in client_addr;
	socklen_t client_addr_len = sizeof(client_addr);

	int client_fd = accept(pollfds[0].fd, (struct sockaddr*)&client_addr, &client_addr_len);
	if (client_fd == -1) {
		std::cout << "error accept" << std::endl;
		return ;
	}
	char client_ip[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(client_addr.sin_addr), client_ip, INET_ADDRSTRLEN);
	
	if (DEBUG){
		std::cout << "Nouvelle connexion de " << client_ip 
			<< ":" << ntohs(client_addr.sin_port) 
			<< " (fd=" << client_fd << ")" << std::endl;
	}
	
	int flags = fcntl(client_fd, F_GETFL, 0);
	if (flags == -1) {
		perror("fcntl F_GETFL failed");
		close(client_fd);
		return;
	} 
		
	if (fcntl(client_fd, F_SETFL, flags | O_NONBLOCK) == -1) {
		perror("fcntl F_SETFL failed");
		close(client_fd);
		return;
	}
		
	struct pollfd new_client_pollfd;
	new_client_pollfd.fd = client_fd;
	new_client_pollfd.events = POLLIN;
	new_client_pollfd.revents = 0;
	
	Client new_client(client_fd);
	pollfds.push_back(new_client_pollfd);
	addClient(new_client);
	std::cout << GREY << "A new client join the irc server ! fd=" << client_fd 
		<< " from " << client_ip << ":" << ntohs(client_addr.sin_port) << RESET << std::endl;
}

void Server::server_iteration(){
	for (size_t i = 0; i < pollfds.size(); ++i)
		pollfds[i].revents = 0;
	int client_event_nbr = poll(&this->pollfds[0], pollfds.size(), -1);
	std::cout << client_event_nbr << std::endl;
	if (client_event_nbr < 0){
		perror("Error socket creation\nServer crash");
		return ;
	}

	if (DEBUG)
		socket_debug();
	
	if (pollfds[0].revents & POLLIN){
		add_new_client();
	}
	else {
		for (std::vector<pollfd>::iterator it = pollfds.begin() + 1; it != pollfds.end(); )
		{
            if (DEBUG)
			    std::cout << "Checking client socket " << it->fd << " revents=" << it->revents << std::endl;
			
			if (it->revents & (POLLERR | POLLHUP | POLLNVAL)) {
				std::cout << "Error event detected on client " << it->fd << std::endl;
				removeClient(it->fd);
				close(it->fd);
				it = pollfds.erase(it);
				continue;
			}
			
			if (it->revents & POLLIN){
				int f = it->fd;
				char line[BUFFER] = {0};
				int read_size;
				read_size = recv(it->fd, line, BUFFER - 1, 0);
				if (read_size == 0){
					removeClient(f);
					close(it->fd);
					it = pollfds.erase(it);
					std::cout << GREY << "A client leaved the irc server !" << RESET << std::endl;
					continue ;
				}
				else if (read_size < 0) {
					if (errno == EAGAIN || errno == EWOULDBLOCK) {
						++it;
						continue;
					}
					std::cout << GREY << "Error reading from client " << f << ": " << strerror(errno) << RESET << std::endl;
					removeClient(f);
					close(it->fd);
					it = pollfds.erase(it);
					continue;
				}
				line[read_size] = '\0';
				std::cout << "line :" << line << std::endl;
				if (line[0] != '\0')
				{
					std::vector<std::string> group = line_split(line);
					for (size_t j = 0; j < group.size(); j++)
					{
						std::vector<std::string> cmd = cmd_parsing(const_cast<char *>(group[j].c_str()));
						if (cmd[0] == "NICK" && cmd.size() == 2)
							clients[f].setNickname(cmd[1]);
						if (cmd[0] == "USER" && cmd.size() == 5){
							clients[f].setUsername(cmd[1]);
							clients[f].setHostname(cmd[2]);
						}
					}
                    if (DEBUG){
                        std::cout << "nickname" << clients[f].getNickname() << std::endl;
					    std::cout << "username" << clients[f].getUsername() << std::endl;
					    std::cout << "hostname" << clients[f].getHostname() << std::endl;  
                    }
					
                    check_Auth(f);
				}
				it->revents = 0;
				++it;
			}
			else {
				++it;
			}
		}
	}
}