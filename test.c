/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmartzol <hmartzol@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/02 21:12:04 by hmartzol          #+#    #+#             */
/*   Updated: 2018/08/15 12:20:17 by hmartzol         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/mman.h>

/*
** mmap, munmap, PROT_READ, MAP_FAILED, MAP_PRIVATE
*/

#include <stdlib.h>

/*
** EXIT_SUCCESS, EXIT_FAILURE
*/

#include <stdio.h>

/*
** DEBUG
** fprintf, stderr, stdout, perror
*/

#include <fcntl.h>

/*
** open, O_RDONLY
*/

#include <unistd.h>

/*
** close
*/

#include <sys/stat.h>

/*
** fstat
*/

#include <mach-o/loader.h>
#include <mach-o/nlist.h>

#include <string.h>

/*
** strcmp
*/

/*
** mac executable
** MH_MAGIC_64, MH_MAGIC, mach_header, mach_header_64, load_command,
** LC_SYMTAB, symtab_command, N_EXT
*/

/*
** mem_acces is the main function to create a random acces to data in buff
** it takes a pointer to the start of the buffer, the total size in bytes of
** said buffer, a random pointer (*ptr) in the buffer and a delta to
** apply to ptr
** if ptr is NULL, it will be initialised with buff
** return NULL on error (move out of buffer range), or the content of *ptr
*/

static inline void	*mem_acces(const char *buff, const size_t size,
								const ssize_t delta, const void *ptr)
{
	size_t	pos;

	pos = delta + (size_t)(ptr == NULL ? buff : ptr);
	if (pos < size + (size_t)buff && pos >= (size_t)buff)
		return ((void*)(ptr == NULL ? buff : ptr) + delta);
	return (NULL);
}

void	error(const char *str)
{
	(void)str;
	exit(EXIT_FAILURE);
}

typedef struct	s_nm_sym
{
	size_t		value;
	char		type;
	char		*str;
}				t_nm_sym;

/*
** like a strlen
*/

static inline size_t	mem_validate_null_string(const char *buff,
											const size_t size, const char *str)
{
	size_t	out;

	if ((size_t)str < (size_t)buff || (size_t)str >= (size_t)buff + size)
		return (-1);
	out = 0;
	while ((size_t)str + out < (size_t)buff + size && str[out] != '\0')
		++out;
	if ((size_t)str + out >= (size_t)buff + size)
		return (-1);
	return (out);
}

int	nm_sym_cmp(const void *a, const void *b)
{
	return (strcmp(((t_nm_sym *)a)->str, ((t_nm_sym *)b)->str));
}

//UTDBCI- (lower case for non-external)
//U: undefined
//A: absolute
//T: text
//D: data
//B: bss
//C: common
//'-' debugger symbol
//S: section not corespondig to any other character
//I: indirect

//options: (mac)
//-a: all symbol entries
//-g: only external
//-n: sort numerically after alphabetically
//-o: prepend file or archive element to each line
//-p: symbol-table order (no sort)
//-r: reverse sort
//-u: only undefined
//-U: don't display undefined
//-m: extend format of type instead of letters
//-x: hexadecimal print of type + position of symbol table
//-j: only names of symbols, no type or position
//-s segname sectname: only list symbols in section (segname, sectname)
//-s (segname,sectname): used version of andling -s (only one string)
//-arch: architecture to be printed in fat files, all is an option
//--arch: used version of andling -arch
//-f format: bsd, sysv, posix, darwin
//-A: same as -o, but print full path
//-P: portable output (which ever this means)
//-t format: precise the format to be used by -P
//	d: decimal, o: octal, x: hexadecimal
//-L: see man, i'm tired

char nm_type_char(uint8_t n_type, uint64_t value, uint64_t nsect, t_nm_env *env)
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

int	nm_64_sym(const char *buff, const size_t size, struct symtab_command sc)
{
	size_t			i;
	char			*strings;
	struct nlist_64	*nl;
	t_nm_sym		*nm_sym;

	i = -1;
	nl = mem_acces(buff, size, sc.symoff, buff);
	mem_acces(buff, size, sc.nsyms * sizeof(struct nlist_64), nl);
	strings = mem_acces(buff, size, sc.stroff, buff);
	nm_sym = malloc(sizeof(*nm_sym) * sc.nsyms);
	while (++i < sc.nsyms)
	{
		nm_sym[i].value = nl[i].n_value;
		nm_sym[i].type = nm_type(nl[i].n_type);
		nm_sym[i].str = mem_acces(buff, size, nl[i].n_un.n_strx, strings);
		if (mem_validate_null_string(buff, size, nm_sym[i].str) == (size_t)-1)
			error("");
	}
	qsort(nm_sym, sc.nsyms, sizeof(*nm_sym), &nm_sym_cmp);
	i = -1;
	while (++i < sc.nsyms)
		if (nm_sym[i].type == 'U')
			fprintf(stdout, "                 U %s\n", nm_sym[i].str);
		else
			fprintf(stdout, "%016lx %c %s\n", nm_sym[i].value, nm_sym[i].type, nm_sym[i].str);
	return (0);
}

int	nm_64(const char *buff, const size_t size)
{
	struct mach_header_64	head;
	struct load_command		*lc;
	size_t					i;

	if (!mem_acces(buff, size, sizeof(struct mach_header_64), NULL))
		error("");
	head = *(struct mach_header_64*)buff;
	lc = (struct load_command*)(buff + sizeof(struct mach_header_64));
	i = -1;
	while (++i < head.ncmds)
	{
		if (lc->cmd == LC_SYMTAB)
		{
			if (!mem_acces(buff, size, sizeof(struct symtab_command), lc))
				error("");
			nm_64_sym(buff, size, *(struct symtab_command *)lc);
			break ;
		}
		if (!(lc = mem_acces(buff, size, lc->cmdsize, lc)))
			error("");
	}
	return (0);
}

int	nm_32(const char *buff, const size_t size)
{
	(void)buff;
	(void)size;
	return (0);
}

int	nm(const char *buff, const size_t size)
{
	if (size < 4)
		return (-1);
	if (*(uint32_t *)buff == MH_MAGIC)
		return (nm_32(buff, size));
	else if (*(uint32_t *)buff == MH_MAGIC_64)
		return (nm_64(buff, size));
	else
		return (-1);
}

int	main(int argc, char **argv)
{
	int			fd;
	struct stat	file_stats;
	char		*buff;

	if (argc != 2)
	{
		fprintf(stderr, "usage: %s <file>\n", argv[0]);
		return (EXIT_FAILURE);
	}
	if ((fd = open(argv[1], O_RDONLY)) == -1)
	{
		perror("open");
		fprintf(stderr, "can't open file '%s'\n", argv[1]);
		return (EXIT_FAILURE);
	}
	if (fstat(fd, &file_stats) == -1)
	{
		perror("fstat");
		fprintf(stderr, "can't get stats on file '%s'\n", argv[1]);
		return (EXIT_FAILURE);
	}
	if ((buff = mmap(0, file_stats.st_size, PROT_READ, MAP_PRIVATE, fd, 0)) == MAP_FAILED)
	{
		perror("mmap");
		fprintf(stderr, "can't read '%s'\n", argv[1]);
		return (EXIT_FAILURE);
	}
	nm(buff, file_stats.st_size);
	if (munmap(buff, file_stats.st_size) == -1)
	{
		perror("munmap");
		fprintf(stderr, "can't release buffered file '%s'\n", argv[1]);
		return (EXIT_FAILURE);
	}
	if (close(fd) == -1)
	{
		perror("close");
		fprintf(stderr, "can't close file '%s'\n", argv[1]);
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}
