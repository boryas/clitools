#include <dirent.h>
#include <errno.h>
#include <libgen.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

struct rcli {
  char *path;
  char *name;
  size_t sz;
  struct rcli *subclis;
};

int rcli_init(struct rcli *cli, char *path) {
  char *cpy;
  char *name;
  int path_len = strlen(path);
  int ret;

  cli->path = malloc(path_len);
  if (!cli->path) {
    ret = ENOMEM;
    goto out;
  }
  cli->path = strcpy(cli->path, path);

  cpy = malloc(path_len);
  if (!cpy) {
    ret = ENOMEM;
    goto free_path;
  }
  cpy = strcpy(cpy, path);
  name = basename(cpy);
  cli->name = malloc(strlen(name));
  if (!cli->name) {
    ret = ENOMEM;
    goto free_cpy;
  }
  cli->name = strcpy(cli->name, name);
  ret = 0;
  goto out;

free_cpy:
  free(cpy);
free_path:
  free(cli->path);
out:
  return ret;
}

int rcli_traverse(struct rcli *cli, int (*dir_op)(struct rcli*, struct dirent*, int i)) {
  int ret = 0;
  DIR *dir;
  struct dirent *dent;
  int i = 0;

  dir = opendir(cli->path);
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
  // TODO refactor and use rcli_init
  cli->subclis[i].path = malloc(strlen(cli->path) + strlen(dent->d_name) + 1);
  if (!cli->subclis[i].path) {
    return ENOMEM;
  }
  cli->subclis[i].path = strcpy(cli->subclis[i].path, cli->path);
  cli->subclis[i].path = strcat(cli->subclis[i].path, "/");
  cli->subclis[i].path = strcat(cli->subclis[i].path, dent->d_name);

  cli->subclis[i].name = malloc(strlen(dent->d_name));
  if (!cli->subclis[i].name) {
    return ENOMEM;
  }
  cli->subclis[i].name = strcpy(cli->subclis[i].name, dent->d_name);
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

  printf("rcli_populate %s\n", cli->path);

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

struct rcli *rcli_find_subcli(struct rcli *cli, int argc, char *argv[]) {
  struct rcli *cur = cli;

  for (int i = optind; i < argc; ++i) {
    for (int j = 0; j < cur->sz; ++j) {
      if (!strcmp(cur->subclis[j].name, argv[i])) {
        cur = &cur->subclis[j];
        break;
      }
    }
  }
  return cur;
}

int rcli_run_cli(struct rcli *cli, int argc, char *argv[]) {
  int c;
  bool help = false;
  bool verbose = false;
  struct rcli *sub_cli;

  // handle universal options
  // ignore unknown
  // get optind to the sub-command
  while ((c = getopt(argc, argv, "hv")) != -1) {
    switch (c) {
      case 'h':
        help = true;
        break;
      case 'v':
        verbose = true;
        break;
      case '?':
        printf("unknown option '-%c'", optopt);
        break;
      default:
        return -1;
    }
  }
  // walk rcli to find subcommand
  sub_cli = rcli_find_subcli(cli, argc, argv);
  printf("final cli: %s, %s\n", sub_cli->path, sub_cli->name);

  // get options for subcommand

  // re-init optind and run subcommand

}

int main(int argc, char *argv[]) {
  char *name;
  struct rcli cli;
  int ret = 0;

  memset(&cli, 0, sizeof(cli));

  if (argc < 2) {
    printf("usage: rcli <CLI-DIR> [command]\n");
    exit(1);
  }

  ret = rcli_init(&cli, argv[1]);
  if (ret) {
    exit(ret);
  }

  ret = rcli_populate(&cli);
  if (ret) {
    exit(ret);
  }

  --argc;
  ++argv;
  return rcli_run_cli(&cli, argc, argv);
}
