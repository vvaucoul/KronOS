# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    checkHephaistos.sh                                 :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/11/21 12:21:18 by vvaucoul          #+#    #+#              #
#    Updated: 2022/11/21 12:41:09 by vvaucoul         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

#!/bin/bash

# Check if Hephaistos is installed

ls ./Hephaistos/Makefile > /dev/null 2>&1
output=$?

if [ $output -ne 0 ]; then
    echo "false"
    exit 1
else
    echo "true"
    exit 0
fi