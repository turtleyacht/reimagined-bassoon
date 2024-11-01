#include <stdio.h>
#include "board.c"

extern char letters[LEN];
extern char adjacents[LEN];
extern short int states[LEN];

int main(void) {
  printf("%lu\n", sizeof(letters));
  return 0;
}

