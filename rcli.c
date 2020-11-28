#include <dirent.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

struct rcli {
  char *dir;
  size_t sz;
  struct rcli *subclis;
};

int count_subclis(struct rcli *cli) {
  int ret = 0;
  DIR *dir;
  struct dirent *dent;

  dir = opendir(cli->dir);
  if (!dir) {
    return -errno;
  }
  while (true) {
    errno = 0;
    dent = readdir(dir);
    if (!dent) {
      if (errno) {
        ret = -errno;
        goto out;
      }
      break;
    }
    if (dent->d_type == DT_DIR) {
      if (!strcmp(dent->d_name, "..") || !strcmp(dent->d_name, ".")) {
        continue;
      }
      ++ret;
    }
  }
out:
  closedir(dir);
  return ret;
}

int get_subclis(struct rcli *cli) {
  int ret = 0;
  int i = 0;
  DIR *dir;
  struct dirent *dent;

  dir = opendir(cli->dir);
  if (!dir) {
    exit(errno);
  }
  while (true) {
    errno = 0;
    dent = readdir(dir);
    if (!dent) {
      if (errno) {
        ret = -errno;
        goto out;
      }
      break;
    }
    if (dent->d_type == DT_DIR) {
      if (!strcmp(dent->d_name, "..") || !strcmp(dent->d_name, ".")) {
        continue;
      }
      cli->subclis[i].dir = malloc(strlen(cli->dir) + strlen(dent->d_name) + 1);
      if (!cli->subclis[i].dir) {
        ret = -ENOMEM;
        goto out;
      }
      cli->subclis[i].dir = strcpy(cli->subclis[i].dir, cli->dir);
      cli->subclis[i].dir = strcat(cli->subclis[i].dir, "/");
      cli->subclis[i].dir = strcat(cli->subclis[i].dir, dent->d_name);
      ++i;
    }
  }
out:
  closedir(dir);
  return ret;
}

int rcli_populate(struct rcli *cli) {
  int ret;
  int i = 0;

  printf("rcli_populate %s\n", cli->dir);

  ret = count_subclis(cli);
  if (ret < 0) {
    return ret;
  }
  cli->sz = ret;
  cli->subclis = malloc(cli->sz * sizeof(struct rcli));
  if (!cli->subclis) {
    return -ENOMEM;
  }

  ret = get_subclis(cli);
  if (ret < 0) {
    return ret;
  }

  for (i = 0; i < cli->sz; ++i) {
    ret = rcli_populate(&cli->subclis[i]);
    if (ret < 0) {
      return ret;
    }
  }
  return 0;
}

int main(int argc, char *argv[]) {
  char *name;
  struct rcli cli;

  memset(&cli, 0, sizeof(cli));

  if (argc != 2) {
    printf("usage: rcli <CLI-DIR>\n");
    exit(1);
  }

  name = malloc(strlen(argv[1]));
  if (!name) {
    exit(ENOMEM);
  }
  name = strcpy(name, argv[1]);
  cli.dir = name;

  return rcli_populate(&cli);
}
