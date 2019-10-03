#ifndef MACH_O_VALIDATOR_H
# define MACH_O_VALIDATOR_H

/*
** macho headers
*/

# include <mach-o/loader.h>
# include <mach-o/fat.h>
# include <mach-o/nlist.h>
# include <mach-o/swap.h>

/*
** struct stat, fstat
*/

# include <sys/types.h>
# include <sys/stat.h>

/*
** mmap
*/

# include <sys/mman.h>

/*
** open
*/

# include <fcntl.h>

/*
** close
*/

# include <unistd.h>

# pragma pack(push, 1)

typedef struct					s_mach_o_segment_command
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
}								t_mach_o_segment_command;

typedef struct mach_header		t_moh32;
typedef struct mach_header_64	t_moh64;

typedef union					u_macho_header_ptr
{
	uint8_t	*data;
	t_moh32	*h32;
	t_moh64	*h64;
}								t_macho_header_ptr;

# pragma pack(pop)

/*
** 0x00: unused/padding
** 0x01: file header
*/

typedef enum									e_memory_map
{
	MM_PAD = 0x00,
	MM_HEADER = 0x01,
	MM_CMD = 0x2
}												t_memory_map;

typedef enum									e_validator_error
{
	VE_OK = 0,
	VE_NULL_PTR,
	VE_COULD_NOT_OPEN_FILE,
	VE_COULD_NOT_RETRIEVE_STATS,
	VE_MAPPING_ERROR,
	VE_INVALID_HEAD_SIZE,
	VE_INVALID_MAGIC_NUMBER,
	VE_INVALID_WORD_SIZE,
	VE_INVALID_ENDIAN,
	VE_INVALID_ELF_VERSION,
	VE_INVALID_OS_ABI,
	VE_INVALID_TYPE,
	VE_INVALID_MACHINE,
	VE_INVALID_ENTRY_POSITION,
	VE_INVALID_SEGMENT_TABLE_POSITION,
	VE_INVALID_SECTION_TABLE_POSITION,
	VE_INVALID_SEGMENT_HEADER_SIZE,
	VE_INVALID_SECTION_HEADER_SIZE,
	VE_INVALID_SEGMENT_COUNT,
	VE_INVALID_SECTION_COUNT,
	VE_FILE_MAPPING_CONFLICT,
	VE_INVALID_BLOC_SIZE,
	VE_INVALID_FILE_POSITION,
	VE_INVALID_BLOC_COUNT,
	VE_INVALID_MAPING,
	VE_INVALID_CLAIM,
	VE_INVALID_CPU_TYPE,
	VE_INVALID_CPU_SUBTYPE,
	VE_INVALID_FILE_TYPE,
	VE_INVALID_NUMBER_OF_COMMANDS,
	VE_INVALID_TOTAL_COMMAND_SIZE,
	VE_INVALID_COMMAND_SIZE,
	VE_INVALID_LOAD_COMMAND_EXACT_VALUE,
	VE_INVALID_LOAD_COMMAND_IN_FILE_ADDR
}												t_validator_error;

typedef struct									s_cursor
{
	size_t			index;
	size_t			nb_blocs;
	size_t			block_size;
	size_t			align;
	t_memory_map	expected_mapping;
}												t_cursor;

typedef struct									s_macho_file
{
	int						fd;
	size_t					size;
	t_macho_header_ptr		file;
	uint8_t					*file_map;
	uint8_t					*memory_map;
	t_moh64					head;
	t_validator_error		err;
	int						endian;
	int						format;
}												t_macho_file;

typedef union									u_load_command_union
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
												t_load_command_descriptor *lcd,
												t_load_command_union lcu,
												t_macho_file *file);

struct										s_load_command_descriptor
{
	uint32_t				id;
	size_t					minimum_size;
	int						is_exact_size : 1;
	uint8_t					nb_elems;
	t_vlc					vlc;
	t_load_command_element	elems[20];
};

#endif
