#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <dirent.h>


int readDir(const char* dirpath)
{
    DIR *dir = opendir( dirpath );
    if ( dir == NULL )
    {
        perror( "Failed to open directory" );
        return 1;
    }

    struct dirent *entry = NULL;
    printf( "Items of directory <%s> :\n", dirpath );
    while (  ( entry = readdir( dir ) ) != NULL )
    {
        printf( "%s\n", entry->d_name );
    }


    if ( closedir( dir ) != 0 )
    {
        perror( "Failed to close directory" );
        return 1;
    }
    return 0;
}

int main(int argc, char *argv[])
{

    if ( readDir( "/" ) != 0 )
    {
        perror( "Cannot read root directory" );
        return 1;
    }

    char *buf = (char*) calloc( sizeof(char), 256 );
    buf = getcwd( buf, 256 );

    puts( "" );

    if ( readDir( buf ) != 0 )
    {
        perror( "Cannot read working directory" );
        return 1;
    }

    free( buf );
    return 0;
}
