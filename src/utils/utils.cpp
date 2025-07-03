/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdhallen <jdhallen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/23 12:44:40 by jdhallen          #+#    #+#             */
/*   Updated: 2025/07/03 16:21:43 by jdhallen         ###   ########.fr       */
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
	for (size_t i = 0; i < group.size(); i++){
		if (group[i].size() > 510)
			throw std::runtime_error("Line to long");
	}
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
		std::string trailing = string.substr(colon);
		while (1){
			size_t pos = trailing.find('\n');
			if (pos != std::string::npos)
				trailing.erase(pos);
			else
				break ;
		}
		while (1){
			size_t pos = trailing.find('\r');
			if (pos != std::string::npos)
				trailing.erase(pos);
			else
				break ;
		}
		if (!trailing.empty())
			cmd.push_back(trailing);
	} else {
		cmd = cmd_parsing_base(string);
	}
	return cmd;
}

std::string to_string(int value) {
	std::stringstream ss;
	ss << value;
	return ss.str();
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

