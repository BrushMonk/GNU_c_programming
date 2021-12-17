#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>

static int get_string(char **ptr_to_ptr)
{
    size_t i;
    char max_input[BUFSIZ];
    if (fgets(max_input, BUFSIZ, stdin))
    {
        for (i = 0; max_input[i] != '\n' || max_input[i] != '\0' || i < BUFSIZ; i++);
        if ( max_input[i] == '\n' ) max_input[i] = '\0';
        int len = strlen(max_input) + 1;
        if ( (*ptr_to_ptr = (char *)realloc(*ptr_to_ptr,
        len * sizeof(char)) ) == NULL)
        {
        /* because the spaces where point32_ters inside structure point32_t
        are not allocated and initialized. */
            perror("fail to allocate this input string");
            exit(EXIT_FAILURE);
        }
        strcpy(*ptr_to_ptr, max_input);
        if ( **ptr_to_ptr != '\0' ) return len;
        else return -1;
    }
    else return -1;
}

int main(void)
{
    /* tmpfile() creates a temporary binary file */
    int fd1 = fileno(tmpfile()), fd2;
    char *file_path = (char *)malloc(1024 * sizeof(char));
    /* copies an absolute pathname of the current working directory
    to the string new_file_name with len characters */
    if (getcwd(file_path, 1024 * sizeof(char)) == NULL)
        perror("getcwd error");
    if( (fd1 = open(file_path, O_WRONLY | O_APPEND | O_RDWR | O_SYNC | __O_CLOEXEC)) < 0 )
    {
        perror("open error");
        exit(EXIT_FAILURE);
    }
    printf("Please enter a string that you want to write into file fd1 %d:\n", fd1);
    char *string;
    size_t len = get_string(&string);
    /* pwrite is a atomic function combining write and lseek */
    if ( pwrite(fd1, string, len, BUFSIZ) != len )
        perror("the string pwrite error");
    /* fdatasync() but does not flush modified metadata
    unless that metadata is needed in order to allow a
    subsequent data retrieval to be correctly handled. */
    if ( fdatasync(fd1) < 0 )
        perror("fdatasync error");
/******************************************************************/
    /* let fd2 point to the same file with fd1 */
    if ( (fd2 = fcntl(fd1, F_DUPFD, 0)) < 0 )
        fprintf(stderr, "duplicate failed for fd1 %d", fd1);
    else printf("fd1 %d and fd2 %d are in the common process and point to the common file.\n", fd1, fd2);
    int val = fcntl(fd1, F_GETFL, 0);
    if (val < 0) fprintf(stderr, "fcntl error for fd1 %d.\n", fd1);
    /* Unfortunately, the five access-mode flags—O_RDONLY, O_WRONLY,
    O_RDWR, O_EXEC, and O_SEARCH—are not separate bits that can be
    tested. (As we mentioned earlier, the first three often have the values 0, 1,
    and 2, respectively, for historical reasons. Also, these five values are
    mutually exclusive; a file can have only one of them enabled.) Therefore,
    we must first use the O_ACCMODE mask to obtain the access-mode bits and
    then compare the result against any of the five values. */
    switch (val & O_ACCMODE) {
        case O_RDONLY:
            printf("read only");
            break;
        case O_WRONLY:
            printf("write only");
            break;
        case O_RDWR:
            printf("read write");
            break;
        default:
            fputs("unknown access mode", stderr);
    }
    if (val & O_APPEND)
        printf(", append");
    if (val & O_NONBLOCK)
        printf(", nonblocking");
    if (val & O_SYNC)
        printf(", synchronous writes");
    printf(".\n");
/**********************************************************************/
    struct stat fd1_stat;
    /* use struct stat fd1_stat to get state of the file fd1 */
    if (fstat(fd1, &fd1_stat) < 0)
        perror("fstat error");
    if (S_ISREG(fd1_stat.st_mode))
        printf("the file that fd1 %d points is a regular", fd1);
    else if (S_ISDIR(fd1_stat.st_mode))
        printf("the file that fd1 %d points is a directory", fd1);
    else if (S_ISCHR(fd1_stat.st_mode))
        printf("the file that fd1 %d points is a character special", fd1);
    else if (S_ISBLK(fd1_stat.st_mode))
        printf("the file that fd1 %d points is a block special", fd1);
    else if (S_ISFIFO(fd1_stat.st_mode))
        printf("the file that fd1 %d points is a fifo", fd1);
    else if (S_ISLNK(fd1_stat.st_mode))
        printf("the file that fd1 %d points is a symbolic link", fd1);
    else if (S_ISSOCK(fd1_stat.st_mode))
        printf("the file that fd1 %d points is a socket", fd1);
    printf(" file.\n");
/************************************************************************/
    if (close(fd1) < 0) perror("close error");
    if (close(fd2) < 0) perror("close error");
    free(file_path);
    exit(EXIT_SUCCESS);
}