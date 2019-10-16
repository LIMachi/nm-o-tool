/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vlc_segment_64.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmartzol <hmartzol@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2000/00/00 00:00:00 by hmartzol          #+#    #+#             */
/*   Updated: 2000/00/00 00:00:00 by hmartzol         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <mach_o_validator.h>
#include <stdio.h>
#include <string.h>

t_validator_error	vlc_sections_32(const t_load_command_descriptor *lcd,
									const t_load_command_union lcu, t_macho_file *file)
{
	size_t				it;
	size_t				h;
	unsigned long long	id;
	t_struct_descriptor	sd_sect32 = {2, 1, sizeof(struct section),
		{{16, 2, 1, 0}, {4, 10, 4, 1}}};
	struct section		sec;

	it = (size_t)-1;
	id = file->vmm.biggest_id;
	while (++it < lcu.segment_32.nsects)
	{
		if (read_struct_in_memory(&file->mm, &sec, MM_CMD, sd_sect32) != ME_OK)
			return (file->err = VE_ME_MASK | file->mm.err);
		// sec.segname[15] = '\0';
		printf("'%.16s' : sect32: '%.16s'\n", sec.segname, sec.sectname);
		// if (strcmp(sec.segname, lcu.segment_64.segname)) //warning only
		// {
			// return (macho_error(file, VE_MISMATCHED_SEGNAME, DEBUG_TUPLE));
		// }
		if (vm_map_recut_entry(&file->vmm, (t_vm_map_entry){file->vmm.biggest_id + 1, sec.addr, sec.addr + sec.size}, id) != ME_OK)
			return (file->err = VE_ME_MASK | file->vmm.err);
		if (sec.flags != S_ZEROFILL) //special case: S_ZEROFILL does not require a valid file offset
		{
			h = file->mm.cursor;
			file->mm.cursor = sec.offset;
			if (claim_map(&file->mm, (t_memory_descriptor){sec.size, 1, 1ull << sec.align, 0}, MM_SECTION, 0) != ME_OK)
				return (file->err = VE_ME_MASK | file->vmm.err);
			file->mm.cursor = h;
		}
		if (sec.nreloc)
		{
			h = file->mm.cursor;
			file->mm.cursor = sec.reloff;
			if (claim_map(&file->mm, (t_memory_descriptor){sizeof(struct relocation_info), sec.nreloc, sizeof(struct relocation_info), 0}, MM_RELOCATION, 0) != ME_OK)
				return (file->err = VE_ME_MASK | file->vmm.err);
			file->mm.cursor = h;
		}
	}
	return (VE_OK);
}

t_validator_error	vlc_segment_32(const t_load_command_descriptor *lcd,
									const t_load_command_union lcu, t_macho_file *file)
{
	size_t						es;
	struct segment_command		seg;

	seg = lcu.segment_32;
	((t_load_command_union)lcu).segment_32.segname[15] = '\0';
	es = lcd->sd.total_size + seg.nsects * sizeof(struct section);
	if (es != (size_t)lcu.lc.cmdsize)
		return (macho_error(file, VE_INVALID_SEGMENT_COUNT, DEBUG_TUPLE));
	printf("segment32: '%.16s'\n", lcu.segment_64.segname);
	if (vm_map_new_entry(&file->vmm, seg.vmaddr, seg.vmaddr + seg.vmsize,
						 file->vmm.biggest_id + 1) != ME_OK)
		return (file->err = VE_ME_MASK | file->vmm.err);
	if (seg.fileoff >= file->mm.size)
		return (macho_error(file, VE_INVALID_LOAD_COMMAND_IN_FILE_ADDR,
							DEBUG_TUPLE));
	if (seg.fileoff + seg.filesize > file->mm.size)
		return (macho_error(file, VE_INVALID_LOAD_COMMAND_FILE_BLOCK_SIZE,
							DEBUG_TUPLE));
	return (vlc_sections_32(lcd, lcu, file));
}
