/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nm.h                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmartzol <hmartzol@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/11 11:28:22 by hmartzol          #+#    #+#             */
/*   Updated: 2018/08/15 13:53:17 by hmartzol         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef NM_H
# define NM_H

#include <stdint.h>

#include <sys/stat.h>

/*
** fstat, struct stat
*/

/*
** e_nm_flags:
**   DEFAULT_FLAGS: set the default flags
**   *_ALL: set all flags starting with *, usually used as a mask
**   PRINT_*: determine which sector and type must be shown
**   PRINT_DEBBUGER: disabled by default, print debug symbols
**   SORT_ASCII: sort using the name of symbols
**   SORT_NUMERICAL: sort using the address of symbols.
**     Applied after ascii, so undefined symbols (with address 0) are ascii
**     sorted if SORT_ASCII is used
**   SORT_REVERT: reverse the order of print
**   PREPEND_NAME: add the name of the file (or archive part) at the start of
**     each line
**   PREPEND_PATH: same as PREPEND_NAME, but use a full path instead
**   FORMAT_*: change the format of the output, might not be implemented
*/

typedef enum	e_nm_flags
{
	PRINT_UNDEFINED = 0x000001,
	PRINT_ABSOLUTE = 0x0000002,
	PRINT_TEXT = 0x00000000004,
	PRINT_DATA = 0x00000000008,
	PRINT_BSS = 0x000000000010,
	PRINT_COMMON = 0x000000020,
	PRINT_SECTION = 0x00000040,
	PRINT_INDIRECT = 0x0000080,
	PRINT_DEBBUGER = 0x0000100,
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
	FORMAT_ALL = 0x000003FC000,
	DEFAULT_FLAGS = 0x000002FF
}				t_nm_flags;

/*
** s_nm_env:
**   flags: see above
**   file_stats: see man fstat
**   file: mmaped content of currently processed file
**   file_start: starting position of subfile in file (0 for single file fornat)
**   file_end: ending position of subfile in file (file size for single file
**     format)
**   bss_sect: index (in number of segments) of the bss segment in the file
**   text_sect: same as bss_sect for text segment
**   data_sect: same as bss_sect for data segment
*/

typedef struct	s_nm_env
{
	t_nm_flags	flags;
	struct stat	file_stats;
	uint8_t		*file;
	uint64_t	file_start;
	uint64_t	file_end;
	uint64_t	bss_sect;
	uint64_t	text_sect;
	uint64_t	data_sect;
}				t_nm_env;

char	nm_type_char(uint8_t n_type, uint64_t value, uint64_t nsect,
					t_nm_env *env);

int		nm(char *path, t_nm_env *env);
#endif
