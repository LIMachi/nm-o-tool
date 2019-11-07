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

#include <vm.h>

static void	*realloc_f(void *ptr, size_t size)
{
	void	*tmp;

	if ((tmp = realloc(ptr, size)) == NULL)
		free(ptr);
	return (tmp);
}

int	vm_map_new_entry(t_vm_map *vmm, size_t start, size_t finish,
	uint64_t id)
{
	size_t	t;

	if (get_error()->error)
		return (VME_PENDING_ERROR);
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
			return (set_error(VME_INVALID_MAPPING, ERROR_TUPLE));
	if ((vmm->entries = realloc_f(vmm->entries,
			sizeof(t_vm_map_entry) * (vmm->nb_entries + 1))) == NULL)
		return (set_error(VME_MAPPING_FAILED, ERROR_TUPLE));
	vmm->entries[vmm->nb_entries++] = (t_vm_map_entry){id, start, finish};
	if (finish + 1 > vmm->total_size)
		vmm->total_size = finish + 1;
	if (id > vmm->biggest_id)
		vmm->biggest_id = id;
	return (VME_OK);
}
