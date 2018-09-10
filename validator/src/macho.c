/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   macho.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lee <lee@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/23 18:43:14 by lee               #+#    #+#             */
/*   Updated: 2018/09/10 22:34:47 by lee              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <validator.h>

static inline void					swap_head(t_macho_info *info)
{
	if (info->x64)
		info->header.x64.reserved = swap32(info->header.x64.reserved);
	info->header.x32.magic = swap32(info->header.x32.magic);
	(void)swap(&info->header.x32.cputype, 1, sizeof(cpu_type_t));
	(void)swap(&info->header.x32.cpusubtype, 1, sizeof(cpu_subtype_t));
	info->header.x32.filetype = swap32(info->header.x32.filetype);
	info->header.x32.ncmds = swap32(info->header.x32.ncmds);
	info->header.x32.sizeofcmds = swap32(info->header.x32.sizeofcmds);
	info->header.x32.flags = swap32(info->header.x32.flags);
}

static inline t_validation_error	load_commands_first_pass(t_macho_info *info)
{
	struct load_command	*lc;
	uint32_t			count;
	uint32_t			cmdsize;

	count = -1;
	lc = (struct load_command*)info->commands_area.start;
	while (++count < info->header.x32.ncmds && lc < info->commands_area.end)
	{
		if (lc + sizeof(struct load_command) >= info->commands_area.end)
			return (EVE_INVALID_LOAD_COMMAND_SIZE);
		cmdsize = info->endian ? swap32(lc->cmdsize) : ls->cmdsize;
		if (lc + cmdsize <= lc
				|| lc + cmdsize > info->commands_area.end)
			return (EVE_INVALID_LOAD_COMMAND_SIZE);
		lc = lc + cmdsize;
	}
	if (count != info->header.x32.ncmds)
		return (EVE_MISMATCHED_COMMAND_COUNT);
	return (EVE_OK);
}

t_validation_error					macho(t_mem_area *area, uint32_t magic)
{
	t_macho_info		info;
	size_t				headsize;
	t_validation_error	error;

	info.endian = (magic == MH_CIGAM || magic == MH_CIGAM_64);
	info.x64 = (magic == MH_CIGAM_64 || magic == MH_MAGIC_64);
	headsize = info.x64 ? sizeof(mach_header_64) : sizeof(mach_header);
	if (area->start + headsize >= area->end)
		return (EVE_INVALID_HEADER);
	(void)memcpy(&info.header, area->start, headsize);
	if (info.endian)
		swap_head(&info);
	info.file_area = *area;
	info.head_area.start = area->start;
	info.head_area.end = area->start + headsize;
	info.commands_area.start = info.head_area.end;
	info.commands_area.end = info.commands_area.start
		+ info.header.x32.sizeofcmds;
	info.data_area.start = info.commands_area.end;
	info.data_area.end = info.file_area.end;
	if ((error = load_commands_first_pass(&info)) != EVE_OK)
		return (error);
	return (load_commands(&info));
}
