//task 3

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <curses.h>
#include <locale.h>


#include <fcntl.h>
#include <unistd.h>


#define STOP_SYMB '\n'

#define ERR_PRM "Incorrect parameter"
#define ERR_OPEN "Cannot open file"
#define ERR_WRITE "Cannot write to file"
#define ERR_CLOSE "Cannot close file"




int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fwrite(ERR_PRM, strlen(ERR_PRM), 1, stderr);
        return -1;
    }

    FILE *file = fopen(argv[1], "w+");
    if (file == NULL)
    {
        fwrite(ERR_OPEN, strlen(ERR_OPEN), 1, stderr);
        return 1;
    }

    char symb = 0;
    while (symb != STOP_SYMB)
    {
        symb = getc(stdin);
        if (fputc(symb, file) == EOF)
        {
            fwrite(ERR_WRITE, strlen(ERR_WRITE), 1, stderr);
            return 2;
        }

    }

    if (fclose(file) != 0)
    {
        fwrite(ERR_CLOSE, strlen(ERR_CLOSE), 1, stderr);
        return 3;
    }

    return 0;
}
