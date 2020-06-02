/********** cat.c file *************/
#include "ucode.c"

char buf[24];
char string[128];
char tty[32];

int main(int argc, char *argv[ ]) {

  int n;
  int in, out;
  int i;

  gettty(tty);
  int ttyfd = open(tty, O_WRONLY);

  //STDIN
  if (argc == 1) {
    in = 0;
    out = 1;
  }

  // FILE
  else {
    in = open(argv[1], O_RDONLY);
    out = 1;
  }

  i = 0;

  while(1) {
    n = read(in, buf, 1);      //read 1 byte from input
    if (n < 1) { break; }         //if we get nothing, break out
    string[i] = buf[0];

    if (!in) { write(ttyfd, buf, 1); }//cat without parameters


    //new line
    if (string[i] == '\n') {    
      i++;
      string[i] = '\r';        

      write(out, string, i);
      write(ttyfd, "\r", 1);     //return cursor without stuffing the string

      memset(string, 0, 128);
      i = 0;
    }

    //"enter" key pressed, STDIN
    else if (string[i] == '\r' && !in) {
      string[i++] = '\n';
      string[i++] = '\r';
      write(ttyfd, "\n\r", 2);     //return cursor without stuffing the string
      write(out, string, i);
      memset(string, 0, 128);
      i = 0;
    }

    else {
      i++;
    }
  }

  close(in); close(out); close(ttyfd);
}
