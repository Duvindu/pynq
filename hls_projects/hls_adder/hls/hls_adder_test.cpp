#include "hls_adder_test.h"
#include <stdio.h>

int main()
{
	int a,b,hls_result;

	printf("Test Started\n");

	a = 3; b =5;
	hls_result = hls_add(a,b);
	printf("a : %d, b: %d, out:%d\n", a, b, hls_result);

	a = 1; b =2;
	hls_result = hls_add(a,b);
	printf("a : %d, b: %d, out:%d\n", a, b, hls_result);

	a = -1; b =3;
	hls_result = hls_add(a,b);
	printf("a : %d, b: %d, out:%d\n", a, b, hls_result);

	return 0;
}
