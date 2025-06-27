/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdhallen <jdhallen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/27 11:53:21 by jdhallen          #+#    #+#             */
/*   Updated: 2025/06/27 12:16:26 by jdhallen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "Global.hpp"
#include "Channel.hpp"

class Channel;

class Client{
	private :
		int client_fd;
		std::string nickname;
		std::string username;
		std::string hostname;
		bool authenticated;
		std::map<std::string, Channel> in_channels;
	public :
		Client(void);
		Client(int _client_fd);
		~Client(void);
		Client(const Client &cpy);
		Client &operator=(const Client &src);

		int	getClientfd(void) const;
		const std::string	getNickname(void) const;
		const std::string	getUsername(void) const;
		const std::string	getHostname(void) const;
		bool				getAuthenticated(void) const;
		std::map<std::string, Channel>	&getChannels(void);
		void setClientfd(int _client_fd);
		void setNickname(std::string _nickname);
		void setUsername(std::string _username);
		void setHostname(std::string _hostname);
		void setAuthenticated(bool _authenticated);
		
		void addChannel(Channel &_new_channel);
		void removeChannel(std::string _channel_name);
};

#endif