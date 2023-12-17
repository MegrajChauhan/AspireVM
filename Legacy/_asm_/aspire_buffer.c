#include "../../Manager/aspire_buffer.h"

_Asp_Buffer *_asp_buffer_init(aSize_t buffer_size)
{
    _Asp_Buffer *nbuf = (_Asp_Buffer *)malloc(sizeof(_Asp_Buffer));
    if (nbuf == NULL)
        return NULL;
    nbuf->node_count = buffer_size;
    nbuf->head = (_Asp_Buffer_Node *)malloc(sizeof(_Asp_Buffer_Node));
    if (nbuf->head == NULL)
    {
        free(nbuf);
        return NULL;
    }
    _Asp_Buffer_Node *current = (nbuf)->head;
    for (aSize_t i = 0; i < buffer_size; i++)
    {
        current->next = (_Asp_Buffer_Node *)malloc(sizeof(_Asp_Buffer_Node));
        if (current->next == NULL)
        {
            _asp_buffer_destroy(nbuf);
            return NULL;
        }
        current = current->next;
    }
    current->next = (nbuf)->head;
    (nbuf->tail) = (nbuf)->head; /*The queue is empty*/
    return nbuf;
}

aBool _asp_buffer_write(_Asp_Buffer *buf, aByte _to_write)
{
    if (_ASP_IS_BUFFER_FULL(buf))
        return aFalse;
    (buf)->tail->value = (_to_write);
    (buf)->data_count++;
    (buf)->tail = (buf)->tail->next;
    return aTrue;
}

aBool _asp_buffer_read(_Asp_Buffer *buf, aBptr_t *_store_in)
{
    if (_ASP_IS_BUFFER_EMPTY(buf))
        return aFalse;
    *_store_in = (buf)->head->value;
    (buf)->head = (buf)->head->next;
    (buf)->data_count--;
    return aTrue;
}

void _asp_buffer_destroy(_Asp_Buffer *buffer)
{
    if (buffer == NULL)
        return;
    if (buffer->head == NULL)
        return;
    _Asp_Buffer_Node *current = (buffer)->head;
    _Asp_Buffer_Node *next;
    for (aSize_t i = 0; i < (buffer)->node_count; i++)
    {
        next = current->next;
        free(current);
        current = next;
    }
    (buffer)->head = NULL;
    (buffer)->tail = NULL;
    (buffer)->node_count = 0;
    free(buffer);
}