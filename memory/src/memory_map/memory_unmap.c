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
	if (get_error()->error != ME_OK)
		return (ME_PENDING_ERROR);
	if (mm->ptr != MAP_FAILED && mm->ptr != NULL)
		munmap(mm->ptr, mm->size);
	memory_map_clear(mm);
	return (ME_OK);
}
