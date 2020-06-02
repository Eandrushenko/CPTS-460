/********** pipe.c file *************/
#include "ucode.c"



char buf[1024];
char uinput[128];

char args[10][32];//tokenize pipe arguments
char ios[10][2];//io redirection

int child;


int main(int argc, char *argv[ ]) {

  int i;
  int n;
  int in, out;
  int status;
  char tty[32];
  gettty(tty);

  int stdin = 0;

  while (1) {

    for (i = 0; i < 10; i++) {
      memset(args[i], 0, 32);       
      memset(ios[i], 0, 2);        
    }
    i = 0;

    printf("\n\r scsh #%d $ ", getpid());//receive input
    gets(uinput);

    if (uinput[0] == 0) {
      continue;        
    }

    if (!strcmp(uinput, "logout")) {
      break;
    }


    while(strtok(uinput, args[i], '|', i)) {      //tokenize pipes into arguments
      i++;
    }

    int count = i;     
    i--;
    printf("count = %d\n\r", count);

    char* cp;
    int len;

    //clear whitespace
    for (; i >= 0; i--) {
      cp = args[i];              
      len = 0;
      while(*cp == ' ') { cp++; }
      strcpy(args[i], cp);
      printf("args[%d] = %s\n\r", i, args[i]);

      //check for io redirection
      int j = 0;
      while (args[i][j]) {
        if (args[i][j] == '>') {
          if (args[i][j+1] == '>') {      //check if its append
            strcpy(ios[i], ">>");
            break;
          }
          else {
            strcpy(ios[i], ">");          //check if its create
            break;
          }
        }
        else if (args[i][j] == '<') {     //check if it takes input
          strcpy(ios[i], "<");
          break;
        }
        j++;
      }
    }

    i = 0;
    for (i = 0; i < 3; i++) {
      printf("args[i] = %s\t", args[i]);
      printf("ios[i] = %s\n\r", ios[i]);
    }


    i = 0;

    child = fork();

    if (child) {
      wait(&status);        // wait for child to die
    }
    else { 

      if (count == 2) {
        prints("CREATING PIPE\n\r");

        if (ios[0][0] != 0) {
          handle_IO(args[0], ios[0]);
        }

        if (ios[1][0] != 0) {
          handle_IO(args[1], ios[1]);
        }

        do_pipe(args[0], args[1]);      //recursive call
      }

      else if (count == 1) {
        if (ios[0][0] != 0) {
          handle_IO(args[0], ios[0]);
        }
        exec(args[0]);
      }
    }

    memset(uinput, 0, 128);
    i = 0;

  }
}


int handle_IO(char* arg, char* IO) {
  char cmd[32];
  char file[32];
  int type;
  int fd;

  int i=0;

  memset(cmd, 0, 32);
  memset(file, 0, 32);

  while(arg[i]) {
    if (arg[i] == '<' || arg[i] == '>') {
      i++;
      break;
    }
    else {
      cmd[i] = arg[i]; 
    }
    i++;
  }

  int j = 0;
  while (arg[i] == '>' || arg[i] == ' ') {  
    i++;
  }

  while (arg[i] && arg[i] != ' ') {
    file[j] = arg[i];
    j++;
    i++;
  }



  printf("cmd = %s\n\r", cmd);
  printf("file = %s\n\r", file);

  strcpy(arg, cmd);


  //IO redirection
  if (!strcmp(IO, ">")) {
    fd = open(file, O_WRONLY|O_CREAT);  //open
    if (fd<=0) { printf("ERROR - Couldn't open/create file %s\n\r", file); return -1; }

    dup2(fd, 1); 
                    //redirect stdout to file
    close(fd); 
    exec(cmd);   
    return 1;
  }

  if (!strcmp(IO, ">>")) {
    fd = open(file, O_WRONLY|O_CREAT|O_APPEND);  
    if (fd<=0) { printf("Couldn't open/create file %s\n\r", file); return -1; }

    dup2(fd, 1);
    close(fd);
    exec(cmd);
    return 1;
  }

  if (!strcmp(IO, "<")) {
    fd = open(file, O_RDONLY);       //unlike the others this one is read only
    if (fd<=0) { printf("Couldn't open file %s\n\r", file); return -1; }

    dup2(fd, 0);    //redirect stdin instead
    close(fd);
    exec(cmd);
    return 1;
  }
}


int do_pipe(char* cmd1, char* cmd2){        
  int pid, pd[2];
  int status;
  pipe(pd);     //pd[0] = READ, pd[1] = WRITE
  pid = fork(); 
  if (pid) {       
    close(pd[1]);
    dup2(pd[0], 0);     //redirect stdin to pipe
    exec(cmd2);
  }
  else {    
    close(pd[0]);   
    dup2(pd[1], 1);     //redirect stdout to pipe
    exec(cmd1);
  }
}
