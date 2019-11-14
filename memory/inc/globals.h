/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   globals.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmartzol <hmartzol@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2000/00/00 00:00:00 by hmartzol          #+#    #+#             */
/*   Updated: 2000/00/00 00:00:00 by hmartzol         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GLOBALS_H
# define GLOBALS_H

t_struct_descriptor	g_mach_header_descriptor;
t_struct_descriptor	g_mach_header_64_descriptor;
t_struct_descriptor	g_section_descriptor;
t_struct_descriptor	g_section_64_descriptor;
t_struct_descriptor	g_load_commands_descriptors[0x40];

#endif
