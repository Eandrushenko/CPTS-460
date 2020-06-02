/********** login.c file *************/

#include "ucode.c"
char buf[1024];

// upon entry, argv[0]=login, argv[1]=/dev/ttyX  (virtual console)
int main(int argc, char *argv[ ]) {
  int i;
  int in, out, err;
  int passfd, n;

  char name[128];     // stores username
  char pass[128];     // stores password (from passwd file)

  if (argc < 2) {
    printf("ERROR - need at least 2 arguments\n");
    printf("Sample usage: login /dev/tty0\n");
    return -1;
  }

  //fd from init
  close(0);
  close(1);

  in =  open(argv[1], 0);
  out = open(argv[1], 1);
  err = open(argv[1], 2);

  // set tty name in the procs
  settty(argv[1]);

  //confirm username and passwords
  passfd = open("/etc/passwd", O_RDONLY);

  while (1) {

    prints("username:\n\r"); gets(name);
    prints("password:\n\r"); gets(pass);

    n = read(passfd, buf, 1024);

    if (n <= 0) {
      prints("ERROR - no passwords were found.\n");
      close(passfd); close(in); close(out); close(err);
      return -1;
    }

    //token is username
    int toki = 0;
    char token[256];
    int result = strtok(buf, token, '\n', toki++);
    if (!result) { printf("nothing found!\n\r"); return -1; }

    //look through every line in the password file
    while (token) {
      char token_name[32];
      strtok(token, token_name, ':', 0);

      if (!strcmp(token_name,name)) {
        char token_pass[32];
        strtok(token, token_pass, ':', 1);
        printf("pass = %s\n\r", token_pass);

        //password and password file have a match
        if (!strcmp(token_pass, pass)) {
          char uid[16];
          char gid[16];
          char am[32];
          char alias[16];
          char mode[16];
          char directory[16];
          char execcmd[16];

          strtok(token, uid, ':', 2);//username
          strtok(token, gid, ':', 3);//password
          strtok(token, am, ':', 4);
          strtok(am, mode, ' ', 1);
          strtok(am, alias, ' ', 0);
          strtok(token, directory, ':', 5);
          strtok(token, execcmd, ':',6);

          //set  username and password to users username and password
          //set the current working directory to the user's home directory
          //close password file

          printf("uid=%s, gid=%s, alias=%s, mode=%s, dir=%s, cmd=%s\n",
                  uid, gid, alias, mode, directory, execcmd);

          chuid(atoi(uid), atoi(gid));        //username and password
          chdir(directory);

          close(passfd);

          printf("Welcome %s. Taking you to %s\n\r", token_name, directory);
          exec(execcmd);
          return 1;
        }
      }

      memset(token, 0, 32);
      strtok(buf, token, 10, toki++);

      if (token[0] == 0) {
        printf("ERROR - login failed\n\r");
        break;

        close(passfd);
        passfd = open("/etc/passwd", O_RDONLY);
      }
    }
  }
  printf("exit\n");
}
