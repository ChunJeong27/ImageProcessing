#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void) {
	srand((unsigned int)time(NULL));	// seed값으로 현재시간 부여
	printf("난수 : %d\n", rand());
	printf("난수 : %d\n", rand());
	printf("난수 : %d\n", rand());
	printf("난수 : %d\n", rand());
	printf("난수 : %d\n", rand());
	return 0;
}

