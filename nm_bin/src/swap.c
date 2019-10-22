/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   swap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmartzol <hmartzol@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/16 10:35:07 by hmartzol          #+#    #+#             */
/*   Updated: 2018/08/17 15:54:35 by hmartzol         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdint.h>

uint16_t	swap16(uint16_t v)
{
	return (((v & 0xFFull) << 8) | ((v & 0xFF00ull) >> 8));
}

uint32_t	swap32(uint32_t v)
{
	return (((v & 0xFF) << 24) | ((v & 0xFF00) << 8)
		| ((v & 0xFF0000) >> 8) | ((v & 0xFF000000) >> 24));
}

uint64_t	swap64(uint64_t v)
{
	return (((v & 0xFFull) << 56) | ((v & 0xFF00ull) << 40)
		| ((v & 0xFF0000ull) << 24) | ((v & 0xFF000000ull) << 8)
		| ((v & 0xFF00000000ull) >> 8) | ((v & 0xFF0000000000ull) >> 24)
		| ((v & 0xFF000000000000ull) >> 40)
		| ((v & 0xFF00000000000000ull) >> 56));
}

void		*swap(void *mem, uint64_t size, uint64_t block)
{
	uint8_t		t;
	uint64_t	i;
	uint64_t	j;
	uint8_t		*m;

	if (block <= 1)
		return (mem);
	i = -1;
	while (++i < size && (j = -1))
		while (++j < block / 2)
		{
			m = &((uint8_t*)mem)[i * block];
			t = m[j];
			m[j] = m[block - j - 1];
			m[block - j - 1] = t;
		}
	return (mem);
}
