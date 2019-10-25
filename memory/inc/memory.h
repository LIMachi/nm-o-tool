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
# include <sys/mman.h>

# include <error.h>
# include <vm.h>

typedef enum	e_memory_error
{
	ME_OK = VME_OK,
	ME_PENDING_ERROR = VME_PENDING_ERROR,
	ME_INVALID_BLOC_SIZE = LAST_VM_ERROR_DEFINITION,
	ME_INVALID_BLOC_COUNT,
	ME_COULD_NOT_OPEN_FILE,
	ME_COULD_NOT_STAT_FILE,
	ME_MAPPING_FAILED,
	ME_OUTSIDE_MAPPING,
	LAST_MEMORY_ERROR_DEFINITION
}				t_memory_error;

/*
** All functions (using *mm as the first parameter) can be chained without check
** on the same memory_map, they will not execute if the err flag in the
** memory_map is not equal to ME_OK.
** A final check after calling all the functions can be perfomed to know if any
** failed (the faulty call can be traced back by setting a breakpoint on the
** memory_error function used to set the err flag)
*/

/*
** sizeof(t_memory_descriptor): 16
** block_size
** nb_blocks
** align        how the cursor must be aligned after accessing each block
** endian       used to know how the memory should be reduced/expanded/copied
**    endian values: 0123 (little)
**                   3210 (big)
**                   0000 (do not swap)
** -
** example:
** given a string of 16 chars:
** (t_memory_descriptor){1, 16, 1, 0} or (t_memory_descriptor){16, 1, 1, 0}
** given an array of 64 uint16_t
** (t_memory_descriptor){2, 64, 2, 1}
*/

typedef struct	s_memory_descriptor
{
	uint32_t	block_size;
	uint32_t	nb_blocks;
	uint32_t	align;
	uint32_t	endian;
}				t_memory_descriptor;

/*
** sizeof(t_struct_descriptor): 528
** nb_members  how many t_memory_descriptor there are
** align       how the cursor must be aligned after accessing each member
** total_size  sum of all the member size (aligned)
** member[32]  up to 32 t_memory_descriptors
** -----------------------------------------------------------------------------
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
	uint32_t			nb_members;
	uint32_t			align;
	uint64_t			total_size;
	t_memory_descriptor	member[32];
}				t_struct_descriptor;

/*
** sizeof(t_cast_memory_descriptor): 40
** Perform resize, alignements and swap from one buffer to another using two
** t_cast_memory_descriptor
** 'delta' is used to move the blocks from their original position, 0x80000000
** (CTD_VOID) is reserved to indicate the absence of copy (used in
** t_cast_struct_descriptor to add/remove/jump blocks)
** Note: using the same buffer in and out of a cast is undefined behavior
*/

typedef struct	s_cast_memory_descriptor
{
	t_memory_descriptor	in;
	t_memory_descriptor	out;
	int64_t				delta;
}				t_cast_memory_descriptor;

# define CTD_VOID 0x80000000ll

/*
** sizeof(t_cast_struct_descriptor): 1312
** perform a series of t_cast_type_descriptor on one buffer to the other,
** see example below
*/

typedef struct	s_cast_struct_descriptor
{
	uint32_t					nb_members;
	uint32_t					swap;
	uint32_t					align_in;
	uint32_t					align_out;
	uint64_t					total_size_in;
	uint64_t					total_size_out;
	t_cast_memory_descriptor	members[32];
}				t_cast_struct_descriptor;

/*
** Visual example of t_cast_struct_descriptor
** we want to transform s1 to s2 by removing the life member, moving and
** changing the size of the id, moving the pos array and adding a string of 16
** characters to the s2 structure
** Note1: the out structure is initialized with 0 on all it's bytes before write
** Note2: CTD_VOID can be used to jump an 'in' or 'out' member or even both
** Note3: support for float numbers is not there, resizing or swaping of
** float/double is undefined behavior
** -----------------------------------------------------------------------------
** 	struct	s1
** 	{
** 		uint32_t	life; //{4, 1, 4, 1} $0
** 		uint32_t	id; //{4, 1, 4, 1} $4
** 		float		pos[3]; //{4, 3, 4, 1} $8 - $16
** 	};
** 	struct	s2
** 	{
** 		float		pos[3]; //{4, 3, 4, 1} $0 - $8
** 		uint64_t	id; //{8, 1, 8, 1} $16
** 		char		death_message[16]; //{1, 16, 1, 0} $20
** 	};
** 	t_cast_struct_descriptor	s1_to_s2 = {
** 		.nb_members = 4, .swap = 0, .align_in = 4, .align_out = 4,
** 		.total_size_in = 20, .total_size_out = 36,
** 		.members = {
** 			[0] = {.delta = CTD_VOID, .in = {4, 1, 4, 1}},
** 			[1] = {.delta = 12, .in = {4, 1, 4, 1}, .out = {8, 1, 8, 1}},
** 			[2] = {.delta = -4, .in = {4, 3, 4, 1}, .out = {4, 3, 4, 1}},
** 			[3] = {.delta = CTD_VOID, .out = {1, 16, 1, 0}}
** 		}
** 	};
*/

enum			e_type_descriptor
{
	TD_ARRAY,
	TD_STRUCT,
	TD_CAST_ARRAY,
	TD_CAST_STRUCT
};

union			u_type_descriptor
{
	t_memory_descriptor			array;
	t_struct_descriptor			structure;
	t_cast_memory_descriptor	cast_array;
	t_cast_struct_descriptor	cast_structure;
};

/*
** sizeof(t_type_descriptor): 1320
** structure that describe arrays/structures and their casts, usable by most
** functions of this library
*/

typedef struct	s_type_descriptor
{
	uint32_t				type;
	union u_type_descriptor	dscr;
}				t_type_descriptor;

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
	size_t			size;
	size_t			cursor;
	uint8_t			*ptr;
}				t_memory_map;

t_memory_error	memory_map_clear(t_memory_map *mm);
t_memory_error	memory_unmap(t_memory_map *mm);

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
**                                  const t_memory_descriptor md,
**                                  const int swap):
** copies memory (mm) mm at position/restriction (mapping/mm.cursor) in (buffer)
**   using (md), eventually using an endian (swap)
*/

t_memory_error	read_in_memory(t_memory_map *mm,
								void *buffer,
								const t_memory_descriptor md,
								const uint32_t endian);

/*
** t_memory_error    read_struct_in_memory(t_memory_map *mm,
**                                         void *buffer,
**                                         const t_struct_descriptor sd,
**                                         const int swap):
** similar to read_in_memory, retrieve memory from mm, but uses a structure
** descriptor instead of a memory descriptor
*/

t_memory_error	read_struct_in_memory(t_memory_map *mm,
										void *buffer,
										const t_struct_descriptor sd,
										const uint32_t endian);

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
					const uint32_t endian);

#endif
