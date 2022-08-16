/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   alloc.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.Fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/16 15:08:21 by vvaucoul          #+#    #+#             */
/*   Updated: 2022/08/16 16:25:53 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ALLOC_H
#define ALLOC_H

/*******************************************************************************
 *                               PHYSICAL MEMORY                               *
 ******************************************************************************/

extern void *kmalloc(size_t size);
extern void kfree(void *ptr);
extern size_t ksize(void *ptr);
extern void *kbrk(void);

/*******************************************************************************
 *                               VIRUTAL MEMORY                                *
 ******************************************************************************/

extern void *vmalloc(size_t size);
extern void vfree(void *ptr);
extern size_t vsize(void *ptr);
extern void *vbrk(void);

#endif /* ALLOC_H */