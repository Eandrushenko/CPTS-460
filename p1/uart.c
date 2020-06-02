/********************************************************************
Copyright 2010-2017 K.C. Wang, <kwang@eecs.wsu.edu>
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
********************************************************************/
#define DR   0x00
#define FR   0x18

#define RXFE 0x10
#define TXFF 0x20

typedef struct uart{
  char *base;
  int n;
}UART;

UART uart[4];

int BASE = 10;
char *table = "0123456789ABCDEF";

int uart_init()
{
  int i; UART *up;

  // for (i=0; i<4; i++){
  // up = &uart[i];
  //up->base = (char *)(0x101F1000 + i*0x1000);
  // up->n = i;
  //}
 
  uart[0].base = (char *)(0x10009000);
  uart[0].n = 0;

  uart[1].base = (char *)(0x1000A000);
  uart[1].n = 1;

  uart[2].base = (char *)(0x1000B000);
  uart[2].n = 2;

  uart[3].base = (char *)(0x1000C000);
  uart[3].n = 0;

  
}

int ugetc(UART *up)
{
  while (*(up->base + FR) & RXFE);
  return *(up->base + DR);
}

int uputc(UART *up, char c)
{
  while(*(up->base + FR) & TXFF);
  *(up->base + DR) = c;
}

int ugets(UART *up, char *s)
{
  while ((*s = (char)ugetc(up)) != '\r'){
    uputc(up, *s);
    s++;
  }
 *s = 0;
}

int uprints(UART *up, char *s)
{
  while(*s)
    uputc(up, *s++);
}

//////////////////
int urpu(UART *up, unsigned long x)
{
  char c;
  if (x){
     c = (char)((x % 10) + '0');
      urpu(up,(x / 10));
     uputc(up,c);
  }
}

int urpul(UART *up, unsigned long x)
{
  char c;
  if (x){
     c = (char)((x % 10) + '0');
     urpul(up,(x / 10));
     uputc(up,c);
  }
}

int uprintu(UART *up, unsigned int x)
{
  if (x==0)
    uputc(up,'0');
  else{
    urpu(up,x);
    uputc(up,' ');
  }
}

int uprintl(UART *up, unsigned long x)
{
  if (x==0)
    uputc(up,'0');
  else{
    urpul(up,x);
    uputc(up,' ');
  }
}

int uprintd(UART *up, int x)
{
  if (x==0){
    uputc(up,'0');
  }
  else if (x < 0)
    {
      x = -x;
      uputc(up,'-');
    }
  urpu(up,x);
}

int uprinto(UART *up, unsigned int x)
{
  BASE = 8;
  uprintu(up,x);
  BASE = 10;
}

int uprintx(UART *up, unsigned int x)
{
  BASE = 16;
  uprintu(up,x);
  BASE = 10;
}


int fuprintf(UART *up, char *fmt, ...)
{
  int *inptr = &fmt + 1;
  char *cp = fmt;

  while(*cp)
    {
      if (*cp == '%')
	switch (*(++cp)) {
        case 'u':
	  uprintu(up,*inptr++);
	  break;
        case 'l':
	  uprintl(up,* (unsigned int *)inptr++);
	  break;
	case 'd':
	  uprintd(up,*inptr++);
	  break;
	case 'o':
	  uprinto(up,*inptr++);
	  break;
	case 'x':
	  uprintx(up,*inptr++);
	  break;
	case 's':
	  uprints(up,*inptr++);
	  break;
	case 'c':
	  uputc(up,*inptr++);
	  break;
	}
      else
	{
	  if(*cp == '\n')
	    {
	      uputc(up,'\n');
	      uputc(up,'\r');
	    }
	  else
	    {
	      uputc(up,*cp);
	    }
	}
      cp++;
    }
}
