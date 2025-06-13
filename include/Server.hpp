/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdhallen <jdhallen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/11 17:06:39 by jdhallen          #+#    #+#             */
/*   Updated: 2025/06/13 15:25:37 by jdhallen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

#include "Global.hpp"
#include "Client.hpp"
#include "Channel.hpp"

class Server{
	private :
		int port;
		std::string password;
		std::vector<Channel> channels;
		std::vector<Client> clients;
		struct pollfd server;
	public :
		Server(void);
		Server(int _port, std::string _password);
		~Server(void);
		Server(const Server &cpy);
		Server &operator=(const Server &src);

		void parsing(char **argv);
		void server_creation(void);
		void server_iteration(void);		
		void enter_command(void);
		void channel_update(void);

		const std::string			getPassword(void) const;
		int							getPort(void) const;
		const std::vector<Channel>	getChannels(void) const;
		const std::vector<Client>	getClients(void) const;
		const Channel	&getChannel(std::string _name) const;
		const Client	&getClient (std::string _nickname) const;
		std::vector<struct pollfd>	getClientPollfds(void);
		
		void setPassword(std::string _password);
		void setPort(int _port);
		void addChannel(Channel &_new_channel);
		void addClient(Client &_new_client);

		void	removeChannel(std::string _channel_name);
		void	removeClient(std::string _client_name);
		void	removeClient(size_t pos);
};

#endif