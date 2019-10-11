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

t_memory_error	memory_error(t_memory_map *mm, const t_memory_error err,
	t_debug_tuple debug_tuple)
{
	if (err != ME_OK)
		dprintf(2, "Caught error(%d) in memory manipulation, please set a break"
			"point\nCaused by %s:%s:%d\n", err, debug_tuple.file,
			debug_tuple.function, debug_tuple.line);
	if (mm != NULL)
		mm->error = err;
	return (err);
}
