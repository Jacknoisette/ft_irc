# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    source.mk                                          :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: jdhallen <jdhallen@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/02/27 14:20:43 by jdhallen          #+#    #+#              #
#    Updated: 2025/06/11 18:17:29 by jdhallen         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

include make_source/serv_source.mk
include make_source/utils_source.mk

vpath %.cpp $(SERV_DIR) $(UTILS_DIR)