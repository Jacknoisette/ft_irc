/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdhallen <jdhallen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/13 10:23:50 by jdhallen          #+#    #+#             */
/*   Updated: 2025/06/13 11:14:48 by jdhallen         ###   ########.fr       */
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
		std::vector<Client> client_list;
	public :
		Channel();
		Channel(std::string _name);
		~Channel();
		Channel(const Channel &cpy);
		Channel operator=(const Channel &src);
		
		const std::string	getName(void) const;
		const std::vector<Client>	getClients(void) const;
};

#endif