/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   workflow_ide.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/19 11:34:48 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/01/19 16:34:42 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <drivers/device/ide.h>
#include <system/pit.h>
#include <workflows/workflows.h>

#define NUM_SECTORS 10

// Test reading and writing a single sector
void test_ide_single_sector(IDEDevice *dev) {
    char write_buf[SECTOR_SIZE];
    char read_buf[SECTOR_SIZE];
    const char *test_str = "IDE single sector test";

    // Write test
    memset(write_buf, 0, SECTOR_SIZE);
    strcpy(write_buf, test_str);
    ide_write(dev, 1, 1, write_buf);

    // Read test
    memset(read_buf, 0, SECTOR_SIZE);
    ide_read(dev, 1, 1, read_buf);

    // Validation
    if (strcmp(read_buf, test_str) == 0) {
        printk("IDE: Single sector test "_GREEN
               "[OK]"_END
               "\n");
    } else {
        printk("IDE: Single sector test "_RED
               "[KO]"_END
               "\n");
        printk("IDE: Read: [%s]\n", read_buf);
    }
}

// Test reading and writing multiple sectors
void test_ide_multiple_sectors(IDEDevice *dev, uint32_t start_sector, uint8_t num_sectors) {
    uint32_t buffer_size = SECTOR_SIZE * num_sectors;
    char write_buf[buffer_size];
    char read_buf[buffer_size];
    const char *test_str = "IDE multiple sectors test";

    // Préparation des données pour l'écriture
    memset(write_buf, 0, buffer_size);
    for (int i = 0; i < num_sectors; ++i) {
        strcpy(write_buf + i * SECTOR_SIZE, test_str);
    }

    // Ecriture des données sur plusieurs secteurs
    ide_write(dev, start_sector, num_sectors, write_buf);

    // Lecture des données à partir de plusieurs secteurs
    memset(read_buf, 0, buffer_size);
    ide_read(dev, start_sector, num_sectors, read_buf);

    // Validation des données lues
    bool success = true;
    for (int i = 0; i < num_sectors; ++i) {
        if (strcmp(read_buf + i * SECTOR_SIZE, test_str) != 0) {
            success = false;
            break;
        }
    }

    if (success) {
        printk("IDE: Multiple sectors test "_GREEN
               "[OK]"_END
               "\n");
    } else {
        printk("IDE: Multiple sectors test "_RED
               "[KO]"_END
               "\n");
        printk("IDE: Read: [%s]\n", read_buf); // Affiche les données du premier secteur
    }
}

void test_ide_simple(IDEDevice *dev) {

    char buffer_write[512] = {0};
    char buffer_read[512] = {0};

    memset(buffer_write, 0, 512);
    memcpy(buffer_write, "Hello World", 11);

    ide_write(dev, 0, 1, buffer_write);

    memset(buffer_read, 0, 512);
    ide_read(dev, 0, 1, buffer_read);

    if (strcmp(buffer_read, "Hello World") != 0) {
        printk("IDE: Write/Read "_RED
               "[KO]"_END
               "\n");
        printk("IDE: Read: [%s]\n", buffer_read);
    } else {
        printk("IDE: Write/Read "_GREEN
               "[OK]"_END
               "\n");
    }
}

void test_ide_read_write_loop(IDEDevice *dev, uint32_t lba) {
    char write_buf[SECTOR_SIZE];
    char read_buf[SECTOR_SIZE];

    memset(write_buf, 0, SECTOR_SIZE);
    strcpy(write_buf, "IDE read/write loop test");

    // Write/Read loop test (128 iterations)
    for (int i = 0; i < 128; i++) {
        ide_write(dev, lba, 1, write_buf);
        memset(read_buf, 0, SECTOR_SIZE);
        ide_read(dev, lba, 1, read_buf);

        if (strcmp(read_buf, write_buf) != 0) {
            printk("IDE: Read/Write loop test failed at iteration %d\n", i);
            return;
        }
    }
    printk("IDE: Read/Write loop test "_GREEN
           "[OK]" _END "\n");
}

void test_ide_error_handling(IDEDevice *dev) {
    char buffer[SECTOR_SIZE];
    // This LBA is intentionally set beyond the size of a typical disk to induce an error
    uint32_t invalid_lba = 0xFFFFFF;

    int result = ide_read(dev, invalid_lba, 1, buffer);
    if (result < 0) {
        printk("IDE: Error handling "_GREEN
               "[OK]"_END
               "\n");
    } else {
        printk("IDE: Error handling "_RED
               "[KO]"_END
               "\n");
    }
}

void test_ide_sector_limits(IDEDevice *dev, uint32_t lba) {
    char buffer[SECTOR_SIZE];
    // Test de lecture de 0 secteur
    if (ide_read(dev, lba, 0, buffer) >= 0) {
        printk("IDE: Zero sector read test "_RED
               "[KO]" _END "\n");
    }

    // Test de lecture au-delà des limites
    if (ide_read(dev, dev->size + 1, 1, buffer) >= 0) {
        printk("IDE: Out-of-bounds read test "_RED
               "[KO]" _END "\n");
    }
}

void test_ide_hardware_error_handling(IDEDevice *dev, uint32_t bad_lba) {
    char buffer[SECTOR_SIZE];
    if (ide_read(dev, bad_lba, 1, buffer) < 0) {
        printk("IDE: Hardware error handling "_GREEN
               "[OK]" _END "\n");
    } else {
        printk("IDE: Hardware error handling "_RED
               "[KO]" _END "\n");
    }
}

void test_ide_perf(IDEDevice *dev) {
    char buffer[NUM_SECTORS * SECTOR_SIZE];
    counter_t counter;

    counter_start(&counter);

    ide_write(dev, 0, NUM_SECTORS, buffer);
    ide_read(dev, 0, NUM_SECTORS, buffer);

    counter_stop(&counter);

    uint32_t elapsed = counter_get_time(&counter);
    printk("IDE: Performance test: %u sectors read in %u ms\n", NUM_SECTORS, elapsed);
}

void workflow_ide(void) {
    __WORKFLOW_HEADER();

    IDEDevice *dev = ide_get_device(0);
    if (!dev) {
        __WARND("Failed to get IDE Device\n");
        return;
    }

    test_ide_simple(dev);
    test_ide_single_sector(dev);
    test_ide_multiple_sectors(dev, 10, 5);
    test_ide_read_write_loop(dev, 10);
    test_ide_error_handling(dev);
    test_ide_sector_limits(dev, 0);
    test_ide_hardware_error_handling(dev, 0xFFFFFF);
    test_ide_perf(dev);

    __WORKFLOW_FOOTER();
}
