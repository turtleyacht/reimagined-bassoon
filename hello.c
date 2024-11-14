#include <stdio.h>
#include <stdlib.h>
#include "board.c"

extern char letters[LEN];
extern char adjacents[LEN];
extern short int states[LEN];

#define TERMINAL_LEN 1000

void prompt(void);

int main(void) {
  printf("%lu\n", sizeof(letters));

  char choice;
  char display[TERMINAL_LEN];
  char word_to_add[LEN];
  int i;

  prompt();

  while ((choice = getchar())) {
    switch(choice) {
      case 'a':
        render_board(display, TERMINAL_LEN);
        printf("%s\n\n", display);
        printf("word? ");
        scanf("%s", word_to_add);
        /* toupper */
        printf("Begin %s at which position? ", word_to_add);
        /* scanf or getchar() */
        /* search valid directions only */
        printf("direction? ");
        /* getchar() */
        break;
      case 'd':
        render_board(display, TERMINAL_LEN);
        printf("%s\n", display);
        break;
      case 'q':
        exit(0);
        break;
      default:
        prompt();
        break;
    }
  }

  return 0;
}

void prompt() {
  printf("(a)dd word, (d)isplay, (q)uit\n");
  printf("? ");
}
