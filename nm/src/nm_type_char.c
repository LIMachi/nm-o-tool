/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nm_type_char.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmartzol <hmartzol@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/12 10:38:58 by hmartzol          #+#    #+#             */
/*   Updated: 2018/08/15 13:54:03 by hmartzol         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <nm.h>

#include <mach-o/loader.h>
#include <mach-o/nlist.h>

char	nm_type_char(uint8_t n_type, uint64_t value, uint64_t nsect,
					t_nm_env *env)
{
	uint8_t	type;
	char	out;

	if (n_type & N_STAB)
		return ((char)'-');
	out = (char)'?';
	type = N_TYPE & n_type;
	if (type == N_UNDF)
		out = value ? (char)'c' : (char)'u';
	if (type == N_SECT)
	{
		out = (char)'s';
		out = nsect == env->bss_sect ? (char)'b' : out;
		out = nsect == env->data_sect ? (char)'d' : out;
		out = nsect == env->text_sect ? (char)'t' : out;
	}
	out = type == N_ABS ? (char)'a' : out;
	out = type == N_PBUD ? (char)'u' : out;
	out = type == N_INDR ? (char)'i' : out;
	if (n_type & N_EXT && out != (char)'?')
		out += (char)'A' - (char)'a';
	return (out);
}
