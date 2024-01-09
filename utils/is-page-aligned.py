# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    is-page-aligned.py                                 :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/05/30 09:31:59 by vvaucoul          #+#    #+#              #
#    Updated: 2023/05/30 09:32:22 by vvaucoul         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# Check if Page is aligned to 4096 bytes (0x1000)

import sys

def is_page_aligned(addr):
    return (addr & 0xFFF) == 0

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: %s <address>" % sys.argv[0])
        sys.exit(1)
    addr = int(sys.argv[1], 16)
    if is_page_aligned(addr):
        print("0x%x is page aligned" % addr)
    else:
        print("0x%x is not page aligned" % addr)