# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    mk-loader.py                                       :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/11/17 00:52:26 by vvaucoul          #+#    #+#              #
#    Updated: 2022/11/17 00:56:36 by vvaucoul         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

import sys

def __main__():
    if len(sys.argv) != 3:
        print("Usage: python mk-loader.py <file> <count>")
        return
    file = sys.argv[1]
    nb_files = int(sys.argv[2])
    print(file)

if __name__ == "__main__":
    __main__()