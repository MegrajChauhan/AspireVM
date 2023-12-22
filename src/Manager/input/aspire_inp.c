#include "../../../Manager/input/aspire_inp.h"

aBool _asp_read_input_file(acStr_t _file_path, aQptr_t _data, aQptr_t instrs, aSize_t *_data_len, aSize_t *_inst_len)
{
    // we read from _file_path
    // put data in _data
    // put instructions in instrs
    FILE *_inp = fopen(_file_path, "r");
    if (_inp == NULL)
    {
        fprintf(stderr, "Read error: The given file %s either doesn't exist or is a directory.\n", _file_path);
        return aFalse;
    }
    register aSize_t len = _asp_retrieve_file_size(_inp);
    if (len == 0)
    {
        fprintf(stderr, "Read error: The file %s is an empty file.\n", _file_path); // print the error
        return aFalse;                                                              // file is empty
    }
    register char file_contents[len];
    if (fread(file_contents, 1, len, _inp) < len)
    {
        fprintf(stderr, "Read error: Couldn't read the input file.\n");
        return aFalse; // couldn't read the file
    }
}

_Asp_Input_File *_asp_lex_input_file(acStr_t _file_contents, aSize_t _len)
{
    register _Asp_Input_File *inp = (_Asp_Input_File *)malloc(sizeof(_Asp_Input_File));
    if (inp == NULL)
        return NULL;
    inp->attributes.format_binary = aTrue; // binary by default
    register aSize_t curr = 0;
    register char *ptr = _file_contents;
    while (curr < _len)
    {
        if (*ptr == ' ' || *ptr == '\n' || *ptr == '\t')
        {
            // ignore all whitespaces
            while (curr < _len && (*ptr == ' ' || *ptr == '\n' || *ptr == '\t'))
            {
                ptr++;
                curr++;
            }
        }
        else
        {
            // everything else is of interest here
            if (*ptr == '.')
            {
                // an attribute
                // we have to figure out what it is
                char temp[15]; // at most the attributes are only 
            }
        }
    }
}