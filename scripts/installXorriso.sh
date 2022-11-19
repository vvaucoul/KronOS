# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    installXorriso.sh                                  :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/09/02 13:23:58 by vvaucoul          #+#    #+#              #
#    Updated: 2022/11/17 02:42:19 by vvaucoul         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

#!/bin/sh

# This is a minimal set of ANSI/VT100 color codes
_END='\033[0m'
_BOLD='\033[1m'
_UNDER='\033[4m'
_REV='\033[7m'

# Colors
_GREY='\033[30m'
_RED='\033[31m'
_GREEN='\033[32m'
_YELLOW='\033[33m'
_BLUE='\033[34m'
_PURPLE='\033[35m'
_CYAN='\033[36m'
_WHITE='\033[37m'

# Inverted, i.e. colored backgrounds
_IGREY='\033[40m'
_IRED='\033[41m'
_IGREEN='\033[42m'
_IYELLOW='\033[43m'
_IBLUE='\033[44m'
_IPURPLE='\033[45m'
_ICYAN='\033[46m'
_IWHITE='\033[47m'

# Light colors
_LGREY='\033[90m'
_LRED='\033[91m'
_LGREEN='\033[92m'
_LYELLOW='\033[93m'
_LBLUE='\033[94m'
_LPURPLE='\033[95m'
_LCYAN='\033[96m'
_LWHITE='\033[97m'

# Others
_DIM='\033[2m'
_CLEAR_LINE='\033[K'

pwd=$(pwd)
xorriso_dir=dependencies
xorriso=xorriso-1.4.6
xorriso_installed=$(sh scripts/checkXorriso.sh)

printf "$_LWHITE- Check package $_LYELLOW[XORRISO] $_LYELLOW> $_END$_DIM$xorriso or latest version$_END$_LYELLOW <$_END\n"

if [ $xorriso_installed = "true" ]; then
    printf "$_LWHITE-$_LYELLOW [XORRISO]$_LGREEN already installed $_END$_DIM---------$_END $_LGREEN[$_LWHITE✓$_LGREEN]$_END\n\n"
    exit 0
else
    printf "$_LWHITE-$_LYELLOW [XORRISO]$_LRED not installed $_LRED[$_LWHITE✗$_LRED]$_END\n\n"
    sleep 1
    printf "$_LWHITE-$_END$_DIM Compiling$_END$_LYELLOW [XORRISO] $_END$_DIM---$_END $_LYELLOW[$_LWHITE⚠️ $_LYELLOW]$_END $_LYELLOW> $_END$_DIM$pwd/$xorriso/$_END$_LYELLOW <$_END\n"
    tar xf $xorriso_dir/$xorriso.tar.gz -C $xorriso_dir
    cd $xorriso_dir/$xorriso && ./configure --prefix=/usr > /dev/null 2>&1 && make -j$(nproc) > /dev/null 2>&1
    printf "$_LWHITE- $xorriso $_END$_DIM---------$_END $_LGREEN[$_LWHITE✓$_LGREEN]$_END\n"
    sleep 1
    exit 0
fi