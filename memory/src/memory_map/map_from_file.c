/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   map_from_file.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmartzol <hmartzol@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2000/00/00 00:00:00 by hmartzol          #+#    #+#             */
/*   Updated: 2000/00/00 00:00:00 by hmartzol         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <memory.h>

/*
** <fcntl.h>
** int open(const char *path, int oflag, ...)
*/

#include <fcntl.h>

/*
** <unistd.h>
** int close(int fildes)
*/

#include <unistd.h>

/*
** <sys/stat.h>
** struct stat
** int fstat(int fildes, struct stat *buf)
*/

#include <sys/stat.h>

/*
** <sys/mman.h>
** void *mmap(void *addr, size_t len, int prot, int flags, int fd, off_t offset)
*/

#include <sys/mman.h>

t_memory_error	memory_map_from_file(t_memory_map *mm,
									const char *path)
{
	struct stat	sb;
	int			fd;

	if (get_error()->error != ME_OK)
		return (ME_PENDING_ERROR);
	if ((fd = open(path, O_RDONLY)) == -1)
		return (set_error(ME_COULD_NOT_OPEN_FILE, ERROR_TUPLE));
	if (fstat(fd, &sb))
		return ((close(fd) & 0)
			| set_error(ME_COULD_NOT_STAT_FILE, ERROR_TUPLE));
	mm->size = (size_t)sb.st_size;
	mm->ptr = mmap(NULL, mm->size, PROT_READ, MAP_PRIVATE, fd, 0);
	close(fd);
	if (mm->ptr == MAP_FAILED)
		return (set_error(ME_MAPPING_FAILED, ERROR_TUPLE));
	*mm = (t_memory_map){mm->size, 0, mm->ptr};
	return (ME_OK);
}
