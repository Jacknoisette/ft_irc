/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdhallen <jdhallen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/13 14:30:02 by jdhallen          #+#    #+#             */
/*   Updated: 2025/06/19 14:13:42 by jdhallen         ###   ########.fr       */
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

std::vector<std::string> cmd_parsing_base(char *string){
	std::vector<std::string> cmd;
	std::string str(string);
	while (!str.empty() && (str[0] == ' ' || str[0] == '\t'))
        str.erase(0, 1);
    while (!str.empty() && (str[str.size() - 1] == ' ' || str[str.size() - 1] == '\t'))
         str.erase(str.size() - 1, 1);
	size_t start = 0;
	size_t pos = 0;
	while (start < str.size())
	{
		pos = str.find(' ', pos);
		if (pos == std::string::npos){
			cmd.push_back(str.substr(start, pos - start));
			break ;
		}
		cmd.push_back(str.substr(start, pos - start));
        start = pos;
        while (start < str.size() && (str[start] == ' ' || str[start] == '\t'))
            start++;
	}
	return (cmd);
}

std::vector<std::string> cmd_parsing(char *string){
	std::vector<std::string> cmd;
	std::string str(string);
	std::string after(string);
	size_t before = 0;
	before = str.find(':', 0);
	if (before != str.size()){
		cmd = cmd_parsing_base(const_cast<char *>(str.substr(0, before).c_str()));
		cmd.push_back(str.substr(before, str.size() - before));
	}
	else{
		cmd = cmd_parsing(string);
	}
	return (cmd);
}

void sendRPL(int fd, const std::string& servername,
			const std::string& code, const std::string& nick,
			const std::string& message) {
    std::string rpl = ":" + servername + " " + code + " " + nick + " :" + message + "\r\n";
    send(fd, rpl.c_str(), rpl.size(), 0);
	std::cout << rpl << std::endl;
}

void sendRPL(int fd, const std::string& servername,
				const std::string& code, const std::string& nick,
				const std::string& host, const std::string& version,
				const std::string& usermod, const std::string& channelmod) {
    std::string rpl = ":" + servername + " " + code + " " + nick + " " + host + " " + version + " " + usermod + " " + channelmod + "\r\n";
    send(fd, rpl.c_str(), rpl.size(), 0);
	std::cout << rpl << std::endl;
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