#ifndef _ASP_INPUT_
#define _ASP_INPUT_

#include "../../Utils/aspire_typedefs.h"
#include "../../Utils/aspire_helpers.h"
#include <stdlib.h>
#include <stdio.h>

typedef struct _Asp_Lexer _Asp_Lexer;

struct _Asp_Lexer
{
    acStr_t _file_contents; // the file contents
    aSize_t _file_len;      // file contents length
    char *curr;             // the current character
    aSize_t _data_size;
    aSize_t _inst_size;
    aQptr_t data;
    aQptr_t instrs;
    aSize_t _tpos; // internal field
};

static aSize_t _asp_retrieve_file_size(FILE *file)
{
    register aSize_t len = 0;
    if (fseek(file, SEEK_END, SEEK_CUR) != 0)
    {
        return 0; // this tells the caller that the file is empty when in fact there was problem calculating the size
    }
    len = ftell(file); // get the size
    rewind(file);      // rewind the stream
    return len;
}

aSize_t _asp_get_inst_len(_Asp_Lexer *lexer);

aSize_t _asp_get_data_len(_Asp_Lexer *lexer);

_Asp_Lexer *_asp_read_file(FILE *_file_path, acStr_t _file_contents);

void _asp_read_instructions(_Asp_Lexer *lexer);

void _asp_read_data(_Asp_Lexer *lexer);

#endif