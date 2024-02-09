# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    watch_disk_content.sh                              :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/02/09 21:47:53 by vvaucoul          #+#    #+#              #
#    Updated: 2024/02/09 21:48:35 by vvaucoul         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

#!/bin/bash

# Watch disk content
watch --color --interval=0.1 './read_disk_content.sh ; xxd -cols 28 .sector.bin'
