#include "../../../Manager/input/aspire_read_input.h"

_Asp_Lexer *_asp_read_file(FILE *_file_path, acStr_t _file_contents)
{
    _Asp_Lexer *file = (_Asp_Lexer *)malloc(sizeof(_Asp_Lexer));
    if (file == NULL)
        return NULL;
    FILE *_inp = fopen(_file_path, "r");
    if (_inp == NULL)
    {
        fprintf(stderr, "Read error: The given file %s either doesn't exist or is a directory.\n", _file_path);
        free(file);
        return NULL;
    }
    register aSize_t len = _asp_retrieve_file_size(_inp);
    if (len == 0)
    {
        fprintf(stderr, "Read error: The file %s is an empty file.\n", _file_path); // print the error
        free(file);
        return NULL; // file is empty
    }
    register char file_contents[len + 1];
    if (fread(file_contents, 1, len, _inp) < len)
    {
        fprintf(stderr, "Read error: Couldn't read the input file.\n");
        free(file);
        return NULL; // couldn't read the file
    }
    file->_file_contents = file_contents;
    file->curr = file->_file_contents;
    file->_file_len = len;
    register aSize_t _dlen = 0;
    register aSize_t _ilen = 0;
    if ((_ilen = _asp_get_inst_len(file)) == 0)
    {
        free(file);
        return NULL;
    }
    register aBool _is_data_present = aTrue;
    if ((_dlen = _asp_get_data_len(file)) == 0)
        _is_data_present = aFalse; // there is no data present
    else if (_dlen == file->_file_len)
    {
        // this is an error
        free(file);
        return NULL;
    }
    // now we have almost everything we need
    file->_inst_size = _ilen / 8 + (_ilen % 8 == 0) ? 1 : 0;
    file->_data_size = (_is_data_present == aTrue) ? (_dlen / 8 + (_dlen % 8 == 0) ? 1 : 0) : 0;
    // now we know how many qwords there are
    file->instrs = (aQptr_t)malloc(8 * file->_inst_size);
    if (file->instrs == NULL)
    {
        fprintf(stderr, "Read Internal error: Unable to read instructions.\n");
        free(file);
        return NULL;
    }
    if (_is_data_present == aTrue)
    {
        file->data = (aQptr_t)malloc(8 * file->_data_size);
        if (file->data == NULL)
        {
            fprintf(stderr, "Read Internal error: Unable to read data.\n");
            free(file->instrs);
            free(file);
            return NULL;
        }
    }
    else
        file->data = NULL; // we have nothing to do here
    _asp_read_instructions(file);
    if (_is_data_present == aTrue)
        _asp_read_data(file);
    return file;
}

aSize_t _asp_get_inst_len(_Asp_Lexer *lexer)
{
    // if some error were to be encountered, we return 0
    register aSize_t i = 0;
    register char *temp = lexer->curr;
    register aBool encountered = aFalse;
    while (*temp != '\0')
    {
        if (*temp == '0' || *temp == '1')
            i++;
        else
        {
            fprintf(stderr, "Read error: Unknown character %c in the input file.\n", *temp);
            return 0;
        }
        *temp++;
        if (*temp == ';')
        {
            encountered = aTrue;
            lexer->_tpos = _ASP_PTR_DISTANCE(temp, lexer->curr);
            break;
        }
    }
    if (encountered == aFalse)
    {
        fprintf(stderr, "Read error: The input file doesn't adhere to input file format.\n");
        return 0;
    }
    return i;
}

aSize_t _asp_get_data_len(_Asp_Lexer *lexer)
{
    // inst len must be called before this
    register aSize_t i = 0;
    register char *temp = lexer->curr + lexer->_tpos;
    while (*temp != '\0')
    {
        if (*temp == '0' || *temp == '1')
            i++;
        else
        {
            fprintf(stderr, "Read error: Unknown character %c in the input file.\n", *temp);
            return lexer->_file_len; // this indicates error
        }
        *temp++;
    }
    return i; // here 0 wouldn't mean error it means we do not have any data
}

void _asp_read_instructions(_Asp_Lexer *lexer)
{
    // this will extract the instructions
    register char inst[64]; // since we read 64 characters in one big swoop
    register aSize_t pos = 0;
    register aSize_t temp = 0;
    register aSize_t count = 0;
    while (pos < lexer->_tpos)
    {
        while (pos < lexer->_tpos && temp < 64)
        {
            if (*(lexer->curr) == ' ' || *(lexer->curr) == '\n' || *(lexer->curr) == '\t')
            {
            }
            else
            {
                // this must be either 0 or a 1
                inst[temp] = *(lexer->curr);
                temp++;
            }
            pos++;
            lexer->curr++;
        }
        if (temp == 63)
            temp = 0; // this will help in identifying incomplete qwords
        if (temp == 0)
        {
            char *x;
            lexer->instrs[count] = strtoull((const char *)inst, &x, 2);
            // the above shouldn't produce an error
            count++;
        }
        else
        {
            // we have reached the end
            // this happened because the input file didn't have an even number of bytes
            // say, the file had "1011111001010001100111110011110011100010111100000011110100010101101111100101000110011111001111001110001011110000"
            // here, we can group these bits to 1 complete qword but there are 16 bits less for the next qword to be grouped and hence we reach this point
            for (; temp < 64; temp++)
            {
                inst[temp] = '0'; // we have to simply interpret the missing bits as 0
            }
            // then the same as above
            char *x;
            lexer->instrs[count] = strtoull((const char *)inst, &x, 2);
            // the above shouldn't produce an error
            count++;
        }
    }
    lexer->curr++; // point past ';'
    // we have read everything successfully
}

void _asp_read_data(_Asp_Lexer *lexer)
{
    // this will extract the data
    register char data[64]; // since we read 64 characters in one big swoop
    register aSize_t pos = lexer->_tpos + 1;
    register aSize_t temp = 0;
    register aSize_t count = 0;
    while (pos < lexer->_file_len)
    {
        while (pos < lexer->_file_len && temp < 64)
        {
            if (*(lexer->curr) == ' ' || *(lexer->curr) == '\n' || *(lexer->curr) == '\t')
            {
            }
            else
            {
                // this must be either 0 or a 1
                data[temp] = *(lexer->curr);
                temp++;
            }
            pos++;
            lexer->curr++;
        }
        if (temp == 63)
            temp = 0; // this will help in identifying incomplete qwords
        if (temp == 0)
        {
            char *x;
            lexer->data[count] = strtoull((const char *)data, &x, 2);
            // the above shouldn't produce an error
            count++;
        }
        else
        {
            // we have reached the end
            // this happened because the input file didn't have an even number of bytes
            // say, the file had "1011111001010001100111110011110011100010111100000011110100010101101111100101000110011111001111001110001011110000"
            // here, we can group these bits to 1 complete qword but there are 16 bits less for the next qword to be grouped and hence we reach this point
            for (; temp < 64; temp++)
            {
                data[temp] = '0'; // we have to simply interpret the missing bits as 0
            }
            // then the same as above
            char *x;
            lexer->data[count] = strtoull((const char *)data, &x, 2);
            // the above shouldn't produce an error
            count++;
        }
    }
}