#include <memory.h>

t_memory_error	valid_cursor(t_memory_map *mm,
							const t_memory_descriptor md,
							size_t *align)
{
	size_t	tmp;

	if (align == NULL)
		align = &tmp;
	if (mm->cursor >= mm->size)
		return (memory_error(mm, ME_OUTSIDE_MAPPING));
	if ((*align = md.block_size % md.align))
		*align = md.block_size + md.align - *align;
	else
		*align = md.block_size;
	if (mm->cursor + *align < mm->cursor || mm->cursor + *align >= mm->size)
		return (memory_error(mm, ME_INVALID_BLOC_SIZE));
	if (md.nb_blocks && (md.nb_blocks * *align < md.nb_blocks
						|| md.nb_blocks * *align < *align))
		return (memory_error(mm, ME_INVALID_BLOC_COUNT));
	return (ME_OK);
}