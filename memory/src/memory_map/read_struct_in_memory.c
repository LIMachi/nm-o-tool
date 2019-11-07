/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   read_struct_in_memory.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmartzol <hmartzol@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2000/00/00 00:00:00 by hmartzol          #+#    #+#             */
/*   Updated: 2000/00/00 00:00:00 by hmartzol         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <memory.h>

t_memory_error	read_struct_in_memory(t_memory_map *mm, void *buffer,
	const t_struct_descriptor sd, uint32_t endian)
{
	size_t	it;
	size_t	t;
	size_t	c;

	if (get_error()->error != ME_OK)
		return (ME_PENDING_ERROR);
	it = (size_t)-1;
	c = 0;
	while (++it < sd.nb_members)
	{
		t = mm->cursor;
		if (read_in_memory(mm, buffer != NULL
				? &((uint8_t*)buffer)[c] : NULL,
				sd.member[it], endian) != ME_OK)
			return (ME_PENDING_ERROR);
		t = mm->cursor - t;
		c += t;
		if (t % sd.align)
		{
			t = sd.align - (t % sd.align);
			mm->cursor += t;
			c += t;
		}
	}
	return (ME_OK);
}
