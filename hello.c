#include <stdio.h>
#include "board.c"

extern char letters[LEN];
extern char adjacents[LEN];
extern short int states[LEN];

#define TERMINAL_LEN 1000

int main(void) {
  printf("%lu\n", sizeof(letters));

  char choice;
  char display[TERMINAL_LEN];
  int i;

  printf("(d)isplay, (q)uit\n");
  printf("? ");

  switch(choice = getchar()) {
    case 'd':
      render_board(display, TERMINAL_LEN);
      printf("%s\n", display);
      break;
    case 'q':
      break;
  }

  return 0;
}

