#ifndef _ASP_INP_
#define _ASP_INP_

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
typedef struct _Asp_Input_File _Asp_Input_File;
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

struct _Asp_Input_File
{
    // we don't really care about the filename
    _Asp_Input_File_Attr attributes; // the attributes of the input file
    aSize_t _data_size;              // increased accordingly while reading the data
    aSize_t _inst_size;
    aBptr_t _data_st, _data_ed, _inst_st, _inst_ed;
};

struct _Asp_Lexer
{
    _Asp_Input_File file;
    aSize_t curr;
    char *ptr;
};

_Asp_Input_File *_asp_lex_input_file(acStr_t _file_contents, aSize_t _len);

aBool _asp_read_input_file(acStr_t _file_path, aQptr_t _data, aQptr_t instrs, aSize_t *_data_len, aSize_t *_inst_len);

#endif