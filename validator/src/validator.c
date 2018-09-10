/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   validator.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lee <lee@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/23 18:42:14 by lee               #+#    #+#             */
/*   Updated: 2018/08/23 19:24:05 by lee              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <validator.h>

#include <mach-o/loader.h>
#include <mach-o/fat.h>
#include <mach-o/nlist.h>
#include <ar.h>

int	validator(t_mem_area *file)
{
	uint32_t	magic;

	if (file == NULL || file->start == NULL || file->end == NULL
			|| file->end < file->start)
		return (EVE_MALFORMATED_MEM_AREA);
	if (file->start + sizeof(uint32_t) >= file->end)
		return (EVE_INVALID_HEADER);
	magic = *(uint32_t*)file->start;
	if (magic == FAT_MAGIC || magic == FAT_CIGAM)
		return (fat(file, magic));
	else if (magic == MH_MAGIC || magic == MH_CIGAM
			|| magic == MH_MAGIC_64 || magic == MH_CIGAM_64)
		return (macho(file, magic));
	else if (file->start + SARMAG < file->end
			&& !strncmp(file->start, ARMAG, SARMAG))
		return (archive(file));
	return (EVE_UNKNOWN_HEADER);
}
