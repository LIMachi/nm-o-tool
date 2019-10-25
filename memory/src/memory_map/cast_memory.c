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

t_memory_error	cast_memory(t_memory_map *out, t_memory_map *in,
	t_cast_memory_descriptor cmd)
{
	size_t	cur_out;
	size_t	aling_in;
	size_t	aling_out;
	size_t	it;
	size_t	sw;

	if (valid_cursor(in, cmd.in, &aling_in) != ME_OK)
		return (ME_PENDING_ERROR);
	cur_out = out->cursor;
	if (cmd.delta != CTD_VOID)
		out->cursor += cmd.delta;
	if (valid_cursor(out, cmd.out, &aling_out) != ME_OK)
	{
			out->cursor = cur_out;
			return (ME_PENDING_ERROR);
	}
	it = 0;
	if (cmd.delta != CTD_VOID)
		while (it < cmd.in.nb_blocks && it < cmd.out.nb_blocks)
		{
			sw = 0;
			if (cmd.in.endian && cmd.out.endian && cmd.in.endian != cmd.out.endian)
			{

			}
		}
}

