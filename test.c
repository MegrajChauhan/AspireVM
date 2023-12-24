#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int main()
{
    char x[21];
    FILE * f = fopen("test.txt", "r");
    fread(x, 1, 21, f);
    printf("%s %zu\n", x, sizeof(x));
    char *p = x;
    p += 12;
    printf("%lu\n", p - x);
    p -= 12;
    while (*p != '\0')
    {
        printf("%c", *p);
        p++;
    }
    char y[6] = {x[6], x[7], x[8], x[9], x[10], x[11]};
    printf("\n%s\n", y);
    char *q = NULL;
    printf("%llu\n", strtoull(y, &q, 10));
}