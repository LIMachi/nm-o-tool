/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   validator.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lee <lee@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/23 18:44:24 by lee               #+#    #+#             */
/*   Updated: 2018/09/11 00:34:57 by lee              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef VALIDATOR_H
# define VALIDATOR_H

typedef struct	s_mem_area
{
	void		*start;
	void		*end;
}				t_mem_area;

typedef enum	e_validation_error
{
	EVE_OK = 0,
	EVE_UNIMPLEMENTED_FEATURE,
	EVE_MALFORMATED_PATH,
	EVE_MALFORMATED_MEM_AREA,
	EVE_CANT_OPEN_FILE,
	EVE_CANT_STAT_FILE,
	EVE_CANT_READ_FILE,
	EVE_CANT_CLOSE_FILE,
	EVE_INVALID_HEADER,
	EVE_UNKNOWN_HEADER,
	EVE_INVALID_LOAD_COMMAND,
	EVE_INVALID_LOAD_COMMAND_SIZE,
	EVE_MISMATCHED_COMMAND_COUNT,
	EVE_INVALID_OFFSET,
}				t_validation_error;

union			u_macho_header
{
	mach_header		x32;
	mach_header_64	x64;
};

typedef union	u_load_command
{
	struct load_command					lc;
	struct segment_command				seg32;
	struct segment_command_64			seg64;
	struct fvmlib_command				fvmlib;
	struct dylib_command				dylib;
	struct sub_framework_command		subframe;
	struct sub_client_command			subclient;
	struct sub_umbrella_command			subumbrella;
	struct sub_library_command			sublib;
	struct prebound_dylib_command		prebounddylib;
	struct dylinker_command				dylinker;
	struct thread_command				thread;
	struct routines_command				routine32;
	struct routines_command_64			routine64;
	struct symtab_command				symtab;
	struct dysymtab_command				dysymtab;
	struct twolevel_hints_command		twolevelhint;
	struct prebind_chksum_command		cheksum;
	struct uuid_command					uuid;
	struct rpath_command				rpath;
	struct link_edit_data_command		linkeditdata;
	struct encryption_info_command		encryptioninfo32;
	struct encryption_info_command_64	encryptioninfo64;
	struct version_min_command			versionmin;
	struct build_version_command		buildversion;
	struct dylib_info_command			dylibinfo;
	struct linker_option_command		linkeroption;
	struct symseg_command				symseg;
	struct fvmfile_command				fvmfile;
	struct entry_point_command			entrypoint;
	struct source_version_command		sourceversion;
	struct note_command					note;
}				t_load_command;

# define P32 1, 4
# define P232 P32, P32
# define P432 P232, P232
# define P264 1, 8, 1, 8
# define PCPU 1, sizeof(cpu_type_t), 1, sizeof(cpu_subtype_t)
# define P2VP 1, sizeof(vm_prot_t), 1, sizeof(vm_prot_t)
# define MACHO_HEAD32_PATTERN ((size_t[15]){7, P32, PCPU, P432})
# define MACHO_HEAD64_PATTERN ((size_t[17]){8, P32, PCPU, P432, P32})
# define LC_PATTERN ((size_t[5]){2, P232})
# define SEG32_PATTERN ((size_t[23]){11, P232, 0, 16, P232, P232, P2VP, P232})
# define SEG64_PATTERN ((size_t[23]){11, P232, 0, 16, P264, P264, P2VP, P232})
# define SYMTAB_PATTERN ((size_t[13]){6, P232, P232, P232})

typedef struct	s_macho_info
{
	uint32_t				x64 : 1;
	uint32_t				endian : 1;
	uint32_t				pad_0 : 30;
	t_mem_area				file_area;
	t_mem_area				head_area;
	union u_macho_header	header;
	t_mem_area				commands_area;
	t_mem_area				data_area;
}				t_macho_info;

#endif
