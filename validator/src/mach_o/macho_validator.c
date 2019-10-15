/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   macho_validator.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmartzol <hmartzol@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2000/00/00 00:00:00 by hmartzol          #+#    #+#             */
/*   Updated: 2000/00/00 00:00:00 by hmartzol         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <mach_o_validator.h>

/*
** validate a single macho (object/executable) file using his map
*/

t_validator_error	validate_macho(t_memory_map mm)
{
	t_macho_file	obj;

	obj = (t_macho_file){.err = VE_OK, .mm = mm, .file = {.data = mm.ptr},
		.vmm = {.total_size = 0, .nb_entries = 0, .err = ME_OK, .entries = NULL,
			.biggest_id = 0}};
	if (validate_head(&obj) != VE_OK)
		return (obj.err);
	if (claim_map(&obj.mm, (t_memory_descriptor){1, obj.head.sizeofcmds, 1, 0},
			MM_CMD, 0) != ME_OK)
		return (VE_ME_MASK | obj.mm.err);
	if (validate_commands(&obj) != VE_OK)
		return (obj.err);
	return (VE_OK);
}

int					main(int argc, char *argv[])
{
	t_memory_map		mm;

	if (argc < 2)
		return (0);
	if (memory_map_from_file(&mm, argv[1]) != VE_OK)
		return (mm.err);
	return (validate_macho(mm));
}
