/********** grep.c file *************/

#include "ucode.c"

char buf[1024];
char tty[32];

int main(int argc, char *argv[ ]) {

  prints("GREP\n\r");

  int i, j, k, n, length;
  int in, out;
  char* pattern;
  char line[128];
  char c;

  gettty(tty);

  int outtty = open(tty, O_WRONLY);

  if (argc < 2) {
    prints("Not enough arguments\n\r");
    return -1;
  }

  // STDIN
  if (argc == 2) {
    pattern = argv[1];
    in = 0;
    out = 1;
  }

  // FILE
  else {
    pattern = argv[1];
    in = open(argv[2], O_RDONLY);
    if (in < 1) {
      printf("Bad file: %s\n\r", argv[2]);
      return -1;
    }
    out = 1;
  }

  //Check if it is a proper pattern
  if (!pattern) { return -1; }

  //caculate the length of the pattern
  length = 0;
  i = 0;
  while (pattern[length] != '\0') {
    length++;
  }

  i = j = k = 0;
  int to_print = 0;
  memset(line, 0, 128);
  while (1) {
    n = read(in, buf, 1);  	  //Read in from the buf one byte at a time
    if (n < 1) { break; }         // exit if we have run out of bytes


    line[i] = buf[0];


    if (buf[0] == 10) {
      line[++i] = 13; 
      if (to_print) {
        write(out, line, i);
        write(outtty, "\r", 1); 

      }
      to_print = 0;
      i = j = 0;
      memset(line, 0, 128);
    }

    else {
      if (line[i] == pattern[j]) {
        j++;
        if (j == length && !to_print) {
          to_print = 1;
        }
      }
      else {
        j = 0;
      }
      i++;
    }
  }
  close(in); close(out);
}
