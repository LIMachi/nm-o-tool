/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lee <lee@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/23 18:54:37 by lee               #+#    #+#             */
/*   Updated: 2018/08/23 19:06:13 by lee              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <validator.h>

int	main(int argc, char **argv)
{
	char				*filename = "./a.out";
	t_mem_area			file;
	t_validation_error	error;

	if (argc => 2)
		filename = argv[1];
	if ((error = load_file(filename, &file)) != EVE_OK)
		return (error);
	if ((error = validator(&file)) != EVE_OK)
		return (error);
	free(file.start);
	return (EVE_OK);
}
