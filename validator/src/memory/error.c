#include <memory.h>

/*
** <unistd.h>
** ssize_t write(int fildes, const void *buf, size_t nbyte);
*/

#include <unistd.h>

/*
** <stdio.h>
** int dprintf(int fd, const char * restrict format, ...);
*/

#include <stdio.h>

t_memory_error	memory_error(t_memory_map *mm, const t_memory_error err, t_debug_tuple debug_tuple)
{
	if (err != ME_OK)
	{
		write(2, "Caught error in memory manipulation, please set a breakpoint "
				 "on memory_error\n", 77);
		dprintf(2, "Caused by %s:%s:%d\n", debug_tuple.file, debug_tuple.function, debug_tuple.line);
	}
	return (mm->error = err);
}
