/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdhallen <jdhallen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/23 12:44:40 by jdhallen          #+#    #+#             */
/*   Updated: 2025/06/23 15:56:03 by jdhallen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Global.hpp"

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
	while (!string.empty() && (string[0] == ' ' || string[0] == '\t'))
		string.erase(0, 1);
	while (!string.empty() && (string[string.size() - 1] == ' ' || string[string.size() - 1] == '\t'))
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

void sendRPL(int fd, const std::string& servername,
			const std::string& code, const std::string& nick,
			const std::string& message) {
	std::string rpl = ":" + servername + " " + code + " " + nick + " :" + message + "\r\n";
	send(fd, rpl.c_str(), rpl.size(), 0);
	if (DEBUG)
		std::cout << std::flush << rpl;
}

void sendRPL(int fd, const std::string& servername,
				const std::string& code, const std::string& nick,
				const std::string& host, const std::string& version,
				const std::string& usermod, const std::string& channelmod) {
	std::string rpl = ":" + servername + " " + code + " " + nick + " " + host + " " + version + " " + usermod + " " + channelmod + "\r\n";
	send(fd, rpl.c_str(), rpl.size(), 0);
	if (DEBUG)
		std::cout << std::flush << rpl;
}

std::string to_string(int value) {
	std::stringstream ss;
	ss << value;
	return ss.str();
}


