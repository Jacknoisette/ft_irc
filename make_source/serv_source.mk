# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    serv_source.mk                                     :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: jdhallen <jdhallen@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/02/27 14:19:42 by jdhallen          #+#    #+#              #
#    Updated: 2025/07/02 11:23:00 by jdhallen         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

SERV_DIR = src/Server

SRCS_SERV = Server.cpp Server_checker.cpp Server_iteration.cpp \
		Server_debug.cpp Server_command.cpp Server_utils.cpp\
		Channel.cpp Client.cpp 
	