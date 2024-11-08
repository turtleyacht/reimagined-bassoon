/* board.c */

#include <assert.h>

/* The original board is 30x30 letters in print. */

/* Sample board:

     A A A A A A
     A A A A A A
     A A A H I A
     A A A A A A

   Having the A's here gives a kind of gradient effect.
   Or just me?
*/

#define LEN 24

/* These variables describe the board. */

/* The letters used */
char letters[LEN] =   "               HI       ";

/* Each position is aware of its neighbors. */
/* We could simplify which corner with more numbers, but that would muddle
   the idea of "number of directions" and "nw ne se sw corner." Tradeoffs.
 */
char adjacents[LEN] = "355553588885588885355553";

/* Unavoidably, retain state in another variable . */
/* Express positions clockwise:

     0 1 2                    0 1 2 3 4 5 6 7
     7 H 3   translates to    0 0 0 1 0 0 0 0
     6 5 4

   Which just happens to be eight bits, or a byte!

     0 1 2                    0 1 2 3 4 5 6 7
     7 I 3   translates to    0 0 0 0 0 0 0 1
     6 5 4
*/
short int states[LEN]    = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 1, 0, 0, 0, 0, 0, 0, 0 };

/* Of course, we'll need functions to initialize these.
   The board will be too large to do by hand.
*/

int clear_display(char[], int);
int render_board(char[], int);

int clear_display(char data[], int len) {
  int i;
  for (i = 0; i < len; i++) {
    data[i] = '.';
  }
  data[i-1] = '\0';
  return 0;
}

int render_board(char fill[], int len) {
  assert(LEN <= len);
  clear_display(fill, len);

  /* Board index */
  int bi;

  /* Display index (terminal) */
  int di = 0;

  /* State tracking */
  int st = 0;

  for (bi = 0; bi < LEN; bi++) {
    /* fill[di++] = ' '; */
    fill[di++] = letters[bi];
    /* fill[di++] = ' '; */

    printf("%d %c\n", st, adjacents[bi]);
    if (st > 0 && adjacents[bi] == '3') {
      fill[di] = '\n';
    }

    switch(adjacents[bi]) {
      case '3':
        if (st == 0) {
          st++;
        } else {
          st--;
        }
        break;
      case '5':
        if (st == 0) {
          st++;
        } else {
        }
        break;
    }
    assert(st == 0 || st == 1);
    di++;
  }
  fill[di] = '\0';

  return 0;
}
