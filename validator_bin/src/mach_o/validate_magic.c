/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   validate_magic.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmartzol <hmartzol@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2000/00/00 00:00:00 by hmartzol          #+#    #+#             */
/*   Updated: 2000/00/00 00:00:00 by hmartzol         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <mach_o_validator.h>

t_validator_error	validate_magic(t_macho_file *obj)
{
	obj->head.magic = obj->file.h32->magic;
	if (obj->head.magic == MH_MAGIC)
	{
		obj->format = 32;
		obj->mm.swap = 0;
	}
	else if (obj->head.magic == MH_MAGIC_64)
	{
		obj->format = 64;
		obj->mm.swap = 0;
	}
	else if (obj->head.magic == MH_CIGAM)
	{
		obj->format = 32;
		obj->mm.swap = 1;
	}
	else if (obj->head.magic == MH_CIGAM_64)
	{
		obj->format = 64;
		obj->mm.swap = 1;
	}
	else
		return (macho_error(obj, VE_INVALID_MAGIC_NUMBER, DEBUG_TUPLE));
	return (VE_OK);
}
