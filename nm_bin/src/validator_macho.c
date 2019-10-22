/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   validator_macho.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lee <lee@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/23 17:26:41 by lee               #+#    #+#             */
/*   Updated: 2018/08/23 17:49:06 by lee              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <nm.h>

/*
** will return a pointer to a macho explorer, or NULL (and output errors to
** stderr) on any inconsitency in the file
*/

typedef union		u_uint32x64
{
	uint32_t		x32;
	uint64_t		x64;
}					t_uint32x64;

typedef struct		s_nlist
{
	uint32_t			n_strx;
	uint8_t				n_type;
	uint8_t				n_sect;
	int16_t				n_desc;
	t_uint32x64			n_value;
}					t_nlist;

typedef struct		s_macho_explorer
{
	uint32_t			valid : 1;
	uint32_t			x64 : 1;
	uint32_t			reversed_endian : 1;
	uint32_t			pad0 : 29;
	cpu_type_t			cpu;
	cpu_subtype_t		cpu_subtype;
	uint32_t			filetype;
	uint32_t			ncmds;
	uint32_t			sizeofcmds;
	uint32_t			flags;
	uint32_t			reserved;
	struct load_command	*lcs;
	void				*strings;
	t_
}					t_macho_explorer;

t_macho_explorer	validator_macho(s_nm_env *env)
{

}
