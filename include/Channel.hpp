/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdhallen <jdhallen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/13 10:23:50 by jdhallen          #+#    #+#             */
/*   Updated: 2025/06/27 13:49:22 by jdhallen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include "Global.hpp"
#include "Client.hpp"

class Client;

class Channel {
	private :
		std::string name;
		// std::string topic;
		std::map<int, std::pair<Client, bool> > clients_list;
	public :
		Channel();
		Channel(std::string _name);
		~Channel();
		Channel(const Channel &cpy);
		Channel &operator=(const Channel &src);
		
		const std::string	getName(void) const;
		const std::map<int, std::pair<Client, bool> >	getClients(void) const;

		void	addClient(int client_fd, Client new_client);
		void	removeClient(int fd);
};

#endif