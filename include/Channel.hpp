/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdhallen <jdhallen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/13 10:23:50 by jdhallen          #+#    #+#             */
/*   Updated: 2025/07/03 16:18:07 by jdhallen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include "Global.hpp"
#include "Client.hpp"

class Client;

class Channel {
	private :
		std::string og_name;
		std::string name;
		std::string topic;
		std::map<int, std::pair<Client, bool> > clients_list;
		std::map<std::string, std::pair<Client, bool> > strClientMap;
		bool isOnInvite;
		bool isTopicRestricted;
		std::string password;
		unsigned long channelLimit;
	public :
		Channel();
		Channel(std::string _name);
		~Channel();
		Channel(const Channel &cpy);
		Channel &operator=(const Channel &src);
		
		const std::string	&getOGName(void) const;
		const std::string	&getName(void) const;
		bool				getIsOnInvite(void) const;
		void				setIsOnInvite(bool isOnInvite);
		const std::string &getTopic(void) const;
		void setTopic(const std::string& topicName);
		bool 				getIsTopicRestricted(void) const;
		void				setIsTopicRestricted(bool isRestricted);
		const std::string& getPassword(void) const;
		void							setPassword(const std::string& password);
		void				setChannelLimit(int channelLimit);
		int					getChannelLimit(void) const;
		const std::map<int, std::pair<Client, bool> >	&getClients(void) const;
		std::map<std::string, std::pair<Client, bool> >& getstrClientMap(void);

		void	addClient(int client_fd, Client new_client, bool is_op);
		void	removeClient(int fd);

		// void sendRPL_Channel(bool self_display, int fd, std::string msg) const;
		const std::string getListClientByType(void) const;
		// void	sendWelcomeChannelMsg(const Client &client) const;
};

#endif
