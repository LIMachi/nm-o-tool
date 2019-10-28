#include <stdio.h>

struct st{
	size_t			size;
	int				endian;
	int				sign;
};

void	conv(unsigned char *mem1, struct st s1,
			unsigned char *mem2, struct st s2)
{
	unsigned char	s;
	unsigned char	*signer;
	size_t			sw;

	signer = &mem1[s1.endian == 1234 ? s1.size - 1 : 0];
	s = (s1.sign && *signer & 0x80) * 0xFF;
	if (s)
		*signer &= 0x7F;
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
	if (s && (*signer |= 0x80))
		mem2[s2.endian == 1234 ? s2.size - 1 : 0] |= 0x80;
}

int	main(void)
{
	struct st		st1 = {8, 4321, 1};
	struct st		st2 = {4, 1234, 1};
	size_t			sw;
	unsigned char	mem1[8] = {0x80, 6, 5, 4, 3, 2, 1, 0};
	unsigned char	mem2[8];

	for (sw = 0; sw < st1.size; ++sw)
		printf("%hhu%c", mem1[sw], sw + 1 < st1.size ? ' ' : '\n');
	printf("e: %.4d - s: %zu -> e: %.4d - s: %zu\n", st1.endian, st1.size, st2.endian, st2.size);
	conv(mem1, st1, mem2, st2);
	conv(mem2, st2, mem1, st1);
	for (sw = 0; sw < st1.size; ++sw)
		printf("%hhu%c", mem1[sw], sw + 1 < st1.size ? ' ' : '\n');
}
