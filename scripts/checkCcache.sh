# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    checkCcache.sh                                     :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/11/17 02:37:55 by vvaucoul          #+#    #+#              #
#    Updated: 2022/11/17 12:02:16 by vvaucoul         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

#!/bin/sh

ls /usr/bin/ccache > /dev/null 2>&1
output=$?

if [ $output -ne 0 ]; then
    echo "false"
    exit 1
else
    echo "true"
    exit 0
fi