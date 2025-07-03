/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdhallen <jdhallen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/23 12:44:40 by jdhallen          #+#    #+#             */
/*   Updated: 2025/07/01 13:52:50 by jdhallen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Global.hpp"
#include "Server.hpp"

std::vector<std::string> line_split(std::string string){
	std::vector<std::string> group;
	size_t pos = 0;
	size_t tmp = 0;
	std::string str(string);
	while ((pos = str.find("\r\n", tmp)) != std::string::npos)
	{
		group.push_back(str.substr(tmp, pos - tmp));
		tmp = pos + 2;
	}
	if (tmp < str.size())
		group.push_back(str.substr(tmp));
	return (group);
}

std::vector<std::string> cmd_parsing_base(std::string string){
	std::vector<std::string> cmd;
	while (!string.empty() && (string[0] == ' ' || string[0] == '\t' || string[0] == '\n' || string[0] == '\r'))
		string.erase(0, 1);
	while (!string.empty() && (string[string.size() - 1] == ' ' || string[string.size() - 1] == '\t' || string[string.size() - 1] == '\n' || string[string.size() - 1] == '\r'))
		string.erase(string.size() - 1, 1);
	size_t start = 0;
	while (start < string.size())
	{
		size_t pos = string.find(' ', start);
        if (pos == std::string::npos) {
            cmd.push_back(string.substr(start));
            break;
        }
        if (pos > start)
            cmd.push_back(string.substr(start, pos-start));
        start = pos+1;
        while (start < string.size() && (string[start] == ' ' || string[start] == '\t'))
            start++;
	}
	return (cmd);
}

std::vector<std::string> cmd_parsing(std::string string){
	std::vector<std::string> cmd;
	size_t colon = string.find(':');
	if (colon != std::string::npos) {
		std::vector<std::string> base = cmd_parsing_base(string.substr(0, colon));
		cmd.insert(cmd.end(), base.begin(), base.end());
		std::string trailing = string.substr(colon + 1);
		if (!trailing.empty())
			cmd.push_back(trailing);
	} else {
		cmd = cmd_parsing_base(string);
	}
	return cmd;
}

//Enter fd and any const char * to create the rpl msg
void sendRPL(int fd, ...) {
	va_list args;
	va_start(args, fd);
	std::string rpl = ":";
	const char *msg;
	while ((msg = va_arg(args, const char*)) != NULL)
		rpl += std::string(msg) + " ";
	rpl += "\r\n";
	send(fd, rpl.c_str(), rpl.size(), 0);
	if (DEBUG)
		std::cout << std::flush << WHITE << rpl;
	va_end(args);
}

std::string to_string(int value) {
	std::stringstream ss;
	ss << value;
	return ss.str();
}

std::vector<std::string>	parsing_channel(std::string arg){
	std::vector<std::string>	channel_list;
	size_t start = 0;
	while (start < arg.size())
	{
		size_t pos = arg.find(',', start);
        if (pos == std::string::npos) {
            channel_list.push_back(arg.substr(start));
            break;
        }
        if (pos > start)
            channel_list.push_back(arg.substr(start, pos-start));
        start = pos + 1;
	}
	return channel_list;
}

std::vector<std::string> check_channel_name(int fd, std::string arg){
	if (arg.size() > 50){
		sendRPL(fd, "Channel name too long", NULL);
		throw std::runtime_error(info(std::string("Client " + to_string(fd) + " tried to create a wrong Channel").c_str()));
	}
	if (arg.size() <= 1){
		sendRPL(fd, "Channel name too little", NULL);
		throw std::runtime_error(info(std::string("Client " + to_string(fd) + " tried to create a wrong Channel").c_str()));
	}
	if (arg[0] != '#')
	{
		sendRPL(fd, "Channel name don't start by '#'", NULL);
		throw std::runtime_error(info(std::string("Client " + to_string(fd) + " tried to create a wrong Channel").c_str()));
	}
	for (size_t i = 0; i < arg.size(); i++){
		if (arg[i] <= 32 || arg[i] == ':'){
			sendRPL(fd, "Channel name contains incorrect char", NULL);
			throw std::runtime_error(info(std::string("Client " + to_string(fd) + " tried to create a wrong Channel").c_str()));
		}
	}
	std::vector<std::string> channels = parsing_channel(arg);
	for (size_t i = 0; i < channels.size(); i++){
		if (channels[i][0] != '#'){
			sendRPL(fd, "Channel name don't start by '#'", NULL);
			throw std::runtime_error(info(std::string("Client " + to_string(fd) + " tried to create a wrong Channel").c_str()));
		}		
	}
	return channels;
}

std::vector<std::string>	check_key_string(int fd, std::string arg){
	if (arg.size() > 23){
		sendRPL(fd, "Key name too long", NULL);
		throw std::runtime_error(info(std::string("Client " + to_string(fd) + " tried to create a wrong Channel").c_str()));
	}
	for (size_t i = 0; i < arg.size(); i++){
		if (arg[i] <= 32 || arg[i] == ':'){
			sendRPL(fd, "Channel name contains incorrect char", NULL);
			throw std::runtime_error(info(std::string("Client " + to_string(fd) + " tried to create a wrong Channel").c_str()));
		}
	}
	return parsing_channel(arg);
}

void	ValidateMsgContent(int fd, std::string arg)
{
	if (arg.size() > 510)
	{
		sendRPL(fd, "Msg too long", NULL);
		throw std::runtime_error(info(std::string("Client " + to_string(fd)
		                 + " tried to create a wrong Channel").c_str()));
	}
	if (arg.size() <= 1)
	{
		sendRPL(fd, "Channel name too short", NULL);
		throw std::runtime_error(info(std::string("Client " + to_string(fd)
		                  + " tried to create a wrong Channel").c_str()));
	}
	// if (arg[0] != ':'){
	// 	sendRPL(fd, "Msg don't start by ':'", NULL);
	// 	throw std::runtime_error(info(std::string("Client " + to_string(fd) + " tried to create a wrong Msg").c_str()));
	// }
		size_t pos = arg.find_first_of("\r\v\f");
		if (pos != arg.npos)
		{
			std::cerr << "the char which causes the error is at pos: " << pos << " " << arg.at(pos) << std::endl;
			sendRPL(fd, "Msg contains incorrect char", NULL);
			throw std::runtime_error(info(std::string("Client " + to_string(fd)
			                  + " tried to create a wrong Channel").c_str()));
		}
}

std::string toLowerString(const std::string& s) {
	std::string out = s;
	std::transform(out.begin(), out.end(), out.begin(), ::tolower);
	return out;
}

std::vector< std::pair<std::string, std::string> > toLowerVector(const std::vector<std::string>& v) {
	std::vector< std::pair<std::string, std::string> > out;
	for (std::vector<std::string>::const_iterator it = v.begin(); it != v.end(); ++it)
		out.push_back(std::make_pair(toLowerString(*it), *it));
	return out;
}
