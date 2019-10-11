#include <memory.h>

t_memory_error	in(const void *search, const t_memory_descriptor md,
	const void *mem, const int should_swap)
{
	size_t	it;
	size_t	sw;
	size_t	align;

	if ((align = md.block_size % md.align))
		align = md.block_size + md.align - align;
	else
		align = md.block_size > md.align ? md.block_size : md.align;
	it = (size_t)-1;
	while (++it < md.nb_blocks)
	{
		sw = (size_t)-1;
		while (++sw < md.block_size)
			if (should_swap && md.should_swap)
			{
				if (((uint8_t*)search)[sw]
						!= ((uint8_t*)mem)[it * align + md.block_size - sw])
					break ;
			}
			else if (((uint8_t*)search)[sw] != ((uint8_t*)mem)[it * align + sw])
				break ;
		if (sw == md.block_size)
			return (ME_OK);
	}
	return (ME_OUTSIDE_MAPPING);
}
