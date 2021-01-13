#include <error.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

/*
 * TODO:
 * shmem writer side lib
 * shmem reader side lib
 * shmem getopt
 *   write side parses all opts
 *   runs "generated" getopt
 *   calculates size to store opt struct
 *   opens write side
 *   saves opt struct
 *   fork-execs read side
 *   opens read side
 *   loads opt struct
 *
 *   open Q: how do they specify/agree on opt struct?
 *           just expect a header file they're both linked with?
 *           or a .so?
 *           some kind of crappy generic ser/deser?
 *           expect synced structs?
 *           has to be easier than just doing getopt lol
 */

int main(int argc, char *argv[]) {
  void *mapping;
  int fd;
  int ret;
  int ret2;
  size_t sz = 4096;
  char buf[sz];
  int tmp_errno;

  fd = shm_open("/my_shmona", O_RDWR | O_CREAT,
      S_IWUSR | S_IRUSR | S_IWGRP | S_IRGRP | S_IROTH);
  if (fd < 0) {
    error(1, errno, "failed to shm_open");
  }
  ret = ftruncate(fd, sz);
  if (ret < 0) {
    error(1, errno, "failed to ftruncate");
  }
  mapping = mmap(NULL, sz, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (!mapping) {
    error(1, errno, "failed to mmap");
  }
  close(fd);

  if (argc > 1) {
    strcpy(mapping, argv[1]);
    ret = munmap(mapping, sz);
    if (ret < 0) {
      error(1, errno, "failed to munmap");
    }
  } else {
    strncpy(buf, mapping, sz);
    ret = munmap(mapping, sz);
    tmp_errno = errno;
    ret2 = shm_unlink("/my_shmona");
    if (ret2 < 0) {
      error(0, errno, "failed to shm_unlink");
    }
    if (ret < 0) {
      error(1, tmp_errno, "failed to munmap");
    }
    if (ret2 < 0) {
      return 1;
    }
    printf("read shmem: %s\n", buf);
  }

  return 0;
}
