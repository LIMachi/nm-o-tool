#ifndef ELF_VALIDATOR_H
# define ELF_VALIDATOR_H

# include <stdint.h>
# include <sys/mman.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <unistd.h>
# include <fcntl.h>
# include <stddef.h>

# define HEAD_SIZE_32 52
# define HEAD_SIZE_64 64

# define MAGIC_0 0x7F
# define MAGIC_1 'E'
# define MAGIC_2 'L'
# define MAGIC_3 'F'

# define FORMAT_32 1
# define FORMAT_64 2

# define ENDIAN_LOW 1
# define ENDIAN_HIGH 2

# define ELF_VERSION 1

# define SEGMENT_SIZE_32 0x20
# define SEGMENT_SIZE_64 0x38

# define SECTION_SIZE_32 0x28
# define SECTION_SIZE_64 0x40

# pragma pack(push, 1)

typedef struct	s_elf_magic
{
	char		name[4];
	char		format;
	char		endian;
	char		version;
	char		os_abi;
	char		abi_ver;
	char		pad[7];
}				t_elf_magic;

typedef union	u_uint16
{
	uint16_t	i;
	char		b[2];
}				t_uint16;

typedef union	u_uint32
{
	uint32_t	i;
	char		b[4];
}				t_uint32;

typedef union	u_uint64
{
	uint64_t	i;
	char		b[8];
}				t_uint64;

typedef struct	s_elf_64_head
{
	t_elf_magic	magic;
	uint16_t	type;
	uint16_t	machine;
	uint32_t	version;
	uint64_t	entry;
	uint64_t	ht_off;
	uint64_t	st_off;
	uint32_t	flags;
	uint16_t	head_size;
	uint16_t	phe_size;
	uint16_t	phe_num;
	uint16_t	she_size;
	uint16_t	she_num;
	uint16_t	inames;
}				t_elf_64_head;

typedef struct	s_elf_32_head
{
	t_elf_magic	magic;
	uint16_t	type;
	uint16_t	machine;
	uint32_t	version;
	uint32_t	entry;
	uint32_t	ht_off;
	uint32_t	st_off;
	uint32_t	flags;
	uint16_t	head_size;
	uint16_t	phe_size;
	uint16_t	phe_num;
	uint16_t	she_size;
	uint16_t	she_num;
	uint16_t	inames;
}				t_elf_32_head;

typedef union	u_elf_head
{
	char			*data;
	t_elf_64_head	*h64;
	t_elf_32_head	*h32;
}				t_elf_head;

typedef struct	s_elf_32_segment
{
	uint32_t	type;
	uint32_t	offset;
	uint32_t	vaddr;
	uint32_t	paddr;
	uint32_t	size_file;
	uint32_t	size_mem;
	uint32_t	flags;
	uint32_t	align;
}				t_elf_32_segment;

typedef struct	s_elf_64_segment
{
	uint32_t	type;
	uint32_t	flags;
	uint64_t	offset;
	uint64_t	vaddr;
	uint64_t	paddr;
	uint64_t	size_file;
	uint64_t	size_mem;
	uint64_t	align;
}				t_elf_64_segment;

typedef struct	s_elf_32_section
{
	uint32_t	name;
	uint32_t	type;
	uint32_t	flags;
	uint32_t	addr;
	uint32_t	offset;
	uint32_t	size;
	uint32_t	link;
	uint32_t	info;
	uint32_t	align;
	uint32_t	ent_size;
}				t_elf_32_section;

typedef struct	s_elf_64_section
{
	uint32_t	name;
	uint32_t	type;
	uint64_t	flags;
	uint64_t	addr;
	uint64_t	offset;
	uint64_t	size;
	uint32_t	link;
	uint32_t	info;
	uint64_t	align;
	uint64_t	ent_size;
}				t_elf_64_section;

# pragma pack(pop)

/*
** 0x00: unused/padding
** 0x01: program headers
** range 0x02 - 0x7F: program segments (0x02 + i % 0x7E)
** 0x80: file header
** range 0x81 - 0xFE: sections (0x81 + i % 0x7E)
** 0xFF: section headers
*/

typedef enum			e_memory_map
{
	MM_PAD = 0x00,
	MM_HEADER = 0x80,
	MH_PH = 0x01,
	MH_SH = 0xFF
}						t_memory_map;

typedef enum	e_elf_error
{
	EE_OK = 0,
	EE_NULL_PTR,
	EE_COULD_NOT_OPEN_FILE,
	EE_COULD_NOT_RETRIEVE_STATS,
	EE_MAPPING_ERROR,
	EE_INVALID_HEAD_SIZE,
	EE_INVALID_MAGIC_NUMBER,
	EE_INVALID_WORD_SIZE,
	EE_INVALID_ENDIAN,
	EE_INVALID_ELF_VERSION,
	EE_INVALID_OS_ABI,
	EE_INVALID_TYPE,
	EE_INVALID_MACHINE,
	EE_INVALID_ENTRY_POSITION,
	EE_INVALID_SEGMENT_TABLE_POSITION,
	EE_INVALID_SECTION_TABLE_POSITION,
	EE_INVALID_SEGMENT_HEADER_SIZE,
	EE_INVALID_SECTION_HEADER_SIZE,
	EE_INVALID_SEGMENT_COUNT,
	EE_INVALID_SECTION_COUNT,
	EE_FILE_MAPPING_CONFLICT
}				t_elf_error;

typedef union	u_seg_ptr
{
	void				*p;
	t_elf_32_segment	*p32;
	t_elf_64_segment	*p64;
}				t_seg_ptr;

typedef union	u_sect_ptr
{
	void				*p;
	t_elf_32_section	*p32;
	t_elf_64_section	*p64;
}				t_sect_ptr;

typedef struct	s_objfile
{
	int					fd;
	size_t				size;
	t_elf_head			file;
	uint8_t				*map;
	t_elf_64_head		head;
	t_elf_error			err;
	int					endian;
	int					format;
	t_seg_ptr			segments;
	t_sect_ptr			sections;
}				t_objfile;

#endif
