/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mach_o_validator.h                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmartzol <hmartzol@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2000/00/00 00:00:00 by hmartzol          #+#    #+#             */
/*   Updated: 2000/00/00 00:00:00 by hmartzol         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MACH_O_VALIDATOR_H
# define MACH_O_VALIDATOR_H

/*
** archive headers (<common_path>/usr/include)
*/

# include <ar.h>

/*
** fat/universal headers (<common_path>/usr/include)
*/

# include <mach-o/fat.h>

/*
** macho headers (<common_path>/usr/include)
*/

# include <mach-o/loader.h>
# include <mach-o/nlist.h>
# include <mach-o/swap.h>
# include <mach-o/reloc.h>
# include <mach-o/stab.h>

/*
** local headers (<project_root>/inc)
*/

# include <memory.h>

# pragma pack(push, 1)

typedef struct								s_mach_o_segment_command
{
	uint32_t	cmd;
	uint32_t	cmdsize;
	char		segname[16];
	uint32_t	vmaddr;
	uint32_t	vmsize;
	uint32_t	fileoff;
	uint32_t	filesize;
	vm_prot_t	maxprot;
	vm_prot_t	initprot;
	uint32_t	nsects;
	uint32_t	flags;
}											t_mach_o_segment_command;

typedef struct mach_header					t_moh32;
typedef struct mach_header_64				t_moh64;

typedef union								u_macho_header_ptr
{
	uint8_t	*data;
	t_moh32	*h32;
	t_moh64	*h64;
}											t_macho_header_ptr;

# pragma pack(pop)

/*
** 0xFF: unusable, throw error on operation
** 0x00: unused/padding/default state
** 0x01: file header
** 0x02: load commands
** 0x03-0x31: command
** 0x32-0xFE: data
*/

typedef enum								e_memory_map_claim
{
	MM_BADLAND = 0xFF,
	MM_PAD = 0x00,
	MM_HEADER,
	MM_CMD,
	MM_SECTION,
	MM_RELOCATION,
	MM_SYMTAB,
	MM_STRTAB,
	MM_DYLIB,
	MM_FUNC,
	MM_DCE,
	MM_DYSYMTAB,

}											t_memory_map_claim;

typedef enum								e_validator_error
{
	VE_OK = 0,
	VE_ME_MASK = 0x80000000,
	VE_INVALID_MAGIC_NUMBER = 1,
	VE_INVALID_SEGMENT_COUNT,
	VE_INVALID_CPU_TYPE,
	VE_INVALID_FILE_TYPE,
	VE_INVALID_NUMBER_OF_COMMANDS,
	VE_INVALID_TOTAL_COMMAND_SIZE,
	VE_INVALID_COMMAND_SIZE,
	VE_INVALID_LOAD_COMMAND_IN_FILE_ADDR,
	VE_INVALID_LOAD_COMMAND_FILE_BLOCK_SIZE,
	VE_INVALID_LOAD_COMMAND_ID
}											t_validator_error;

typedef struct								s_macho_file
{
	t_memory_map			mm;
	t_vm_map				vmm;
	t_macho_header_ptr		file;
	t_moh64					head;
	t_validator_error		err;
	int						format;
}											t_macho_file;

typedef union								u_load_command_union
{
	uint8_t								data[80];
	struct load_command					lc;
	struct segment_command				segment_32;
	struct segment_command_64			segment_64;
	struct fvmlib_command				fmvlib;
	struct dylib_command				dylib;
	struct sub_framework_command		sub_framework;
	struct sub_client_command			sub_client;
	struct sub_umbrella_command			sub_umbrella;
	struct sub_library_command			sub_library;
	struct prebound_dylib_command		prebound_dylib;
	struct dylinker_command				dylinker;
	struct thread_command				thread;
	struct routines_command				routines_32;
	struct routines_command_64			routines_64;
	struct symtab_command				symtab;
	struct dysymtab_command				dysymtab;
	struct twolevel_hints_command		twolevel_hints;
	struct prebind_cksum_command		prebind_cksum;
	struct uuid_command					uuid;
	struct rpath_command				rpath;
	struct linkedit_data_command		linkedit_data;
	struct encryption_info_command		encryption_info_32;
	struct encryption_info_command_64	encryption_info_64;
	struct version_min_command			version_min;
	struct dyld_info_command			dyld_info;
	struct linker_option_command		linker_option;
	struct symseg_command				symseg;
	struct ident_command				ident;
	struct fvmfile_command				fvmfile;
	struct entry_point_command			entry_point;
	struct source_version_command		source_version;
}											t_load_command_union;

typedef struct								s_load_command_element
{
	uint8_t		size;
	uint8_t		swap;
}											t_load_command_element;

typedef struct s_load_command_descriptor	t_load_command_descriptor;

typedef t_validator_error					(*t_vlc)(
										const t_load_command_descriptor *lcd,
										const t_load_command_union lcu,
										t_macho_file *file);

struct										s_load_command_descriptor
{
	uint32_t			id;
	t_vlc				vlc;
	t_struct_descriptor	sd;
};

t_validator_error							macho_error(t_macho_file *mo,
											const t_validator_error err,
											const t_debug_tuple debug_tuple);

t_validator_error							validate_magic(t_macho_file *obj);
t_validator_error							validate_head(t_macho_file *obj);

t_validator_error							validate_commands(
															t_macho_file *obj);

t_validator_error							vlc_sections_64(
	const t_load_command_descriptor *lcd,
	const t_load_command_union lcu,
	t_macho_file *file);
t_validator_error							vlc_segment_64(
	const t_load_command_descriptor *lcd,
	const t_load_command_union lcu,
	t_macho_file *file);

t_validator_error							vlc_sections_32(
	const t_load_command_descriptor *lcd,
	const t_load_command_union lcu,
	t_macho_file *file);
t_validator_error							vlc_segment_32(
	const t_load_command_descriptor *lcd,
	const t_load_command_union lcu,
	t_macho_file *file);

t_validator_error							vlc_symtab(
	const t_load_command_descriptor *lcd,
	const t_load_command_union lcu,
	t_macho_file *file);

#endif
