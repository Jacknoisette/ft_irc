/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdhallen <jdhallen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/12 16:36:39 by jdhallen          #+#    #+#             */
/*   Updated: 2025/06/12 16:39:46 by jdhallen         ###   ########.fr       */
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
        bool authenticated;
        bool is_in_channel;
        std::string channel;
	public :
		Client(void);
		Client(int _client_fd);
		~Client(void);
		Client(const Client &cpy);
		Client &operator=(const Client &src);

        const int	getClientfd(void) const;
        const std::string	getNickname(void) const;
        const std::string	getUsername(void) const;
        bool				getAuthenticated(void) const;
        bool				getIsInChannel(void) const;
        const std::string	getChannel(void) const;
        void setClientfd(int _client_fd);
        void setNickname(std::string _nickname);
        void setUsername(std::string _username);
        void setAuthenticated(bool _authenticated);
        void setIsInChannel(bool _is_in_channel);
};

#endif