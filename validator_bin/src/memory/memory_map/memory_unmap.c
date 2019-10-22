/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   memory_unmap.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmartzol <hmartzol@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2000/00/00 00:00:00 by hmartzol          #+#    #+#             */
/*   Updated: 2000/00/00 00:00:00 by hmartzol         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <memory.h>

t_memory_error	memory_unmap(t_memory_map *mm)
{
	if (mm->ptr != MAP_FAILED && mm->ptr != NULL)
		munmap(mm->ptr, mm->size);
	if (mm->map != MAP_FAILED && mm->map != NULL)
		munmap(mm->map, mm->size);
	memory_map_clear(mm);
	return (ME_OK);
}
