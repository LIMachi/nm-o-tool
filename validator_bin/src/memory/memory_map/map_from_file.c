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

	if (mm->err != ME_OK)
		return (mm->err);
	if ((fd = open(path, O_RDONLY)) == -1)
		return (memory_error(&mm->err, ME_COULD_NOT_OPEN_FILE, DEBUG_TUPLE));
	if (fstat(fd, &sb))
		return ((close(fd) & 0)
			| memory_error(&mm->err, ME_COULD_NOT_STAT_FILE, DEBUG_TUPLE));
		mm->size = (size_t)sb.st_size;
	mm->ptr = mmap(NULL, mm->size, PROT_READ, MAP_PRIVATE, fd, 0);
	close(fd);
	if (mm->ptr == MAP_FAILED)
		return (memory_error(&mm->err, ME_MAPPING_FAILED, DEBUG_TUPLE));
	mm->map = mmap(NULL, mm->size, PROT_READ | PROT_WRITE,
		MAP_ANON | MAP_PRIVATE, -1, 0);
	if (mm->map == MAP_FAILED)
		return ((munmap(mm->ptr, mm->size) & 0)
			| memory_error(&mm->err, ME_MAPPING_FAILED, DEBUG_TUPLE));
		fd = -1;
	while (++fd < (int)mm->size)
		mm->map[fd] = 0;
	*mm = (t_memory_map){0, mm->size, 0, ME_OK, mm->ptr, mm->map};
	return (ME_OK);
}
