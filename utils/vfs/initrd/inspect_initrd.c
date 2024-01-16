/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   inspect_initrd.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/16 13:53:44 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/01/16 16:00:12 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define INITRD_FILE_SIZE 64

typedef struct {
    uint32_t nfiles;
} __attribute__((packed)) InitrdHeader;

typedef struct {
    uint8_t magic;
    char name[INITRD_FILE_SIZE];
    uint32_t size;
    uint32_t offset;
} __attribute__((packed)) InitrdFileHeader;

static void __display_files_content(FILE *initrd, InitrdFileHeader *file_header) {
    fseek(initrd, file_header->offset, SEEK_SET);

    uint8_t *content = malloc(file_header->size);
    if (content) {
        fread(content, file_header->size, 1, initrd);

        printf("\t- [%s] %s:\n", file_header->name, content);

        free(content);
    } else {
        perror("Memory allocation error");
    }
}

int main() {
    FILE *initrd = fopen("initrd.img", "rb");
    if (!initrd) {
        perror("Could not open initrd.img");
        return EXIT_FAILURE;
    }

    InitrdHeader header;
    fread(&header, sizeof(InitrdHeader), 1, initrd);

    printf("Inspecting the initrd image\n");
    printf("Number of files: %d\n\n", header.nfiles);

    printf("File table in the initrd:\n");
    printf("| Magic | File Name                           | Size (bytes)    | Offset   | Content  |\n");
    printf("|-------|-------------------------------------|-----------------|----------|----------|\n");

    InitrdFileHeader *file_headers = malloc(header.nfiles * sizeof(InitrdFileHeader));
    if (!file_headers) {
        perror("Memory allocation error");
        fclose(initrd);
        return EXIT_FAILURE;
    }

    for (uint32_t i = 0; i < header.nfiles; i++) {
        fread(&file_headers[i], sizeof(InitrdFileHeader), 1, initrd);

        printf("| %5d | %-35s | %15d | %8d | %8s |\n",
               file_headers[i].magic,
               file_headers[i].name,
               file_headers[i].size,
               file_headers[i].offset,
               "...");
    }

    printf("|-------|-------------------------------------|-----------------|----------|----------|\n");

    rewind(initrd);
    fread(&header, sizeof(InitrdHeader), 1, initrd);

    for (uint32_t i = 0; i < header.nfiles; i++) {
        __display_files_content(initrd, &file_headers[i]);
    }

    free(file_headers);
    fclose(initrd);

    return 0;
}