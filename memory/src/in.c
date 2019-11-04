/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   in.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmartzol <hmartzol@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2000/00/00 00:00:00 by hmartzol          #+#    #+#             */
/*   Updated: 2000/00/00 00:00:00 by hmartzol         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <memory.h>

size_t	in(const void *search, const t_memory_descriptor md,
	const void *mem, const uint32_t endian)
{
	size_t	it;
	size_t	sw;
	size_t	align;

	if ((align = md.block_size % md.align))
		align = md.block_size + md.align - align;
	else
		align = md.block_size > md.align ? md.block_size : md.align;
	it = (size_t)-1;
	while (++it < md.nb_blocks)
	{
		sw = (size_t)-1;
		while (++sw < md.block_size)
			if (md.endian != endian)
			{
				if (((uint8_t*)search)[sw]
						!= ((uint8_t*)mem)[it * align + md.block_size - sw])
					break ;
			}
			else if (((uint8_t*)search)[sw] != ((uint8_t*)mem)[it * align + sw])
				break ;
		if (sw == md.block_size)
			return (it);
	}
	return ((size_t)-1);
}
