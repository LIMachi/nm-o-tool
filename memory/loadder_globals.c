/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   loadder_globals.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmartzol <hmartzol@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2000/00/00 00:00:00 by hmartzol          #+#    #+#             */
/*   Updated: 2000/00/00 00:00:00 by hmartzol         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <memory.h>
#include <mach-o/loader.h>

t_struct_descriptor	g_mach_header_descriptor = {
	.nb_members = 7, .align = 4, .total_size = sizeof(struct mach_header),
	.member = {
		{.block_size = sizeof(uint32_t), .nb_blocks = 1, .align = 4, .sign = 0},
		{.block_size = sizeof(cpu_type_t), .nb_blocks = 1, .align = 4,
			.sign = 0},
		{.block_size = sizeof(cpu_subtype_t), .nb_blocks = 1, .align = 4,
			.sign = 0},
		{.block_size = sizeof(uint32_t), .nb_blocks = 1, .align = 4, .sign = 0},
		{.block_size = sizeof(uint32_t), .nb_blocks = 1, .align = 4, .sign = 0},
		{.block_size = sizeof(uint32_t), .nb_blocks = 1, .align = 4, .sign = 0},
		{.block_size = sizeof(uint32_t), .nb_blocks = 1, .align = 4, .sign = 0}
	}
};

t_struct_descriptor	g_mach_header_64_descriptor = {
	.nb_members = 8, .align = 4, .total_size = sizeof(struct mach_header_64),
	.member = {
		{.block_size = sizeof(uint32_t), .nb_blocks = 1, .align = 4, .sign = 0},
		{.block_size = sizeof(cpu_type_t), .nb_blocks = 1, .align = 4,
			.sign = 0},
		{.block_size = sizeof(cpu_subtype_t), .nb_blocks = 1, .align = 4,
			.sign = 0},
		{.block_size = sizeof(uint32_t), .nb_blocks = 1, .align = 4, .sign = 0},
		{.block_size = sizeof(uint32_t), .nb_blocks = 1, .align = 4, .sign = 0},
		{.block_size = sizeof(uint32_t), .nb_blocks = 1, .align = 4, .sign = 0},
		{.block_size = sizeof(uint32_t), .nb_blocks = 1, .align = 4, .sign = 0},
		{.block_size = sizeof(uint32_t), .nb_blocks = 1, .align = 4, .sign = 0}
	}
};

t_struct_descriptor	g_section_descriptor = {
	.nb_members = 11, .align = 4, .total_size = sizeof(struct section),
	.member = {
		{.block_size = 1, .nb_blocks = 16, .align = 1, .sign = 0},
		{.block_size = 1, .nb_blocks = 16, .align = 1, .sign = 0},
		{.block_size = sizeof(uint32_t), .nb_blocks = 1, .align = 4, .sign = 0},
		{.block_size = sizeof(uint32_t), .nb_blocks = 1, .align = 4, .sign = 0},
		{.block_size = sizeof(uint32_t), .nb_blocks = 1, .align = 4, .sign = 0},
		{.block_size = sizeof(uint32_t), .nb_blocks = 1, .align = 4, .sign = 0},
		{.block_size = sizeof(uint32_t), .nb_blocks = 1, .align = 4, .sign = 0},
		{.block_size = sizeof(uint32_t), .nb_blocks = 1, .align = 4, .sign = 0},
		{.block_size = sizeof(uint32_t), .nb_blocks = 1, .align = 4, .sign = 0},
		{.block_size = sizeof(uint32_t), .nb_blocks = 1, .align = 4, .sign = 0},
		{.block_size = sizeof(uint32_t), .nb_blocks = 1, .align = 4, .sign = 0}
	}
};

t_struct_descriptor	g_section_64_descriptor = {
	.nb_members = 11, .align = 4, .total_size = sizeof(struct section_64),
	.member = {
		{.block_size = 1, .nb_blocks = 16, .align = 1, .sign = 0},
		{.block_size = 1, .nb_blocks = 16, .align = 1, .sign = 0},
		{.block_size = sizeof(uint64_t), .nb_blocks = 1, .align = 4, .sign = 0},
		{.block_size = sizeof(uint64_t), .nb_blocks = 1, .align = 4, .sign = 0},
		{.block_size = sizeof(uint32_t), .nb_blocks = 1, .align = 4, .sign = 0},
		{.block_size = sizeof(uint32_t), .nb_blocks = 1, .align = 4, .sign = 0},
		{.block_size = sizeof(uint32_t), .nb_blocks = 1, .align = 4, .sign = 0},
		{.block_size = sizeof(uint32_t), .nb_blocks = 1, .align = 4, .sign = 0},
		{.block_size = sizeof(uint32_t), .nb_blocks = 1, .align = 4, .sign = 0},
		{.block_size = sizeof(uint32_t), .nb_blocks = 1, .align = 4, .sign = 0},
		{.block_size = sizeof(uint32_t), .nb_blocks = 1, .align = 4, .sign = 0}
	}
};

t_struct_descriptor	g_load_commands_descriptors[0x40] = {
	[0] = {2, 4, sizeof(struct load_command), {
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0}
		}},
	[LC_SEGMENT & 0x3F] = {11, 4, sizeof(struct segment_command), {
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{1, 16, 1, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(vm_prot_t), 1, 4, 0},
			{sizeof(vm_prot_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0}
		}},
	[LC_SEGMENT_64 & 0x3F] = {11, 4, sizeof(struct segment_command_64), {
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{1, 16, 1, 0},
			{sizeof(uint64_t), 1, 4, 0},
			{sizeof(uint64_t), 1, 4, 0},
			{sizeof(uint64_t), 1, 4, 0},
			{sizeof(uint64_t), 1, 4, 0},
			{sizeof(vm_prot_t), 1, 4, 0},
			{sizeof(vm_prot_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0}
		}},
	[LC_IDFVMLIB & 0x3F] = {5, 4, sizeof(struct fvmlib_command), {
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(union lc_str), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0}
		}},
	[LC_LOADFVMLIB & 0x3F] = {5, 4, sizeof(struct fvmlib_command), {
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(union lc_str), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0}
		}},
	[LC_ID_DYLIB & 0x3F] = {6, 4, sizeof(struct dylib_command), {
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(union lc_str), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0}
		}},
	[LC_LOAD_WEAK_DYLIB & 0x3F] = {6, 4, sizeof(struct dylib_command), {
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(union lc_str), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0}
		}},
	[LC_LOAD_DYLIB & 0x3F] = {6, 4, sizeof(struct dylib_command), {
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(union lc_str), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0}
		}},
	[LC_REEXPORT_DYLIB & 0x3F] = {6, 4, sizeof(struct dylib_command), {
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(union lc_str), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0}
		}},
	[LC_SUB_FRAMEWORK & 0x3F] = {3, 4, sizeof(struct sub_framework_command), {
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(union lc_str), 1, 4, 0}
		}},
	[LC_SUB_CLIENT & 0x3F] = {3, 4, sizeof(struct sub_client_command), {
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(union lc_str), 1, 4, 0}
		}},
	[LC_SUB_UMBRELLA & 0x3F] = {3, 4, sizeof(struct sub_umbrella_command), {
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(union lc_str), 1, 4, 0}
		}},
	[LC_SUB_LIBRARY & 0x3F] = {3, 4, sizeof(struct sub_library_command), {
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(union lc_str), 1, 4, 0}
		}},
	[LC_PREBOUND_DYLIB & 0x3F] = {5, 4, sizeof(struct prebound_dylib_command), {
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(union lc_str), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(union lc_str), 1, 4, 0}
		}},
	[LC_ID_DYLINKER & 0x3F] = {3, 4, sizeof(struct dylinker_command), {
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(union lc_str), 1, 4, 0}
		}},
	[LC_LOAD_DYLINKER & 0x3F] = {3, 4, sizeof(struct dylinker_command), {
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(union lc_str), 1, 4, 0}
		}},
	[LC_DYLD_ENVIRONMENT & 0x3F] = {3, 4, sizeof(struct dylinker_command), {
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(union lc_str), 1, 4, 0}
		}},
	[LC_THREAD & 0x3F] = {2, 4, sizeof(struct thread_command), {
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0}
		}},
	[LC_UNIXTHREAD & 0x3F] = {2, 4, sizeof(struct thread_command), {
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0}
		}},
	[LC_ROUTINES & 0x3F] = {11, 4, sizeof(struct routines_command), {
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0}
		}},
	[LC_ROUTINES_64 & 0x3F] = {11, 4, sizeof(struct routines_command_64), {
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint64_t), 1, 4, 0},
			{sizeof(uint64_t), 1, 4, 0},
			{sizeof(uint64_t), 1, 4, 0},
			{sizeof(uint64_t), 1, 4, 0},
			{sizeof(uint64_t), 1, 4, 0},
			{sizeof(uint64_t), 1, 4, 0},
			{sizeof(uint64_t), 1, 4, 0},
			{sizeof(uint64_t), 1, 4, 0},
			{sizeof(uint64_t), 1, 4, 0}
		}},
	[LC_SYMTAB & 0x3F] = {6, 4, sizeof(struct symtab_command), {
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint64_t), 1, 4, 0},
			{sizeof(uint64_t), 1, 4, 0},
			{sizeof(uint64_t), 1, 4, 0},
			{sizeof(uint64_t), 1, 4, 0}
		}},
	[LC_DYSYMTAB & 0x3F] = {20, 4, sizeof(struct dysymtab_command), {
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint64_t), 1, 4, 0},
			{sizeof(uint64_t), 1, 4, 0},
			{sizeof(uint64_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint64_t), 1, 4, 0},
			{sizeof(uint64_t), 1, 4, 0},
			{sizeof(uint64_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint64_t), 1, 4, 0},
			{sizeof(uint64_t), 1, 4, 0},
			{sizeof(uint64_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint64_t), 1, 4, 0},
			{sizeof(uint64_t), 1, 4, 0},
			{sizeof(uint64_t), 1, 4, 0}
		}},
	[LC_PREBIND_CKSUM & 0x3F] = {3, 4, sizeof(struct prebind_cksum_command), {
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint64_t), 1, 4, 0}
		}},
	[LC_UUID & 0x3F] = {3, 4, sizeof(struct uuid_command), {
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint8_t), 16, 1, 0}
		}},
	[LC_RPATH & 0x3F] = {3, 4, sizeof(struct rpath_command), {
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(union lc_str), 1, 4, 0}
		}},
	[LC_CODE_SIGNATURE & 0x3F] = {4, 4, sizeof(struct linkedit_data_command), {
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0}
		}},
	[LC_SEGMENT_SPLIT_INFO & 0x3F] = {4, 4,
		sizeof(struct linkedit_data_command), {
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0}
		}},
	[LC_FUNCTION_STARTS & 0x3F] = {4, 4, sizeof(struct linkedit_data_command), {
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0}
		}},
	[LC_DATA_IN_CODE & 0x3F] = {4, 4, sizeof(struct linkedit_data_command), {
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0}
		}},
	[LC_DYLIB_CODE_SIGN_DRS & 0x3F] = {4, 4,
		sizeof(struct linkedit_data_command), {
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0}
		}},
	[LC_LINKER_OPTIMIZATION_HINT & 0x3F] = {4, 4,
		sizeof(struct linkedit_data_command), {
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0}
		}},
	[LC_ENCRYPTION_INFO & 0x3F] = {5, 4, sizeof(struct encryption_info_command),
		{
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0}
		}},
	[LC_ENCRYPTION_INFO_64 & 0x3F] = {6, 4,
		sizeof(struct encryption_info_command_64), {
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0}
		}},
	[LC_VERSION_MIN_MACOSX & 0x3F] = {4, 4, sizeof(struct version_min_command),
		{
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0}
		}},
	[LC_VERSION_MIN_IPHONEOS & 0x3F] = {4, 4,
		sizeof(struct version_min_command), {
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0}
		}},
	[LC_VERSION_MIN_WATCHOS & 0x3F] = {4, 4, sizeof(struct version_min_command),
		{
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0}
		}},
	[LC_VERSION_MIN_TVOS & 0x3F] = {4, 4, sizeof(struct version_min_command), {
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0}
		}},
	[LC_BUILD_VERSION & 0x3F] = {6, 4, sizeof(struct build_version_command), {
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0}
		}},
	[LC_DYLD_INFO & 0x3F] = {12, 4, sizeof(struct dyld_info_command), {
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0}
		}},
	[LC_LINKER_OPTION & 0x3F] = {3, 4, sizeof(struct linker_option_command), {
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0}
		}},
	[LC_SYMSEG & 0x3F] = {4, 4, sizeof(struct symseg_command), {
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0}
		}},
	[LC_IDENT & 0x3F] = {2, 4, sizeof(struct ident_command), {
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0}
		}},
	[LC_FVMFILE & 0x3F] = {4, 4, sizeof(struct fvmfile_command), {
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(union lc_str), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0}
		}},
	[LC_MAIN & 0x3F] = {4, 4, sizeof(struct entry_point_command), {
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint64_t), 1, 4, 0},
			{sizeof(uint64_t), 1, 4, 0}
		}},
	[LC_SOURCE_VERSION & 0x3F] = {3, 4, sizeof(struct source_version_command), {
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint64_t), 1, 4, 0}
		}},
	[LC_NOTE & 0x3F] = {5, 4, sizeof(struct note_command), {
			{sizeof(uint32_t), 1, 4, 0},
			{sizeof(uint32_t), 1, 4, 0},
			{1, 16, 1, 0},
			{sizeof(uint64_t), 1, 4, 0},
			{sizeof(uint64_t), 1, 4, 0}
		}}
};
