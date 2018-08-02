/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmartzol <hmartzol@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/02 21:12:04 by hmartzol          #+#    #+#             */
/*   Updated: 2018/08/03 00:28:37 by hmartzol         ###   ########.fr       */
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

int	nm_64_sym(const char *buff, const size_t size, struct symtab_command sc)
{
	size_t			i;
	char			*strings;
	struct nlist_64	*nl;

	i = -1;
	nl = mem_acces(buff, size, sc.symoff, buff);
	mem_acces(buff, size, sc.nsyms * sizeof(struct nlist_64), nl);
	strings = mem_acces(buff, size, sc.stroff, buff);
	while (++i < sc.nsyms)
	{
		char *tmp = (char*)mem_acces(buff, size, nl[i].n_un.n_strx, strings);
		char c = '*'; //FIXME: faire les autres charactères
		if ((nl[i].n_type & N_TYPE) == N_SECT)
			c = nl[i].n_type & N_EXT ? 'T' : 't';
		if ((nl[i].n_type & N_TYPE) != N_UNDF)
			fprintf(stdout, "%016llx %c %s\n", nl[i].n_value, c, tmp);
		else
			fprintf(stdout, "                 U %s\n", tmp);
	}
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
