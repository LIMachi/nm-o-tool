#include <stdio.h>
#include <unistd.h>
#include <stdint.h>

#include <error.h>
#include <memory.h>
#include <vm.h>

struct st{
	size_t			size;

	int				endian;
	int				sign;
};

/*
** convert an integer memory to another (conversion must change the size in
** bytes and may change the signedness or the endianese of the integer)
** this function is not protected against invalid addresses and values of
** description structures
*/

void	conv(unsigned char *mem1, struct st s1,
			unsigned char *mem2, struct st s2)
{
	unsigned char	s;
	size_t			sw;

	s = (s1.sign && (mem1[s1.endian == 1234 ? s1.size - 1 : 0] & 0x80)) * 0xFF;
	sw = (size_t)-1;
	if (s1.endian == 1234 && s2.endian == 4321)
		while (++sw < s2.size)
			mem2[sw] = s2.size - sw - 1 < s1.size ? mem1[s2.size - sw - 1] : s;
	if (s1.endian == 4321 && s2.endian == 1234)
		while (++sw < s2.size)
			mem2[sw] = s1.size >= sw + 1 ? mem1[s1.size - sw - 1] : s;
	if (s1.endian == 1234 && s2.endian == 1234)
		while (++sw < s2.size)
			mem2[sw] = sw < s1.size ? mem1[sw] : s;
	if (s1.endian == 4321 && s2.endian == 4321)
		while (++sw < s2.size)
			mem2[sw] = sw + s1.size - s2.size < s1.size
				? mem1[sw + s1.size - s2.size] : s;
}

int	main(void)
{
	{
		struct st     st1     = {4, 4321, 1};
		struct st     st2     = {2, 1234, 1};
		size_t        sw;
		unsigned char mem1[8] = {0x80, 6, 5, 4, 3, 2, 1, 0};
		unsigned char mem2[8];
		for (sw = 0; sw < st1.size; ++sw)
			printf("%hhu%c", mem1[sw], sw + 1 < st1.size ? ' ' : '\n');
		printf("e: %.4d - s: %zu (0x%llX) -> e: %.4d - s: %zu\n", st1.endian, st1.size, *(long long unsigned *)mem1,
			   st2.endian, st2.size);
		conv(mem1, st1, mem2, st2);
		printf("(0x%llX)\n", *(long long unsigned *)mem2);
		for (sw = 0; sw < st2.size; ++sw)
			printf("%hhu%c", mem2[sw], sw + 1 < st2.size ? ' ' : '\n');
		int16_t  i16;
		uint32_t u32;
		i16 = -42;
		u32 = i16;
		printf("test: %hd %hx %u %x %d %x %hd %hx %d %x\n\n", i16, i16, u32, u32, (int32_t)(uint16_t)-42,
			   (int32_t)(uint16_t)-42, (uint16_t)(int32_t)0xf8423964, (uint16_t)(int32_t)0xf8423964, 0xf8423964,
			   0xf8423964);
	}
	{
		unsigned char mem1[8] = {0x80, 6, 5, 4, 3, 2, 1, 0};
		unsigned char mem2[8];
		t_cast_memory_descriptor	cmd = {.out = {8, 1, 1, 1, 1}, .in = {4, 1, 1, 0, 1}};
		size_t        sw;
		for (sw = 0; sw < cmd.in.block_size; ++sw)
			printf("%hhu%c", mem1[sw], sw + 1 < cmd.in.block_size ? ' ' : '\n');
		cast_memory((t_memory_map){8, 0, mem2}, (t_memory_map){8, 0, mem1}, cmd);
		for (sw = 0; sw < 8; ++sw)
			printf("%hhu%c", mem2[sw], sw + 1 < 8 ? ' ' : '\n');
	}
}
