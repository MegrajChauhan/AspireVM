#include "../../../Utils/aspire_typedefs.h"
#include <stdio.h>

extern int _asp_read_byte(aBptr_t buffer);
extern int _asp_read_bytes(aBptr_t buffer);
extern int _asp_read_until(aBptr_t buffer, char until);
extern void _asp_set_buf_limit(aSize_t lim);

int main()
{
   _asp_set_buf_limit(2);
   aByte inp[2];
   _asp_read_byte(inp);
   _asp_read_until(inp, '\n');
}