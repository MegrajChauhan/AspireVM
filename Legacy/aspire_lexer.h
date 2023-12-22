#ifndef _ASP_LEXER_
#define _ASP_LEXER_

#include "../../Utils/aspire_typedefs.h"
#include "../../Utils/aspire_helpers.h"
#include <stdlib.h>
#include <stdio.h> // for file

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

typedef struct _Asp_Input_File_Attr _Asp_Input_File_Attr;
typedef struct _Asp_Lexer _Asp_Lexer;

struct _Asp_Input_File_Attr
{
    aBool format_binary : 1;               // is the format of the file binary[true by default]
    aBool _data_block_null : 1;            // is the data block null?
    aBool _mem_size_specified : 1;         // is the memory size specified?
    aBool _mem_page_count_specified : 1;   // is the page count specified?
    aBool _data_addr_offset_specified : 1; // is the data address offset specified?
    aBool _data_nomem : 1;                 // does the program want any memory?
};

struct _Asp_Lexer
{
    _Asp_Input_File_Attr attributes;  // the file's attributes
    acStr_t _file_contents;           // the file contents
    aSize_t _file_len;                // file contents length
    char *curr;                       // the current character
    aSize_t _prg_mentioned_data_size; // if the program specifies data memory size then the value will be here
    aSize_t _prg_mentioned_pg_count;  // if the program specifies the number of pages to begin with, it will be here
    aSize_t _data_size;
    aSize_t _inst_size;
    aQptr_t data;
    aQptr_t instrs;
};

#endif