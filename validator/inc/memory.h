/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   memory.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmartzol <hmartzol@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2000/00/00 00:00:00 by hmartzol          #+#    #+#             */
/*   Updated: 2000/00/00 00:00:00 by hmartzol         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MEMORY_H
# define MEMORY_H

# include <stdlib.h>

/*
** block_size
** nb_blocks
** align        how the cursor must be aligned after accessing each block
** should_swap  if t_memory_map.swap then swap bytes in each block
** -
** example:
** given a string of 16 chars:
** (t_memory_descriptor){1, 16, 1, 0} or (t_memory_descriptor){16, 1, 1, 0}
** given an array of 64 uint16_t
** (t_memory_descriptor){2, 64, 2, 1}
*/

typedef struct	s_memory_descriptor
{
	size_t		block_size;
	size_t		nb_blocks;
	size_t		align;
	int			should_swap;
}				t_memory_descriptor;

/*
** nb_members  how many t_memory_descriptor there are
** align       how the cursor must be aligned after accessing each member
** total_size  sum of all the member size (aligned)
** member[16]  up to 16 t_memory_descriptors
** -
** example:
** struct section {
** char		sectname[16];
** char		segname[16];
** uint32_t	addr;
** uint32_t	size;
** uint32_t	offset;
** uint32_t	align;
** uint32_t	reloff;
** uint32_t	nreloc;
** uint32_t	flags;
** uint32_t	reserved1;
** uint32_t	reserved2;
** };
** will be represented by:
** t_struct_descriptor g_section_descriptor = {
**   2, 4, sizeof(struct section), {{16, 2, 1, 0}, {4, 9, 4, 1}}};
*/

typedef struct	s_struct_descriptor
{
	size_t				nb_members;
	size_t				align;
	size_t				total_size;
	t_memory_descriptor	member[16];
}				t_struct_descriptor;

typedef enum	e_memory_error
{
	ME_OK = 0,
	ME_INVALID_MAPPING,
	ME_OUTSIDE_MAPPING,
	ME_MAPPING_FAILED,
	ME_INVALID_BLOC_SIZE,
	ME_INVALID_BLOC_COUNT,
	ME_COULD_NOT_OPEN_FILE,
	ME_COULD_NOT_STAT_FILE,
	ME_INVALID_CLAIM,
	ME_INVALID_UNCLAIM
}				t_memory_error;

/*
** swap    if TRUE, then will apply an endian swap if needed on reading/writing
** size    total size of this memory_map
** cursor  actual position of the reader/writer
** error   last error caught
** ptr     pointer to the actual data
** map     pointer to a map, each byte having a flag to tell the owner of the
**   corresponding byte in ptr
*/

typedef struct	s_memory_map
{
	int				swap;
	size_t			size;
	size_t			cursor;
	t_memory_error	err;
	uint8_t			*ptr;
	uint8_t			*map;
}				t_memory_map;

typedef struct	s_vm_map_entry
{
	unsigned long long	id;
	size_t				start;
	size_t				finish;
}				t_vm_map_entry;

typedef struct	s_vm_map
{
	size_t				total_size;
	unsigned long long	biggest_id;
	size_t				nb_entries;
	t_memory_error		err;
	t_vm_map_entry		*entries;
}				t_vm_map;

typedef struct	s_debug_tuple
{
	char		*file;
	char		*function;
	int			line;
}				t_debug_tuple;

# define DEBUG_TUPLE ((t_debug_tuple){__FILE__, (char*)__FUNCTION__, __LINE__})
# define MD_UINT32 ((t_memory_descriptor){4, 1, 4, 1})
# define MD_UINT64 ((t_memory_descriptor){8, 1, 8, 1})
# define MD_CHAR16 ((t_memory_descriptor){1, 16, 1, 0})

# define MD_LOAD_COMMAND ((t_memory_descriptor){4, 2, 4, 1})

/*
** t_memory_error    memory_error(t_memory_map *mm,
**                                t_memory_error err,
**                                t_debug_tuple debug_tuple):
** set error in mm, operation mainly used to catch thrown errors in LLDB/GDB,
** use DEBUG_TUPLE as the third arg
*/

t_memory_error	memory_error(t_memory_error *me,
							const t_memory_error err,
							t_debug_tuple debug_tuple);

t_memory_error	memory_map_from_file(t_memory_map *mm,
									const char *path);
t_memory_error	memory_map_create(t_memory_map *mm,
									const size_t size,
									const int prot);
t_memory_error	memory_map_from_memory_map(t_memory_map *mm,
											t_memory_map *src,
											const size_t index,
											const size_t size);
t_memory_error	memory_map_resize(t_memory_map *mm,
									const size_t new_size);

/*
** t_memory_error    valid_cursor(t_memory_map *mm,
**                                const t_memory_descriptor md,
**                                size_t *align):
** make sure the cursor and requested memory is inside the memory map
*/

t_memory_error	valid_cursor(t_memory_map *mm,
							const t_memory_descriptor md,
							size_t *align);

/*
** t_memory_error    read_in_memory(t_memory_map *mm,
**                                  void *buffer,
**                                  const uint8_t mapping,
**                                  const t_memory_descriptor md):
** copies memory (mm) mm at position/restriction (mapping/mm.cursor) in (buffer)
**   using (md)
** mapping 0 is free/non-mapped memory
** mapping -1 (0xFF) is ignored mapping
*/

t_memory_error	read_in_memory(t_memory_map *mm,
								void *buffer,
								const uint8_t mapping,
								const t_memory_descriptor md);

/*
** t_memory_error    read_struct_in_memory(t_memory_map *mm,
**                                         void *buffer,
**                                         const uint8_t mapping,
**                                         const t_struct_descriptor sd):
** similar to read_in_memory, retrieve memory from mm, but uses a structure
** descriptor instead of a memory descriptor
*/

t_memory_error	read_struct_in_memory(t_memory_map *mm,
										void *buffer,
										const uint8_t mapping,
										const t_struct_descriptor sd);

/*
** 'search' is a single block to find in 'mem' (which is described by 'md')
** return index on success and (size_t)-1 for a failure
** example usage:
** in((uint32_t[1]){3}, (t_memory_descriptor){4, 5, 4, 0},
**    (uint32_t[5]){1, 2, 3, 4, 5}, 0) != -1ull
*/

size_t			in(const void *search,
					const t_memory_descriptor md,
					const void *mem,
					const int should_swap);

t_memory_error	claim_map(t_memory_map *mm, const t_memory_descriptor md,
							uint8_t claim, int jump);
t_memory_error	unclaim_map(t_memory_map *mm, const t_memory_descriptor md,
							uint8_t claim, int jump);

/*
** try to insert a new vm entry, if there is a potential overlap, return
** ME_INVALID_MAPPING and on memory error return ME_MAPPING_FAILED
** return ME_OK on success
*/

t_memory_error	vm_map_new_entry(t_vm_map *vmm, size_t start, size_t finish,
									unsigned long long id);

/*
** try to create a new entry over a previous entry (the previous entry should
** contain the entirety of the new entry), the size of the original entry will
** change, and if the new is not at the start or end of the old entry, the old
** entry will be duplicated to englobe the new entry
** return ME_OUTSIDE_MAPPING if the new is not contained in the old
** return ME_INVALID_MAPPING if the original_id does not exist
** return ME_MAPPING_FAILED on allocation/insertion error
** return ME_OK on success
*/

t_memory_error	vm_map_recut_entry(t_vm_map *vmm, t_vm_map_entry new,
									unsigned long long original_id);

#endif
