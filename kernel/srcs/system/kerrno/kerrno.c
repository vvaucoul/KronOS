/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kerrno.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/17 16:12:44 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/12/11 13:53:25 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <system/kerrno.h>
#include <kernel.h>

__KerrnoSector __kerrno_kernel_sector; // Current Sector
__KerrnoStruct __kerrno_segment; // Current Table Segment
__KerrnoStruct __kerrno_table[__KERRNO_SECTOR_SIZE][__KERRNO_TABLE_SIZE]; // All Kernel Error Tables
int __kerrno_error_code; // Current Error Code
char *__kerrno_file_name; // Current File Name
char __kerrno_function_name[__KERRNO_FUNCTION_SIZE]; // Current Function Name

void init_kerrno(void)
{
    __kerrno_kernel_sector = __KERRNO_DEFAULT_SECTOR;
    __kerrno_segment = __KERRNO_DEFAULT_SEGMENT;
    __kerrno_error_code = 0;
    __kerrno_file_name = NULL;
    bzero(__kerrno_function_name, __KERRNO_FUNCTION_SIZE);
    memset(__kerrno_table, 0, __KERRNO_SECTOR_SIZE);
    for (size_t i = 0; i < __KERRNO_SECTOR_SIZE; i ++)
    {
        memset(__kerrno_table[i], 0, __KERRNO_TABLE_SIZE);
    }
    __kerrno_sectors();
}

int kerrno_assign_error(__KerrnoSector sector, int error_code, char *file_name, const char function_name[])
{
    if (sector >= __KERRNO_SECTOR_SIZE)
        return (__KERRNO_ERROR_SECTOR_INVALID);
    else if (error_code >= __KERRNO_TABLE_SIZE)
        return (__KERRNO_ERROR_CODE_INVALID);
    else
    {
        __kerrno_kernel_sector = sector;
        __kerrno_segment = __kerrno_table[sector][error_code];
        __kerrno_error_code = error_code;
        __kerrno_file_name = file_name;
        memcpy(__kerrno_function_name, function_name, __KERRNO_FUNCTION_SIZE - 1);
    }
    return (0);
}

#undef __KerrnoSector 
#undef __KERRNO_SECTOR_SIZE 
#undef __KERRNO_DEFAULT_SECTOR 
#undef __KerrnoStruct
#undef __KERRNO_DEFAULT_SEGMENT 
#undef __Kerrno_Error_Code
#undef __Kerrno_Error_Message
#undef __KERRNO_STRUCT
#undef __KERRNO_TABLE_SIZE 
#undef __KERRNO_FUNCTION_SIZE 
#undef __KERRNO_ERROR_CODE_INVALID 
#undef __KERRNO_ERROR_SECTOR_INVALID 