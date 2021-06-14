#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include <include/shell.h>
#include <include/utils.h>
#include <include/disarm.h>

config_t image;

char *function = NULL;

#define which_eydis "1.1"

char eydis_database[32];

// What do you like in a reversing tool?

/************** main **************/

void process_handler(int quit) {
  signal(quit, SIG_IGN);

  exit_eydis();
}

void usage(char *argv[]) {
  char *owo = NULL;

  owo = strrchr(argv[0], '/');

  printf("usage: %s [-d] <image> [options]\n", owo ? owo + 1 : argv[0]);
  printf("   -w [file]\tspecify a database,\n");
  printf("   -b [addr]\tspecify the rebase address,\n\n");

  printf("   -e [offset]\tend to a specified offset,\n");
  printf("   -s [offset]\tstart from a specified offset,\n\n");

  printf("   -a\t\tanalyze the aera until an ending offset.\n\n");

  exit(-1);
}

int main(int argc, char *argv[]) {
  int dis = 0;
  int spec = 0;
  int version = 0;

  image.analyze = 0;

  image.start = 0x0;
  image.base = 0x0;
  image.end = 0x0;

  printf("\n\t\t\t\t\t   eydis disassembler\n");
  printf("\t\t\t\t\tversion %s | haiyuidesu\n\n", which_eydis);

  if (argc < 3) usage(argv);

  int arg_counter = argc - 1;

  signal(SIGINT, process_handler);

  while (arg_counter) {
    if (*argv[arg_counter] == '-') {

      char arg = *(argv[arg_counter] + 1);

      switch (arg) {
        case 'd':
          if (argv[arg_counter + 1] != NULL) {
            image.filename = argv[arg_counter + 1];

            dis = 1;
          }

          break;
        case 'w':
          if (argv[arg_counter + 1] != NULL) {
            sprintf(eydis_database, "%s", argv[arg_counter + 1]);

            spec = 1; // i need to know if the database was specified

            x = 1; // the database already have the soubroutines addresses
          }

          break;
        case 'b':
          if (argv[arg_counter + 1] != NULL) {
            image.base = strtol(argv[arg_counter + 1], NULL, 0x10);
          }

          break;
        case 's':
          if (argv[arg_counter + 1] != NULL) {
            image.start = strtol(argv[arg_counter + 1], NULL, 0x10);
          }

          break;
        case 'e':
          if (argv[arg_counter + 1] != NULL) {
            image.end = strtol(argv[arg_counter + 1], NULL, 0x10);
          }

          break;
        case 'a':
          image.analyze = 1;

          break;
        default:
          printf("[%s]: warning: unrecognized argument: %s.\n\n", __func__, argv[arg_counter]);

          usage(argv);
      }
    }

    arg_counter--;
  }

  if (dis) {
    printf("[%s]: starting...\n", __func__);

    if (image.analyze == 1 && image.end <= 0) {
      printf("\n[%s]: refusing to avoid a full analysis without any ending limit set.\n", __func__);
      return -1;
    }

    FILE *fd = fopen(image.filename, "rb");

    if (!fd) {
      printf("[%s]: unable to open %s.\n", __func__, image.filename);
      return -1;
    }

    fseek(fd, 0x0, SEEK_END);

    image.length = ftell(fd);

    fseek(fd, 0x0, SEEK_SET);

    image.img = malloc(image.length);

    fread(image.img, image.length, 1, fd);

    fflush(fd);
    fclose(fd);

    if (image.end == 0) {
      image.end = image.length;
    }

    // for those who are often dealing with Apple bootloaders

    if (memmem(image.img, image.length, "\x69\x42\x6F\x6F", 0x4)) {
      image.filetype = "iBoot";

      if (memmem(image.img, image.length, "\x72\x65\x52\x4F", 0x4)) {
        image.filetype = "SecureROM";
      }

      version = atoi(image.img + 0x286);

      image.base = *(uint32_t *)(image.img + 0x20) & ~0xfffff; // thank you @iH8Sn0w!

      switch (image.base) {
        case 0x0:
        case 0x84000000:
        case 0x5ff00000:
        case 0xbff00000:
        case 0x10000000:
          break;
        default:
          image.base = *(uint64_t *)(image.img + ((version >= 6723) ? 0x300 : 0x318));
          break;
      }
    } else if (memmem(image.img, image.length, "\x64\x2E\x2E\x2E\x28\x72\x6F\x6F", 0x8)) {
      image.base = 0x240000000;

      image.filetype = "SEPROM";

      version = atoi(image.img + 0xC2B);
    } else if (memmem(image.img, image.length, "\x65\x53\x45\x50", 0x4)) {
      image.base = 0x10000000;

      image.filetype = "SEPROM";

      version = atoi(image.img + 0x80C);
    } else {
      image.filetype = "ROM";

      version = 0;
    }

    if (spec == 0) {
      sprintf(eydis_database, "%s-%d", ".eydis", version); // no database were specified | found so i just create a new one
    }

    printf("[%s]: \033[38;5;227mbase_addr\033[30;0m = \033[38;5;196m0x%llx\033[30;0m\n\n", __func__, image.base);

    if (image.analyze == 1) {
      image.length = image.end;
    }

    printf("[%s]: loading file into database "
           "(\033[38;5;196m0x%llx\033[30;0m - \033[38;5;196m0x%llx\033[30;0m)...\n\n",
           __func__, image.base, (image.base + image.length));

    if (access(eydis_database, F_OK) == -1) {
      create_database();
    }

    if (disarm() != 0) {
      free(image.img);

      return -1;
    }

    eydish();
  }

  return 0;
}