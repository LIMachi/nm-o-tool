/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nm.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmartzol <hmartzol@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/15 13:52:03 by hmartzol          #+#    #+#             */
/*   Updated: 2018/08/16 11:25:09 by hmartzol         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <nm.h>

#include <stdlib.h>

/*
** EXIT_SUCCESS, EXIT_FAILURE
*/

int	nm_object_handler(t_nm_env *env)
{

}

int	nm_execute_handler(t_nm_env *env)
{

}

int	nm_magic_handler(uint32_t magic, t_nm_env *env)
{
	env->revert_endian = magic == MH_CIGAM || magic == MH_CIGAM_64;
	env->x64 = magic == MH_CIGAM_64 || magic == MH_MAGIC_64;
	if (env->file_stats.st_size < sizeof(mach_header_64))
		return (EXIT_FAILURE);
	env->header = *(mach_header_64*)env->file;
	if (!env->x64)
		env->header.reserved = 0;
	if (!(magic == MH_MAGIC || magic == MH_CIGAM || magic == MH_MAGIC_64
			|| magic == MH_CIGAM_64))
		return (EXIT_FAILURE);
	if (env->header.filetype == MH_OBJECT)
		return (nm_object_handler(env));
	if (env->header.filetype == MH_EXECUTE)
		return (nm_execute_handler(env));
	return (EXIT_FAILURE);
}

int	nm(t_nm_env *env)
{
	uint32_t	magic;

	if (env->file_stats.st_size < sizeof(uint32_t))
		return (EXIT_FAILURE);
	magic = *(uint32_t*)env->file;
	if ((env->revert_endian = (magic == FAT_CIGAM || magic == FAT_CIGAM_64)))
		 magic = swap32(magic);
	if (magic == FAT_MAGIC || magic == FAT_MAGIC_64)
	{

	}
	else
		return (nm_magic_handler(magic, env));
	return (EXIT_FAILURE);
}
