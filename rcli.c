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

int rcli_traverse(struct rcli *cli, int (*dir_op)(struct rcli*, struct dirent*, int i)) {
  int ret = 0;
  DIR *dir;
  struct dirent *dent;
  int i = 0;

  dir = opendir(cli->dir);
  if (!dir) {
    return errno;
  }
  while (true) {
    errno = 0;
    dent = readdir(dir);
    if (!dent) {
      if (errno) {
        ret = errno;
        goto out;
      }
      break;
    }
    if (dent->d_type == DT_DIR) {
      if (!strcmp(dent->d_name, "..") || !strcmp(dent->d_name, ".")) {
        continue;
      }
      ret = dir_op(cli, dent, i);
      if (ret) {
        goto out;
      }
      ++i;
    }
  }
out:
  closedir(dir);
  return ret;
}

int rcli_inc_sz(struct rcli *cli, struct dirent *dent, int i) {
  ++cli->sz;
  return 0;
}

int rcli_add_subcli(struct rcli *cli, struct dirent *dent, int i) {
  cli->subclis[i].dir = malloc(strlen(cli->dir) + strlen(dent->d_name) + 1);
  if (!cli->subclis[i].dir) {
    return ENOMEM;
  }
  cli->subclis[i].dir = strcpy(cli->subclis[i].dir, cli->dir);
  cli->subclis[i].dir = strcat(cli->subclis[i].dir, "/");
  cli->subclis[i].dir = strcat(cli->subclis[i].dir, dent->d_name);
  return 0;
}

int count_subclis(struct rcli *cli) {
  return rcli_traverse(cli, rcli_inc_sz);
}

int get_subclis(struct rcli *cli) {
  return rcli_traverse(cli, rcli_add_subcli);
}

int rcli_populate(struct rcli *cli) {
  int ret;
  int i = 0;

  printf("rcli_populate %s\n", cli->dir);

  ret = count_subclis(cli);
  if (ret < 0) {
    return ret;
  }
  cli->subclis = calloc(cli->sz, sizeof(struct rcli));
  if (!cli->subclis) {
    return ENOMEM;
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
  int ret = 0;

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

  ret = rcli_populate(&cli);
  if (ret) {
    exit(ret);
  }
}
