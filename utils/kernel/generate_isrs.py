# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    generate_isrs.py                                   :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/12/08 21:32:47 by vvaucoul          #+#    #+#              #
#    Updated: 2022/12/08 21:40:38 by vvaucoul         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

import os


def generateISRS():
    isrs_with_errors_code = [8, 10, 11, 12, 13, 14, 17, 21]

    with open("isrs.txt", "w") as f:
        for i in range(0, 32):
            if i in isrs_with_errors_code:
                f.write("ISR({}) = ✔️\n".format(i, i))
            else:
                f.write("ISR({}) = ❌\n".format(i, i))


if __name__ == "__main__":
    generateISRS()
