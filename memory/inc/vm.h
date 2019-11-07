/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vm.h                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmartzol <hmartzol@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2000/00/00 00:00:00 by hmartzol          #+#    #+#             */
/*   Updated: 2000/00/00 00:00:00 by hmartzol         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef VM_H
# define VM_H

# include <stdlib.h>
# include <stdint.h>

# include <error.h>

typedef enum	e_vm_error
{
	VME_OK = 0,
	VME_INVALID_MAPPING,
	VME_MAPPING_FAILED,
	VME_OUTSIDE_MAPPING,
	VME_PENDING_ERROR,
	LAST_VM_ERROR_DEFINITION
}				t_vm_error;

typedef struct	s_vm_map_entry
{
	uint64_t	id;
	size_t		start;
	size_t		finish;
}				t_vm_map_entry;

typedef struct	s_vm_map
{
	size_t			total_size;
	uint64_t		biggest_id;
	size_t			nb_entries;
	t_vm_map_entry	*entries;
}				t_vm_map;

/*
** try to insert a new vm entry, if there is a potential overlap, return
** ME_INVALID_MAPPING and on memory error return ME_MAPPING_FAILED
** return ME_OK on success
*/

int				vm_map_new_entry(t_vm_map *vmm, size_t start, size_t finish,
	uint64_t id);

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

int				vm_map_recut_entry(t_vm_map *vmm, t_vm_map_entry new,
	uint64_t original_id);

#endif
