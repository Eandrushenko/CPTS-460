/********** more.c file *************/
#include "ucode.c"

char buf[1024];
char buf2[1024];

int main(int argc, char *argv[ ]) {

  int fd, n;
  int in, out;

  char tty[32];
  gettty(tty);
  int outtty = open(tty, O_WRONLY);
  int intty = open(tty, O_RDONLY);

  int lines = 0;
  int stopprint = 0;

  write(outtty, "MORE\n\r", 14);

  if (argc < 2) {
    in = 0;
    out = 1;
  }
  else {
    in = open(argv[1], O_RDONLY);
    out = 1;
  }

  while ((n = read(in, buf, 1)) == 1) {//read in 1 byte at a time

    if (!in) { write(outtty, buf, 1); }//spit it out if its stdin
    else { write(out, buf, 1); }

    if (buf[0] == '\n') {
      lines++;
      if (lines == 25) {
        stopprint=1;
      }
      if (stopprint) {

        while (1) {
          char c = 0;

          c = mygetc(intty);

          if (c == '\n' || c == '\r') { break; }

          else if (c == ' ') {
            lines = 0;
            stopprint = 0;
            break;
          }
        }
      }
      prints("\r");
    }
  }

  close(in); close(out); close(outtty);
}
