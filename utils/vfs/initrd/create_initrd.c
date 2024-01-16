/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   create_initrd.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/16 13:41:12 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/01/16 16:04:16 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <dirent.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

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

int main() {
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir("initrd_files")) != NULL) {
        int file_count = 0;
        while ((ent = readdir(dir)) != NULL) {
            if (ent->d_type == DT_REG) { // If the entry is a regular file
                file_count++;
            }
        }
        closedir(dir);

        InitrdHeader header;
        header.nfiles = file_count;

        FILE *initrd = fopen("initrd.img", "wb");
        fwrite(&header, sizeof(InitrdHeader), 1, initrd);

        uint32_t offset = sizeof(InitrdHeader) + sizeof(InitrdFileHeader) * header.nfiles;

        // First pass: write headers
        dir = opendir("initrd_files");
        while ((ent = readdir(dir)) != NULL) {
            if (ent->d_type == DT_REG) {
                InitrdFileHeader file_header;
                file_header.magic = 0xBF;
                strncpy(file_header.name, ent->d_name, INITRD_FILE_SIZE);

                struct stat st;
                char filepath[512];
                snprintf(filepath, sizeof(filepath), "initrd_files/%s", ent->d_name);
                stat(filepath, &st);

                file_header.size = st.st_size;
                file_header.offset = offset;

                fwrite(&file_header, sizeof(InitrdFileHeader), 1, initrd);

                offset += file_header.size;
            }
        }
        closedir(dir);

        // Second pass: write file contents
        dir = opendir("initrd_files");
        while ((ent = readdir(dir)) != NULL) {
            if (ent->d_type == DT_REG) {
                char filepath[512];
                snprintf(filepath, sizeof(filepath), "initrd_files/%s", ent->d_name);
                FILE *file = fopen(filepath, "rb");

                struct stat st;
                stat(filepath, &st);
                char *buffer = malloc(st.st_size);
                fread(buffer, st.st_size, 1, file);
                fwrite(buffer, st.st_size, 1, initrd);

                free(buffer);
                fclose(file);
            }
        }
        closedir(dir);

        fclose(initrd);

    } else {
        perror("Could not open directory initrd_files");
        return EXIT_FAILURE;
    }

    return 0;
}