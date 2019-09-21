#include <mach_o_validator.h>

#include <stdio.h>
#include <stdlib.h>

t_validator_error	obj_err(t_macho_file *file, t_validator_error error)
{
	return (file->err = error);
}

t_validator_error	clean_objfile(t_macho_file *file, t_validator_error error)
{
	obj_err(file, error);
	if (file->fd != -1 && file->file.data != MAP_FAILED)
	{
		munmap(file->file.data, file->size);
		file->file.data = MAP_FAILED;
	}
	if (file->fd != -1 && file->map != MAP_FAILED)
	{
		munmap(file->map, file->size);
		file->map = MAP_FAILED;
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
	// int			fd0;

	if (path == NULL || obj == NULL)
		return (VE_NULL_PTR);
	*obj = (t_macho_file){.fd = -1, .file = {MAP_FAILED}, .map = MAP_FAILED};
	// if ((fd0 = open("/dev/zero", O_RDWR)) == -1)
	// 	return (clean_objfile(obj, VE_COULD_NOT_OPEN_FILE));
	if ((obj->fd = open(path, O_RDONLY)) == -1)
		return (clean_objfile(obj, VE_COULD_NOT_OPEN_FILE));
	if (fstat(obj->fd, &sb))
		return (clean_objfile(obj, VE_COULD_NOT_RETRIEVE_STATS));
	obj->size = (size_t)sb.st_size;
	if ((obj->file.data = mmap(NULL, obj->size, PROT_READ, MAP_PRIVATE, obj->fd,
								0)) == MAP_FAILED)
		return (clean_objfile(obj, VE_MAPPING_ERROR));
	if ((obj->map = mmap(NULL, obj->size, PROT_WRITE | PROT_READ,
			MAP_PRIVATE | MAP_ANON, -1, 0)) == MAP_FAILED)
		return (clean_objfile(obj, VE_MAPPING_ERROR));
	it = (size_t)-1;
	while (++it < obj->size)
		obj->map[it] = MM_PAD;
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
			if (obj->map[rc.index + it * align + sw] != rc.expected_mapping)
				return (VE_INVALID_MAPING);
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
			if (obj->map[rc.index + align * it + sw] != MM_PAD)
				return (obj_err(obj, VE_INVALID_CLAIM));
			else
				obj->map[rc.index + align * it + sw] = claim;
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
				obj->map[rc.index + align * it + sw] = MM_PAD;
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
		|| read_in_object(obj, (t_cursor){__offsetof(t_moh32, filetype), 1,
		sizeof(uint32_t), 4, MM_HEADER}, &obj->head.filetype, 1))
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
			|| read_in_object(obj, (t_cursor){__offsetof(t_moh64, filetype), 1,
			sizeof(uint32_t), 5, MM_HEADER}, &obj->head.filetype, 1))
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

int		main(int argc, char *argv[])
{
	t_macho_file		file;
	t_validator_error	err;

	if (argc < 2)
		return (0);
	if ((err = reader(argv[1], &file)) != VE_OK)
		return (err);
	if ((validate_magic(&file) != VE_OK)
			|| (file.format == 64 ? validate_head_64(&file)
								: validate_head_32(&file)))
		return (err);
	return (clean_objfile(&file, file.err));
}
