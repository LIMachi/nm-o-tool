#include <elf_validator.h>

t_elf_error	elf_obj_err(t_objfile *file, t_elf_error error)
{
	return (file->err = error);
}

t_elf_error	clean_objfile(t_objfile *file, t_elf_error error)
{
	elf_obj_err(file, error);
	if (file->fd != -1)
	{
		close(file->fd);
		file->fd = -1;
	}
	if (file->file.data != MAP_FAILED)
	{
		munmap(file->file.data, file->size);
		file->file.data = MAP_FAILED;
	}
	if (file->map != MAP_FAILED)
	{
		munmap(file->map, file->size);
		file->map = MAP_FAILED;
	}
	file->size = 0;
	file->head = (t_elf_64_head){.head_size = 0};
	return (error);
}

t_elf_error	reader(const char *path, t_objfile *obj)
{
	struct stat	sb;
	size_t		it;
	int			fd0;

	if (path == NULL || obj == NULL)
		return (EE_NULL_PTR);
	*obj = (t_objfile){-1, 0, MAP_FAILED, MAP_FAILED, {.head_size = 0}, EE_OK};
	if ((fd0 = open("/dev/zero", O_RDWR)) == -1)
		return (clean_objfile(obj, EE_COULD_NOT_OPEN_FILE));
	if ((obj->fd = open(path, O_RDONLY)) == -1)
		return (clean_objfile(obj, EE_COULD_NOT_OPEN_FILE));
	if (fstat(obj->fd, &sb))
		return (clean_objfile(obj, EE_COULD_NOT_RETRIEVE_STATS));
	obj->size = (size_t)sb.st_size;
	if ((obj->file.data = mmap(NULL, obj->size, PROT_READ, MAP_SHARED, obj->fd,
			0)) == MAP_FAILED)
		return (clean_objfile(obj, EE_MAPPING_ERROR));
	if ((obj->map = mmap(NULL, obj->size, PROT_WRITE | PROT_READ, MAP_SHARED,
			fd0, 0)) == MAP_FAILED)
		return (clean_objfile(obj, EE_MAPPING_ERROR));
	it = (size_t)-1;
	while (++it < obj->size)
		obj->map[it] = MM_PAD;
	close(obj->fd);
	close(fd0);
	obj->fd = -1;
	return (EE_OK);
}

/*
** validate the header first part of the header and determine if an endian swap
** will be needed for further readings (the first part of the header is size and
** endianese independent)
*/

t_elf_error	valid_header(t_objfile *obj)
{
	int	endian_test;

	if (obj->size < HEAD_SIZE_32)
		return (elf_obj_err(obj, EE_INVALID_HEAD_SIZE));
	if (obj->file.h32->magic.name[0] != MAGIC_0 || obj->file.h32->magic.name[1] != MAGIC_1
	|| obj->file.h32->magic.name[2] != MAGIC_2 || obj->file.h32->magic.name[3] != MAGIC_3)
		return (elf_obj_err(obj, EE_INVALID_MAGIC_NUMBER));
	if (obj->file.h32->magic.format != FORMAT_32 && obj->file.h32->magic.format != FORMAT_64)
		return (elf_obj_err(obj, EE_INVALID_WORD_SIZE));
	if (obj->file.h32->magic.endian != ENDIAN_LOW && obj->file.h32->magic.endian != ENDIAN_HIGH)
		return (elf_obj_err(obj, EE_INVALID_ENDIAN));
	if (obj->file.h32->magic.version != ELF_VERSION)
		return (elf_obj_err(obj, EE_INVALID_ELF_VERSION));
	if (obj->file.h32->magic.os_abi == 5 || obj->file.h32->magic.os_abi > 0x11)
		return (elf_obj_err(obj, EE_INVALID_OS_ABI));
	endian_test = 1;
	obj->endian = (*(char*)&endian_test != (obj->file.h32->magic.endian == ENDIAN_LOW));
	obj->format = obj->file.h32->magic.format == FORMAT_32 ? 32 : 64;
	if (obj->file.h32->magic.format == FORMAT_64 && obj->size < 64)
		return (elf_obj_err(obj, EE_INVALID_HEAD_SIZE));
	return (EE_OK);
}

uint16_t	swap_16(uint16_t v)
{
	t_uint16	t;

	t.i = v;
	return (((t_uint16){.b = {t.b[1], t.b[0]}}).i);
}

uint32_t	swap_32(uint32_t v)
{
	t_uint32	t;

	t.i = v;
	return (((t_uint32){.b = {t.b[3], t.b[2], t.b[1], t.b[0]}}).i);
}

uint64_t	swap_64(uint64_t v)
{
	t_uint64	t;

	t.i = v;
	return (((t_uint64){.b = {t.b[7], t.b[6], t.b[5], t.b[4], t.b[3], t.b[2],
		t.b[1], t.b[0]}}).i);
}

t_elf_64_head	read_head64(t_objfile *obj)
{
	return (*obj->file.h64);
}

t_elf_64_head	read_head64_swap(t_objfile *obj)
{
	t_elf_64_head	head;

	head = *obj->file.h64;
	return ((t_elf_64_head){
		.type = swap_16(head.type),
		.machine = swap_16(head.machine),
		.version = swap_32(head.version),
		.entry = swap_64(head.entry),
		.ht_off = swap_64(head.ht_off),
		.st_off = swap_64(head.st_off),
		.flags = swap_32(head.flags),
		.head_size = swap_16(head.head_size),
		.phe_size = swap_16(head.phe_size),
		.phe_num = swap_16(head.phe_num),
		.she_size = swap_16(head.she_size),
		.she_num = swap_16(head.she_num),
		.inames = swap_16(head.inames)});
}

t_elf_64_head	read_head32_swap(t_objfile *obj)
{
	t_elf_32_head	head;

	head = *obj->file.h32;
	return ((t_elf_64_head){
		.type = swap_16(head.type),
		.machine = swap_16(head.machine),
		.version = swap_32(head.version),
		.entry = swap_64((uint64_t)head.entry),
		.ht_off = swap_64((uint64_t)head.ht_off),
		.st_off = swap_64((uint64_t)head.st_off),
		.flags = swap_32(head.flags),
		.head_size = swap_16(head.head_size),
		.phe_size = swap_16(head.phe_size),
		.phe_num = swap_16(head.phe_num),
		.she_size = swap_16(head.she_size),
		.she_num = swap_16(head.she_num),
		.inames = swap_16(head.inames)});
}

t_elf_64_head	read_head32(t_objfile *obj)
{
	t_elf_32_head	head;

	head = *obj->file.h32;
	return ((t_elf_64_head){
		.type = head.type,
		.machine = head.machine,
		.version = head.version,
		.entry = (uint64_t)head.entry,
		.ht_off = (uint64_t)head.ht_off,
		.st_off = (uint64_t)head.st_off,
		.flags = head.flags,
		.head_size = head.head_size,
		.phe_size = head.phe_size,
		.phe_num = head.phe_num,
		.she_size = head.she_size,
		.she_num = head.she_num,
		.inames = head.inames});
}

int		in(const int val, size_t cnt, const int test[])
{
	while (cnt--)
		if (val == test[cnt])
			return (1);
	return (0);
}

t_elf_error	valid_head_0(t_objfile *obj)
{
	if (!in((int)obj->head.type, 9,
		(int[9]){0, 1, 2, 3, 4, 0xFE00, 0xFEFF, 0xFF00, 0xFFFF}))
		return (elf_obj_err(obj, EE_INVALID_TYPE));
	if (!in((int)obj->head.machine, 12,
		(int[12]){0, 2, 3, 8, 0x14, 0x16, 0x28, 0x2A, 0x32, 0x3E, 0xB7, 0xF3}))
		return (elf_obj_err(obj, EE_INVALID_MACHINE));
	if (obj->head.version != ELF_VERSION)
		return (elf_obj_err(obj, EE_INVALID_ELF_VERSION));
	if (obj->head.ht_off >= obj->size || obj->map[obj->head.ht_off] != MM_PAD)
		return (elf_obj_err(obj, EE_INVALID_SEGMENT_TABLE_POSITION));
	obj->segments.p = obj->file.data + obj->head.ht_off;
	if (obj->head.st_off >= obj->size || obj->map[obj->head.st_off] != MM_PAD)
		return (elf_obj_err(obj, EE_INVALID_SECTION_TABLE_POSITION));
	obj->sections.p = obj->file.data + obj->head.st_off;
	if (!((obj->head.head_size == 64 && obj->format == 64)
			|| (obj->head.head_size == 52 && obj->format == 32)))
		return (elf_obj_err(obj, EE_INVALID_HEAD_SIZE));
	if (!((obj->head.phe_size == SEGMENT_SIZE_64 && obj->format == 64)
			|| (obj->head.phe_size == SEGMENT_SIZE_32 && obj->format == 32)))
		return (elf_obj_err(obj, EE_INVALID_SEGMENT_HEADER_SIZE));
	if (!((obj->head.she_size == SECTION_SIZE_64 && obj->format == 64)
			|| (obj->head.she_size == SECTION_SIZE_32 && obj->format == 32)))
		return (elf_obj_err(obj, EE_INVALID_SECTION_HEADER_SIZE));
	return (EE_OK);
}

t_elf_error	valid_head(t_objfile *obj)
{
	size_t	it;
	size_t	s;

	if (valid_head_0(obj) != EE_OK)
		return (obj->err);
	it = obj->head.ht_off - 1;
	s = obj->head.ht_off + obj->head.phe_size * obj->head.phe_num;
	if (s > obj->size)
		return (elf_obj_err(obj, EE_INVALID_SEGMENT_COUNT));
	while (++it < s)
		if (obj->map[it] == MM_PAD)
			obj->map[it] = MH_PH;
		else
			return (elf_obj_err(obj, EE_FILE_MAPPING_CONFLICT));
	it = obj->head.st_off - 1;
	s = obj->head.st_off + obj->head.she_size * obj->head.she_num;
	if (s > obj->size)
		return (elf_obj_err(obj, EE_INVALID_SECTION_COUNT));
	while (++it < s)
		if (obj->map[it] == MM_PAD)
			obj->map[it] = MH_SH;
		else
			return (elf_obj_err(obj, EE_FILE_MAPPING_CONFLICT));
	return (obj->err);
}

t_elf_error	valid_elf_head(t_objfile *obj)
{
	size_t	it;

	if (valid_header(obj) != EE_OK)
		return (obj->err);
	if (obj->format == 64)
		if (obj->endian)
			obj->head = read_head64_swap(obj);
		else
			obj->head = read_head64(obj);
	else
		if (obj->endian)
			obj->head = read_head32_swap(obj);
		else
			obj->head = read_head32(obj);
	it = (size_t)-1;
	while (++it < (obj->format == 64 ? HEAD_SIZE_64 : HEAD_SIZE_32))
		obj->map[it] = MM_HEADER;
	return (valid_head(obj));
}

t_elf_error	valid_elf_headers(t_objfile *obj)
{
	(void)obj;
	return (EE_OK);
}

int			main()
{
	t_objfile	file;

	file = (t_objfile){.err = EE_OK, .fd = -1, .map = MAP_FAILED,
		.file = MAP_FAILED};
	if ((file.err = reader("a.out", &file)) != EE_OK)
		return (file.err);
	if (valid_elf_head(&file) != EE_OK)
		return (file.err);
	if (valid_elf_headers(&file) != EE_OK)
		return (file.err);
	clean_objfile(&file, EE_OK);
	return (0);
}
