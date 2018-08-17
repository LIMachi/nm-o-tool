/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mem_access.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmartzol <hmartzol@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/16 09:46:47 by hmartzol          #+#    #+#             */
/*   Updated: 2018/08/17 15:59:35 by hmartzol         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <nm.h>

/*
** size is the size of the area in which the pointer can move,
** delta is the actual movement
** the zone ptr to ptr + size is tested to be included in the file portion
** being processed, and delta is checked to be non overflowing
*/

inline void	*mem_access(const uint64_t size, void *ptr,
						const uint64_t delta, t_nm_env *env)
{
	uint64_t	pos;

	if ((uint64_t)ptr < (uint64_t)env->file + env->file_start
			|| (uint64_t)ptr >= (uint64_t)env->file + env->file_end)
		return (NULL);
	if (ptr == NULL)
		ptr = &env->file[env->file_start];
	if (size == 0)
		size = (uint64_t)ptr - (uint64_t)&env->file[env->file_end];
	pos = delta + (uint64_t)ptr;
	if (pos >= size + (uint64_t)ptr || pos < delta || pos < (uint64_t)ptr
			|| pos >= (uint64_t)env->file + env->file_end)
		return (NULL);
	return ((void*)pos);
}
