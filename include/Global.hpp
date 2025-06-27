/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Global.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdhallen <jdhallen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/11 17:41:41 by jdhallen          #+#    #+#             */
/*   Updated: 2025/06/27 11:44:32 by jdhallen         ###   ########.fr       */
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
#include <string>
#include <string.h>
#include <map>
#include <arpa/inet.h> 
#include <cctype>
#include <errno.h>
#include <cstdarg>
#include <cstring>

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

#ifndef DEBUG
# define DEBUG 1
#endif

//TEMPLATE FUNCTION

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

template <typename T, typename M>
std::string error(T error_title, M message){
	return RED + std::string("Error ") + std::string(error_title) + "\n"
		+ std::string(message) + RESET;
}

template <typename T, typename M>
std::string warning(T warning_title, M message){
	return YELLOW + std::string("Warning ") + std::string(warning_title) + "\n"
		+ std::string(message) + RESET;
}

template <typename M>
std::string info(M message){
	return GREY + std::string(message) + RESET;
}

template <typename M>
std::string confirm(M message){
	return GREEN + std::string("IRC : ") + std::string(message) + RESET;
}

//UNIVERSAL FUNCTION

std::vector<std::string>	cmd_parsing(std::string string);
std::vector<std::string>	line_split(std::string string);

std::vector<std::string>	check_channel_name(int fd, std::string arg);
std::vector<std::string>	check_key_string(int fd, std::string arg);
void	check_msg_string(int fd, std::string arg);

void	sendRPL(int fd, ...);
std::string to_string(int value);

#endif

