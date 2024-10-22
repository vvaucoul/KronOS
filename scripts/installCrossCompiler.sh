# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    installCrossCompiler.sh                            :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/10/14 15:22:18 by vvaucoul          #+#    #+#              #
#    Updated: 2024/10/14 16:11:11 by vvaucoul         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

#!/bin/bash

# Mise à jour des paquets et installation des prérequis
sudo apt update
sudo apt install build-essential bison flex libgmp3-dev libmpc-dev libmpfr-dev texinfo wget -y

# Définition des variables d'environnement
sudo mkdir -p /opt/cross
sudo chown $USER:$USER /opt/cross
export PREFIX="/opt/cross"
export TARGET=i386-elf # Target est i386-elf (32 bits)
export PATH="$PREFIX/bin:$PATH"

# Téléchargement des sources de binutils et GCC
wget https://ftp.gnu.org/gnu/binutils/binutils-2.43.1.tar.xz
wget https://ftp.gnu.org/gnu/gcc/gcc-14.2.0/gcc-14.2.0.tar.xz

# Extraction des archives
tar -xf binutils-2.43.1.tar.xz
tar -xf gcc-14.2.0.tar.xz

# Création des répertoires de build
mkdir build-binutils
mkdir build-gcc

# Construction de binutils
cd build-binutils
../binutils-2.43.1/configure --target=$TARGET --prefix=$PREFIX --with-sysroot --disable-nls --disable-werror
make -j$(nproc)
make install
cd ..

# Construction de GCC
cd build-gcc
../gcc-14.2.0/configure --target=$TARGET --prefix=$PREFIX --disable-nls --enable-languages=c --without-headers
make all-gcc -j$(nproc)
make all-target-libgcc -j$(nproc)
make install-gcc
make install-target-libgcc
cd ..

# Nettoyage des fichiers d'installation
echo "Nettoyage des fichiers..."
rm -rf binutils-2.43.1 binutils-2.43.1.tar.xz gcc-14.2.0 gcc-14.2.0.tar.xz build-binutils build-gcc

echo "Installation du cross-compilateur terminée !"

# Vérification de l'installation
$PREFIX/bin/$TARGET-gcc --version

echo "Le cross-compilateur est prêt à être utilisé."
