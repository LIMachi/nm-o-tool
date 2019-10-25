/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmartzol <hmartzol@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2000/00/00 00:00:00 by hmartzol          #+#    #+#             */
/*   Updated: 2000/00/00 00:00:00 by hmartzol         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ERROR_H
# define ERROR_H

# ifndef DEBUG
#  define DEBUG 0
# endif

typedef struct	s_error_tuple
{
	int			error;
	char		*file;
	char		*function;
	int			line;
}				t_error_tuple;

t_error_tuple	*get_error(void);
int				set_error(int error, char *file, char *function, int line);

# define ERROR_TUPLE __FILE__, (char*)__FUNCTION__, __LINE__

#endif
