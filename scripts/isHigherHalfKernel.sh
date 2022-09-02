# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    isHigherHalfKernel.sh                              :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/09/02 11:49:13 by vvaucoul          #+#    #+#              #
#    Updated: 2022/09/02 12:08:11 by vvaucoul         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

#!/bin/sh

grep -hnr "__HIGHER_HALF_KERNEL__" kernel/includes/kernel.h > output
cat output | tail -c 6 | sed 's/ //g' > output2

if [ "$(cat output2)" == true ]; then
    echo "true"
    rm output output2
    exit 0
else
    echo "false"
    rm output output2
    exit 1
fi