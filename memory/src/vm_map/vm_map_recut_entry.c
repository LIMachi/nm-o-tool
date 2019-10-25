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

#include <vm.h>

int	vm_map_recut_entry(t_vm_map *vmm, t_vm_map_entry new, uint64_t original_id)
{
	size_t	t;
	int		f;

	if (get_error()->error)
		return (VME_PENDING_ERROR);
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
		return (set_error(f ? VME_OUTSIDE_MAPPING : VME_INVALID_MAPPING, ERROR_TUPLE));
	if (new.start == vmm->entries[t].start)
		if (new.finish == vmm->entries[t].finish)
			vmm->entries[t].id = new.id;
		else
		{
			if ((vmm->entries = reallocf(vmm->entries, sizeof(t_vm_map_entry) * (1 + vmm->nb_entries))) == NULL)
				return (set_error(VME_MAPPING_FAILED, ERROR_TUPLE));
			vmm->entries[t].start = new.finish + 1;
			vmm->entries[vmm->nb_entries++] = new;
		}
	else if (new.finish == vmm->entries[t].finish)
	{
		if ((vmm->entries = reallocf(vmm->entries, sizeof(t_vm_map_entry) * (1 + vmm->nb_entries))) == NULL)
			return (set_error(VME_MAPPING_FAILED, ERROR_TUPLE));
		vmm->entries[t].finish = new.start - 1;
		vmm->entries[vmm->nb_entries++] = new;
	}
	else
	{
		if ((vmm->entries = reallocf(vmm->entries, sizeof(t_vm_map_entry) * (2 + vmm->nb_entries))) == NULL)
			return (set_error(VME_MAPPING_FAILED, ERROR_TUPLE));
		vmm->entries[vmm->nb_entries++] = new;
		vmm->entries[vmm->nb_entries++] = (t_vm_map_entry){.start = new.finish + 1, .finish = vmm->entries[t].finish, .id = vmm->entries[t].id};
		vmm->entries[t].finish = new.start - 1;
	}
	if (vmm->biggest_id < new.id)
		vmm->biggest_id = new.id;
	return (VME_OK);
}
