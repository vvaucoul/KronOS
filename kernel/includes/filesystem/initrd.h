/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   initrd.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/20 09:43:59 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/10/27 17:36:48 by vvaucoul         ###   ########.fr       */
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

#define FS_OPEN 0x01

typedef struct {
    uint32_t nfiles;
} InitrdHeader;

typedef struct {
    uint8_t magic;
    int8_t name[64];
    uint32_t offset;
    uint32_t length;
} InitrdFileHeader;

extern Ext2Inode *initrd_init(uint32_t location);

// Debug
extern void initrd_debug_read_disk(void);
extern void initrd_display_hierarchy(void);
#endif /* INITRD_H */