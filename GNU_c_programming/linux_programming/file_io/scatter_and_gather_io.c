#include <sys/stat.h>
#include <sys/uio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <setjmp.h>
#include <time.h>
int main(int argc, char *argv[])
{
    int fd;
    struct iovec __iovec[3];
    ssize_t byte_num, tot_required;
    if (argc!=2 || strcmp(argv[1], "--help") == 0)
    printf("%s file\n", argv[0]);
    fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror("open");
        exit(EXIT_SUCCESS);}
    tot_required = 0;
    /* assign first buffer to __iovec[0] */
    struct stat file_info; /* First buffer, struct stat is like that:
struct stat {
mode_t st_mode; // file type & mode (permissions)
ino_t st_ino; // i-node number (serial number)
dev_t st_dev; // device number (file system)
dev_t st_rdev; // device number for special files
nlink_t st_nlink; // number of links
uid_t st_uid; // user ID of owner
gid_t st_gid; // group ID of owner
off_t st_size; // size in bytes, for regular files
struct timespec st_atim; // time of last access
struct timespec st_mtim; // time of last modification
struct timespec st_ctim; // time of last file status change
blksize_t st_blksize; // best I/O block size
blkcnt_t st_blocks; // number of disk blocks allocated
};*/
    __iovec[0].iov_base = &file_info;
    __iovec[0].iov_len = sizeof(struct stat);
    tot_required += __iovec[0].iov_len;
    /* assign second buffer to __iovec[1] */
    static jmp_buf env; /* Second buffer */
    __iovec[1].iov_base = &env;
    __iovec[1].iov_len = sizeof(jmp_buf);
    tot_required += __iovec[1].iov_len;
    /* assign third buffer to __iovec[2] */
    time_t tot_time;
    time(&tot_time);
    struct tm *cur_time = localtime(&tot_time); /* Third buffer */
    __iovec[2].iov_base = cur_time;
    __iovec[2].iov_len = sizeof(struct tm);
    tot_required += __iovec[2].iov_len;
    /* readv() and writev() are both atomic operations */
    byte_num = readv(fd, __iovec, 3);
    if (byte_num == -1){
        perror("readv");
        exit(EXIT_SUCCESS);}
    if (byte_num < tot_required)
    printf("Read fewer bytes than requested\n");
    printf("total bytes requested: %ld; bytes read: %ld\n",
    (long)tot_required, (long)byte_num);
    exit(EXIT_SUCCESS);
}