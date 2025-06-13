/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Global.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdhallen <jdhallen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/11 17:41:41 by jdhallen          #+#    #+#             */
/*   Updated: 2025/06/13 15:40:36 by jdhallen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GLOBAL_HPP
# define GLOBAL_HPP

#include <iostream>
#include <vector>
#include <exception>
#include <stdexcept>
#include <sstream>
#include <fstream>
#include <poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <fstream>
#include <string.h>

# define RED "\033[1;30m"
# define PURPLE "\033[1;31m"
# define WHITE "\033[1;32m"
# define YELLOW "\033[1;33m"
# define ORANGE "\033[1;34m"
# define PINK "\033[1;35m"
# define GREEN "\033[1;36m"
# define GREY "\033[1;37m"
# define RESET "\033[0m"
# define BIG "\033[1m"

#ifndef BUFFER
# define BUFFER 1024
#endif

// Fonction Template

template <typename T>
T	strToNbr(std::string str){
    std::stringstream	Sstream(str);
	T    result;
	char temp;

	if (!(Sstream >> result))
		throw (std::invalid_argument("Error in Conversion\nInvalid char"));
	while (Sstream.peek() != EOF && std::isspace(Sstream.peek()))
		Sstream.get(temp);
	if (Sstream.peek() != EOF)
		throw (std::invalid_argument("Error in Conversion\nUnexpected character"));
	return (result);
}

std::vector<std::string> cmd_parsing(char *string);
std::vector<std::string> line_split(char *string);
void sendRPL(int fd, const std::string& servername, 
			const std::string& code, const std::string& nick,
			const std::string& message);
void sendRPL(int fd, const std::string& servername,
			const std::string& code, const std::string& nick,
			const std::string& host, const std::string& version,
			const std::string& usermod, const std::string& channelmod);


#endif

