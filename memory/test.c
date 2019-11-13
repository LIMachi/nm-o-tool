/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmartzol <hmartzol@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2000/00/00 00:00:00 by hmartzol          #+#    #+#             */
/*   Updated: 2000/00/00 00:00:00 by hmartzol         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <unistd.h>
#include <stdint.h>

#include <error.h>
#include <memory.h>
#include <vm.h>

#include <mach-o/loader.h>
#include <mach-o/fat.h>
#include <ar.h>

#include <string.h>

#include <stddef.h>

/*
** 0b001 -> true: 32 bit, false: 64 bit
** 0b010 -> true: reversed, false: normal
** 0b100 -> true: fat, false: object
** 0x08  -> reserved for archives, other flags are ignored
** 0xFF  -> reserved for error
*/

uint8_t				mm_file_type(t_memory_map *mm)
{
	const uint32_t	magics[8] = {MH_MAGIC_64, MH_MAGIC, MH_CIGAM_64, MH_CIGAM,
							FAT_MAGIC_64, FAT_MAGIC, FAT_CIGAM_64, FAT_CIGAM};
	uint32_t		magic;
	uint8_t			it;

	if (mm->size < sizeof(uint32_t))
		return (0xFF);
	magic = *(uint32_t*)mm->ptr;
	it = 0;
	while (it < 8 && magics[it] != magic)
		++it;
	if (it < 8)
		return (it);
	if (mm->size < SARMAG || strncmp((char*)mm->ptr, ARMAG, SARMAG))
		return (0xFF);
	return (8);
}

t_struct_descriptor	g_mach_header_descriptor = {
	.nb_members = 7, .align = 4, .total_size = sizeof(struct mach_header),
	.member = {
		{.block_size = sizeof(uint32_t), .nb_blocks = 1, .align = 4, .sign = 0},
		{.block_size = sizeof(cpu_type_t), .nb_blocks = 1, .align = 4,
			.sign = 0},
		{.block_size = sizeof(cpu_subtype_t), .nb_blocks = 1, .align = 4,
			.sign = 0},
		{.block_size = sizeof(uint32_t), .nb_blocks = 1, .align = 4, .sign = 0},
		{.block_size = sizeof(uint32_t), .nb_blocks = 1, .align = 4, .sign = 0},
		{.block_size = sizeof(uint32_t), .nb_blocks = 1, .align = 4, .sign = 0},
		{.block_size = sizeof(uint32_t), .nb_blocks = 1, .align = 4, .sign = 0}
	}
};

t_struct_descriptor	g_mach_header_64_descriptor = {
	.nb_members = 8, .align = 4, .total_size = sizeof(struct mach_header_64),
	.member = {
		{.block_size = sizeof(uint32_t), .nb_blocks = 1, .align = 4, .sign = 0},
		{.block_size = sizeof(cpu_type_t), .nb_blocks = 1, .align = 4,
			.sign = 0},
		{.block_size = sizeof(cpu_subtype_t), .nb_blocks = 1, .align = 4,
			.sign = 0},
		{.block_size = sizeof(uint32_t), .nb_blocks = 1, .align = 4, .sign = 0},
		{.block_size = sizeof(uint32_t), .nb_blocks = 1, .align = 4, .sign = 0},
		{.block_size = sizeof(uint32_t), .nb_blocks = 1, .align = 4, .sign = 0},
		{.block_size = sizeof(uint32_t), .nb_blocks = 1, .align = 4, .sign = 0},
		{.block_size = sizeof(uint32_t), .nb_blocks = 1, .align = 4, .sign = 0}
	}
};

int					macho(t_memory_map *mm, int b32)
{
	uint8_t					tmp[sizeof(struct mach_header_64)];
	struct mach_header_64	head;
	int						endian;
	t_memory_map			mm_tmp;
	t_memory_map			mm_head;

	endian = get_local_endian();
	mm_head = (t_memory_map){sizeof(head), 0, endian, (uint8_t*)&head};
	if (b32)
	{
		if (read_struct_in_memory(mm, &tmp, g_mach_header_descriptor, endian))
			return (1);
		mm_tmp = (t_memory_map){sizeof(tmp), 0, endian, tmp};
		if (cast_struct(&mm_head, &mm_tmp, (t_cast_struct_descriptor){
				g_mach_header_descriptor, g_mach_header_64_descriptor, {
				0, 1, 2, 3, 4, 5, 6}}))
			return (1);
	}
	else if (read_struct_in_memory(mm, &head, g_mach_header_64_descriptor, endian))
		return (1);
	return (0);
}

int					test(t_memory_map *mm)
{
	uint8_t type;

	type = mm_file_type(mm);
	if (type == 0xFF)
		return (set_error(-1, ERROR_TUPLE));
	if (type == 8)
		return (0);
	if (type & 0b010)
		mm->endian = !get_local_endian();
	else
		mm->endian = get_local_endian();
	if (type & 0b100)
		NULL;
	else
		return (macho(mm, type & 0b001));
	return (0);
}

int					main(int argc, char *argv[])
{
	static char		*replace_argv[2] = {NULL, "a.out"};
	int				i;
	t_memory_map	mm;

	if (argc == 1)
	{
		replace_argv[0] = argv[0];
		argv = replace_argv;
		argc = 2;
	}
	i = 0;
	memory_map_clear(&mm);
	while (++i < argc)
	{
		write(1, "\n", 1);
		if (memory_map_from_file(&mm, argv[i]) != ME_OK)
			return (get_error()->error);
		if (test(&mm))
			return (get_error()->error);
	}
	return (0);
}
