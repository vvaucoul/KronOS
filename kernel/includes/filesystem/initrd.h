/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   initrd.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/20 09:43:59 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/10/27 12:58:10 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef INITRD_H
#define INITRD_H

#include <filesystem/ext2/ext2.h>

/**
 * Initrd
 * 
 * Initrd est une technique utilisée pendant le processus de démarrage du système d'exploitation.
 * Il charge une image de disque en mémoire qui est ensuite montée comme système de fichiers racine.
 * Le noyau utilise ensuite ce système de fichiers pour démarrer le reste du système.
 * L'image initrd est généralement utilisée pour charger les modules du noyau nécessaires
 * pour monter le véritable système de fichiers racine, qui pourrait être sur un disque dur, un réseau, etc.
 */

typedef struct {
    uint32_t nfiles;
} initrd_header_t;

typedef struct {
    uint8_t magic;
    int8_t name[64];
    uint32_t offset;
    uint32_t length;
} initrd_file_header_t;

extern Ext2Inode *initialise_initrd(uint32_t location);
extern void read_disk(void);

#endif /* INITRD_H */