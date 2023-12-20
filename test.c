#include <stdio.h>

int main()
{
    unsigned char x[16] = {65, 66, 67, 68, 69, 70, 71, 72, 73,0,0,0,0,0,0,0};
    unsigned long long* y = &x;
    printf("%p\n%p\n%p\n", y, &x[15], &x[0]);
    printf("%llu\n%llu\n", *y, *(y++)); // 4142434445464748, 494A000000000000
}

// 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0