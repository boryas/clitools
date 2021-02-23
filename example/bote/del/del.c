#include <errno.h>
#include <error.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static const char bote_home[] = "~/.bote";

int main(int argc, char *argv[]) {
  int ret;
  char *fname;

  fname = malloc(strlen(bote_home) + strlen(argv[1]) + 1);
  if (!fname) {
    error(1, ENOMEM, "failed to allocate note filename\n");
  }
  fname = strcpy(fname, bote_home);
  fname = strcat(fname, argv[1]);
  ret = unlink(fname);
  if (ret)
    error(1, errno, "failed to unlink note %s\n", fname);
  return 0;
}
