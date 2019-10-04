#include <memory.h>

/*
** <unistd.h>
** ssize_t write(int fildes, const void *buf, size_t nbyte);
*/

#include <unistd.h>

t_memory_error	memory_error(t_memory_error err)
{
	if (err != ME_OK)
		write(2, "Caught error in memory manipulation, please set a breakpoint "
					"on memory_error\n", 77);
	return (err);
}
