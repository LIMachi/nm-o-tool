/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cast_memory.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmartzol <hmartzol@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2000/00/00 00:00:00 by hmartzol          #+#    #+#             */
/*   Updated: 2000/00/00 00:00:00 by hmartzol         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <memory.h>

static t_memory_error	conv_integer_memory(uint8_t *mem_out,
	t_memory_descriptor md_out, uint8_t *mem_in, t_memory_descriptor md_in)
{
	uint8_t	pad;
	size_t	sw;

	pad = (md_in.sign && (mem_in[!md_in.endian
		? md_in.block_size - 1 : 0] & 0x80)) * 0xFF;
	sw = (size_t)-1;
	if (!md_in.endian && md_out.endian)
		while (++sw < md_out.block_size)
			mem_out[sw] = md_out.block_size - sw - 1 < md_in.block_size
				? mem_in[md_out.block_size - sw - 1] : pad;
	if (md_in.endian && !md_out.endian)
		while (++sw < md_out.block_size)
			mem_out[sw] = md_in.block_size >= sw + 1
				? mem_in[md_in.block_size - sw - 1] : pad;
	if (!md_in.endian && !md_out.endian)
		while (++sw < md_out.block_size)
			mem_out[sw] = sw < md_in.block_size ? mem_in[sw] : pad;
	if (md_in.endian && md_out.endian)
		while (++sw < md_out.block_size)
			mem_out[sw] = sw + md_in.block_size - md_out.block_size
				< md_in.block_size ? mem_in[sw + md_in.block_size
				- md_out.block_size] : pad;
	return (ME_OK);
}

t_memory_error			cast_memory(t_memory_map out, t_memory_map in,
							t_cast_memory_descriptor cmd)
{
	size_t	align_in;
	size_t	align_out;
	size_t	it;
	size_t	j;

	out.cursor += cmd.delta;
	if (valid_cursor(&out, cmd.out, &align_out) != ME_OK)
		return (ME_PENDING_ERROR);
	out.cursor -= cmd.delta;
	if (valid_cursor(&in, cmd.in, &align_in) != ME_OK)
		return (ME_PENDING_ERROR);
	it = (size_t)-1;
	while (++it < cmd.out.nb_blocks)
	{
		j = (size_t)-1;
		while (++j < align_out)
			out.ptr[it * align_out + j + cmd.delta] = 0;
		if (it < cmd.in.nb_blocks)
			conv_integer_memory(&out.ptr[it * align_out + cmd.out.endian
			* (align_out - cmd.out.block_size) + cmd.delta],
			cmd.out, &in.ptr[it * align_in + cmd.in.endian * (align_in
			- cmd.in.block_size)], cmd.in);
	}
	return (ME_OK);
}
