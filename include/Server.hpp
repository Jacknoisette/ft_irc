/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdhallen <jdhallen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/11 17:06:39 by jdhallen          #+#    #+#             */
/*   Updated: 2025/07/09 11:09:54 by jdhallen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

#include "Global.hpp"
#include "Client.hpp"
#include "Channel.hpp"

class Server{
	private :
		static std::map<std::string, void (Server::*)(int, std::vector<std::string>)> cmd_func_list;
		static volatile sig_atomic_t shutdownRequested;

		int port;
		std::string password;
		std::list<Channel> allChannels;
		std::list<Client> allClients;
		std::map<std::string, Channel*> channels;
		std::map<int, Client*> clients;
		std::map<std::string, Client*> strClients;
		std::vector<struct pollfd> pollfds;
		std::vector<std::pair<int, std::string> > garbage;
	public :
		Server(void);
		Server(int _port, std::string _password);
		~Server(void);
		Server(const Server &cpy);
		Server &operator=(const Server &src);
		void	commandConfig(void);

		void	parsing(char **argv);
		void	server_creation(void);

		Channel*	getOrCreateChannel(const std::string& name);
		Client*		getOrCreateClient(const int fd);

		//SERVER ITERATION
		void	server_iteration(void);
		std::string	make_client_nonblock(int client_fd, sockaddr_in &client_addr);
		void	add_new_client(void);
		std::string analyse_line(int client_fd, std::vector<pollfd>::iterator &it);
		void	detect_client_input(void);
		void	detect_client_output(void);
		void	take_out_the_trash();
		// void enter_command(void);
		// void channel_update(void);

		//GET/SETTER/UTILS
		const std::string			getPassword(void) const;
		int							getPort(void) const;
		const	std::map<std::string, Channel*> 	getChannels(void) const;
		const	std::map<int, Client*>	getClients(void) const;

		void	setPassword(std::string _password);
		void	setPort(int _port);
		
		// void	addChannel(Channel &_new_channel);
		// void	addClient(Client &_new_client);
		void	removeChannel(std::string _channel_name);
		void	removeClient(int fd);
		static void signalHandler(int sig);
		bool isValidNickname(const std::string& nick);

		//CLIENT COMMAND
		void	client_command(int client_fd, std::vector<std::vector<std::string> > &cmd_group);
		void	check_BaseCmd(int fd, std::vector<std::vector<std::string> > &cmd_group);
		void	checkAuth(int fd, std::vector<std::vector<std::string> > &cmd_group);

		//COMMAND LIST
		void	join(int fd, std::vector<std::string> arg);
		void	part(int fd, std::vector<std::string> arg);
		void	quit(int fd, std::vector<std::string> arg);
		void	ping(int fd, std::vector<std::string> arg);
		void	privmsg(int fd, std::vector<std::string> arg);
		void	notice(int fd, std::vector<std::string> arg);
		void	mode(int fd, std::vector<std::string> arg);
		void	nick(int fd, std::vector<std::string> arg);
		void	invite(int fd, std::vector<std::string> arg);
		void	topic(int fd, std::vector<std::string> arg);
		void	kick(int fd, std::vector<std::string> arg);

		void	applykick(int fd, int kick_client_fd, std::string &channel_lower, std::string &kick_msg);
		//DEBUG
		void	socket_debug(void);
		void	command_debug(int client_fd, const std::vector<std::vector<std::string> > &cmd_group, const std::string& where);
		void	ultimateDebug();

		//PARSING_UTILS
		std::vector<std::string>	check_channel_name(int fd, std::string arg, std::string cmd_name);
		std::vector<std::string>	check_channel_name_notice(int fd, std::string arg);
		std::vector<std::string>	check_client_name(int fd, std::string arg);
		std::vector<std::string>	check_key_string(int fd, std::string arg);
		void						ValidateMsgContent(int fd, std::string arg);
		void						ValidateMsgContentNotice(int fd, std::string arg);
		void 						client_name_parsing(int fd, std::string arg);

		//RPL

		void	sendWelcomeChannelMsg(const Client *client, std::string channel_name);
		void	sendRPL_Channel(bool self_display, int fd, std::string channel_name, std::string msg);
		void	sendRPL(int fd, ...);
		void 	sendToClient(int fd, const std::string& msg);
		void	flushSendBuffer(int fd);
		void	enablePOLLOUT(int fd);
		void	disablePOLLOUT(int fd);
};

#endif
