# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    is-page-aligned.py                                 :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/05/30 09:31:59 by vvaucoul          #+#    #+#              #
#    Updated: 2024/10/19 16:11:08 by vvaucoul         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# Check if Page is aligned to 4096 bytes (0x1000)

import sys

def is_page_aligned(addr):
	return (addr & 0xFFF) == 0

def previous_aligned_address(addr):
	return addr & ~0xFFF

def next_aligned_address(addr):
	return (addr + 0xFFF) & ~0xFFF

if __name__ == "__main__":
	if len(sys.argv) != 2:
		print("Usage: %s <address>" % sys.argv[0])
		sys.exit(1)
	try:
		addr = int(sys.argv[1], 16)
	except ValueError:
		print("Invalid address format. Please provide a hexadecimal address.")
		sys.exit(1)
	
	if is_page_aligned(addr):
		print("0x%x is page aligned" % addr)
	else:
		prev_aligned_addr = previous_aligned_address(addr)
		aligned_addr = next_aligned_address(addr)
		print("0x%x is not page aligned" % addr)
		print("Previous aligned address is 0x%x" % prev_aligned_addr)
		print("Next aligned address is 0x%x" % aligned_addr)