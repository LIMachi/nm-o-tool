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

#include <mach_o_validator.h>

t_memory_error	vm_map_recut_entry(t_vm_map *vmm, t_vm_map_entry new,
									unsigned long long original_id)
{
	size_t	t;
	int		f;

	if (new.start > new.finish)
	{
		t = new.start;
		new.start = new.finish;
		new.finish = t;
	}
	--new.finish;
	f = 0;
	t = (size_t)-1;
	while (++t < vmm->nb_entries)
		if (vmm->entries[t].id == original_id && (f = 1) && vmm->entries[t].start <= new.start && vmm->entries[t].finish >= new.finish)
			break ;
	if (t == vmm->nb_entries)
		return (memory_error(&vmm->err, f ? ME_OUTSIDE_MAPPING : ME_INVALID_MAPPING, DEBUG_TUPLE));
	if (new.start == vmm->entries[t].start)
		if (new.finish == vmm->entries[t].finish)
			vmm->entries[t].id = new.id;
		else
		{
			if ((vmm->entries = reallocf(vmm->entries, sizeof(t_vm_map_entry) * (1 + vmm->nb_entries))) == NULL)
				return (memory_error(&vmm->err, ME_MAPPING_FAILED, DEBUG_TUPLE));
			vmm->entries[t].start = new.finish + 1;
			vmm->entries[vmm->nb_entries++] = new;
		}
	else if (new.finish == vmm->entries[t].finish)
	{
		if ((vmm->entries = reallocf(vmm->entries, sizeof(t_vm_map_entry) * (1 + vmm->nb_entries))) == NULL)
			return (memory_error(&vmm->err, ME_MAPPING_FAILED, DEBUG_TUPLE));
		vmm->entries[t].finish = new.start - 1;
		vmm->entries[vmm->nb_entries++] = new;
	}
	else
	{
		if ((vmm->entries = reallocf(vmm->entries, sizeof(t_vm_map_entry) * (2 + vmm->nb_entries))) == NULL)
			return (memory_error(&vmm->err, ME_MAPPING_FAILED, DEBUG_TUPLE));
		vmm->entries[vmm->nb_entries++] = new;
		vmm->entries[vmm->nb_entries++] = (t_vm_map_entry){.start = new.finish + 1, .finish = vmm->entries[t].finish, .id = vmm->entries[t].id};
		vmm->entries[t].finish = new.start - 1;
	}
	if (vmm->biggest_id < new.id)
		vmm->biggest_id = new.id;
	return (ME_OK);
}
