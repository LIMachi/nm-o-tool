/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cast_struct.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmartzol <hmartzol@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2000/00/00 00:00:00 by hmartzol          #+#    #+#             */
/*   Updated: 2000/00/00 00:00:00 by hmartzol         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <memory.h>

t_memory_error			cast_struct(t_memory_map out, t_memory_map in,
	t_cast_struct_descriptor csd)
{
	t_memory_map	sout;
	t_memory_map	sin;
	size_t			align_in;
	size_t			align_out;
	uint32_t		it;

	if (valid_cursor(&out,
			(t_memory_descriptor){1, csd.total_size_out, 1, 0}, &align_out))
		return (ME_PENDING_ERROR);
	if (valid_cursor(&in,
			(t_memory_descriptor){1, csd.total_size_in, 1, 0}, &align_in))
		return (ME_PENDING_ERROR);
	sout = (t_memory_map){csd.total_size_out, 0, out.endian, &out.ptr[out.cursor]};
	sin = (t_memory_map){csd.total_size_in, 0, in.endian, &in.ptr[in.cursor]};
	it = (uint32_t)-1;
	while (++it < csd.nb_members)
		if (cast_memory(sout, sin, csd.members[it]) != ME_OK)
			return (ME_PENDING_ERROR);
	return (ME_OK);
}
