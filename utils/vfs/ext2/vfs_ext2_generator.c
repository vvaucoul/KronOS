/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vfs_ext2_generator.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/19 23:43:54 by vvaucoul          #+#    #+#             */
/*   Updated: 2023/10/25 11:54:58 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct initrd_header {
    unsigned char magic; // The magic number is there to check for consistency.
    char name[64];
    unsigned int offset; // Offset in the initrd the file starts.
    unsigned int length; // Length of the file.
};

int main(char argc, char **argv) {
    if (argc < 3 || argc % 2 == 0) {
        printf("%s <source_file> <dest_name> ...\n", argv[0]);
        return 1;
    }

    int nheaders = (argc - 1) / 2;
    if (nheaders > 64) {
        printf("Too many files, max is 64\n");
        return 1;
    }

    struct initrd_header headers[64];
    printf("size of header: %ld\n", sizeof(struct initrd_header));
    unsigned int off = sizeof(struct initrd_header) * 64 + sizeof(int);
    int i;
    for (i = 0; i < nheaders; i++) {
        printf("writing file %s->%s at 0x%x\n", argv[i * 2 + 1], argv[i * 2 + 2], off);
        strcpy(headers[i].name, argv[i * 2 + 2]);
        headers[i].offset = off;
        FILE *stream = fopen(argv[i * 2 + 1], "r");
        if (stream == 0) {
            printf("Error: file not found: %s\n", argv[i * 2 + 1]);
            return 1;
        }
        fseek(stream, 0, SEEK_END);
        headers[i].length = ftell(stream);
        off += headers[i].length;
        fclose(stream);
        headers[i].magic = 0xBF;
    }

    FILE *wstream = fopen("initrd.img", "w");
    if (wstream == NULL) {
        perror("Could not open output file");
        return 1;
    }

    unsigned char *data = (unsigned char *)malloc(off);
    fwrite(&nheaders, sizeof(int), 1, wstream);
    fwrite(headers, sizeof(struct initrd_header), 64, wstream);

    for (i = 0; i < nheaders; i++) {
        FILE *stream = fopen(argv[i * 2 + 1], "r");
        if (stream == NULL) {
            perror("Could not open input file");
            fclose(wstream);
            return 1;
        }
        unsigned char *buf = (unsigned char *)malloc(headers[i].length);
        fread(buf, 1, headers[i].length, stream);
        fwrite(buf, 1, headers[i].length, wstream);
        fclose(stream);
        free(buf);
    }

    fclose(wstream);
    free(data);

    return 0;
}