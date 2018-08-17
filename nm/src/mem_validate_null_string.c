/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mem_validate_null_string.c                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmartzol <hmartzol@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/16 10:07:41 by hmartzol          #+#    #+#             */
/*   Updated: 2018/08/16 10:12:36 by hmartzol         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <nm.h>

inline uint64_t	mem_validate_null_string(char *str, t_nm_env *env)
{
	uint64_t	out;

	if ((uint64_t)str < (uint64_t)env->file + env->file_start
			|| (uint64_t)str >= (uint64_t)env->file + env->file_end)
		return (-1);
	out = 0;
	while ((uint64_t)str + out < (uint64_t)env->file + env->file_end
			&& str[out] != '\0')
		++out;
	if ((uint64_t)str + out == (uint64_t)env->file + env->file_end)
		return (-1);
	return (out);
}
