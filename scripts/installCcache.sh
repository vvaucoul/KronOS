# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    installCcache.sh                                   :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/11/17 02:48:22 by vvaucoul          #+#    #+#              #
#    Updated: 2022/11/17 12:02:01 by vvaucoul         ###   ########.fr        #
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
ccache_dir=dependencies
ccache=ccache-4.7.3-linux-x86_64.tar.xz
ccache_installed=$(sh scripts/checkCcache.sh)

printf "$_LWHITE- Check package $_LYELLOW[CCACHE] $_LYELLOW> $_END$_DIM$ccache or latest version$_END$_LYELLOW <$_END\n"

if [ "$ccache_installed" = "true" ]; then
    printf "$_LWHITE-$_LYELLOW [CCACHE]$_LGREEN already installed $_END$_DIM----------$_END $_LGREEN[$_LWHITE✓$_LGREEN]$_END\n\n"
    exit 0
else
    printf "$_LWHITE-$_LYELLOW [CCACHE]$_LRED not installed $_LRED[$_LWHITE✗$_LRED]$_END\n\n"
    sleep 1
    printf "$_LWHITE-$_END$_DIM Compiling$_END$_LYELLOW [CCACHE] $_END$_DIM---$_END $_LYELLOW[$_LWHITE⚠️ $_LYELLOW]$_END $_LYELLOW> $_END$_DIM$pwd/$ccache/$_END$_LYELLOW <$_END\n"
    tar xf $ccache_dir/$ccache -C $ccache_dir
    mv $ccache_dir/ccache-4.7.3-linux-x86_64 $ccache_dir/ccache
    printf "$_LWHITE- $ccache $_END$_DIM----$_END $_LGREEN[$_LWHITE✓$_LGREEN]$_END\n"
    exit 0
fi