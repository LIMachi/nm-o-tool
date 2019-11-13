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
	uint8_t *mem_in, t_memory_descriptor cmd[2], uint32_t endian_mode)
{
	uint8_t	pad;
	size_t	sw;

	pad = (cmd[0].sign && (mem_in[!(endian_mode & 1)
		? cmd[0].block_size - 1 : 0] & 0x80)) * 0xFF;
	sw = (size_t)-1;
	if (2 == endian_mode)
		while (++sw < cmd[1].block_size)
			mem_out[sw] = cmd[1].block_size - sw - 1 < cmd[0].block_size
				? mem_in[cmd[1].block_size - sw - 1] : pad;
	if (1 == endian_mode)
		while (++sw < cmd[1].block_size)
			mem_out[sw] = cmd[0].block_size >= sw + 1
				? mem_in[cmd[0].block_size - sw - 1] : pad;
	if (0 == endian_mode)
		while (++sw < cmd[1].block_size)
			mem_out[sw] = sw < cmd[0].block_size ? mem_in[sw] : pad;
	if (3 == endian_mode)
		while (++sw < cmd[1].block_size)
			mem_out[sw] = sw + cmd[0].block_size - cmd[1].block_size
				< cmd[0].block_size ? mem_in[sw + cmd[0].block_size
				- cmd[1].block_size] : pad;
	return (ME_OK);
}

t_memory_error			cast_memory(t_memory_map *out, t_memory_map *in,
	t_memory_descriptor md_out, t_memory_descriptor md_in)
{
	size_t	align_in;
	size_t	align_out;
	size_t	it;
	size_t	j;

	if (valid_cursor(out, md_out, &align_out) != ME_OK)
		return (ME_PENDING_ERROR);
	if (valid_cursor(in, md_in, &align_in) != ME_OK)
		return (ME_PENDING_ERROR);
	it = (size_t)-1;
	while (++it < md_out.nb_blocks)
	{
		j = (size_t)-1;
		while (++j < align_out)
			out->ptr[it * align_out + j + out->cursor] = 0;
		if (it < md_in.nb_blocks)
			conv_integer_memory(&out->ptr[it * align_out + out->endian
			* (align_out - md_out.block_size) + out->cursor],
			&in->ptr[it * align_in + in->endian * (align_in
			- md_in.block_size) + in->cursor], (t_memory_descriptor[2]){md_in,
			md_out}, in->endian + out->endian * 2);
	}
	out->cursor += md_out.nb_blocks * align_out;
	in->cursor += md_in.nb_blocks * align_in;
	return (ME_OK);
}
