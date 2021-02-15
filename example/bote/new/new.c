#include <errno.h>
#include <error.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char bote_home[] = "/home/bb/.bote/";

int main(int argc, char *argv[]) {
  int fd;
  char *fname;

  if (argc != 2) {
    error(EINVAL, EINVAL, "Bad arguments!\n");
  }

  fname = malloc(strlen(bote_home) + strlen(argv[1]) + 1);
  if (!fname) {
    error(1, ENOMEM, "failed to allocate note filename\n");
  }
  fname = strcpy(fname, bote_home);
  fname = strcat(fname, argv[1]);
  fd = open(fname, O_RDWR | O_CREAT,
      S_IWUSR | S_IRUSR | S_IWGRP | S_IRGRP | S_IWOTH | S_IROTH);
  if (fd < 0) {
    error(1, errno, "failed to open note %s\n", fname);
  }
  return 0;
}
