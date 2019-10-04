#include <mach_o_validator.h>

#include <stdio.h>
#include <stdlib.h>

/*
** memcmp
*/

#include <string.h>

t_validator_error	obj_err(t_macho_file *file, t_validator_error error)
{
	if (error != VE_OK)
		printf("caught error, please set a breakpoint on 'obj_err'\n");
	return (file->err = error);
}

t_validator_error	clean_objfile(t_macho_file *file, t_validator_error error)
{
	obj_err(file, error);
	if (file->fd != -1 && file->file_map != MAP_FAILED)
	{
		if (/*debug*/1 && file->file.data != MAP_FAILED)
			for (size_t it = 0; it < file->size && it < 4096; ++it)
				it % 16 == 0 ? printf("%08.8llX ", (unsigned long long)it) : 0, printf("\x1b[38;5;%um%02.2X\x1b[0m%c", 29 + file->file_map[it] % 8, file->file.data[it], it % 16 == 15 ? '\n' : ' ');
		munmap(file->file_map, file->size);
		file->file_map = MAP_FAILED;
	}
	if (file->fd != -1 && file->file.data != MAP_FAILED)
	{
		munmap(file->file.data, file->size);
		file->file.data = MAP_FAILED;
	}
	if (file->fd != -1)
	{
		close(file->fd);
		file->fd = -1;
	}
	file->size = 0;
	file->head = (struct mach_header_64){.magic = 0};
	return (error);
}

t_validator_error	reader(const char *path, t_macho_file *obj)
{
	struct stat	sb;
	size_t		it;

	if (path == NULL || obj == NULL)
		return (VE_NULL_PTR);
	*obj = (t_macho_file){.fd = -1, .file = {MAP_FAILED}, .file_map = MAP_FAILED};
	if ((obj->fd = open(path, O_RDONLY)) == -1)
		return (clean_objfile(obj, VE_COULD_NOT_OPEN_FILE));
	if (fstat(obj->fd, &sb))
		return (clean_objfile(obj, VE_COULD_NOT_RETRIEVE_STATS));
	obj->size = (size_t)sb.st_size;
	if ((obj->file.data = mmap(NULL, obj->size, PROT_READ, MAP_PRIVATE, obj->fd,
								0)) == MAP_FAILED)
		return (clean_objfile(obj, VE_MAPPING_ERROR));
	if ((obj->file_map = mmap(NULL, obj->size, PROT_WRITE | PROT_READ,
			MAP_PRIVATE | MAP_ANON, -1, 0)) == MAP_FAILED)
		return (clean_objfile(obj, VE_MAPPING_ERROR));
	it = (size_t)-1;
	while (++it < obj->size)
		obj->file_map[it] = MM_PAD;
	return (VE_OK);
}

/*
** secured accessor of memory
** check if index is in the file, if the last block would got outside the file
** if the block_size might overflow and the expected mapping.
** return VE_OK on success, another error on faillure (also stored in obj)
*/

t_validator_error	valid_cursor(t_macho_file *obj, t_cursor rc, size_t *align)
{
	size_t	tmp;

	if (align == NULL)
		align = &tmp;
	if (rc.index >= obj->size)
		return (obj_err(obj, VE_INVALID_FILE_POSITION));
	if ((*align = rc.block_size % rc.align))
		*align = rc.block_size + rc.align - *align;
	else
		*align = rc.block_size;
	if (rc.index + *align < rc.index || rc.index + *align >= obj->size)
		return (obj_err(obj, VE_INVALID_BLOC_SIZE));
	if (rc.nb_blocs && (rc.nb_blocs * *align < rc.nb_blocs
						|| rc.nb_blocs * *align < *align))
		return (obj_err(obj, VE_INVALID_BLOC_COUNT));
	return (VE_OK);
}

/*
** first validate the cursor
** then retrieve some memory
** should swap is a flag to tell if the endian swap should be used
** buffer should be a memory big enough to contain the read data
** buffer can be NULL to only test the area
** return VE_OK on success, another error on faillure (also stored in obj)
*/

t_validator_error	read_in_object(t_macho_file *obj, t_cursor rc,
									void *buffer, int should_swap)
{
	size_t	align;
	size_t	it;
	size_t	sw;

	if (valid_cursor(obj, rc, &align) != VE_OK)
		return (obj->err);
	it = (size_t)-1;
	while (++it < rc.nb_blocs && (sw = (size_t)-1))
	{
		while (++sw < align)
			if (obj->file_map[rc.index + it * align + sw] != rc.expected_mapping)
				return (obj_err(obj, VE_INVALID_MAPING));
		if (buffer == NULL)
			continue ;
		sw = (size_t)-1;
		while (++sw < align)
			if (should_swap && obj->endian && sw < rc.block_size)
				((uint8_t*)buffer)[it * align + sw] = obj->file.data[
					rc.index + it * align + rc.block_size - sw];
			else
				((uint8_t*)buffer)[it * align + sw]
					= obj->file.data[rc.index + it * align + sw];
	}
	return (VE_OK);
}

t_struct_descriptor g_section_structure = {sizeof(struct section), 2, {{16, 2, 1, 0}, {4, 9, 4, 1}}};

t_validator_error	read_struct_in_object(t_macho_file *obj, void *buffer, t_struct_cursor sc, t_struct_descriptor sd)
{

}

/*
** first validate the cursor
** then try to claim part of the file map
** return VE_INVALID_CLAIM if the are is already claimed
** return VE_OK on success
** by default the padding are not mapped
*/

t_validator_error	claim_map(t_macho_file *obj, t_cursor rc, uint8_t claim)
{
	size_t	align;
	size_t	it;
	size_t	sw;

	if (valid_cursor(obj, rc, &align) != VE_OK)
		return (obj->err);
	it = (size_t)-1;
	while (++it < rc.nb_blocs && (sw = (size_t)-1))
		while (++sw < rc.block_size)
			if (obj->file_map[rc.index + align * it + sw] != MM_PAD)
				return (obj_err(obj, VE_INVALID_CLAIM));
			else
				obj->file_map[rc.index + align * it + sw] = claim;
	return (VE_OK);
}

/*
** first validate the cursor
** then unclaim (set to MM_PAD) part of the file map
** return VE_OK on success
*/

t_validator_error	unclaim_map(t_macho_file *obj, t_cursor rc)
{
	size_t	align;
	size_t	it;
	size_t	sw;

	if (valid_cursor(obj, rc, &align) != VE_OK)
		return (obj->err);
	it = (size_t)-1;
	while (++it < rc.nb_blocs && (sw = (size_t)-1))
		while (++sw < rc.block_size)
				obj->file_map[rc.index + align * it + sw] = MM_PAD;
	return (VE_OK);
}

int		in(const int val, size_t cnt, const int test[])
{
	while (cnt--)
		if (val == test[cnt])
			return (1);
	return (0);
}

t_validator_error	validate_head(t_macho_file *obj)
{
	if (!in(obj->head.cputype, 14, (int[14]){CPU_TYPE_VAX, CPU_TYPE_MC680x0,
			CPU_TYPE_X86, CPU_TYPE_I386, CPU_TYPE_X86_64, CPU_TYPE_MC98000,
			CPU_TYPE_HPPA, CPU_TYPE_ARM, CPU_TYPE_ARM64, CPU_TYPE_MC88000,
			CPU_TYPE_SPARC, CPU_TYPE_I860, CPU_TYPE_POWERPC,
			CPU_TYPE_POWERPC64}))
		return (obj_err(obj, VE_INVALID_CPU_TYPE));
	//subtype ignored for now, would require a huge switch - case
	if (!in(obj->head.filetype, 11, (int[11]){MH_OBJECT, MH_EXECUTE, MH_FVMLIB,
			MH_CORE, MH_PRELOAD, MH_DYLIB, MH_DYLINKER, MH_BUNDLE,
			MH_DYLIB_STUB, MH_DSYM, MH_KEXT_BUNDLE}))
		return (obj_err(obj, VE_INVALID_FILE_TYPE));
	if (obj->head.ncmds >= obj->size
			|| obj->head.ncmds * sizeof(struct load_command) >= obj->size)
		return (obj_err(obj, VE_INVALID_NUMBER_OF_COMMANDS));
	if (obj->head.sizeofcmds >= obj->size)
		return (obj_err(obj,VE_INVALID_TOTAL_COMMAND_SIZE));
	return (VE_OK);
}

t_validator_error	validate_head_32(t_macho_file *obj)
{
	if (claim_map(obj, (t_cursor){0, 1, sizeof(t_moh32), 1, MM_PAD}, MM_HEADER))
		return (obj->err);
	if (read_in_object(obj, (t_cursor){__offsetof(t_moh32, cputype), 1,
		sizeof(cpu_type_t), 1, MM_HEADER}, &obj->head.cputype, 1)
		|| read_in_object(obj, (t_cursor){__offsetof(t_moh32, cpusubtype),
		1, sizeof(cpu_subtype_t), 1, MM_HEADER}, &obj->head.cpusubtype, 1)
		|| read_in_object(obj, (t_cursor){__offsetof(t_moh32, filetype), 4,
		sizeof(uint32_t), 1, MM_HEADER}, &obj->head.filetype, 1))
		return (obj->err);
	return (validate_head(obj));
}

t_validator_error	validate_head_64(t_macho_file *obj)
{
	if (claim_map(obj, (t_cursor){0, 1, sizeof(t_moh64), 1, MM_PAD}, MM_HEADER))
		return (obj->err);
	if (read_in_object(obj, (t_cursor){__offsetof(t_moh64, cputype), 1,
			sizeof(cpu_type_t), 1, MM_HEADER}, &obj->head.cputype, 1)
			|| read_in_object(obj, (t_cursor){__offsetof(t_moh64, cpusubtype),
			1, sizeof(cpu_subtype_t), 1, MM_HEADER}, &obj->head.cpusubtype, 1)
			|| read_in_object(obj, (t_cursor){__offsetof(t_moh64, filetype), 5,
			sizeof(uint32_t), 1, MM_HEADER}, &obj->head.filetype, 1))
		return (obj->err);
	return (validate_head(obj));
}

t_validator_error	validate_magic(t_macho_file *obj)
{
	obj->head.magic = obj->file.h32->magic;
	if (obj->head.magic == MH_MAGIC)
	{
		obj->format = 32;
		obj->endian = 0;
	}
	else if (obj->head.magic == MH_MAGIC_64)
	{
		obj->format = 64;
		obj->endian = 0;
	}
	else if (obj->head.magic == MH_CIGAM)
	{
		obj->format = 32;
		obj->endian = 1;
	}
	else if (obj->head.magic == MH_CIGAM_64)
	{
		obj->format = 64;
		obj->endian = 1;
	}
	else
		return (obj_err(obj, VE_INVALID_MAGIC_NUMBER));
	return (VE_OK);
}

/*
** default command validator, always return valid on any command
*/

t_validator_error	vlc_noop(t_load_command_descriptor *lcd, t_load_command_union lcu, t_macho_file *file)
{
	(void)lcd;
	(void)file;
	(void)lcu;
	return (VE_OK);
}

t_validator_error	vlc_sections_64(t_load_command_descriptor *lcd, t_load_command_union lcu, t_macho_file *file)
{
	size_t				it;
	struct section_64	sec;

	(void)lcd;
	(void)file;
	it = (size_t)-1;
	while (++it < lcu.segment_64.nsects)
	{

	}
	return (VE_OK);
}

t_validator_error	vlc_segment_64(t_load_command_descriptor *lcd, t_load_command_union lcu, t_macho_file *file)
{
	size_t						es;
	struct segment_command_64	seg;

	seg = lcu.segment_64;
	es = lcd->minimum_size + seg.nsects * sizeof(struct section_64);
	if (es != (size_t)lcu.lc.cmdsize)
		return (obj_err(file, VE_INVALID_SEGMENT_COUNT));
	printf("valid segment (64): '%.16s'\n", lcu.segment_64.segname);
	//vm
	if (seg.fileoff >= file->size)
		return (obj_err(file, VE_INVALID_LOAD_COMMAND_IN_FILE_ADDR));
	if (seg.fileoff + seg.filesize > file->size)
		return (obj_err(file, VE_INVALID_LOAD_COMMAND_FILE_BLOCK_SIZE));
	return (vlc_sections_64(lcd, lcu, file));
}

static const t_load_command_descriptor	g_lcd[46] = {
	{LC_SEGMENT, sizeof(struct segment_command), 0, 11, vlc_noop, {{4, 1}, {4, 1}, {16, 0}, {4, 1}, {4, 1}, {4, 1}, {4, 1}, {sizeof(vm_prot_t), 1}, {sizeof(vm_prot_t), 1}, {4, 1}, {4, 1}}},
	{LC_SEGMENT_64, sizeof(struct segment_command_64), 0, 11, vlc_segment_64, {{4, 1}, {4, 1}, {16, 0}, {8, 1}, {8, 1}, {8, 1}, {8, 1}, {sizeof(vm_prot_t), 1}, {sizeof(vm_prot_t), 1}, {4, 1}, {4, 1}}},
	{LC_IDFVMLIB, sizeof(struct fvmlib_command), 1, 5, vlc_noop, {{4, 1}, {4, 1}, {sizeof(union lc_str), 1}, {4, 1}, {4, 1}}},
	{LC_LOADFVMLIB, sizeof(struct fvmlib_command), 1, 5, vlc_noop, {{4, 1}, {4, 1}, {sizeof(union lc_str), 1}, {4, 1}, {4, 1}}},
	{LC_ID_DYLIB, sizeof(struct dylib_command), 0, 6, vlc_noop, {{4, 1}, {4, 1}, {sizeof(union lc_str), 1}, {4, 1}, {4, 1}, {4, 1}}},
	{LC_LOAD_DYLIB, sizeof(struct dylib_command), 0, 6, vlc_noop, {{4, 1}, {4, 1}, {sizeof(union lc_str), 1}, {4, 1}, {4, 1}, {4, 1}}},
	{LC_LOAD_WEAK_DYLIB, sizeof(struct dylib_command), 0, 6, vlc_noop, {{4, 1}, {4, 1}, {sizeof(union lc_str), 1}, {4, 1}, {4, 1}, {4, 1}}},
	{LC_REEXPORT_DYLIB, sizeof(struct dylib_command), 0, 6, vlc_noop, {{4, 1}, {4, 1}, {sizeof(union lc_str), 1}, {4, 1}, {4, 1}, {4, 1}}},
	{LC_SUB_FRAMEWORK, sizeof(struct sub_framework_command), 0, 3, vlc_noop, {{4, 1}, {4, 1}, {sizeof(union lc_str), 1}}},
	{LC_SUB_CLIENT, sizeof(struct sub_client_command), 0, 3, vlc_noop, {{4, 1}, {4, 1}, {sizeof(union lc_str), 1}}},
	{LC_SUB_UMBRELLA, sizeof(struct sub_umbrella_command), 0, 3, vlc_noop, {{4, 1}, {4, 1}, {sizeof(union lc_str), 1}}},
	{LC_SUB_LIBRARY, sizeof(struct sub_library_command), 0, 3, vlc_noop, {{4, 1}, {4, 1}, {sizeof(union lc_str), 1}}},
	{LC_PREBOUND_DYLIB, sizeof(struct prebound_dylib_command), 0, 5, vlc_noop, {{4, 1}, {4, 1}, {sizeof(union lc_str), 1}, {4, 1}, {sizeof(union lc_str), 1}}},
	{LC_ID_DYLINKER, sizeof(struct dylinker_command), 0, 3, vlc_noop, {{4, 1}, {4, 1}, {sizeof(union lc_str), 1}}},
	{LC_LOAD_DYLINKER, sizeof(struct dylinker_command), 0, 3, vlc_noop, {{4, 1}, {4, 1}, {sizeof(union lc_str), 1}}},
	{LC_DYLD_ENVIRONMENT, sizeof(struct dylinker_command), 0, 3, vlc_noop, {{4, 1}, {4, 1}, {sizeof(union lc_str), 1}}},
	{LC_THREAD, sizeof(struct thread_command), 0, 2, vlc_noop, {{4, 1}, {4, 1}}},
	{LC_UNIXTHREAD, sizeof(struct thread_command), 0, 2, vlc_noop, {{4, 1}, {4, 1}}},
	{LC_ROUTINES, sizeof(struct routines_command), 1, 10, vlc_noop, {{4, 1}, {4, 1}, {4, 1}, {4, 1}, {4, 1}, {4, 1}, {4, 1}, {4, 1}, {4, 1}, {4, 1}}},
	{LC_ROUTINES_64, sizeof(struct routines_command_64), 1, 10, vlc_noop, {{4, 1}, {4, 1}, {8, 1}, {8, 1}, {8, 1}, {8, 1}, {8, 1}, {8, 1}, {8, 1}, {8, 1}}},
	{LC_SYMTAB, sizeof(struct symtab_command), 1, 6, vlc_noop, {{4, 1}, {4, 1}, {4, 1}, {4, 1}, {4, 1}, {4, 1}}},
	{LC_DYSYMTAB, sizeof(struct dysymtab_command), 1, 20, vlc_noop, {{4, 1}, {4, 1}, {4, 1}, {4, 1}, {4, 1}, {4, 1}, {4, 1}, {4, 1}, {4, 1}, {4, 1}, {4, 1}, {4, 1}, {4, 1}, {4, 1}, {4, 1}, {4, 1}, {4, 1}, {4, 1}, {4, 1}, {4, 1}}},
	{LC_TWOLEVEL_HINTS, sizeof(struct twolevel_hints_command), 1, 4, vlc_noop, {{4, 1}, {4, 1}, {4, 1}, {4, 1}}},
	{LC_PREBIND_CKSUM, sizeof(struct prebind_cksum_command), 1, 3, vlc_noop, {{4, 1}, {4, 1}, {4, 1}}},
	{LC_UUID, sizeof(struct uuid_command), 1, 3, vlc_noop, {{4, 1}, {4, 1}, {16, 0}}},
	{LC_RPATH, sizeof(struct rpath_command), 1, 3, vlc_noop, {{4, 1}, {4, 1}, {sizeof(union lc_str), 1}}},
	{LC_CODE_SIGNATURE, sizeof(struct linkedit_data_command), 1, 4, vlc_noop, {{4, 1}, {4, 1}, {4, 1}, {4, 1}}},
	{LC_SEGMENT_SPLIT_INFO, sizeof(struct linkedit_data_command), 1, 4, vlc_noop, {{4, 1}, {4, 1}, {4, 1}, {4, 1}}},
	{LC_FUNCTION_STARTS, sizeof(struct linkedit_data_command), 1, 4, vlc_noop, {{4, 1}, {4, 1}, {4, 1}, {4, 1}}},
	{LC_DATA_IN_CODE, sizeof(struct linkedit_data_command), 1, 4, vlc_noop, {{4, 1}, {4, 1}, {4, 1}, {4, 1}}},
	{LC_DYLIB_CODE_SIGN_DRS, sizeof(struct linkedit_data_command), 1, 4, vlc_noop, {{4, 1}, {4, 1}, {4, 1}, {4, 1}}},
	{LC_LINKER_OPTIMIZATION_HINT, sizeof(struct linkedit_data_command), 1, 4, vlc_noop, {{4, 1}, {4, 1}, {4, 1}, {4, 1}}},
	{LC_ENCRYPTION_INFO, sizeof(struct encryption_info_command), 1, 5, vlc_noop, {{4, 1}, {4, 1}, {4, 1}, {4, 1}, {4, 1}}},
	{LC_ENCRYPTION_INFO_64, sizeof(struct encryption_info_command_64), 1, 6, vlc_noop, {{4, 1}, {4, 1}, {4, 1}, {4, 1}, {4, 1}, {4, 0}}},
	{LC_VERSION_MIN_MACOSX, sizeof(struct version_min_command), 1, 4, vlc_noop, {{4, 1}, {4, 1}, {4, 1}, {4, 1}}},
	{LC_VERSION_MIN_IPHONEOS, sizeof(struct version_min_command), 1, 4, vlc_noop, {{4, 1}, {4, 1}, {4, 1}, {4, 1}}},
	{LC_VERSION_MIN_WATCHOS, sizeof(struct version_min_command), 1, 4, vlc_noop, {{4, 1}, {4, 1}, {4, 1}, {4, 1}}},
	{LC_VERSION_MIN_TVOS, sizeof(struct version_min_command), 1, 4, vlc_noop, {{4, 1}, {4, 1}, {4, 1}, {4, 1}}},
	{LC_DYLD_INFO, sizeof(struct dyld_info_command), 1, 12, vlc_noop, {{4, 1}, {4, 1}, {4, 1}, {4, 1}, {4, 1}, {4, 1}, {4, 1}, {4, 1},{4, 1}, {4, 1}, {4, 1}, {4, 1}}},
	{LC_DYLD_INFO_ONLY, sizeof(struct dyld_info_command), 1, 12, vlc_noop, {{4, 1}, {4, 1}, {4, 1}, {4, 1}, {4, 1}, {4, 1}, {4, 1}, {4, 1},{4, 1}, {4, 1}, {4, 1}, {4, 1}}},
	{LC_LINKER_OPTION, sizeof(struct linker_option_command), 1, 3, vlc_noop, {{4, 1}, {4, 1}, {4, 1}}},
	{LC_SYMSEG, sizeof(struct symseg_command), 1, 4, vlc_noop, {{4, 1}, {4, 1}, {4, 1}, {4, 1}}},
	{LC_IDENT, sizeof(struct ident_command), 0, 2, vlc_noop, {{4, 1}, {4, 1}}},
	{LC_FVMFILE, sizeof(struct ident_command), 0, 4, vlc_noop, {{4, 1}, {4, 1}, {sizeof(union lc_str), 1}, {4, 1}}},
	{LC_MAIN, sizeof(struct entry_point_command), 1, 4, vlc_noop, {{4, 1}, {4, 1}, {8, 1}, {8, 1}}},
	{LC_SOURCE_VERSION, sizeof(struct source_version_command), 1, 3, vlc_noop, {{4, 1}, {4, 1}, {8, 1}}}
};

t_validator_error	test_load_command(t_macho_file *file, size_t head,
										t_load_command_descriptor lcd)
{
	t_cursor				rc;
	t_load_command_union	lcu;
	size_t					it;
	size_t					d;

	lcu.data[0] = 0;
	rc = (t_cursor){head, 2, 4, 1, MM_CMD};
	if (read_in_object(file, rc, lcu.data, 1) != VE_OK)
		return (file->err);
	if (lcu.lc.cmd != lcd.id)
		return (VE_INVALID_TYPE);
	if (lcd.is_exact_size && lcd.minimum_size != lcu.lc.cmdsize)
		return (VE_INVALID_COMMAND_SIZE);
	it = (size_t)1;
	d = 8;
	rc = (t_cursor){head + 8, 1, 4, 1, MM_CMD};
	while (++it < lcd.nb_elems)
	{
		rc.block_size = lcd.elems[it].size;
		if (read_in_object(file, rc, &lcu.data[d], lcd.elems[it].swap) != VE_OK)
			return (file->err);
		d += rc.block_size;
		rc.index += rc.block_size;
	}
	return (lcd.vlc(&lcd, lcu, file));
}

t_validator_error	validate_command(t_macho_file *file, uint32_t cmd, size_t *head)
{
	struct load_command	lc;
	size_t				next;
	size_t				it;

	cmd += MM_CMD + 1;
	if (cmd < MM_CMD)
		printf("Warning: too many commands id, less secure mapping detection\n");
	if (read_in_object(file, (t_cursor){*head, sizeof(uint32_t), 2, 1, MM_CMD},
			&lc, 1) != VE_OK)
		return (file->err);
	next = *head + lc.cmdsize;
	if (lc.cmdsize > 4096 || lc.cmdsize >= file->size
			|| next < *head || next < lc.cmdsize)
		return (obj_err(file, VE_INVALID_COMMAND_SIZE));
	printf("reading command of type 0x%X and size %d as %d at %zu\n", lc.cmd, lc.cmdsize, cmd, *head);
	it = (size_t)-1;
	while (++it < 46)
		if (test_load_command(file, *head, g_lcd[it]) == VE_OK)
			break ;
	if (it == 46)
		return (file->err);
	*head = next;
	return (VE_OK);
}

int		main(int argc, char *argv[])
{
	t_macho_file		file;
	uint32_t			cmd;
	size_t				head;

	if (argc < 2)
		return (0);
	if ((file.err = reader(argv[1], &file)) != VE_OK)
		return (file.err);
	if ((validate_magic(&file) != VE_OK)
			|| (file.format == 64 ? validate_head_64(&file)
								: validate_head_32(&file)))
		return (file.err);
	head = file.format == 64 ? sizeof(t_moh64) : sizeof(t_moh32);
	if (claim_map(&file, (t_cursor){head, 1, file.head.sizeofcmds, 1, MM_PAD}, MM_CMD) != VE_OK)
		return (clean_objfile(&file, file.err));
	cmd = (uint32_t)-1;
	while (++cmd < file.head.ncmds)
		if (validate_command(&file, cmd, &head) != VE_OK)
			break ;
	return (clean_objfile(&file, file.err));
}
