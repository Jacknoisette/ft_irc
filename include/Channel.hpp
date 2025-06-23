/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdhallen <jdhallen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/13 10:23:50 by jdhallen          #+#    #+#             */
/*   Updated: 2025/06/23 16:56:28 by jdhallen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include "Global.hpp"
#include "Client.hpp"

class Channel {
	private :
		std::string name;
		// std::string topic;
		std::map<int, Client> clients_list;
	public :
		Channel();
		Channel(std::string _name);
		~Channel();
		Channel(const Channel &cpy);
		Channel operator=(const Channel &src);
		
		const std::string	getName(void) const;
		const std::map<int, Client>	getClients(void) const;

		void	addClient(int client_fd, Client new_client);
		void	removeClient(std::string _client_name);
		void	removeClient(int fd);
};

#endif