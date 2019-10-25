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

#include <error.h>

/*
** <stdio.h>
** int dprintf(int fd, const char * restrict format, ...);
*/

#include <stdio.h>

t_error_tuple	*get_error(void)
{
	static t_error_tuple tuple = {0, "", "", 0};

	return (&tuple);
}

int				set_error(int error, char *file, char *function, int line)
{
	*get_error() = (t_error_tuple){error, file, function, line};
	if (DEBUG == 1 && error)
		dprintf(2, "Error %d at %d:%s:%s\n", error, line, file, function);
	return (error);
}
