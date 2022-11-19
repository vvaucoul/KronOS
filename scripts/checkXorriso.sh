# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    checkXorriso.sh                                    :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/09/02 13:12:59 by vvaucoul          #+#    #+#              #
#    Updated: 2022/11/17 02:44:16 by vvaucoul         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

#!/bin/sh

ls /bin/xorriso > /dev/null 2>&1
output=$?

if [ $output -ne 0 ]; then
    echo "false"
    exit 1
else
    echo "true"
    exit 0
fi