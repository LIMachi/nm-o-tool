#include <stdio.h>

int	main(void)
{
	size_t			sw;
	size_t			size1 = 8;
	size_t			size2 = 4;
	int				endian1 = 4321;
	int				endian2 = 1234;
	unsigned char mem1[8] = {7, 6, 5, 4, 3, 2, 1, 0};
	unsigned char mem2[8];

	for (sw = 0; sw < size1; ++sw)
		printf("%hhu%c", mem1[sw], sw + 1 < size1 ? ' ' : '\n');
	// for (; sw < 8; ++sw)
	// 	mem1[sw] = 42;
	printf("e: %0.4d - s: %zu -> e: %0.4d - s: %zu\n", endian1, size1, endian2, size2);
	for (sw = 0; sw < size2; ++sw)
	{
		if (endian1 == 1234 && endian2 == 4321)
			mem2[sw] = size2 - sw - 1 >= 0 && size2 - sw - 1 < size1 ? mem1[size2 - sw - 1] : 0;
		if (endian1 == 4321 && endian2 == 1234)
			mem2[sw] = size1 - sw - 1 >= 0 && sw + 1 > 0 ? mem1[size1 - sw - 1] : 0;
		printf("%hhu%c", mem2[sw], sw + 1 < size2 ? ' ' : '\n');
	}
}
