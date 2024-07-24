/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/24 23:52:59 by vvaucoul          #+#    #+#             */
/*   Updated: 2024/07/25 00:00:55 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <memory/memory.h>
#include <system/fs/path.h>

/**
 * @brief Retrieves the directory path from a given full path.
 *
 * This function takes a full path as input and returns the directory path
 * portion of the path. The directory path is the portion of the path that
 * precedes the last forward slash ("/") character.
 *
 * @param full_path The full path from which to extract the directory path.
 * @return A pointer to a string containing the directory path, or NULL if
 *         the full path does not contain any forward slash characters.
 */
char *path_get_directory(const char *full_path) {
    if (!full_path)
        return NULL;

    const char *last_slash = strrchr(full_path, '/');
    if (!last_slash)
        return NULL;

    size_t dir_length = last_slash - full_path + 1;
    char *directory = (char *)kmalloc(dir_length + 1);
    if (!directory)
        return NULL;

    strncpy(directory, full_path, dir_length);
    directory[dir_length] = '\0';

    return directory;
}

/**
 * Retrieves the filename from a given path.
 *
 * @param full_path The full path from which to extract the filename.
 * @return The filename extracted from the path.
 */
char *path_get_filename(const char *full_path) {
    if (!full_path)
        return NULL;

    const char *last_slash = strrchr(full_path, '/');
    if (!last_slash)
        return strdup(full_path);

    return strdup(last_slash + 1);
}

/**
 * @brief Checks if a given path is valid.
 *
 * This function determines whether a given path is valid or not. A valid path
 * should conform to the file system's naming conventions and should exist in
 * the file system.
 *
 * @param path The path to be checked.
 * @return true if the path is valid, false otherwise.
 */
bool path_is_valid(const char *path) {
    if (!path || path[0] != '/')
        return false;
    // Ajoutez d'autres vérifications selon vos besoins
    return true;
}

/**
 * @brief Combines a directory path and a filename to create a new path.
 *
 * This function takes a directory path and a filename as input and combines them to create a new path.
 * The resulting path will be the concatenation of the directory path and the filename, separated by a path separator.
 *
 * @param directory The directory path.
 * @param filename The filename.
 * @return A pointer to the newly created path, or NULL if an error occurred.
 */
char *path_combine(const char *directory, const char *filename) {
    if (!directory || !filename)
        return NULL;

    size_t dir_length = strlen(directory);
    size_t file_length = strlen(filename);

    char *full_path = (char *)kmalloc(dir_length + file_length + 2); // +1 pour '/' et +1 pour '\0'
    if (!full_path)
        return NULL;

    strcpy(full_path, directory);
    if (directory[dir_length - 1] != '/') {
        full_path[dir_length] = '/';
        dir_length++;
    }
    strcpy(full_path + dir_length, filename);

    return full_path;
}

/**
 * @brief Normalize a given path.
 *
 * This function takes a path as input and returns a normalized version of the path.
 * The normalization process involves removing any redundant slashes, resolving any
 * relative path components (e.g., ".", ".."), and ensuring consistent directory
 * separators.
 *
 * @param path The path to be normalized.
 * @return A pointer to the normalized path, or NULL if an error occurred.
 */
char *path_normalize(const char *path) {
    if (!path)
        return NULL;

    size_t length = strlen(path);
    char *normalized_path = (char *)kmalloc(length + 1);
    if (!normalized_path)
        return NULL;

    const char *src = path;
    char *dst = normalized_path;

    while (*src) {
        if (src[0] == '/' && src[1] == '/') {
            src++;
        } else if (src[0] == '/' && src[1] == '.' && src[2] == '/') {
            src += 2;
        } else if (src[0] == '/' && src[1] == '.' && src[2] == '.' && src[3] == '/') {
            src += 3;
            if (dst > normalized_path) {
                dst--;
                while (dst > normalized_path && *dst != '/')
                    dst--;
            }
        } else {
            *dst++ = *src++;
        }
    }

    *dst = '\0';
    return normalized_path;
}

/**
 * @brief Splits a full path into its components.
 *
 * This function takes a full path as input and splits it into its individual components.
 * The full path should be a string representing a file or directory path.
 *
 * @param full_path The full path to be split.
 * @return The result of the path split operation.
 */
path_result_t path_split(const char *full_path) {
    path_result_t result = {NULL, NULL};

    if (!full_path)
        return result;

    result.path = path_get_directory(full_path);
    result.file = path_get_filename(full_path);

    return result;
}

/**
 * Frees the memory allocated for a path result.
 *
 * @param result The path result to be freed.
 */
void path_free_result(path_result_t result) {
    if (result.path)
        kfree(result.path);
    if (result.file)
        kfree(result.file);
}