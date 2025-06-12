/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdhallen <jdhallen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/11 17:06:39 by jdhallen          #+#    #+#             */
/*   Updated: 2025/06/12 14:48:54 by jdhallen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

#include "Global.hpp"

class Server{
	private :
		int port;
		std::string password;
		std::vector<struct pollfd> pollfds;
		int server_fd;
	public :
		Server(void);
		Server(int _port, std::string _password);
		~Server(void);
		Server(const Server &cpy);
		Server &operator=(const Server &src);

		void parsing(char **argv);
		void server_creation(void);
		void server_iteration(void);

		const std::string			getPassword(void) const;
		int							getPort(void) const;
		std::vector<struct pollfd>	getPollfds(void) const;
		void setPassword(std::string _password);
		void setPort(int _port);
		void addPollfds(struct pollfd);
};

#endif