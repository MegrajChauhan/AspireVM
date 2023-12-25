#include <stdio.h>
#include "../Manager/input/aspire_read_input.h"

int main()
{
    _Asp_Lexer *lexer = _asp_read_file("program.txt");
    printf("File contents: \n%s\n", lexer->_file_contents);
    printf("Data len: %lu\nInstruction len: %lu\n", lexer->_data_size, lexer->_inst_size);
    for (int i = 0; i < lexer->_data_size; i++)
    {
        printf("data: %lu\n", lexer->data[i]);
    }
    for (int i = 0; i < lexer->_inst_size; i++)
    {
        printf("inst: %lu\n", lexer->instrs[i]);
    }
    free(lexer->data);
    free(lexer->_file_contents);
    free(lexer->instrs);
    free(lexer);
}