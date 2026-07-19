// MyLinux.h

#ifndef ZIP7_INC_COMMON_MY_LINUX_H
#define ZIP7_INC_COMMON_MY_LINUX_H

// #include "../../C/7zTypes.h"

#define MY_LIN_DT_UNKNOWN   0
#define MY_LIN_DT_FIFO      1
#define MY_LIN_DT_CHR       2
#define MY_LIN_DT_DIR       4
#define MY_LIN_DT_BLK       6
#define MY_LIN_DT_REG       8
#define MY_LIN_DT_LNK       10
#define MY_LIN_DT_SOCK      12
#define MY_LIN_DT_WHT       14

#define MY_LIN_S_IFMT  00170000
#define MY_LIN_S_IFSOCK 0140000
#define MY_LIN_S_IFLNK  0120000
#define MY_LIN_S_IFREG  0100000
#define MY_LIN_S_IFBLK  0060000
#define MY_LIN_S_IFDIR  0040000
#define MY_LIN_S_IFCHR  0020000
#define MY_LIN_S_IFIFO  0010000

#define MY_LIN_S_ISLNK(m)   (((m) & MY_LIN_S_IFMT) == MY_LIN_S_IFLNK)
#define MY_LIN_S_ISREG(m)   (((m) & MY_LIN_S_IFMT) == MY_LIN_S_IFREG)
#define MY_LIN_S_ISDIR(m)   (((m) & MY_LIN_S_IFMT) == MY_LIN_S_IFDIR)
#define MY_LIN_S_ISCHR(m)   (((m) & MY_LIN_S_IFMT) == MY_LIN_S_IFCHR)
#define MY_LIN_S_ISBLK(m)   (((m) & MY_LIN_S_IFMT) == MY_LIN_S_IFBLK)
#define MY_LIN_S_ISFIFO(m)  (((m) & MY_LIN_S_IFMT) == MY_LIN_S_IFIFO)
#define MY_LIN_S_ISSOCK(m)  (((m) & MY_LIN_S_IFMT) == MY_LIN_S_IFSOCK)

#define MY_LIN_S_ISUID 0004000
#define MY_LIN_S_ISGID 0002000
#define MY_LIN_S_ISVTX 0001000

#define MY_LIN_S_IRWXU 00700
#define MY_LIN_S_IRUSR 00400
#define MY_LIN_S_IWUSR 00200
#define MY_LIN_S_IXUSR 00100

#define MY_LIN_S_IRWXG 00070
#define MY_LIN_S_IRGRP 00040
#define MY_LIN_S_IWGRP 00020
#define MY_LIN_S_IXGRP 00010

#define MY_LIN_S_IRWXO 00007
#define MY_LIN_S_IROTH 00004
#define MY_LIN_S_IWOTH 00002
#define MY_LIN_S_IXOTH 00001

/*
// major/minor encoding for makedev(): MMMMMmmmmmmMMMmm:

inline UInt32 MY_dev_major(UInt64 dev)
{
  return ((UInt32)(dev >> 8) & (UInt32)0xfff) | ((UInt32)(dev >> 32) & ~(UInt32)0xfff);
}

inline UInt32 MY_dev_minor(UInt64 dev)
{
  return ((UInt32)(dev) & 0xff) | ((UInt32)(dev >> 12) & ~0xff);
}

inline UInt64 MY_dev_makedev(UInt32 __major, UInt32 __minor)
{
  return (__minor & 0xff) | ((__major & 0xfff) << 8)
      | ((UInt64) (__minor & ~0xff)  << 12)
      | ((UInt64) (__major & ~0xfff) << 32);
}
*/

#if defined(__OS2__)
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <sys/stat.h>
#include <InnoTekLIBC/backend.h>

#define OPENAT_BUFFER_SIZE PATH_MAX

#if !defined(AT_SYMLINK_NOFOLLOW)
#define AT_SYMLINK_NOFOLLOW 0x100
#endif
#if !defined(AT_FDCWD)
#define AT_FDCWD -100
#endif

inline char *
openat_proc_name (char buf[OPENAT_BUFFER_SIZE], int fd, char const *file)
{
  char *result = buf;
  int dirlen;

  /* Make sure the caller gets ENOENT when appropriate.  */
  if (!*file)
    {
      buf[0] = '\0';
      return buf;
    }

  /* OS/2 kLIBC provides a function to retrieve a path from a fd.  */
  {
    char dir[_MAX_PATH];
    size_t bufsize;

    if (__libc_Back_ioFHToPath (fd, dir, sizeof dir)){
      printf("libc error: %i, %i\n", fd, errno);
      return NULL;
    }

    dirlen = strlen (dir);
    bufsize = dirlen + 1 + strlen (file) + 1; /* 1 for '/', 1 for null */
    if (OPENAT_BUFFER_SIZE < bufsize)
      {
        result = (char*) malloc (bufsize);
        if (! result)
          return NULL;
      }

    strcpy (result, dir);
    result[dirlen++] = '/';
  }

  strcpy (result + dirlen, file);
  return result;
}

inline int fstatat(int dirfd, const char *pathname, struct stat *buf, int flags)
{
  int ret;

  printf("trace pathname: %s\n", pathname);
  if (dirfd == AT_FDCWD || pathname[0]=='/' || pathname[1]==':')
    return stat(pathname, buf);

  char proc_buf[OPENAT_BUFFER_SIZE];
  char *proc_file = openat_proc_name (proc_buf, dirfd, pathname);

  printf("trace procfile: %s\n", proc_file);
  if (flags & AT_SYMLINK_NOFOLLOW)
    ret = lstat(proc_file, buf);
  else
    ret = stat(proc_file, buf);

  if (proc_file != proc_buf)
    free (proc_file);

  return (ret);
}
#endif

#endif
