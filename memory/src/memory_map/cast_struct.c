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

static inline uint64_t			jump_member(t_memory_descriptor md,
	uint32_t member_align)
{
	uint64_t	jump;

	jump = md.block_size;
	if (jump % md.align)
		jump += md.align - jump % md.align;
	jump *= md.nb_blocks;
	if (jump & member_align)
		jump += member_align - jump % member_align;
	return (jump);
}

static inline t_memory_error	resolve_adresses(t_struct_descriptor csd_out,
	uint64_t addresses[32])
{
	uint32_t	it;
	uint64_t	cursor;

	it = 0;
	cursor = 0;
	while (it < csd_out.nb_members)
	{
		addresses[it] = cursor;
		cursor += jump_member(csd_out.member[it], csd_out.align);
		++it;
	}
	return (ME_OK);
}

static inline t_memory_error	cast_struct_0(t_memory_map *maps[2],
	t_cast_struct_descriptor csd, t_memory_map *sout, uint64_t addresses[32])
{
	uint64_t		tmp;

	if (valid_cursor(maps[1],
			(t_memory_descriptor) {1, csd.out.total_size, 1, 0}, &tmp))
		return (ME_PENDING_ERROR);
	if (valid_cursor(maps[0],
			(t_memory_descriptor) {1, csd.in.total_size, 1, 0}, &tmp))
		return (ME_PENDING_ERROR);
	*sout = (t_memory_map) {csd.out.total_size, 0, maps[1]->endian,
			&maps[1]->ptr[maps[1]->cursor]};
	if (resolve_adresses(csd.out, addresses) != ME_OK)
		return (ME_PENDING_ERROR);
	tmp = 0;
	while (tmp < csd.out.total_size)
		sout->ptr[tmp++] = 0;
	return (ME_OK);
}

t_memory_error					cast_struct(t_memory_map *out, t_memory_map *in,
	t_cast_struct_descriptor csd)
{
	t_memory_map	sout;
	uint64_t		pc;
	uint32_t		it;
	uint64_t		addresses[32];

	if (cast_struct_0((t_memory_map*[2]){in, out}, csd, &sout,
			addresses) != ME_OK)
		return (ME_PENDING_ERROR);
	it = (uint32_t)-1;
	while (++it < csd.in.nb_members)
		if (csd.mapping[it] != 0xFF)
		{
			pc = in->cursor;
			sout.cursor = addresses[csd.mapping[it]];
			if (cast_memory(&sout, in, csd.out.member[csd.mapping[it]],
					csd.in.member[it]) != ME_OK)
				return (ME_PENDING_ERROR);
			pc = in->cursor - pc;
			if (pc % csd.in.align)
				in->cursor =+ csd.in.align - pc % csd.in.align;
		}
		else
			in->cursor += jump_member(csd.in.member[it], csd.in.align);
	out->cursor += csd.out.total_size;
	return (ME_OK);
}
