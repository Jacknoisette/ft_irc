/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdhallen <jdhallen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/12 16:36:39 by jdhallen          #+#    #+#             */
/*   Updated: 2025/06/16 17:53:47 by jdhallen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "Global.hpp"

class Client{
	private :
        int client_fd;
        std::string nickname;
        std::string username;
        std::string hostname;
        bool authenticated;
        bool is_in_channel;
        std::string channel;
        // struct pollfd pollfd;
	public :
		Client(void);
		Client(int _client_fd);//, struct pollfd &_pollfd);
		~Client(void);
		Client(const Client &cpy);
		Client &operator=(const Client &src);

        int	getClientfd(void) const;
        const std::string	getNickname(void) const;
        const std::string	getUsername(void) const;
        const std::string	getHostname(void) const;
        bool		        getAuthenticated(void) const;
        bool			getIsInChannel(void) const;
        const std::string	getChannel(void) const;
        // const struct pollfd	&getPollfd(void) const;
        void setClientfd(int _client_fd);
        void setNickname(std::string _nickname);
        void setUsername(std::string _username);
        void setHostname(std::string _hostname);
        void setAuthenticated(bool _authenticated);
        void setIsInChannel(bool _is_in_channel);
        void setChannel(std::string _channel);
        // void setPollfd(struct pollfd &_pollfd);
        // void setPollfdFd(int _fd);
};

#endif