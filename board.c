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

/* Total compact length of our string representation (_not_ the display "string") */
#define LEN 24

/* Number of digits in LEN */
#define NUMERIC_PLACES 2

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
int fill_positions(int, int, char[]);
char ntoc(int);
int powten(int);
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
  char last_adjacent = '?';

  /* Tells us next index okay to fill */
  int offset = -1;

  /* Maybe `return' was treated as a jump like `goto', so both were normal. */
  /* Maybe you can tell by implementation if a function is a wrapper or low-level. */
  /* Wrapper: reads more like a paragraph than low-level twiddling */
  /* Granularity in interactivity reflects how many knobs can be adjusted in auto. */

  for (bi = 0; bi < LEN; bi++) {
    /* fill[di++] = ' '; */
    fill[di++] = letters[bi];
    /* fill[di++] = ' '; */

    switch(adjacents[bi]) {
      case '3':
        if (last_adjacent == '5' && st == 0) {
          offset = fill_positions(bi, di, fill);
          di += offset;
          fill[di++] = ';';
          fill[di] = '\n'; /* first row, second corner */
          st++;
        } else if (last_adjacent == '5' && st == 1) {
          st++; /* last row, first corner */
        } else if (last_adjacent == '5' && st == 2) {
          offset = fill_positions(bi, di, fill);
          di += offset;
          fill[di++] = ';';
          fill[di] = '\n'; /* last row, second corner */
          st = 0;
        }
        break;
      case '5':
        if (last_adjacent == '8') {
          offset = fill_positions(bi, di, fill);
          di += offset;
          fill[di++] = ';';
          fill[di] = '\n';
        }
        break;
    }

    last_adjacent = adjacents[bi];
    assert(st == 0 || st == 1 || st == 2);
    di++;
  }
  assert(last_adjacent != '?' && last_adjacent == '3');
  assert(st == 0);
  fill[di] = '\0';

  return 0;
}

/* Fill the board with the terminal position of current row.
   Return offset to caller.
*/
int fill_positions(int i, int start, char fill[]) {
  int numeral = -1;
  char digit = '?';
  int exp;
  int pos = start;
  int tens_counter = NUMERIC_PLACES - 1; /* ugh, minus 1 on a constant */

  /*
     173 is 100 + 70 + 3
     173 has 3 NUMERIC_PLACES
     NUMERIC_PLACES is 0-based: 2, 1, 0
     173 / 100 = 1   173 / (10^2) = 1
     173 % 100 = 73  173 % (10^2) = 73
     73 / 10 = 7     73 / (10^1) = 7
     73 % 10 = 3     73 % (10^1) = 3
     3 / 1 = 3       3 / (10^0) = 3
     3 % 1 = 3       3 % (10^0) = 3
   */

  while (tens_counter >= 0) {
    exp = powten(tens_counter);
    if (exp > i) {
      numeral = i;
    } else {
      numeral = i / exp;
    }

    digit = ntoc(numeral);

    if (exp > i && tens_counter > 0) {
      fill[pos++] = '_';
    } else {
      fill[pos++] = digit;
    }
    i = i % exp;
    tens_counter--;
  }
  return pos - start;
}

/* Convert numeral integer to its character representation. */
char ntoc(int numeral) {
  assert(numeral < 10);
  switch (numeral) {
    case 0: return '0';
    case 1: return '1';
    case 2: return '2';
    case 3: return '3';
    case 4: return '4';
    case 5: return '5';
    case 6: return '6';
    case 7: return '7';
    case 8: return '8';
    case 9: return '9';
    default: assert(1 == 2);
  }
}

/* 10^0 = 1 (base case)
   10^1 = 10 * 1 = 10 = 10 plus itself 0 times
   10^2 = 10 * 10 = 100 = 10 plus itself 9 times
   10^3 = 10 * 10 * 10 = 1000 = ((10 plus itself 9 times) plus itself 9 times)
*/
int powten(int n) {
  assert(n <= NUMERIC_PLACES);

  if (n == 0) {
    return 1;
  } else if (n == 1) {
    return 10;
  }

  int base = 10;
  int i, j;
  int sum = 10;
  for (i = 0; i < n; i++) {
    for (j = 0; j < 9; j++) {
      sum += base;
    }
    base = sum;
  }
  return sum;
}
