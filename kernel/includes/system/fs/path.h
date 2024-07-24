/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/24 23:53:15 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/07/25 00:10:33 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PATH_H
#define PATH_H

#include <kernel.h>

/**
** +---------------------+----------------------------------------------------------------------------------+
** | Function            | Description                                                                      |
** +---------------------+----------------------------------------------------------------------------------+
** | path_get_directory  | Returns the parent directory of a given path. Ex: /test/ for /test/file.txt      |
** +---------------------+----------------------------------------------------------------------------------+
** | path_get_filename   | Returns the filename from a given path. Ex: file.txt for /test/file.txt          |
** +---------------------+----------------------------------------------------------------------------------+
** | path_is_valid       | Checks if a path is valid. Must start with /.                                    |
** +---------------------+----------------------------------------------------------------------------------+
** | path_combine        | Combines a directory and a filename into a complete path.                        |
** +---------------------+----------------------------------------------------------------------------------+
** | path_normalize      | Normalizes a path by handling . and ..                                           |
** +---------------------+----------------------------------------------------------------------------------+
** | path_split          | Splits a path into directory and filename, returns path_result_t.                |
** +---------------------+----------------------------------------------------------------------------------+
** | path_free_result    | Frees the memory allocated by path manipulation functions.                       |
** +---------------------+----------------------------------------------------------------------------------+
**/

/**
 * Example
 * int main() {
 *     const char *full_path = "/test/test_file1.txt";
 *
 *     char *directory = path_get_directory(full_path);
 *     char *filename = path_get_filename(full_path);
 *
 *     printf("Directory: %s\n", directory);
 *     printf("Filename: %s\n", filename);
 *
 *     path_result_t split = path_split(full_path);
 *     printf("Split Directory: %s\n", split.path);
 *     printf("Split Filename: %s\n", split.file);
 *
 *     char *combined_path = path_combine("/test", "test_file1.txt");
 *     printf("Combined Path: %s\n", combined_path);
 *
 *     char *normalized_path = path_normalize("/test/../test_file1.txt");
 *     printf("Normalized Path: %s\n", normalized_path);
 *
 *     // Free allocated memory
 *     free(directory);
 *     free(filename);
 *     path_free_result(split);
 *     free(combined_path);
 *     free(normalized_path);
 *
 *     return 0;
 * }
 **/

// Definition of a result structure for path manipulation functions
typedef struct {
    char *path;
    char *file;
} path_result_t;

// Function to get the directory of a path
extern char *path_get_directory(const char *full_path);

// Function to get the filename of a path
extern char *path_get_filename(const char *full_path);

// Function to check if a path is valid
extern bool path_is_valid(const char *path);

// Function to combine a directory and a filename into a complete path
extern char *path_combine(const char *directory, const char *filename);

// Function to normalize a path (handle ".." and ".")
extern char *path_normalize(const char *path);

// Function to split a path into directory and file
extern path_result_t path_split(const char *full_path);

#endif /* !PATH_H */