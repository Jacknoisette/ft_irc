/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdhallen <jdhallen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/27 11:53:21 by jdhallen          #+#    #+#             */
/*   Updated: 2025/07/04 16:04:11 by jdhallen         ###   ########.fr       */
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
		std::string normalizedNick;
		std::string username;
		std::string hostname;
		bool authenticated;
		bool passwordMatch;
		std::map<std::string, std::pair<Channel*, size_t> > in_channels;
		std::string	current_channel;
		std::string sendBuffer;
	public :
		Client(void);
		Client(int _client_fd);
		~Client(void);
		Client(const Client &cpy);
		Client &operator=(const Client &src);

		int	getClientfd(void) const;
		const std::string	getNickname(void) const;
		const std::string	getNormalizedNick(void) const;
		const std::string	getUsername(void) const;
		const std::string	getHostname(void) const;
		bool	getPasswordMatch(void) const;
		bool				getAuthenticated(void) const;
		std::map<std::string, std::pair<Channel*, size_t> >	&getChannels(void);
		const std::string	&getCurrentChannel(void) const;
		std::string	&getSendBuffer(void);
		
		void setClientfd(int _client_fd);
		void setNickname(std::string _nickname);
		void setNormalizedNick(const std::string& nickname);
		void setUsername(std::string _username);
		void setHostname(std::string _hostname);
		void setAuthenticated(bool _authenticated);
		void setCurrentChannel(std::string _current_channel);
		void setPasswordMatch(bool match);
		void setSendBuffer(std::string _sendBuffer);
		
		void addChannel(Channel *_new_channel);
		void removeChannel(std::string _channel_name);

		size_t	findMaxVal(void);
		void	updateCurrentChannel(void);
};

#endif
