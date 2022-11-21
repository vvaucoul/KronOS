# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    installHephaistos.sh                               :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/11/21 12:20:54 by vvaucoul          #+#    #+#              #
#    Updated: 2022/11/21 12:32:16 by vvaucoul         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

#!/bin/bash

# Install Hephaistos

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
Hephaistos_dir=.
Hephaistos=Hephaistos
HephaistosURL="https://github.com/vvaucoul/Hephaistos.git"
Hephaistos_installed=$(sh scripts/checkHephaistos.sh)

printf "$_LWHITE- Check library $_LYELLOW[HEPHAISTOS] $_LYELLOW> $_END$_DIM$ccache or latest version$_END$_LYELLOW <$_END\n"

if [ "$Hephaistos_installed" = "false" ]; then
    printf "$_LWHITE- Install $_LYELLOW[HEPHAISTOS] $_LYELLOW> $_END$_DIM$ccache or latest version$_END$_LYELLOW <$_END\n"
    if [ ! -d "$Hephaistos_dir" ]; then
        mkdir $Hephaistos_dir
    fi
    cd $Hephaistos_dir
    git clone $HephaistosURL
    cd $Hephaistos
    make
    cd $pwd
    printf "$_LWHITE- $_LYELLOW[HEPHAISTOS] $_LYELLOW> $_END$_DIM$ccache or latest version$_END$_LYELLOW <$_END$_LWHITE installed$_END\n"
else
    printf "$_LWHITE- $_LYELLOW[HEPHAISTOS] $_LYELLOW> $_END$_DIM$ccache or latest version$_END$_LYELLOW <$_END$_LWHITE already installed$_END\n"
fi