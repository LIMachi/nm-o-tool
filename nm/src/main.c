/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmartzol <hmartzol@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/11 11:18:50 by hmartzol          #+#    #+#             */
/*   Updated: 2018/08/11 12:02:28 by hmartzol         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <nm.h>

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

static inline int	sif_error(char form, char comp)
{
	ft_dprintf(2, form, comp);
	return (EXIT_FAILURE);
}

static inline int	sif_read_opts(int argc, char **argv, t_nm_env *env,
									t_getopt_env *ge)
{
	
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
	return (EXIT_SUCCESS);

}

int					main(int argc, char **argv)
{
	int				fd;
	struct stat		file_stats;
	char			*buff;
	t_getopt_env	ge;
	static t_nm_env	env = {.flags = DEFAULT_FLAGS};

	if (sif_read_opts(argc, argv, &env, &ge))
		return (EXIT_FAILURE);
}
