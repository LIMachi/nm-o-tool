/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   valid_memory_cursor.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmartzol <hmartzol@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2000/00/00 00:00:00 by hmartzol          #+#    #+#             */
/*   Updated: 2000/00/00 00:00:00 by hmartzol         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <memory.h>

t_memory_error	valid_cursor(t_memory_map *mm,
							const t_memory_descriptor md,
							size_t *align)
{
	size_t	tmp;

	if (get_error()->error != ME_OK)
		return (ME_PENDING_ERROR);
	if (align == NULL)
		align = &tmp;
	if (mm->cursor >= mm->size)
		return (set_error(ME_OUTSIDE_MAPPING, ERROR_TUPLE));
	if ((*align = md.block_size % md.align))
		*align = md.block_size + md.align - *align;
	else
		*align = md.block_size > md.align ? md.block_size : md.align;
	if (mm->cursor + *align < mm->cursor || mm->cursor + *align > mm->size)
		return (set_error(ME_INVALID_BLOC_SIZE, ERROR_TUPLE));
	if (md.nb_blocks && (md.nb_blocks * *align < md.nb_blocks
						|| md.nb_blocks * *align < *align))
		return (set_error(ME_INVALID_BLOC_COUNT, ERROR_TUPLE));
	return (ME_OK);
}
