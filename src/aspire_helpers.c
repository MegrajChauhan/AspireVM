#include "../Utils/aspire_helpers.h"

aStr_t _asp_concat(acStr_t str1, acStr_t str2)
{
    aSize_t total_size = strlen(str1) + strlen(str2);
    aStr_t result = (aStr_t)malloc(total_size);

    if (result != (void*)0)
    {
        strcpy(result, str1);
        strcat(result, str2);
    }
    else
    {
        return (aStr_t)str1;
    }
    return result;
}