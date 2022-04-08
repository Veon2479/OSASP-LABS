// task 5

#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#include <sys/stat.h>
#include <unistd.h>


#define writeErr(arg) fwrite(arg, strlen(arg), 1, stderr)

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        writeErr("Incorrect number of parameters");
        return 1;
    }

    FILE *src, *dest;
    if ( ( src = fopen(argv[1], "r") ) == NULL )
    {
        writeErr("Failed to open source file");
        return 1;
    }

    struct stat fileStat = { 0 };
    if ( stat(argv[1], &fileStat) != 0)
    {
        writeErr("Failed to read file info");
        return 1;
    }


    char *buffer = calloc(fileStat.st_size, sizeof(char));
    char *point = buffer;

    for (int i = 0; i < fileStat.st_size; i++)
    {
        *point = fgetc(src);
        if (*point == EOF)      //catching error, not end of file
        {
            writeErr("Failed read file to buffer");
            fclose(src);
            return 1;
        }
        point++;
    }

    if (fclose(src) != 0)
    {
        writeErr( "Failed while closing file" );
        return 1;
    }

    if ( ( dest = fopen( argv[2], "w" ) ) == NULL )
    {
        writeErr( "Failed to open destination file" );
        return 1;
    }

    point = buffer;
    for ( int i = 0; i < fileStat.st_size; i++ )
    {
        if ( fputc( *point, dest ) == EOF)      //catching error, not end of file
        {
            writeErr( "Failed write buffer to file" );
            fclose( dest );
            return 1;
        }
        point++;
    }


    if ( fclose( dest ) != 0 )
    {
        writeErr( "Failed while closing destination file" );
        return 1;
    }

        //set stats////
//     uid_t     st_uid;         /* User ID of owner */
//     gid_t     st_gid;         /* Group ID of owner */
//     mode_t    st_mode          access permisions

    if ( chown( argv[2], fileStat.st_uid, fileStat.st_gid ) != 0 && chmod( argv[2], fileStat.st_mode ) != 0 )
    {
        writeErr( "Failed to set permissions" );
        return 1;
    }


    return 0;
}
