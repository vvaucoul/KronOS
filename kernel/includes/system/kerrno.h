/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kerrno.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/17 15:44:01 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/10/21 17:57:49 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef KERRNO_H
#define KERRNO_H

/*******************************************************************************
 *                            KERRNO KERNEL SECTOR                             *
 ******************************************************************************/

typedef enum __e_kerrno_kernel_sector
{
    __KERRNO_SECTOR_NONE,
    __KERRNO_SECTOR_GDT,
    __KERRNO_SECTOR_IDT,
    __KERRNO_SECTOR_PIT,
    __KERRNO_SECTOR_TIMER,
    __KERRNO_SECTOR_IO,
    __KERRNO_SECTOR_IRQ,
    __KERRNO_SECTOR_ISR,
    __KERRNO_SECTOR_TSS,
    __KERRNO_SECTOR_SYSTEM,
    __KERRNO_SECTOR_PMM,
    __KERRNO_SECTOR_MEMORY,
    __KERRNO_SECTOR_TERMINAL
} __kerno_kernel_sector;

#define __KerrnoSector __kerno_kernel_sector
#define __KERRNO_SECTOR_SIZE 12
#define __KERRNO_DEFAULT_SECTOR __KERRNO_SECTOR_NONE

/*******************************************************************************
 *                              KERRNO STRUCTURE                               *
 ******************************************************************************/

typedef struct __s_kerrno_struct
{
    __KerrnoSector __sector;
    int __error_code;
    const char *__error_message;
} __t_kerrno_struct;

#define __KerrnoStruct __t_kerrno_struct

#define __KERRNO_DEFAULT_SEGMENT (__KerrnoStruct){__KERRNO_DEFAULT_SECTOR, 0, NULL}
#define __Kerrno_Error_Code(__KerrnoStruct) __KerrnoStruct.__error_code
#define __Kerrno_Error_Message(__KerrnoStruct) __KerrnoStruct.__error_message
#define __KERRNO_STRUCT(__sector, __error_code, __error_message) \
    (__KerrnoStruct) { __sector, __error_code, __error_message }

/*******************************************************************************
 *                            KERRNO SECTOR ERRORS                             *
 ******************************************************************************/

#include <system/kerrno/sectors.h>

/*******************************************************************************
 *                                 KERRNO VARS                                 *
 ******************************************************************************/

#define __KERRNO_TABLE_SIZE 32

#define __KERRNO_FUNCTION_SIZE 96

extern __KerrnoSector __kerrno_kernel_sector;
extern __KerrnoStruct __kerrno_segment; 
extern __KerrnoStruct __kerrno_table[__KERRNO_SECTOR_SIZE][__KERRNO_TABLE_SIZE];

extern int __kerrno_error_code;
extern char *__kerrno_file_name;
extern char __kerrno_function_name[__KERRNO_FUNCTION_SIZE];

/*******************************************************************************
 *                               KERRNO GETTERS                                *
 ******************************************************************************/

#define KerrnoCode() __kerrno_table[__kerrno_kernel_sector][__kerrno_error_code].__error_code
#define KerrnoCodeSector() __kerrno_kernel_sector
#define KerrnoMessage() __kerrno_table[__kerrno_kernel_sector][__kerrno_error_code].__error_message
#define KerrnoFile() __kerrno_file_name
#define KerrnoFunction() __kerrno_function_name

/*******************************************************************************
 *                              KERRNO FUNCTIONS                               *
 ******************************************************************************/

extern void init_kerrno(void);
extern int kerrno_assign_error(__KerrnoSector sector, int error_code, char *file_name, const char function_name[]);
extern void __kerrno_sectors(void);

/*******************************************************************************
 *                                KERRNO MACROS                                *
 ******************************************************************************/

#define KERNO_ASSIGN_ERROR(sector, error_code) kerrno_assign_error(sector, error_code, __FILE_NAME__, __FUNCTION__)

/*******************************************************************************
 *                                KERRNO ERRORS                                *
 ******************************************************************************/

#define __KERRNO_ERROR_CODE_INVALID 1
#define __KERRNO_ERROR_SECTOR_INVALID 2

#endif /* KERRNO_H */