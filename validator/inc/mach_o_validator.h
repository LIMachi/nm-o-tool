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
** 0x01: program headers
** range 0x02 - 0x7F: program segments (0x02 + i % 0x7E)
** 0x80: file header
** range 0x81 - 0xFE: sections (0x81 + i % 0x7E)
** 0xFF: section headers
*/

typedef enum					e_memory_map
{
	MM_PAD = 0x00,
	MM_HEADER = 0x80,
	MH_PH = 0x01,
	MH_SH = 0xFF
}				t_memory_map;

typedef enum					e_validator_error
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
	VE_INVALID_TOTAL_COMMAND_SIZE
}								t_validator_error;

typedef struct					s_cursor
{
	size_t			index;
	size_t			nb_blocs;
	size_t			block_size;
	size_t			align;
	t_memory_map	expected_mapping;
}								t_cursor;

typedef struct					s_macho_file
{
	int						fd;
	size_t					size;
	t_macho_header_ptr		file;
	uint8_t					*map;
	t_moh64					head;
	t_validator_error		err;
	int						endian;
	int						format;
}								t_macho_file;

#endif
