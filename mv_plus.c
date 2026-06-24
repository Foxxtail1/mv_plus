#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>


bool validate_path(const char *path)
{
    struct stat buffer;
    return (stat(path, &buffer) == 0);
}

bool findOriginal(const char *destination, const char *filename, off_t newSize)
{
    DIR *dstDir = opendir(destination);
    struct dirent *dstEntry;
    struct stat path_stat;
    char full_path[1024];
    while ((dstEntry = readdir(dstDir)) != NULL)
    {
        if(strcmp(dstEntry->d_name, filename) == 0)
        {
            snprintf(full_path, sizeof(full_path), "%s/%s", destination, dstEntry->d_name);
            return stat(full_path, &path_stat) == 0  && S_ISREG(path_stat.st_mode) && path_stat.st_size > newSize; 
        }
               
    }

    return true;
}

void resolveHome(char *path, char *retVal, size_t maxSize)
{
    if(path[0] == '~')
    {
        const char *home_dir = getenv("HOME");
        if(home_dir != NULL)
        {
            snprintf(retVal, maxSize, "%s%s", home_dir, path + 1);
        }
        else {
            printf("Cannot resolve home directory!\n");
        }
    }
    else 
    {
        strncpy(retVal, path, maxSize);
    }
}


void basicloop(char *source, char *destination)
{
    DIR *srcDir = opendir(source);

    struct stat path_stat;
    char full_path[1024];
    char new_path[1024];

    if(srcDir == NULL)
    {
        printf("Error\n");
        return;
    }

    struct dirent *srcEntry;

    while ((srcEntry = readdir(srcDir)) != NULL)
    {
        snprintf(full_path, sizeof(full_path), "%s/%s", source, srcEntry->d_name);
        if(stat(full_path, &path_stat) == 0)
        {
            if(S_ISREG(path_stat.st_mode) && findOriginal(destination, srcEntry->d_name, path_stat.st_size))
            {
                printf("%s - moved\n", srcEntry->d_name);
                snprintf(new_path, sizeof(new_path), "%s/%s", destination, srcEntry->d_name);
                rename(full_path, new_path);    
            }
        }
    }

}

int main(int argc, char *argv[])
{
    if( argc < 3)
    {
        printf("Requires a source and a destination path");
    }
    else
    {
        char source[1024];
        char destination[1024];

        resolveHome(argv[1], source, sizeof(source));
        resolveHome(argv[2], destination, sizeof(destination));

        printf("%s\n",source);
        printf("%s\n", destination);

        if(validate_path(source) && validate_path(destination))
        {
          basicloop(source, destination);
        }
        else
        {
            printf("invalid path!");
        }
        
       
    }
    return 0;
}