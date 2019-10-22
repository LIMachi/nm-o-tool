/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmartzol <hmartzol@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/11 11:18:50 by hmartzol          #+#    #+#             */
/*   Updated: 2018/08/21 22:29:18 by lee              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <nm.h>

#include <libft.h>
#include <ft_getopt.h>

#include <sys/mman.h>

/*
** mmap, munmap, PROT_READ, MAP_FAILED, MAP_PRIVATE
*/

#include <stdlib.h>

/*
** EXIT_SUCCESS, EXIT_FAILURE
*/

#include <fcntl.h>

/*
** open, O_RDONLY
*/

#include <unistd.h>

/*
** close
*/

/*
** options: (mac)
** -a: all symbol entries														-*
** -g: only external															-*
** -n: sort numerically after alphabetically									-*
** -o: prepend file or archive element to each line								-*
** -p: symbol-table order (no sort)												-*
** -r: reverse sort																-*
** -u: only undefined															-*
** -U: don't display undefined													-*
** -m: extend format of type instead of letters									-
** -x: hexadecimal print of type + position of symbol table						-
** -j: only names of symbols, no type or position								-
** -s segname sectname: only list symbols in section (segname, sectname)		-
** -s (segname,sectname): used version of andling -s (only one string)			-
** -arch: architecture to be printed in fat files, all is an option				-
** --arch: used version of andling -arch										-
** -f format: bsd, sysv, posix, darwin											-
** -A: same as -o, but print full path											-
** -P: portable output (which ever this means)									-
** -t format: precise the format to be used by -P								-
** 	d: decimal, o: octal, x: hexadecimal										-
** -L: see man, i'm tired														-
*/

/*
** options: (linux)
** -A -o --print-file-name: -o
** -a --debug-syms: -a
** -B: --format=bsd
** -C --demangle[=style]: try tu print the user-level symbol names
** --no-demangle
** -D --dynamic: display dynamic symbols (shared)
** -f format --format=format: -f
** -g --extern-only: -g
** -h --help
** -l --line-numbers: print debug line number (might not work)
** -n -v --numeric-sort: -n
** -p --no-sort: -p
** -P --portability: posix 2 output format
** -r --reverse-sort: -r
** -S --print-size: specific to bsd format, print both value and size of symbols
** -s --print-armap: see man nm (includes index of members in archives)
** -t radix --radix=radix: similar to -t
** -u --undefined-only: -u
** -v --version
** --defined-only: -U
** --size-sort: sort by symbol size (usually using the delta beetween 2 symbols)
** -X --plugin --special-syms @file ignored in this version
** --synthetic: include synthetic (debug) symbols
** --target: similar to -arch
*/

static inline int	sif_error(char *form, char *comp)
{
	ft_dprintf(2, form, comp);
	return (EXIT_FAILURE);
}

static inline int	sif_help(void)
{
	ft_printf("*placeholder*\n");
	return (EXIT_SUCCESS);
}

static inline int	sif_read_opts(int argc, char **argv, t_nm_env *env,
									t_getopt_env *ge)
{
	int	r;
	const t_getopt_opt	longopts[16] = {{'o', "print-file-name", NO, NULL},
	{'a', "debug-syms", NO, NULL}, {'g', "extern-only", NO, NULL},
	{'n', "numeric-sort", NO, NULL}, {'p', "no-sort", NO, NULL},
	{'r', "reverse-sort", NO, NULL}, {'u', "undefined-only", NO, NULL},
	{'U', "defined-omly", NO, NULL}, {'j', "names-only", NO, NULL},
	{'s', "section", REQUIRED, NULL}, {2, "arch", REQUIRED, NULL},
	{'f', "format", REQUIRED, NULL}, {'A', "print-file-path", NO, NULL},
	{'C', "demangle", NO, NULL}, {'h', "help", NO, NULL},
	{'v', "version", NO, NULL}};

	*ge = ft_getopt_env("oagnpruUmxjs:f:AChv", longopts);
	(void)env;
	while((r = ft_getopt(argc, argv, ge)) > -1 && r != '?' && r != ':')
	{
		if (r == 'h')
			return (sif_help());
	}
	if (r == '?' || r == ':')
		return (EXIT_FAILURE);
	return (EXIT_SUCCESS);
}

static inline int	sif_load_file(char *path, t_nm_env *env)
{
	int	fd;

	if ((fd = open(path, O_RDONLY)) == -1)
		return (sif_error("can't open file '%s'\n", path));
	if (fstat(fd, &env->file_stats) == -1)
		return (sif_error("can't get stats on file '%s'\n", path));
	if ((env->file = mmap(0, env->file_stats.st_size,
			PROT_READ, MAP_PRIVATE, fd, 0)) == MAP_FAILED)
		return (sif_error("can't read '%s'\n", path));
	if (close(fd) == -1)
		return (sif_error("can't close file '%s'\n", path));
	env->file_path = path;
	return (EXIT_SUCCESS);
}

int					main(int argc, char **argv)
{
	t_getopt_env	ge;
	static t_nm_env	env = {.flags = DEFAULT_FLAGS};

	if (sif_read_opts(argc, argv, &env, &ge))
		return (EXIT_FAILURE);
	if (ge.optind-- == argc)
	{
		if (sif_load_file("./a.out", &env) == EXIT_FAILURE)
			return (EXIT_FAILURE);
		if (nm(&env) == EXIT_FAILURE)
			return (EXIT_FAILURE);
		if (munmap(env.file, env.file_stats.st_size) == -1)
			return (sif_error("can't unload file '%s'\n", env.file_path));
	}
	else
		while (++ge.optind < argc)
		{
			if (sif_load_file(argv[ge.optind], &env) == EXIT_FAILURE)
				return (EXIT_FAILURE);
			if (nm(&env) == EXIT_FAILURE)
				return (EXIT_FAILURE);
			if (munmap(env.file, env.file_stats.st_size) == -1)
				return (sif_error("can't unload file '%s'\n", env.file_path));
		}
	return (EXIT_SUCCESS);
}
