/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmartzol <hmartzol@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2000/00/00 00:00:00 by hmartzol          #+#    #+#             */
/*   Updated: 2000/00/00 00:00:00 by hmartzol         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
#include <errno.h>
#include <string.h>

t_memory_error	memory_error(t_memory_error *me, const t_memory_error err,
	t_debug_tuple debug_tuple)
{
	if (err != ME_OK)
		dprintf(2, "Caught error(%d) in memory manipulation, please set a break"
			"point\nCaused by %s:%s:%d (%d: %s)\n", err, debug_tuple.file,
			debug_tuple.function, debug_tuple.line, errno,
			errno ? strerror(errno) : "Library error");
	if (me != NULL)
		*me = err;
	return (err);
}
