#include <stdio.h>
#include <stdlib.h>

#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#define list struct list
#define dirent struct dirent

#define writeErr(arg) fwrite(arg, strlen(arg), 1, stderr)


list
{
    list *next;
    char *path;
    int size;
};

list* getItem( const char* const path, const int size )
{
    list *res = (list*) calloc( sizeof(list), 1 );

    res->next = NULL;
    res->size = size;
    res->path = calloc( sizeof(char), strlen(path) + 1);
    strcpy( res->path, path );

    printf("LOG: new item: ( %d, %s )\n", size, path);

    return res;
}

int isDir( const char * const path )   // 0 for dir, 1 for not a dir, -1 for an error
{
    struct stat *statbuf = calloc( sizeof(struct stat), 1 );
    int code = stat( path, statbuf );
    if ( code != 0 )
    {
        writeErr( "ERR: Failed to determine type of file " );
        writeErr( path );
        writeErr( "\n" );
        free( statbuf );
        return -1;
    }
    else
    {
        if ( ( statbuf->st_mode & __S_IFMT ) == __S_IFDIR )
            code = 0;
        else
            code = 1;
        free( statbuf );
        return code;
    }
}

int getDirList( const char * const dirpath, list **HEAD, const int min, const int max )
{

    //recursively create list of ALL files, excluding directories and '.' and '..',
    //that matches the specified size
    list *last = *HEAD;
    int code = 0;
    DIR *dir = opendir( dirpath );
    if ( dir == NULL )
    {
        writeErr( "ERR: Failed to open directory " );
        writeErr( dirpath );
        writeErr( "\n" );
        return -1;
    }

    dirent *entry = calloc( sizeof(dirent), 1 );
    char *path = calloc( sizeof(char), strlen( dirpath ) + 256 + 1 );

    while (  ( entry = readdir( dir ) ) != NULL )
    {

        if ( strcmp( entry->d_name, "." ) == 0 || strcmp( entry->d_name, ".." ) == 0 )
            continue;

        path = strcpy( path, dirpath );
        path = strcat( path, "/" );
        path = strcat( path, entry->d_name );
        path = strcat( path, "\0" );

        if ( isDir( path ) == 0 )  //directory
        {
                if ( abs( getDirList( path, &last, min, max ) != 0 ) )
                    code = 1;

        }
        else if ( isDir( path ) == 1 )     //file
        {
            struct stat *stats = calloc( sizeof(struct stat), 1 );

            if ( stat( path, stats ) == 0 )
            {
                int size = stats->st_size;
                if ( stats->st_size >=  min && stats->st_size <= max )
                {
                    last->next = getItem( path, stats->st_size );
                    last = last->next;
                }
            }
            else
            {
                writeErr( "ERR: Failed to read size of file " );
                writeErr( path );
                writeErr( "\n" );
                code = 1;
            }
            free( stats );
        }
        else
        {
            writeErr( "ERR: Failed to determine type of file\n" );
            code = 1;
        }
    }

    free( path );
    free( entry );
    if ( closedir( dir ) == -1 )
    {
        writeErr( "ERR: Failed to close directory\n" );
        return -1;
    }

    return code;
}

void writeDups( const list * const file1, const list * const file2 )
{
    if ( file1 != NULL && file2 != NULL )
    {
        printf("%s", file1->path);
        printf("%s\n", file2->path);
    }
    else
    {
        writeErr( "ERR: Failed to write found dublicates\n" );
    }
}

int isDup( const char* const path1, const char* const path2 ) //0 for equals files, 1 for not, -1 for error
{

    //opening files
    //comparing files by small blocks (256 bytes?)
    char buf1[1024] = {0}, buf2[1024] = {0};
    int fd1 = 0, fd2 = 0;
    fd1 = open( path1, O_RDONLY );
    fd2 = open( path2, O_RDONLY );
    if ( fd1 == -1 || fd2 == -2 )
    {
        writeErr( "ERR: Failed open files to compare\n" );
        return -1;
    }

    int code = 0;

    while ( ( code = read( fd1, buf1, 1024 ) ) != 0 && read( fd2, buf2, 1024 ) != 0 )
    {
        if ( strncmp( buf1, buf2, code ) != 0 )
            return 1;
    }

    if ( close( fd1 ) != 0 || close( fd2 ) != 0 )
    {
        writeErr( "ERR: Failed to close files\n" );
    }


    return 0;
}

int findDup(  list *HEAD )
{

    //first - check sizes, only then - file's  content
    list *tmp = HEAD->next;
    while ( HEAD->next != NULL )
    {
        while ( tmp->next != NULL )
        {
            if ( HEAD->next->size == tmp->next->size )
                if ( isDup( HEAD->next->path, tmp->next->path ) == 0 )
                    writeDups( HEAD->next, tmp->next );
            tmp = tmp->next;
        }

        HEAD = HEAD->next;
    }
    return 0;
}



int main(int argc, char *argv[])
{
    if ( argc != 4 )
    {
        writeErr( "ERR: Incorrect parameters\n" );
        return 1;
    }

    DIR *dir = opendir( argv[1] );
    if ( dir == NULL )
    {
        writeErr( "ERR: Incorrect path\n" );
        return 1;
    }
    closedir( dir );

    int N1 = 0, N2 = 0;

    if ( sscanf( argv[2], "%d", &N1) != 1 || sscanf( argv[3], "%d", &N2) != 1 )
    {
        writeErr( "ERR: Specified sizes aren't numbers\n" );
        return 1;
    }

    if ( N1 > N2 )
    {
        writeErr( "ERR: Incorrect sizes\n" );
        return 1;
    }

    list *Head = (list*) calloc( sizeof(list), 1 );
    Head->next = NULL;
    Head->path = NULL;
    Head->size = -1;

    if ( getDirList( argv[1], &Head, N1, N2 ) != 0 )
    {
        writeErr( "ERR: Failed collecting items info\n" );
        return 1;
    }

    if ( findDup( Head ) != 0 )
    {
        writeErr( "ERR: Failed to find duplicates\n" );
        return 1;
    }

    list *tmp = NULL;
    while ( Head->next != NULL )
    {
        tmp = Head->next;
        Head->next = tmp->next;
        free( tmp->path );
        free( tmp );
    }
    free( Head );
    puts( "Done" );
    return 0;
}
