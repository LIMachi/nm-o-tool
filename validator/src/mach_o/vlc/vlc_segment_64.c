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

t_validator_error	vlc_sections_64(const t_load_command_descriptor *lcd,
									const t_load_command_union lcu, t_macho_file *file)
{
	size_t				it;
	unsigned long long	id;
	t_struct_descriptor	sd_sect64 = {3, 1, sizeof(struct section_64),
		{{16, 2, 1, 0}, {8, 2, 8, 1}, {4, 8, 4, 1}}};
	struct section_64	sec;

	(void)lcd;
	(void)file;
	it = (size_t)-1;
	id = file->vmm.biggest_id;
	while (++it < lcu.segment_64.nsects)
	{
		if (read_struct_in_memory(&file->mm, &sec, MM_CMD, sd_sect64) != ME_OK)
			return (file->err = VE_ME_MASK | file->mm.err);
		sec.segname[15] = '\0';
		printf("sect64: '%.16s'\n", sec.sectname);
		if (strcmp(sec.segname, lcu.segment_64.segname))
			return (macho_error(file, VE_MISMATCHED_SEGNAME, DEBUG_TUPLE));
		if (vm_map_recut_entry(&file->vmm, (t_vm_map_entry){file->vmm.biggest_id + 1, sec.addr, sec.addr + sec.size}, id) != ME_OK)
			return (file->err = VE_ME_MASK | file->vmm.err);
		//test sec.offset
		//test sec.align
		//test sec.reloff
		//test sec.nreloc
	}
	return (VE_OK);
}

t_validator_error	vlc_segment_64(const t_load_command_descriptor *lcd,
									const t_load_command_union lcu, t_macho_file *file)
{
	size_t						es;
	struct segment_command_64	seg;

	seg = lcu.segment_64;
	((t_load_command_union)lcu).segment_64.segname[15] = '\0';
	es = lcd->sd.total_size + seg.nsects * sizeof(struct section_64);
	if (es != (size_t)lcu.lc.cmdsize)
		return (macho_error(file, VE_INVALID_SEGMENT_COUNT, DEBUG_TUPLE));
	printf("valid segment (64): '%.16s'\n", lcu.segment_64.segname);
	if (vm_map_new_entry(&file->vmm, seg.vmaddr, seg.vmaddr + seg.vmsize,
						 file->vmm.biggest_id + 1) != ME_OK)
		return (file->err = VE_ME_MASK | file->vmm.err);
	if (seg.fileoff >= file->mm.size)
		return (macho_error(file, VE_INVALID_LOAD_COMMAND_IN_FILE_ADDR,
							DEBUG_TUPLE));
	if (seg.fileoff + seg.filesize > file->mm.size)
		return (macho_error(file, VE_INVALID_LOAD_COMMAND_FILE_BLOCK_SIZE,
							DEBUG_TUPLE));
	return (vlc_sections_64(lcd, lcu, file));
}
