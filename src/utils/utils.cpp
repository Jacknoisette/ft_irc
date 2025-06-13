/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdhallen <jdhallen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/13 14:30:02 by jdhallen          #+#    #+#             */
/*   Updated: 2025/06/13 15:39:51 by jdhallen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Global.hpp"

std::vector<std::string> line_split(char *string){
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

std::vector<std::string> cmd_parsing(char *string){
	std::vector<std::string> cmd;
	size_t pos = 0;
	size_t tmp = 0;
	std::string str(string);
	while (str[0] == ' ' || str[0] == '\t')
		str = str.erase(0, 1);
	while (str[str.size() - 1] == ' ' || str[str.size() - 1] == '\t')
		str = str.erase(str.size() - 1, 1);
	while (pos < str.size())
	{
		pos = str.find(' ', pos);
		if (pos == str.size())
			break ;
		cmd.push_back(str.substr(tmp, pos - tmp));
		while (str[pos] == ' ' || str[pos] == '\t')
			str = str.erase(pos, 1);
		tmp = pos;
	}
	return (cmd);
}

void sendRPL(int fd, const std::string& servername,
			const std::string& code, const std::string& nick,
			const std::string& message) {
    std::string rpl = ":" + servername + " " + code + " " + nick + " :" + message + "\r\n";
    send(fd, rpl.c_str(), rpl.size(), 0);
}

void sendRPL(int fd, const std::string& servername,
				const std::string& code, const std::string& nick,
				const std::string& host, const std::string& version,
				const std::string& usermod, const std::string& channelmod) {
    std::string rpl = ":" + servername + " " + code + " " + nick + " " + host + " " + version + " " + usermod + " " + channelmod + "\r\n";
    send(fd, rpl.c_str(), rpl.size(), 0);
}

// int main(int argc, char **argv){
// 	if (argc < 2)
// 		return 1;
// 	std::cout << argv[1] << std::endl;
// 	std::vector<std::string> cmd = cmd_parsing(argv[1]);
// 	for (size_t i = 0; i < cmd.size(); i++)
// 		std::cout << cmd[i] << "-";
// 	std::cout << std::endl;
// }

// int main(){
// 	std::string line = "NICK foo\r\nUSER bar 0 * :baz\r\n";
// 	std::vector<std::string> cmd = line_split(const_cast<char *>(line.c_str()));
// 	for (size_t i = 0; i < cmd.size(); i++)
// 		std::cout << cmd[i] << "-";
// 	std::cout << std::endl;
// }