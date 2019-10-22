/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vm_map_new_entry.h                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmartzol <hmartzol@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2000/00/00 00:00:00 by hmartzol          #+#    #+#             */
/*   Updated: 2000/00/00 00:00:00 by hmartzol         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <memory.h>

t_memory_error	vm_map_new_entry(t_vm_map *vmm, size_t start, size_t finish,
	unsigned long long id)
{
	size_t	t;

	if (start > finish)
	{
		t = start;
		start = finish;
		finish = t;
	}
	--finish;
	t = (size_t)-1;
	while (++t < vmm->nb_entries)
		if (start <= vmm->entries[t].finish && finish >= vmm->entries[t].start)
			return (memory_error(&vmm->err, ME_INVALID_MAPPING, DEBUG_TUPLE));
	if ((vmm->entries = reallocf(vmm->entries,
			sizeof(t_vm_map_entry) * (vmm->nb_entries + 1))) == NULL)
		return (memory_error(&vmm->err, ME_MAPPING_FAILED, DEBUG_TUPLE));
	vmm->entries[vmm->nb_entries++] = (t_vm_map_entry){id, start, finish};
	if (finish + 1 > vmm->total_size)
		vmm->total_size = finish + 1;
	if (id > vmm->biggest_id)
		vmm->biggest_id = id;
	return (ME_OK);
}
