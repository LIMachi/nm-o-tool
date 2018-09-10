/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   load_commands.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lee <lee@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/09/10 15:25:12 by lee               #+#    #+#             */
/*   Updated: 2018/09/11 00:50:32 by lee              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <validator.h>

/*
** will apply a endian patern of the form {nb_blocs, <aplly, byte_size> ...}
** to a copy of src and store it in new (if new is not NULL), make sur that
** new and src are both at least of the total size of pattern
** return new
** if new is NULL, then the return will be allocated (and must be freed)
** note: if new is NULL, this function can return NULL on memory error
** note2: src and new can be the same to do an in place endian swap
** example:
** struct {int32_t a, int32_t b} lc = {0x01, 0x42000000};
** struct {int32_t a, int32_t b} lc2;
** #define LC_PATTERN ((size_t[5]){2, 1, 4, 1, 4})
** apply_endian_pattern(&lc, LC_PATTERN, &lc2)
** // in lc2:
** a -> 0x01000000
** b-> 0x42
*/

static inline void	apply_endian(void *src, size_t size, void *new)
{
	if (size == 2)
		*(uint16_t*)new = swap16(*(uint16_t*)src);
	else if (size == 4)
		*(uint32_t*)new = swap32(*(uint32_t*)src);
	else if (size == 8)
		*(uint64_t*)new = swap64(*(uint64_t*)src);
	else
	{
		memcpy(new, src, size);
		swap(new, 1, size);
	}
}

void	*apply_endian_pattern(void *src, size_t pattern[], void *new)
{
	size_t	total_size;
	size_t	i;

	if (new == NULL)
	{
		total_size = 0;
		i = -1;
		while (++i < pattern[0])
			total_size += pattern[2 + 2 * i];
		if (!(new = malloc(total_size)))
			return (NULL);
	}
	i = -1;
	total_size = 0;
	while (++i < pattern[0])
	{
		if (pattern[1 + 2 * i])
			apply_endian(&src[total_size],
				pattern[2 + 2 * i], &new[total_size]);
		total_size += pattern[2 + 2 * i];
	}
}

t_validation_error	segment(t_macho_info *info, t_load_command *lc)
{
	(void)info;
	(void)lc;
	return (EVE_UNIMPLEMENTED_FEATURE);
}

void	*mem_access(t_mem_area area, ssize_t offset, void *reference,
					void **out)
{
	if (reference == NULL)
		reference == area.start;
	if (out == NULL)
		out = (void[1]*){reference + offset};
	else
		*out = reference + offset;
	if (*out < area.start || *out > area.end)
		return ((*out = NULL));
	return (*out);
}

/*
** 32 bits
*/

t_validation_error	symtab(t_macho_info *info, t_load_command *lc)
{
	struct nList	*nl;
	char			*strings;

	if (lc->symtab.cmdsize != sizeof(struct symtab_command))
		return (EVE_INVALID_LOAD_COMMAND_SIZE);
	if (!mem_access(info->data_area, lc->symtab.symoff,
			info->file_area.start, &nl))
		return (EVE_INVALID_OFFSET);
	if (!mem_access(info->data_area, lc->symtab.nsyms * sizeof(struct nlist),
			nl, NULL))
		return (EVE_INVALID_OFFSET);
	return (EVE_OK);
}

t_validation_error	load_commands(t_macho_info *info)
{
	struct load_command	*lc;
	uint32_t			count;
	uint32_t			type;
	t_validation_error	error;

	count = -1;
	lc = (struct load_command*)info->commands_area.start;
	while (++count < info->header.x32.ncmds)
	{
		type = info->endian ? swap32(lc->cmd) : lc->cmd;
		if (type == LC_SEGMENT || type == LC_SEGMENT_64)
			if ((error = segment(info, lc)) != EVE_OK)
				return (error);
		if (type == LC_SYMTAB)
			if ((error = symtab(info, lc)) != EVE_OK)
				return (error);
		lc += info->endian ? swap32(lc->cmdsize) : lc->cmdsize;
	}
	return (EVE_OK);
}
