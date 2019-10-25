/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   macho_validator.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmartzol <hmartzol@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2000/00/00 00:00:00 by hmartzol          #+#    #+#             */
/*   Updated: 2000/00/00 00:00:00 by hmartzol         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <mach_o_validator.h>

#include <string.h> //TODO: change calls to strncmp
#include <stdio.h> //DEBUG

#include <unistd.h>
#include <errno.h>

/*
** validate a single macho (object/executable) file using his map
*/

t_validator_error	validate_macho(t_memory_map *mm, uint8_t type, uint8_t map)
{
	t_macho_file	obj;

	obj = (t_macho_file){.err = VE_OK, .mm = *mm, .file = {.data = mm->ptr},
		.vmm = {.total_size = 0, .nb_entries = 0, .err = ME_OK, .entries = NULL,
			.biggest_id = 0}};
	obj.mm.swap = (type & 2u) == 2;
	obj.format = type & 1u ? 32 : 64;
	if (claim_map(&obj.mm, (t_memory_descriptor){1, obj.head.sizeofcmds, 1, 0},
			MM_CMD, 0) != ME_OK)
		return (VE_ME_MASK | obj.mm.err);
	if (validate_commands(&obj) != VE_OK)
		return (obj.err);
	return (VE_OK);
}

/*
** 0b001 -> true: 32 bit, false: 64 bit
** 0b010 -> true: reversed, false: normal
** 0b100 -> true: fat, false: object
** 0x08  -> reserved for archives, other flags are ignored
** 0xFF  -> reserved for error
*/

uint8_t		mm_file_type(t_memory_map *mm)
{
	uint32_t	magics[8] = {MH_MAGIC_64, MH_MAGIC, MH_CIGAM_64, MH_CIGAM,
		FAT_MAGIC_64, FAT_MAGIC, FAT_CIGAM_64, FAT_CIGAM};
	uint32_t	magic;
	uint8_t		it;

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

t_validator_error	validate_macho_map(t_memory_map *mm, uint8_t map);

t_validator_error	validate_macho_fat(t_memory_map *mm, uint8_t type)
{
	uint32_t			nfat;
	uint32_t			it;
	size_t				cur;
	struct fat_arch_64	fat;
	uint8_t				stype;
	t_validator_error	err;

	mm->swap = (type & 0b10) == 0b10;
	if (claim_map(mm, (t_memory_descriptor){4, 2, 4, 0}, MM_FAT, 0) != ME_OK)
		return (VE_ME_MASK | mm->err);
	mm->cursor += 4;
	if (read_in_memory(mm, &nfat, MM_FAT, (t_memory_descriptor){4, 1, 4, 1}) != ME_OK)
		return (VE_ME_MASK | mm->err);
	printf("nfat: %d (lw: %s)\n", nfat, (type & 1) ? "32" : "64");
	claim_map(mm, (t_memory_descriptor){(type & 1) ? sizeof(struct fat_arch) : sizeof(struct fat_arch_64), nfat, 1, 0}, MM_FAT_ARCH, 0);
	it = (uint32_t)-1;
	while (++it < nfat)
	{
		read_struct_in_memory(mm, &fat, MM_FAT_ARCH, (t_struct_descriptor){3, 1, sizeof(cpu_type_t) + sizeof(cpu_subtype_t) + ((type & 1) ? 4 : 8), {{sizeof(cpu_type_t), 1, 1, 1}, {sizeof(cpu_subtype_t), 1, 1, 1}, {type & 1 ? 4 : 8, 1, 1, 1}}});
		read_in_memory(mm, &fat.size, MM_FAT_ARCH, (t_memory_descriptor){type & 1 ? 4 : 8, 1, 1, 1});
		read_in_memory(mm, &fat.align, MM_FAT_ARCH, (t_memory_descriptor){4, type & 1 ? 1 : 2, 1, 1});
		if (mm->err != ME_OK)
			return (VE_ME_MASK | mm->err);
		if (type & 1)
		{
			fat.offset = *(uint32_t *)&fat.offset;
			fat.size   = *(uint32_t *)&fat.size;
		}
		printf("offset: %llu, size: %llu, align: %d\n", fat.offset, fat.size, 1 << fat.align);
		cur = mm->cursor;
		mm->cursor = fat.offset;
		claim_map(mm, (t_memory_descriptor){fat.size, 1, 1 << fat.align, 0}, -2 - it, 0);
		if ((err = validate_macho_map(mm, -2 - it)) != VE_OK)
			return (err);
		mm->cursor = cur;
	}
	return (VE_OK);
}

t_validator_error	validate_macho_archive(t_memory_map *mm)
{
	(void)mm;
	return (VE_OK);
}

t_validator_error	validate_macho_map(t_memory_map *mm, uint8_t map)
{
	uint8_t			type;

	if ((type = mm_file_type(mm)) == 0xFF)
		return (VE_INVALID_MAGIC_NUMBER);
	if (type == 8)
		return (validate_macho_archive(mm));
	else if (type & 4)
		return (validate_macho_fat(mm, type));
	else
		return (validate_macho(mm, type, map));
	return (VE_INVALID_MAGIC_NUMBER);
}

int					main(int argc, char *argv[])
{
	int				i;
	t_memory_map	mm;
	static char		*replace_argv[2] = {NULL, "a.out"};

	i = 0;
	memory_map_clear(&mm);
	if (argc == 1)
	{
		replace_argv[0] = argv[0];
		argv = replace_argv;
		argc = 2;
	}
	while (++i < argc)
	{
		write(1, "\n", 1);
		memory_map_from_file(&mm, argv[i]);
		if (mm.err != VE_OK)
		{
			printf("%s: %s: %s\n", argv[0], argv[i], strerror(errno));
			return (mm.err);
		}
		if (validate_macho_map(&mm, MM_PAD) != ME_OK)
			return (mm.err);
		memory_unmap(&mm);
	}
	return (VE_OK);
}
