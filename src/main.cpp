/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdhallen <jdhallen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/11 16:23:27 by jdhallen          #+#    #+#             */
/*   Updated: 2025/06/23 17:26:46 by jdhallen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

int main(int argc, char **argv)
{
	Server server;

	if (argc != 3)
		return (std::cout << "Error Wrong Input:\n[./ircserv <port> <password>]" << std::endl, -1);
	try{
		server.commandConfig();
		server.parsing(argv);
		server.server_creation();
		while (1)
			server.server_iteration();
	} catch(const std::runtime_error &e) {
		std::cout << e.what() << std::endl;
	}
}
