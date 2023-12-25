#include <stdio.h>
#include <stdlib.h>

#define R 131072

int main()
{
    unsigned long long **p = (unsigned long long**)malloc(8 * 10);
    if (p == NULL)
      printf("Error2\n");
    int *x = (int*)malloc(R * 8);
    if (x == NULL)
      printf("Error\n");
    int *y = (int*)malloc(R * 8);
    if (y == NULL)
      printf("Error3\n");
    int *z = (int*)malloc(R * 8);
    if (z == NULL)
      printf("Error4\n");
    int *q = (int*)malloc(R * 8);
    if (q == NULL)
      printf("Error5\n");
    
    if (x != NULL) free(x);
    if (y != NULL) free(y);
    if (z != NULL) free(z);
    if (q != NULL) free(q);
    if (p != NULL) free(p);
    printf("Success\n");
}