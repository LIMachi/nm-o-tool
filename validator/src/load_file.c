/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   load_file.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lee <lee@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/23 18:56:13 by lee               #+#    #+#             */
/*   Updated: 2018/08/23 19:07:32 by lee              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/stat.h>

/*
** fstat, struct stat
*/

#include <sys/mman.h>

/*
** mmap, munmap, PROT_READ, MAP_FAILED, MAP_PRIVATE
*/

#include <fcntl.h>

/*
** open, O_RDONLY
*/

#include <unistd.h>

/*
** close
*/

int	load_file(char *path, t_mem_area *area)
{
	int			fd;
	struct stat	file_stats;

	if (path == NULL)
		return (EVE_MALFORMATED_PATH);
	if (file == NULL || file->start == NULL || file->end == NULL
			|| file->end < file->start)
		return (EVE_MALFORMATED_MEM_AREA);
	if ((fd = open(path, O_RDONLY)) == -1)
		return (EVE_CANT_OPEN_FILE);
	if (fstat(fd, &file_stats) == -1)
		return (EVE_CANT_STAT_FILE);
	if ((area->start = mmap(0, file_stats.st_size,
			PROT_READ, MAP_PRIVATE, fd, 0)) == MAP_FAILED)
		return (EVE_CANT_READ_FILE);
	area->end = area->start + file_stats.st_size;
	if (close(fd) == -1)
		return (EVE_CANT_CLOSE_FILE);
	return (EVE_OK);
}
