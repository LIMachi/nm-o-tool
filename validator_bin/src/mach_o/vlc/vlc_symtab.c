/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vlc_symtab.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmartzol <hmartzol@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2000/00/00 00:00:00 by hmartzol          #+#    #+#             */
/*   Updated: 2000/00/00 00:00:00 by hmartzol         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <mach_o_validator.h>
#include <stdio.h>

t_validator_error	vlc_symtab(const t_load_command_descriptor *lcd,
	const t_load_command_union lcu, t_macho_file *file)
{
	size_t						h;
	struct symtab_command		symt;

	(void)lcd;
	symt = lcu.symtab;
	h = file->mm.cursor;
	if (symt.cmdsize != sizeof(symt))
		return (macho_error(file, VE_INVALID_TOTAL_COMMAND_SIZE, DEBUG_TUPLE));
	file->mm.cursor = symt.symoff;
	if (claim_map(&file->mm, (t_memory_descriptor){sizeof(struct nlist),
			symt.nsyms, sizeof(struct nlist), 0}, MM_SYMTAB, 0) != ME_OK)
		return (file->err = VE_ME_MASK | file->mm.err);
	file->mm.cursor = symt.stroff;
	if (claim_map(&file->mm, (t_memory_descriptor){1, symt.strsize, 1, 0},
			MM_STRTAB, 0) != ME_OK)
		return (file->err = VE_ME_MASK | file->mm.err);
	file->mm.cursor = h;
	printf("validated symtab command\n");
	return (VE_OK);
}
