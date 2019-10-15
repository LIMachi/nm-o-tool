/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   read_in_memory.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmartzol <hmartzol@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2000/00/00 00:00:00 by hmartzol          #+#    #+#             */
/*   Updated: 2000/00/00 00:00:00 by hmartzol         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <memory.h>

t_memory_error	read_in_memory(t_memory_map *mm, void *buffer,
							const uint8_t exmap, const t_memory_descriptor md)
{
	size_t			align;
	size_t			it;
	size_t			sw;

	if (valid_cursor(mm, md, &align) != ME_OK)
		return (mm->err);
	it = (size_t)-1;
	while (++it < md.nb_blocks && (sw = (size_t)-1))
	{
		while (++sw < align)
			if (exmap != 0xFF && mm->map[mm->cursor + it * align + sw] != exmap)
				return (memory_error(&mm->err, ME_INVALID_MAPPING,
					DEBUG_TUPLE));
		if (buffer == NULL || !(sw = (size_t)-1))
			continue ;
		while (++sw < align)
			if (md.should_swap && mm->swap && sw < md.block_size)
				((uint8_t*)buffer)[it * align + sw] = mm->ptr[mm->cursor
					+ it * align + md.block_size - sw];
			else
				((uint8_t*)buffer)[it * align + sw] = mm->ptr[mm->cursor
					+ it * align + sw];
	}
	mm->cursor += align * md.nb_blocks;
	return (ME_OK);
}
