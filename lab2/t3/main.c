// task 4

#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#define writeErr(arg) fwrite(arg, strlen(arg), 1, stderr)

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        writeErr("Incorrect number of parameters");
        return 1;
    }
    FILE *file = fopen(argv[1], "r");
    if (file == NULL)
    {
        writeErr("Incorrect specified path to file");
        return 1;
    }
    int lnPart = 0;
    if (sscanf(argv[2], "%d", &lnPart) != 1)
    {
        writeErr("Second parameter is not a number");
        return 1;
    }

    if (lnPart < 0)
    {
        writeErr("Incorrect specified number");
        return 1;
    }

    int nextChar = 0;
    int lnCount = 0;
    while (1)
    {

        nextChar = fgetc(file);

        if (nextChar == EOF)
            break;

        if (lnPart != 0 && nextChar == '\n' && (++lnCount % lnPart == 0) )
        {
            printf("\nLines %d-%d. Press any key to continue..", lnCount-lnPart, lnCount);
            getchar();
            continue;
        }

        putc(nextChar, stdout);


    }


    if (fclose(file) == EOF)
    {
        writeErr("Failed to close file");
        return 1;

    }



    return 0;
}
