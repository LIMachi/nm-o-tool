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
	uint8_t *mem_in, t_cast_memory_descriptor cmd, uint32_t endian_mode)
{
	uint8_t	pad;
	size_t	sw;

	pad = (cmd.in.sign && (mem_in[!(endian_mode & 1)
		? cmd.in.block_size - 1 : 0] & 0x80)) * 0xFF;
	sw = (size_t)-1;
	if (2 == endian_mode)
		while (++sw < cmd.out.block_size)
			mem_out[sw] = cmd.out.block_size - sw - 1 < cmd.in.block_size
				? mem_in[cmd.out.block_size - sw - 1] : pad;
	if (1 == endian_mode)
		while (++sw < cmd.out.block_size)
			mem_out[sw] = cmd.in.block_size >= sw + 1
				? mem_in[cmd.in.block_size - sw - 1] : pad;
	if (0 == endian_mode)
		while (++sw < cmd.out.block_size)
			mem_out[sw] = sw < cmd.in.block_size ? mem_in[sw] : pad;
	if (3 == endian_mode)
		while (++sw < cmd.out.block_size)
			mem_out[sw] = sw + cmd.in.block_size - cmd.out.block_size
				< cmd.in.block_size ? mem_in[sw + cmd.in.block_size
				- cmd.out.block_size] : pad;
	return (ME_OK);
}

t_memory_error			cast_memory(t_memory_map out, t_memory_map in,
	t_cast_memory_descriptor cmd)
{
	size_t	align_in;
	size_t	align_out;
	size_t	it;
	size_t	j;

	out.cursor += cmd.delta_out;
	in.cursor += cmd.delta_in;
	if (valid_cursor(&out, cmd.out, &align_out) != ME_OK)
		return (ME_PENDING_ERROR);
	if (valid_cursor(&in, cmd.in, &align_in) != ME_OK)
		return (ME_PENDING_ERROR);
	it = (size_t)-1;
	while (++it < cmd.out.nb_blocks)
	{
		j = (size_t)-1;
		while (++j < align_out)
			out.ptr[it * align_out + j + out.cursor] = 0;
		if (it < cmd.in.nb_blocks)
			conv_integer_memory(&out.ptr[it * align_out + out.endian
			* (align_out - cmd.out.block_size) + out.cursor],
			&in.ptr[it * align_in + in.endian * (align_in
			- cmd.in.block_size) + in.cursor], cmd, in.endian + out.endian * 2);
	}
	return (ME_OK);
}
