#include <dirent.h>
#include <errno.h>
#include <error.h>
#include <fcntl.h>
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
  struct rcli *sub_clis;
};

static char *get_basename(char *path) {
  char *cpy;
  char *bn;
  char *ret;
  int path_len = strlen(path);

  cpy = malloc(path_len);
  if (!cpy) {
    return NULL;
  }
  cpy = strcpy(cpy, path);
  bn = basename(cpy);
  ret = malloc(strlen(bn));
  if (!ret) {
    ret = NULL;
    goto out;
  }
  ret = strcpy(ret, bn);
out:
  free(cpy);
  return ret;
}

int rcli_init(struct rcli *cli, char *path) {
  char *cpy;
  char *name;
  int path_len = strlen(path);
  int ret;

  if (!cli) {
    ret = EINVAL;
    goto out;
  }

  memset(cli, 0, sizeof(*cli));

  cli->path = malloc(path_len + 1);
  if (!cli->path) {
    ret = ENOMEM;
    goto out;
  }
  cli->path = strcpy(cli->path, path);
  cli->path = strcat(cli->path, "/");
  cli->name = get_basename(path);
  if (!cli->name) {
    goto free_path;
  }

  ret = 0;
  goto out;

free_path:
  free(cli->path);
out:
  return ret;
}

void rcli_free(struct rcli *cli) {
  int i;

  if (cli->path) {
    free(cli->path);
  }
  if (cli->name) {
    free(cli->name);
  }
  for (i = 0; i < cli->sz; ++i) {
    rcli_free(&cli->sub_clis[i]);
  }
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

int rcli_op_inc_sz(struct rcli *cli, struct dirent *dent, int i) {
  ++cli->sz;
  return 0;
}

int rcli_op_add_subcli(struct rcli *cli, struct dirent *dent, int i) {
  int ret;
  char *path = malloc(strlen(cli->path) + strlen(dent->d_name));

  if (!path) {
    return ENOMEM;
  }
  path = strcpy(path, cli->path);
  path = strcat(path, dent->d_name);

  ret = rcli_init(&cli->sub_clis[i], path);

  free(path);
  return ret;
}

static int count_sub_clis(struct rcli *cli) {
  return rcli_traverse(cli, rcli_op_inc_sz);
}

static int get_sub_clis(struct rcli *cli) {
  return rcli_traverse(cli, rcli_op_add_subcli);
}

int rcli_populate(struct rcli *cli) {
  int ret;
  int i = 0;

  printf("rcli_populate %s\n", cli->path);

  ret = count_sub_clis(cli);
  if (ret < 0) {
    return ret;
  }
  cli->sub_clis = calloc(cli->sz, sizeof(struct rcli));
  if (!cli->sub_clis) {
    return ENOMEM;
  }

  ret = get_sub_clis(cli);
  if (ret < 0) {
    return ret;
  }

  for (i = 0; i < cli->sz; ++i) {
    ret = rcli_populate(&cli->sub_clis[i]);
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
      if (!strcmp(cur->sub_clis[j].name, argv[i])) {
        cur = &cur->sub_clis[j];
        break;
      }
    }
  }
  return cur;
}

int rcli_dump(char *fname) {
  char buf[4096];
  int fd;
  int sz;

  fd = open(fname, O_RDONLY);
  if (fd < 0) {
    error(0, errno, "failed to open %s for dump\n", fname);
    return errno;
  }

  while ((sz = read(fd, buf, 4096))) {
    if (sz < 0) {
      if (errno == EINTR) {
        continue;
      }
      error(0, errno, "failed to read %s for dump\n", fname);
      close(fd);
      return errno;
    }
    write(STDOUT_FILENO, buf, sz);
  }
  close(fd);
  return 0;
}

int rcli_run_cli(struct rcli *cli, int argc, char *argv[]) {
  int c;
  bool help = false;
  bool verbose = false;
  struct rcli *sub_cli;
  char *run_f, *help_f, *usage_f;

  // handle universal options
  // ignore unknown
  // get optind to the sub-command
  // long opts?
  // usage
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
  printf("sub_cli: %s\n", sub_cli->path);

  if (help) {
    help_f = malloc(strlen(sub_cli->path) + strlen("help"));
    if (!help_f) {
      error(0, ENOMEM, "failed to allocate rcli help filename\n");
      return ENOMEM;
    }
    help_f = strcpy(help_f, sub_cli->path);
    help_f = strcat(help_f, "help");
    rcli_dump(help_f);
    return 0;
  }

  run_f = malloc(strlen(sub_cli->path) + strlen("run"));
  if (!run_f) {
    error(0, ENOMEM, "failed to allocate rcli run filename\n");
    return ENOMEM;
  }
  run_f = strcpy(run_f, sub_cli->path);
  run_f = strcat(run_f, "run");

  // get options for subcommand

  for (int i = optind; i < argc; ++i) {
    printf("arg left: %s\n", argv[i]);
  }
  printf("execvp %s, %d\n", run_f, optind);
  return execvp(run_f, argv + optind);
}

int main(int argc, char *argv[]) {
  char *name;
  struct rcli cli;
  int ret = 0;

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
