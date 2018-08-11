/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nm.h                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmartzol <hmartzol@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/11 11:28:22 by hmartzol          #+#    #+#             */
/*   Updated: 2018/08/11 11:49:38 by hmartzol         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef NM_H
# define NM_H

#include <stdint.h>

#include <sys/stat.h>

/*
** fstat, struct stat
*/

typedef enum	e_nm_flags
{
	PRINT_UNDEFINED = 0x000001,
	PRINT_ABSOLUTE = 0x0000002,
	PRINT_TEXT = 0x00000000004,
	PRINT_DATA = 0x00000000008,
	PRINT_BSS = 0x000000000010,
	PRINT_COMMON = 0x000000020,
	PRINT_DEBBUGER = 0x0000040,
	PRINT_SECTION = 0x00000080,
	PRINT_INDIRECT = 0x0000100,
	PRINT_ALL = 0x0000000001FF,
	SORT_ASCII = 0x00000000200,
	SORT_NUMERICAL = 0x0000400,
	SORT_REVERT = 0x0000000800,
	SORT_ALL = 0x0000000000E00,
	PREPEND_NAME = 0x000001000,
	PREPEND_PATH = 0x000002000,
	PREPEND_ALL = 0x0000003000,
	FORMAT_EXTEND = 0x00004000,
	FORMAT_HEXA = 0x0000008000,
	FORMAT_SHORT = 0x000010000,
	FORMAT_BSD = 0x00000020000,
	FORMAT_SYSV = 0x0000040000,
	FORMAT_POSIX = 0x000080000,
	FORMAT_DARWIN = 0x00100000,
	FORMAT_PORTABLE = 0x200000,
	FORMAT_ALL = 0x000003FC000
}				t_nm_flags;

typedef struct	s_nm_env
{
	t_nm_flags		flags;
	struct stat		file_stats;
	unsigned char	*file;
}				t_nm_env;

# define DEFAULT_FLAGS ((PRINT_ALL ^ PRINT_DEBBUGER) | SORT_ASCII)

#endif
