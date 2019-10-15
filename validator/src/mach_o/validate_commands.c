/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   validate_commands.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmartzol <hmartzol@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2000/00/00 00:00:00 by hmartzol          #+#    #+#             */
/*   Updated: 2000/00/00 00:00:00 by hmartzol         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <mach_o_validator.h>
#include <stdio.h>

/*
** default command validator, always return valid on any command
*/

t_validator_error	vlc_noop(const t_load_command_descriptor *lcd,
	const t_load_command_union lcu, t_macho_file *file)
{
	(void)lcd;
	(void)file;
	(void)lcu;
	return (VE_OK);
}

static const t_load_command_descriptor	g_lcd[47] = {
	{LC_SEGMENT, vlc_noop, {5, 1, sizeof(struct segment_command),
			{{4, 2, 4, 1}, {1, 16, 1, 0}, {4, 4, 4, 1},
				{sizeof(vm_prot_t), 2, sizeof(vm_prot_t), 1}, {4, 2, 4, 1}}}},
	{LC_SEGMENT_64, vlc_segment_64, {5, 1, sizeof(struct segment_command_64),
			{{4, 2, 4, 1}, {1, 16, 1, 0}, {8, 4, 8, 1},
				{sizeof(vm_prot_t), 2, sizeof(vm_prot_t), 1}, {4, 2, 4, 1}}}},
	{LC_IDFVMLIB, vlc_noop, {3, 1, sizeof(struct fvmlib_command), {{4, 2, 4, 1},
				{sizeof(union lc_str), 1, sizeof(union lc_str), 1},
				{4, 2, 4, 1}}}},
	{LC_LOADFVMLIB, vlc_noop, {3, 1, sizeof(struct fvmlib_command),
			{{4, 2, 4, 1}, {sizeof(union lc_str), 1, sizeof(union lc_str), 1},
				{4, 2, 4, 1}}}},
	{LC_ID_DYLIB, vlc_noop, {3, 1, sizeof(struct dylib_command), {{4, 2, 4, 1},
				{sizeof(union lc_str), 1, sizeof(union lc_str), 1},
				{4, 3, 4, 1}}}},
	{LC_LOAD_DYLIB, vlc_noop, {3, 1, sizeof(struct dylib_command),
			{{4, 2, 4, 1}, {sizeof(union lc_str), 1, sizeof(union lc_str), 1},
				{4, 3, 4, 1}}}},
	{LC_LOAD_WEAK_DYLIB, vlc_noop, {3, 1, sizeof(struct dylib_command),
			{{4, 2, 4, 1}, {sizeof(union lc_str), 1, sizeof(union lc_str), 1},
				{4, 3, 4, 1}}}},
	{LC_REEXPORT_DYLIB, vlc_noop, {3, 1, sizeof(struct dylib_command),
			{{4, 2, 4, 1}, {sizeof(union lc_str), 1, sizeof(union lc_str), 1},
				{4, 3, 4, 1}}}},
	{LC_SUB_FRAMEWORK, vlc_noop, {2, 1, sizeof(struct sub_framework_command),
			{{4, 2, 4, 1},
				{sizeof(union lc_str), 1, sizeof(union lc_str), 1}}}},
	{LC_SUB_CLIENT, vlc_noop, {2, 1, sizeof(struct sub_client_command),
			{{4, 2, 4, 1},
				{sizeof(union lc_str), 1, sizeof(union lc_str), 1}}}},
	{LC_SUB_UMBRELLA, vlc_noop, {2, 1, sizeof(struct sub_umbrella_command),
			{{4, 2, 4, 1},
				{sizeof(union lc_str), 1, sizeof(union lc_str), 1}}}},
	{LC_SUB_LIBRARY, vlc_noop, {2, 1, sizeof(struct sub_library_command),
			{{4, 2, 4, 1},
				{sizeof(union lc_str), 1, sizeof(union lc_str), 1}}}},
	{LC_PREBOUND_DYLIB, vlc_noop, {4, 1, sizeof(struct prebound_dylib_command),
			{{4, 2, 4, 1}, {sizeof(union lc_str), 1, sizeof(union lc_str), 1},
				{4, 1, 4, 1},
				{sizeof(union lc_str), 1, sizeof(union lc_str), 1}}}},
	{LC_ID_DYLINKER, vlc_noop, {2, 1, sizeof(struct dylinker_command),
			{{4, 2, 4, 1},
				{sizeof(union lc_str), 1, sizeof(union lc_str), 1}}}},
	{LC_LOAD_DYLINKER, vlc_noop, {2, 1, sizeof(struct dylinker_command),
			{{4, 2, 4, 1},
				{sizeof(union lc_str), 1, sizeof(union lc_str), 1}}}},
	{LC_DYLD_ENVIRONMENT, vlc_noop, {2, 1, sizeof(struct dylinker_command),
			{{4, 2, 4, 1},
				{sizeof(union lc_str), 1, sizeof(union lc_str), 1}}}},
	{LC_THREAD, vlc_noop, {1, 1, sizeof(struct thread_command),
			{{4, 2, 4, 1}}}},
	{LC_UNIXTHREAD, vlc_noop, {1, 1, sizeof(struct thread_command),
			{{4, 2, 4, 1}}}},
	{LC_ROUTINES, vlc_noop, {1, 1, sizeof(struct routines_command),
			{{4, 10, 4, 1}}}},
	{LC_ROUTINES_64, vlc_noop, {2, 1, sizeof(struct routines_command_64),
			{{4, 2, 4, 1}, {8, 8, 8, 1}}}},
	{LC_SYMTAB, vlc_noop, {1, 1, sizeof(struct symtab_command),
			{{4, 6, 4, 1}}}},
	{LC_DYSYMTAB, vlc_noop, {1, 1, sizeof(struct dysymtab_command),
			{{4, 20, 4, 1}}}},
	{LC_TWOLEVEL_HINTS, vlc_noop, {1, 1, sizeof(struct twolevel_hints_command),
			{{4, 4, 4, 1}}}},
	{LC_PREBIND_CKSUM, vlc_noop, {1, 1, sizeof(struct prebind_cksum_command),
			{{4, 3, 4, 1}}}},
	{LC_UUID, vlc_noop, {2, 1, sizeof(struct uuid_command), {{4, 2, 4, 1},
				{1, 16, 1, 0}}}},
	{LC_RPATH, vlc_noop, {2, 1, sizeof(struct rpath_command), {{4, 2, 4, 1},
				{sizeof(union lc_str), 1, sizeof(union lc_str), 1}}}},
	{LC_CODE_SIGNATURE, vlc_noop, {1, 1, sizeof(struct linkedit_data_command),
			{{4, 4, 4, 1}}}},
	{LC_SEGMENT_SPLIT_INFO, vlc_noop, {1, 1,
			sizeof(struct linkedit_data_command), {{4, 4, 4, 1}}}},
	{LC_FUNCTION_STARTS, vlc_noop, {1, 1, sizeof(struct linkedit_data_command),
			{{4, 4, 4, 1}}}},
	{LC_DATA_IN_CODE, vlc_noop, {1, 1, sizeof(struct linkedit_data_command),
			{{4, 4, 4, 1}}}},
	{LC_DYLIB_CODE_SIGN_DRS, vlc_noop, {1, 1,
			sizeof(struct linkedit_data_command), {{4, 4, 4, 1}}}},
	{LC_LINKER_OPTIMIZATION_HINT, vlc_noop, {1, 1,
			sizeof(struct linkedit_data_command), {{4, 4, 4, 1}}}},
	{LC_ENCRYPTION_INFO, vlc_noop, {1, 1,
			sizeof(struct encryption_info_command), {{4, 5, 4, 1}}}},
	{LC_ENCRYPTION_INFO_64, vlc_noop, {2, 1,
			sizeof(struct encryption_info_command_64), {{4, 5, 4, 1},
				{4, 1, 4, 0}}}},
	{LC_VERSION_MIN_MACOSX, vlc_noop, {1, 1, sizeof(struct version_min_command),
			{{4, 4, 4, 1}}}},
	{LC_VERSION_MIN_IPHONEOS, vlc_noop, {1, 1,
			sizeof(struct version_min_command), {{4, 4, 4, 1}}}},
	{LC_VERSION_MIN_WATCHOS, vlc_noop, {1, 1,
			sizeof(struct version_min_command), {{4, 4, 4, 1}}}},
	{LC_VERSION_MIN_TVOS, vlc_noop, {1, 1, sizeof(struct version_min_command),
			{{4, 4, 4, 1}}}},
	{LC_BUILD_VERSION, vlc_noop, {1, 1, sizeof(struct build_version_command),
			{{4, 6, 4, 1}}}},
	{LC_DYLD_INFO, vlc_noop, {1, 1, sizeof(struct dyld_info_command),
			{{4, 12, 4, 1}}}},
	{LC_DYLD_INFO_ONLY, vlc_noop, {1, 1, sizeof(struct dyld_info_command),
			{{4, 12, 4, 1}}}},
	{LC_LINKER_OPTION, vlc_noop, {1, 1, sizeof(struct linker_option_command),
			{{4, 3, 4, 1}}}},
	{LC_SYMSEG, vlc_noop, {1, 1, sizeof(struct symseg_command),
			{{4, 4, 4, 1}}}},
	{LC_IDENT, vlc_noop, {1, 1, sizeof(struct ident_command),
			{{4, 2, 4, 1}}}},
	{LC_FVMFILE, vlc_noop, {3, 1, sizeof(struct ident_command), {{4, 2, 4, 1},
				{sizeof(union lc_str), 1, sizeof(union lc_str), 1},
				{4, 1, 4, 1}}}},
	{LC_MAIN, vlc_noop, {2, 1, sizeof(struct entry_point_command),
			{{4, 2, 4, 1}, {8, 2, 8, 1}}}},
	{LC_SOURCE_VERSION, vlc_noop, {2, 1, sizeof(struct source_version_command),
			{{4, 2, 4, 1}, {8, 1, 8, 1}}}}
};

t_validator_error	validate_commands(t_macho_file *obj)
{
	uint32_t				it;
	t_load_command_union	lcu;
	size_t					next;
	size_t					cmd;

	it = (uint32_t)-1;
	while (++it < obj->head.ncmds)
	{
		if (read_in_memory(&obj->mm, &lcu.lc, MM_CMD, MD_LOAD_COMMAND) != ME_OK)
			return (obj->err = VE_ME_MASK | obj->mm.err);
		obj->mm.cursor -= sizeof(struct load_command);
		next = obj->mm.cursor + lcu.lc.cmdsize;
		if (lcu.lc.cmdsize > 4096 || lcu.lc.cmdsize >= obj->mm.size
				|| next < obj->mm.cursor || next < lcu.lc.cmdsize)
			return (macho_error(obj, VE_INVALID_COMMAND_SIZE, DEBUG_TUPLE));
		if ((cmd = in(&lcu.lc.cmd, (t_memory_descriptor){4, 47,
				sizeof(t_load_command_descriptor), 0}, g_lcd, 0)) == (size_t)-1)
			return (macho_error(obj, VE_INVALID_LOAD_COMMAND_ID, DEBUG_TUPLE));
		if (read_struct_in_memory(&obj->mm, lcu.data, MM_CMD, g_lcd[cmd].sd))
			return (obj->err = VE_ME_MASK | obj->mm.err);
		if (g_lcd[cmd].vlc(&g_lcd[cmd], lcu, obj) != VE_OK)
			return (obj->err);
		obj->mm.cursor = next;
	}
	return (VE_OK);
}
