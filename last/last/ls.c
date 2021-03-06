#include "ls.c"

char cwd[32];
char buf2[1024];

char *t1 = "xwrxwrxwr-------";
char *t2 = "-----------------";


int main(int argc, char *argv[ ]) {

  struct stat* sbuf, mystat;
  int n, r;
  int fd;
  int i;

  sbuf = &mystat;

  getcwd(cwd);
  if (argc > 1) {
    fd = open(argv[1], O_RDONLY);
    strcpy(cwd, argv[1]); 
  }
  else {
    fd = open(cwd, O_RDONLY);
  }


  chdir(cwd);
  printf("cwd = %s\n", cwd);


  if (fd < 0) {
    printf("File not opened successfully\n");
    close(fd);
    return 0;
  }

  n = read(fd, buf2, 1024);
  char* cp = buf2;
  DIR* dp = (DIR*)cp;

  i = 0;

  char copy[32];

  while (cp < buf2 + 1024) {
    memset(copy, 0, 32);
    strncpy(copy, dp->name, dp->name_len);     
    strcat(copy, "\0");

    if ((r = stat(copy, sbuf)) < 0) {
      printf("cannot stat %s\n\r", dp->name);
    }
    else {
      ls_entry(copy, sbuf);
    }

    cp += dp->rec_len;  
    dp = (DIR*)cp;
    i++;
  }

  i = 0;

  close(fd);
}

int ls_entry(char* name, struct stat *sp){

  if (sp->st_mode == 0x41ed) {
    mputc('d');
  }
  else {
    mputc('-');
  }

  int i;
  for (i = 8; i >= 0; i--) {
    if (sp->st_mode & (1<<i)) {
      mputc(t1[i]);
    }
    else {
      mputc(t2[i]);
    }
  }

  printf("  %d  ", sp->st_nlink);
  printf("%d  ", sp->st_uid);
  printf("%d  ", sp->st_gid);
  printf("%d  ", sp->st_size);
  printf("    %s\n\r", name);
}
