/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unclaim_map.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmartzol <hmartzol@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2000/00/00 00:00:00 by hmartzol          #+#    #+#             */
/*   Updated: 2000/00/00 00:00:00 by hmartzol         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <memory.h>

t_memory_error	unclaim_map(t_memory_map *mm, const t_memory_descriptor md,
							uint8_t claim, int jump)
{
	size_t	align;
	size_t	it;
	size_t	sw;

	if (mm->err != ME_OK || valid_cursor(mm, md, &align) != ME_OK)
		return (mm->err);
	it = (size_t)-1;
	while (++it < md.nb_blocks)
	{
		sw = (size_t)-1;
		while (++sw < md.block_size)
			if (mm->map[mm->cursor + align * it + sw] != claim)
				return (memory_error(&mm->err, ME_INVALID_UNCLAIM,
					DEBUG_TUPLE));
			else
				mm->map[mm->cursor + align * it + sw] = 0;
	}
	if (jump)
		mm->cursor += align * md.nb_blocks;
	return (ME_OK);
}
