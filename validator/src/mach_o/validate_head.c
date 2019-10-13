/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   validate_head.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmartzol <hmartzol@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2000/00/00 00:00:00 by hmartzol          #+#    #+#             */
/*   Updated: 2000/00/00 00:00:00 by hmartzol         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <mach_o_validator.h>

t_validator_error	validate_head_common(t_macho_file *obj)
{
	const uint32_t		cpu_types[14] = {CPU_TYPE_VAX, CPU_TYPE_MC680x0,
		CPU_TYPE_X86, CPU_TYPE_I386, CPU_TYPE_X86_64, CPU_TYPE_MC98000,
		CPU_TYPE_HPPA, CPU_TYPE_ARM, CPU_TYPE_ARM64, CPU_TYPE_MC88000,
		CPU_TYPE_SPARC, CPU_TYPE_I860, CPU_TYPE_POWERPC, CPU_TYPE_POWERPC64};
	const uint32_t		file_types[11] = {MH_OBJECT, MH_EXECUTE, MH_FVMLIB,
		MH_CORE, MH_PRELOAD, MH_DYLIB, MH_DYLINKER, MH_BUNDLE, MH_DYLIB_STUB,
		MH_DSYM, MH_KEXT_BUNDLE};

	if (in(&obj->head.cputype, (t_memory_descriptor){4, 14, 4, 0},
			cpu_types, 0) == (size_t)-1)
		return (macho_error(obj, VE_INVALID_CPU_TYPE, DEBUG_TUPLE));
	if (in(&obj->head.filetype, (t_memory_descriptor){4, 11, 4, 0},
			file_types, 0) == (size_t)-1)
		return (macho_error(obj, VE_INVALID_FILE_TYPE, DEBUG_TUPLE));
	if (obj->head.ncmds >= obj->mm.size
		|| obj->head.ncmds * sizeof(struct load_command) >= obj->mm.size)
		return (macho_error(obj, VE_INVALID_NUMBER_OF_COMMANDS, DEBUG_TUPLE));
	if (obj->head.sizeofcmds >= obj->mm.size)
		return (macho_error(obj, VE_INVALID_TOTAL_COMMAND_SIZE, DEBUG_TUPLE));
	return (VE_OK);
}

t_validator_error	validate_head_32(t_macho_file *obj)
{
	const t_struct_descriptor	sd_head_32 = {4, 1, sizeof(struct mach_header),

	{{4, 1, 4, 0}, {sizeof(cpu_type_t), 1, sizeof(cpu_type_t), 1},
	{sizeof(cpu_subtype_t), 1, sizeof(cpu_subtype_t), 1}, {4, 4, 4, 1}}};
	if (claim_map(&obj->mm, (t_memory_descriptor){sizeof(struct mach_header),
			1, 1, 0}, MM_HEADER, 0) != ME_OK)
		return (obj->err = VE_ME_MASK | obj->mm.error);
	if (read_struct_in_memory(&obj->mm, &obj->head, MM_HEADER, sd_head_32)
			!= ME_OK)
		return (obj->err = VE_ME_MASK | obj->mm.error);
	obj->head.reserved = 0;
	return (validate_head_common(obj));
}

t_validator_error	validate_head_64(t_macho_file *obj)
{
	const t_struct_descriptor	sd_head_64 = {4, 1, sizeof(struct mach_header_64),
		{{4, 1, 4, 0}, {sizeof(cpu_type_t), 1, sizeof(cpu_type_t), 1},
		{sizeof(cpu_subtype_t), 1, sizeof(cpu_subtype_t), 1}, {4, 5, 4, 1}}};

	if (claim_map(&obj->mm, (t_memory_descriptor){sizeof(struct mach_header_64),
			1, 1, 0}, MM_HEADER, 0) != ME_OK)
		return (obj->err = VE_ME_MASK | obj->mm.error);
	if (read_struct_in_memory(&obj->mm, &obj->head, MM_HEADER, sd_head_64)
			!= ME_OK)
		return (obj->err = VE_ME_MASK | obj->mm.error);
	return (validate_head_common(obj));
}

t_validator_error	validate_head(t_macho_file *obj)
{
	if (validate_magic(obj) != VE_OK)
		return (obj->err);
	obj->mm.cursor = 0;
	if (obj->format == 64)
		validate_head_64(obj);
	else
		validate_head_32(obj);
	return (obj->err);
}
