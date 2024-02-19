#include <stdio.h>

#define SET_BIT(n, b) 	n |= (1<<b)

int main(void)
{
	int n = 3;
	int b = 0;
	printf("orig: %d\n", n);

	// set a bit
	SET_BIT(n, b);
	printf("set %d bit: %d\n", b, n);

	// clear a bit
	n &= ~(1<<b);
	printf("clear %d bit: %d\n", b, n);

	// toggle a bit
	n ^= (1<<b);
	printf("toggle %d bit: %d\n", b, n);
	printf("check %d bit: %d\n", b, n & (1<<b));
	n ^= (1<<b);
	printf("toggle %d bit: %d\n", b, n);
	printf("check %d bit: %d\n", b, n & (1<<b));


	return 0;
}
