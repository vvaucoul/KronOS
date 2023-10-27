/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   read_img_disk.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/27 13:50:36 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/10/27 13:51:11 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>

int main(int argc, char *argv[]) {
    // Vérifier que le chemin vers le fichier a été fourni en argument
    if (argc < 2) {
        printf("Usage: %s chemin_vers_votre_fichier.img\n", argv[0]);
        return 1;
    }

    // Ouvrir le fichier .img en mode lecture
    FILE *file = fopen(argv[1], "rb");
    if (!file) {
        printf("Impossible d'ouvrir le fichier %s\n", argv[1]);
        return 1;
    }

    // Lire le contenu du fichier
    char buffer[1024];
    size_t bytesRead;
    while ((bytesRead = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        // Afficher le contenu du fichier
        for (size_t i = 0; i < bytesRead; i++) {
            printf("%c", buffer[i]);
        }
    }

    // Fermer le fichier
    fclose(file);

    return 0;
}