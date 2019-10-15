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
							const uint8_t mapping, const t_struct_descriptor sd)
{
	size_t	it;
	size_t	t;
	size_t	c;

	it = (size_t)-1;
	c = 0;
	while (++it < sd.nb_members)
	{
		t = mm->cursor;
		if (read_in_memory(mm, &buffer[c], mapping, sd.member[it]) != ME_OK)
			return (mm->err);
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
