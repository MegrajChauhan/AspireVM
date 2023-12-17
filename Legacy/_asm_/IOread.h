#ifndef _ASP_IOREAD_
#define _ASP_IOREAD_

#include "../../Utils/aspire_typedefs.h"

// defined in assembly
extern int _asp_read_byte(aBptr_t buffer);
extern int _asp_read_bytes(aBptr_t buffer);
extern int _asp_read_until(aBptr_t buffer, char until);
extern void _asp_set_buf_limit(aSize_t lim);

#endif