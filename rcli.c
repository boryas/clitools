#include <dirent.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

struct dirq {
  char *q[100];
  int front;
  int back;
};

int dirq_sz(struct dirq *dq) {
  return dq->back - dq->front;
}

char *dirq_pop(struct dirq *dq) {
  char *ret = dq->q[dq->front % 100];
  printf("popped %s from %d, set front to %d\n", ret, dq->front, dq->front + 1);
  ++dq->front;
  if (dq->front == dq->back) {
    printf("front caught back, reset it all\n");
    dq->front = 0;
    dq->back = 0;
  }
  return ret;
}

/*
 * return 1 on overflow, 0 on success
 */
int dirq_push(struct dirq *dq, char *dir) {
  if (dirq_sz(dq) > 0 && dq->back % 100 == dq->front % 100) {
    return 1;
  }
  dq->q[dq->back % 100] = dir;
  printf("pushed %s to %d, set back to %d\n", dir, dq->back, dq->back + 1);
  ++dq->back;
  return 0;
}

int main(int argc, char *argv[]) {
  char *name;
  DIR *dir;
  struct dirent *dent;
  struct dirq dq;
  char *new_dir;

  memset(&dq, 0, sizeof(dq));

  if (argc != 2) {
    printf("usage: rcli <CLI-DIR>\n");
    exit(1);
  }

  name = malloc(strlen(argv[1]));
  if (!name) {
    exit(ENOMEM);
  }
  name = strcpy(name, argv[1]);
  dirq_push(&dq, name);
  while (dirq_sz(&dq)) {
    name = dirq_pop(&dq);
    dir = opendir(name);
    if (!dir) {
      exit(errno);
    }
    printf("popped a dir: %s\n", name);

    errno = 0;
    while(dent = readdir(dir)) {
      if (dent->d_type == DT_DIR) {
        if (!strcmp(dent->d_name, "..") || !strcmp(dent->d_name, ".")) {
          continue;
        }
        new_dir = malloc(strlen(name) + 1);
        if (!new_dir) {
          exit(ENOMEM);
        }
        new_dir = strcpy(new_dir, name);
        new_dir = strcat(new_dir, "/");
        new_dir = strcat(new_dir, dent->d_name);
        if (dirq_push(&dq, new_dir)) {
          exit(EINVAL);
        }
        printf("pushed a directory: %s\n", new_dir);
      }
      errno = 0;
    }
    if (errno) {
      // TODO nice error handling
      printf("error in readdir: %d\n", errno);
      exit(errno);
    }
  }
}
