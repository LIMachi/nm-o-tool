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
	const t_memory_descriptor md, const uint32_t endian)
{
	size_t			align;
	size_t			it;
	size_t			sw;

	if (get_error()->error != ME_OK || valid_cursor(mm, md, &align) != ME_OK)
		return (ME_PENDING_ERROR);
	it = (size_t)-1;
	if (buffer != NULL)
		while (++it < md.nb_blocks)
		{
			sw = (size_t)-1;
			if (md.endian && md.endian != endian)
				while (++sw < md.block_size)
					((uint8_t*)buffer)[it * align + sw] = mm->ptr[mm->cursor
						+ it * align + md.block_size - sw - 1];
			else
				while (++sw < md.block_size)
					((uint8_t*)buffer)[it * align + sw] = mm->ptr[mm->cursor
						+ it * align + sw];
		}
	mm->cursor += align * md.nb_blocks;
	return (ME_OK);
}
