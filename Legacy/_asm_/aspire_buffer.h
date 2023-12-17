#ifndef _ASP_BUFFER_
#define _ASP_BUFFER_

#include "../Utils/aspire_typedefs.h"
#include <stdlib.h>

#define _ASP_IS_BUFFER_EMPTY(qptr) (qptr->data_count == 0)
#define _ASP_IS_BUFFER_FULL(qptr) (qptr->data_count == qptr->node_count)

typedef struct _Asp_Buffer_Node _Asp_Buffer_Node;
typedef struct _Asp_Buffer _Asp_Buffer;

struct _Asp_Buffer_Node
{
    aByte value;
    _Asp_Buffer_Node *next;
};

struct _Asp_Buffer
{
    aSize_t node_count;     /* The queue is of fixed size.Once initialized with a size, the size cannot be changed. */
    aSize_t data_count;     /* The number of data pushed to the queue so far*/
    _Asp_Buffer_Node *head; /*The head of the queue's linked list.*/
    _Asp_Buffer_Node *tail; /*The tail of the queue's linked list*/
};

_Asp_Buffer *_asp_buffer_init(aSize_t buffer_size);

aBool _asp_buffer_write(_Asp_Buffer *buf, aByte _to_write);

aBool _asp_buffer_read(_Asp_Buffer *buf, aBptr_t *_store_in);

void _asp_buffer_destroy(_Asp_Buffer *buffer);

#endif