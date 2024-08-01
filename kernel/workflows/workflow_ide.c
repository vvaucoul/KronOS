/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   workflow_ide.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/19 11:34:48 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/08/01 18:16:34 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <drivers/device/ide.h>
#include <mm/mm.h>
#include <system/pit.h>
#include <system/random.h>
#include <workflows/workflows.h>

#include <string.h>
#include <stdio.h>
#include <time.h>

#define NUM_SECTORS 10

// // Test reading and writing a single sector
// void test_ide_single_sector(IDEDevice *dev) {
//     char write_buf[SECTOR_SIZE];
//     char read_buf[SECTOR_SIZE];
//     const char *test_str = "IDE single sector test";

//     // Write test
//     memset(write_buf, 0, SECTOR_SIZE);
//     strcpy(write_buf, test_str);
//     ide_write(dev, 1, 1, write_buf);

//     // Read test
//     memset(read_buf, 0, SECTOR_SIZE);
//     ide_read(dev, 1, 1, read_buf);

//     // Validation
//     if (strcmp(read_buf, test_str) == 0) {
//         printk("IDE: Single sector test "_GREEN
//                "[OK]"_END
//                "\n");
//     } else {
//         printk("IDE: Single sector test "_RED
//                "[KO]"_END
//                "\n");
//         printk("IDE: Read: [%s]\n", read_buf);
//     }
// }

// // Test reading and writing multiple sectors
// void test_ide_multiple_sectors(IDEDevice *dev, uint32_t start_sector, uint8_t num_sectors) {
//     uint32_t buffer_size = SECTOR_SIZE * num_sectors;
//     char write_buf[buffer_size];
//     char read_buf[buffer_size];
//     const char *test_str = "IDE multiple sectors test";

//     // Préparation des données pour l'écriture
//     memset(write_buf, 0, buffer_size);
//     for (int i = 0; i < num_sectors; ++i) {
//         strcpy(write_buf + i * SECTOR_SIZE, test_str);
//     }

//     // Ecriture des données sur plusieurs secteurs
//     ide_write(dev, start_sector, num_sectors, write_buf);

//     // Lecture des données à partir de plusieurs secteurs
//     memset(read_buf, 0, buffer_size);
//     ide_read(dev, start_sector, num_sectors, read_buf);

//     // Validation des données lues
//     bool success = true;
//     for (int i = 0; i < num_sectors; ++i) {
//         if (strcmp(read_buf + i * SECTOR_SIZE, test_str) != 0) {
//             success = false;
//             break;
//         }
//     }

//     if (success) {
//         printk("IDE: Multiple sectors test "_GREEN
//                "[OK]"_END
//                "\n");
//     } else {
//         printk("IDE: Multiple sectors test "_RED
//                "[KO]"_END
//                "\n");
//         printk("IDE: Read: [%s]\n", read_buf); // Affiche les données du premier secteur
//     }
// }

// void test_ide_simple(IDEDevice *dev) {

//     char buffer_write[512] = {0};
//     char buffer_read[512] = {0};

//     memset(buffer_write, 0, 512);
//     memcpy(buffer_write, "Hello World", 11);

//     ide_write(dev, 0, 1, buffer_write);

//     memset(buffer_read, 0, 512);
//     ide_read(dev, 0, 1, buffer_read);

//     if (strcmp(buffer_read, "Hello World") != 0) {
//         printk("IDE: Write/Read "_RED
//                "[KO]"_END
//                "\n");
//         printk("IDE: Read: [%s]\n", buffer_read);
//     } else {
//         printk("IDE: Write/Read "_GREEN
//                "[OK]"_END
//                "\n");
//     }
// }

// void test_ide_read_write_loop(IDEDevice *dev, uint32_t lba) {
//     char write_buf[SECTOR_SIZE];
//     char read_buf[SECTOR_SIZE];

//     memset(write_buf, 0, SECTOR_SIZE);
//     strcpy(write_buf, "IDE read/write loop test");

//     // Write/Read loop test (128 iterations)
//     for (int i = 0; i < 128; i++) {
//         ide_write(dev, lba, 1, write_buf);
//         memset(read_buf, 0, SECTOR_SIZE);
//         ide_read(dev, lba, 1, read_buf);

//         if (strcmp(read_buf, write_buf) != 0) {
//             printk("IDE: Read/Write loop test failed at iteration %d\n", i);
//             return;
//         }
//     }
//     printk("IDE: Read/Write loop test "_GREEN
//            "[OK]" _END "\n");
// }

// void test_ide_error_handling(IDEDevice *dev) {
//     char buffer[SECTOR_SIZE];
//     uint32_t invalid_lba = 0xFFFFFF;

//     int result = ide_read(dev, invalid_lba, 1, buffer);
//     if (result < 0) {
//         printk("IDE: Error handling "_GREEN
//                "[OK]" _END "\n");
//     } else {
//         printk("IDE: Error handling "_RED
//                "[KO]" _END "\n");
//     }
// }

// void test_ide_sector_limits(IDEDevice *dev, uint32_t lba) {
//     char buffer[SECTOR_SIZE];
//     // Test de lecture de 0 secteur
//     if (ide_read(dev, lba, 0, buffer) >= 0) {
//         printk("IDE: Zero sector read test "_RED
//                "[KO]" _END "\n");
//     }

//     // Test de lecture au-delà des limites
//     if (ide_read(dev, dev->size + 1, 1, buffer) >= 0) {
//         printk("IDE: Out-of-bounds read test "_RED
//                "[KO]" _END "\n");
//     }
// }

// void test_ide_hardware_error_handling(IDEDevice *dev, uint32_t bad_lba) {
//     char buffer[SECTOR_SIZE];
//     if (ide_read(dev, bad_lba, 1, buffer) < 0) {
//         printk("IDE: Hardware error handling "_GREEN
//                "[OK]" _END "\n");
//     } else {
//         printk("IDE: Hardware error handling "_RED
//                "[KO]" _END "\n");
//     }
// }

// void test_ide_perf(IDEDevice *dev) {
//     char buffer[NUM_SECTORS * SECTOR_SIZE];
//     counter_t counter;

//     counter_start(&counter);

//     ide_write(dev, 0, NUM_SECTORS, buffer);
//     ide_read(dev, 0, NUM_SECTORS, buffer);

//     counter_stop(&counter);

//     uint32_t elapsed = counter_get_time(&counter);
//     printk("IDE: Performance test: %u sectors read in %u ms\n", NUM_SECTORS, elapsed);
// }

// void test_ide_large_transfer(IDEDevice *dev, uint32_t start_sector, uint32_t num_sectors) {
//     uint32_t buffer_size = SECTOR_SIZE * num_sectors; // 512 * 100 = 51200 = 50KB
//     char *write_buf = (char *)kmalloc(buffer_size);
//     char *read_buf = (char *)kmalloc(buffer_size);

//     // Initialisation du buffer d'écriture avec des motifs de données identifiables
//     for (uint32_t i = 0; i < num_sectors; ++i) {
//         memset(write_buf + i * SECTOR_SIZE, i % 256, SECTOR_SIZE);
//     }

//     // Effectuer l'opération d'écriture
//     printk("IDE: Starting large write test - Start Sector: %u, Number of Sectors: %u, Device Size: %u\n", start_sector, num_sectors, dev->size);
//     if (ide_write(dev, start_sector, num_sectors, write_buf) != 0) {
//         printk("IDE: Write operation failed during large transfer test\n");
//         kfree(write_buf);
//         kfree(read_buf);
//         return;
//     }

//     // Effectuer l'opération de lecture
//     printk("IDE: Starting large read test - Start Sector: %u, Number of Sectors: %u\n", start_sector, num_sectors);
//     if (ide_read(dev, start_sector, num_sectors, read_buf) != 0) {
//         printk("IDE: Read operation failed during large transfer test\n");
//         kfree(write_buf);
//         kfree(read_buf);
//         return;
//     }

//     // Validation des données lues par rapport aux données écrites
//     for (uint32_t i = 0; i < num_sectors; ++i) {
//         if (memcmp(write_buf + i * SECTOR_SIZE, read_buf + i * SECTOR_SIZE, SECTOR_SIZE) != 0) {
//             printk("IDE: Large transfer test "_RED
//                    "[KO]" _END "\n");
//             printk("IDE: Mismatch at sector %u\n", start_sector + i);
//             printk("IDE: Expected: ");
//             for (uint32_t j = 0; j < SECTOR_SIZE && j < 80; ++j) {
//                 printk("%02x ", write_buf[i * SECTOR_SIZE + j]);
//             }
//             printk("\nIDE: Actual: ");
//             for (uint32_t j = 0; j < SECTOR_SIZE && j < 80; ++j) {
//                 printk("%02x ", read_buf[i * SECTOR_SIZE + j]);
//             }
//             printk("\n");
//             kpause();
//             kfree(write_buf);
//             kfree(read_buf);
//             return;
//         }
//     }

//     printk("IDE: Large transfer test "_GREEN
//            "[OK]" _END "\n");

//     kfree(write_buf);
//     kfree(read_buf);
// }

// void test_ide_concurrent_access(IDEDevice *dev) {
//     char write_buf[SECTOR_SIZE];
//     char read_buf[SECTOR_SIZE];

//     strcpy(write_buf, "Concurrent access test");

//     for (int i = 0; i < 4; ++i) {
//         ide_write(dev, i, 1, write_buf);
//     }

//     for (int i = 0; i < 4; ++i) {
//         ide_read(dev, i, 1, read_buf);
//         if (strcmp(write_buf, read_buf) != 0) {
//             printk("IDE: Concurrent access test "_RED
//                    "[KO]" _END "\n");
//             return;
//         }
//     }

//     printk("IDE: Concurrent access test "_GREEN
//            "[OK]" _END "\n");
// }

// void test_ide_simple_read_write(IDEDevice *dev) {
//     const uint32_t test_size = 2048; // Taille du buffer de test
//     char write_buf[test_size];
//     char read_buf[test_size];

//     // Initialisation du buffer de test avec des motifs reconnaissables
//     for (uint32_t i = 0; i < test_size; ++i) {
//         write_buf[i] = (char)(i % 256);
//     }

//     // Liste des offsets et tailles à tester
//     struct {
//         uint32_t offset;
//         uint32_t size;
//     } test_cases[] = {
//         {0, test_size},
//         {512, 1024},
//         {1024, 512},
//         {1, 512},
//         {512, 1},
//         {1023, 2048},
//         {0, 1},
//         {dev->size * SECTOR_SIZE - test_size, test_size}, // Near end of disk
//     };

//     int num_cases = sizeof(test_cases) / sizeof(test_cases[0]);

//     for (int i = 0; i < num_cases; ++i) {
//         uint32_t offset = test_cases[i].offset;
//         uint32_t size = test_cases[i].size;

//         // Effacer le buffer de lecture
//         memset(read_buf, 0, test_size);

//         // Écriture simple
//         if (ide_simple_write(dev, offset, size, write_buf) != 0) {
//             printk("IDE: test case %d - Write failed\n", i);
//             continue;
//         }

//         // Lecture simple
//         if (ide_simple_read(dev, offset, size, read_buf) != 0) {
//             printk("IDE: test case %d - Read failed\n", i);
//             continue;
//         }

//         // Validation des données
//         if (memcmp(write_buf, read_buf, size) != 0) {
//             printk("IDE: test case %d - Data mismatch\n", i);
//         } else {
//             printk("IDE: test case %d - Data match "_GREEN
//                    "[OK]\n"_END,
//                    i);
//         }
//     }

//     // Test des conditions d'erreur
//     printk("IDE: Testing invalid parameters...\n");

//     // Lecture avec offset au-delà de la taille du disque
//     if (ide_simple_read(dev, dev->size * SECTOR_SIZE, test_size, read_buf) < 0) {
//         printk("IDE: Invalid read beyond disk size "_GREEN
//                "[OK]\n"_END);
//     } else {
//         printk("IDE: Invalid read beyond disk size "_RED
//                "[KO]\n"_END);
//     }

//     // Écriture avec offset au-delà de la taille du disque
//     if (ide_simple_write(dev, dev->size * SECTOR_SIZE, test_size, write_buf) < 0) {
//         printk("IDE: Invalid write beyond disk size "_GREEN
//                "[OK]\n"_END);
//     } else {
//         printk("IDE: Invalid write beyond disk size "_RED
//                "[KO]\n"_END);
//     }

//     // Lecture de 0 octets
//     if (ide_simple_read(dev, 0, 0, read_buf) < 0) {
//         printk("IDE: Invalid read with zero size "_GREEN
//                "[OK]\n"_END);
//     } else {
//         printk("IDE: Invalid read with zero size "_RED
//                "[KO]\n"_END);
//     }

//     // Écriture de 0 octets
//     if (ide_simple_write(dev, 0, 0, write_buf) < 0) {
//         printk("IDE: Invalid write with zero size "_GREEN
//                "[OK]\n"_END);
//     } else {
//         printk("IDE: Invalid write with zero size "_RED
//                "[KO]\n"_END);
//     }
// }

// void test_ide_unaligned_sectors(IDEDevice *dev) {
//     char write_buf[SECTOR_SIZE * 2];
//     char read_buf[SECTOR_SIZE * 2];
//     const char *test_str = "IDE unaligned sectors test";

//     // Write test
//     memset(write_buf, 0, SECTOR_SIZE * 2);
//     strcpy(write_buf + 100, test_str); // Write at an unaligned offset
//     ide_write(dev, 1, 2, write_buf);

//     // Read test
//     memset(read_buf, 0, SECTOR_SIZE * 2);
//     ide_read(dev, 1, 2, read_buf);

//     // Validation
//     if (strcmp(read_buf + 100, test_str) == 0) {
//         printk("IDE: Unaligned sectors test "_GREEN
//                "[OK]" _END "\n");
//     } else {
//         printk("IDE: Unaligned sectors test "_RED
//                "[KO]" _END "\n");
//         printk("IDE: Read: [%s]\n", read_buf + 100);
//     }
// }

// void test_ide_partial_sector_write(IDEDevice *dev) {
//     char write_buf[SECTOR_SIZE];
//     char read_buf[SECTOR_SIZE];
//     const char *test_str = "IDE partial sector write test";

//     // Write test
//     memset(write_buf, 0, SECTOR_SIZE);
//     strncpy(write_buf, test_str, strlen(test_str)); // Partial write
//     ide_write(dev, 1, 1, write_buf);

//     // Read test
//     memset(read_buf, 0, SECTOR_SIZE);
//     ide_read(dev, 1, 1, read_buf);

//     // Validation
//     if (strncmp(read_buf, test_str, strlen(test_str)) == 0) {
//         printk("IDE: Partial sector write test "_GREEN
//                "[OK]" _END "\n");
//     } else {
//         printk("IDE: Partial sector write test "_RED
//                "[KO]" _END "\n");
//         printk("IDE: Read: [%s]\n", read_buf);
//     }
// }

// void test_ide_random_access(IDEDevice *dev) {
//     char write_buf[SECTOR_SIZE];
//     char read_buf[SECTOR_SIZE];
//     const char *test_str = "IDE random access test";
//     uint32_t rand_sector;

//     // Initialize random seed
//     srand(pit_get_ticks());

//     // Stress test with random accesses
//     for (int i = 0; i < 100; ++i) {
//         rand_sector = rand() % dev->size; // Random sector within device size
//         memset(write_buf, 0, SECTOR_SIZE);
//         strcpy(write_buf, test_str);

//         ide_write(dev, rand_sector, 1, write_buf);
//         memset(read_buf, 0, SECTOR_SIZE);
//         ide_read(dev, rand_sector, 1, read_buf);

//         if (strcmp(read_buf, test_str) != 0) {
//             printk("IDE: Random access test failed at sector %u\n", rand_sector);
//             return;
//         }
//     }
//     printk("IDE: Random access test "_GREEN
//            "[OK]" _END "\n");
// }

// void test_ide_stress_long_term(IDEDevice *dev) {
//     char write_buf[SECTOR_SIZE];
//     char read_buf[SECTOR_SIZE];
//     const char *test_str = "IDE long term stress test";
//     uint32_t rand_sector;

//     int mi = 10000;
//     srand(pit_get_ticks());

//     for (int i = 0; i < mi; ++i) {
//         rand_sector = rand() % dev->size;
//         memset(write_buf, 0, SECTOR_SIZE);
//         strcpy(write_buf, test_str);

//         workflow_loading((i * 100 / mi));

//         ide_write(dev, rand_sector, 1, write_buf);
//         memset(read_buf, 0, SECTOR_SIZE);
//         ide_read(dev, rand_sector, 1, read_buf);

//         if (strcmp(read_buf, test_str) != 0) {
//             printk("IDE: Long term stress test failed at iteration %d, sector %u\n", i, rand_sector);
//             return;
//         }
//     }
//     workflow_loading(100);
//     printk("IDE: Long term stress test "_GREEN
//            "[OK]" _END "\n");
// }

// void test_ide_performance(IDEDevice *dev) {
//     char buffer[NUM_SECTORS * SECTOR_SIZE];
//     counter_t counter;
//     uint32_t iterations = 1000;

//     counter_start(&counter);

//     for (uint32_t i = 0; i < iterations; ++i) {
//         workflow_loading((i * 100 / iterations));
//         ide_write(dev, 0, NUM_SECTORS, buffer);
//         ide_read(dev, 0, NUM_SECTORS, buffer);
//     }
//     workflow_loading(100);

//     counter_stop(&counter);

//     uint32_t elapsed = counter_get_time(&counter);
//     printk("IDE: Performance test: %u iterations, %u sectors each, in %u ms\n", iterations, NUM_SECTORS, elapsed);
// }

// void workflow_ide(void) {
//     __WORKFLOW_HEADER();

//     IDEDevice *dev = ide_get_device(0);
//     if (!dev) {
//         __WARND("Failed to get IDE Device\n");
//         return;
//     }

//     test_ide_simple(dev);
//     test_ide_single_sector(dev);
//     test_ide_multiple_sectors(dev, 10, 5);

//     test_ide_read_write_loop(dev, 10);
//     test_ide_error_handling(dev);
//     test_ide_sector_limits(dev, 0);
//     test_ide_hardware_error_handling(dev, 0xFFFFFF);
//     test_ide_perf(dev);

//     test_ide_large_transfer(dev, 20, 100);
//     test_ide_large_transfer(dev, 100, 100);
//     test_ide_large_transfer(dev, 200, 100);

//     test_ide_concurrent_access(dev);
//     test_ide_simple_read_write(dev);

//     test_ide_unaligned_sectors(dev);
//     test_ide_partial_sector_write(dev);
//     test_ide_random_access(dev);

// #if __WHUGE_TESTS__ == 1
//     test_ide_stress_long_term(dev);
//     test_ide_performance(dev);
// #else
//     __WARND("Huge tests are disable");
// #endif 

//     __WORKFLOW_FOOTER();
// }
