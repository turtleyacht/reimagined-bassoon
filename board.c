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

int board_row(int);
int clear_display(char[], int);
int fill_positions(int, int, char[]);
int fill_directions(int[], int);
int fit_word(char[], int);
char ntoc(int);
int powten(int);
int render_board(char[], int);
int reset_direction_bound(int[], int, int, int, int);
int row_offset(void);

/* Determine which row a board index falls on. Board is stored as a string. */
int board_row(int position) {
  assert(position >= 0 && position < LEN);

  /* If 0 is a valid answer, and it's used as a loop counter, start at -1 */
  /* But you can't, because it must be calculated starting from zero. */
  /* That's why you need the `break' later. */
  int current_row = 0;

  int max_extents_index = LEN;

  /* Our "unit ruler." */
  int window_size = row_offset();

  /* Offset becomes an absolute. */
  int row_len = window_size + 1;

  /* We could return early here for the zero case (when current_row should be */
  /* 0) but then we lose post-condition assertions. */

  for (; position < max_extents_index; current_row++) {
    max_extents_index = (current_row * row_len) + window_size;

    /* We never see this in the earliest for-loop examples because it doesn't */
    /* appear so elegant. Yet we must abort for `current_row' because it      */
    /* means something before it's incremented. Without the abort, when the   */
    /* loop exits, `current_row' is 1 when we wanted 0, and 1 when we want 1. */
    if (position <= max_extents_index && current_row == 0) {
      break;
    }
  }

  assert(max_extents_index > 0 && max_extents_index < LEN);

  return current_row;
}

int clear_display(char data[], int len) {
  int i;
  for (i = 0; i < len; i++) {
    data[i] = '.';
  }
  data[i-1] = '\0';
  return 0;
}

/* Return derived result of the maximum rows */
int fill_directions(int directions[], int start) {
  int current_row = 0;
  int i;
  int reset_count = 0;

  int window_size = row_offset(); /* unit ruler idea */

  /* Calculate offsets first, regardless of bounds */
  directions[0] = start + -window_size - 2;
  directions[1] = start + -window_size - 1;
  directions[2] = start + -window_size;
  directions[3] = start + 1;
  directions[4] = start + window_size + 2;
  directions[5] = start + window_size + 1;
  directions[6] = start + window_size;
  directions[7] = start - 1;

  printf("0nw: %d, 1n: %d, 2ne: %d, 3e: %d, 4se: %d, 5s: %d, 6sw: %d, 7w: %d\n",
    directions[0], directions[1], directions[2], directions[3],
    directions[4], directions[5], directions[6], directions[7]);

  /* To make refactoring easier: init vs assignment close to usage site */
  /* Determine absolute_row because i is a data index, not board index */
  current_row = board_row(start);

  for (i = 0; i < 8; i++) {
    /* The largest index for the current row */
    int max_extents_index = (current_row * (window_size + 1)) + window_size;

    assert(max_extents_index < LEN);

    /* The smallest index for the current row */
    int min_extents_index = max_extents_index - window_size;

    /* Sanity check the bounds */
    /* TODO prefix with maybe_ */
    if (reset_direction_bound(directions, start, i,
          min_extents_index, max_extents_index)) {
      reset_count++;
    }

    assert(max_extents_index < LEN);
  }

  return reset_count;
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
     3 % 1 = 0       3 % (10^0) = 0
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

/* Return direction as numeric just like in states[]
   The directions are clockwise, so

     0 1 2
     7   3          0 1 2 3 4 5 6 7
     6 5 4
*/
int fit_word(char word[], int start) {
  printf("Fit %s at position %d\n", word, start);
  /* Build out a direction map, i.e. orient yourself */
  /* Search is travel. */

  /* Record indices for each direction */
  int directions[] = { -1, -1, -1, -1, -1, -1, -1, -1 };

  int max_rows = fill_directions(directions, start);



  /* Check if letter already occupies that direction (states[]) */

  /* Return something useful */

  /* refactor the below based on unit ruler */

  char num_neighbors = adjacents[start];

  /* We're ants moving across a string; at least it's not braided. */
  int bound_left = start;
  int bound_right = start;

  /* As corner, seek row start or row end. */
  if (num_neighbors == '3' && start == 0) { /* nw corner */
    for (; bound_right < LEN; bound_right++) {
      if (adjacents[bound_right] == '3') {
        break;
      }
    }
  } else if (num_neighbors == '3' && start == (LEN-1)) { /* se corner */
    for (; bound_left > 0; bound_left--) {
      if (adjacents[bound_left] == '3') {
        break;
      }
    }
  } else if (num_neighbors == '3' && adjacents[start-1] == '5' &&
    adjacents[start-2] == '8' &&
    adjacents[start+1] == '5') { /* sw corner */
    for (; bound_right < LEN; bound_right++) {
      if (adjacents[bound_right] == '3') {
        break;
      }
    }
  } else if (num_neighbors == '3' && adjacents[start-1] == '5' &&
    adjacents[start+1] == '5' &&
    adjacents[start+2] == '8') { /* ne corner */
    for (; bound_left > 0; bound_left--) {
      if (adjacents[bound_left] == '3') {
        break;
      }
    }
  }

  /* Left rank just after corner */
  /* Right rank just before corner */
  /* Middle days */
  assert(bound_left <= start && bound_right >= start);
  assert(bound_left != bound_right);

  int neighbors[8] = { -1 };
  return 0;
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

/* Set directions[direction] to -1 if it's invalid based on other parameters:

   direction is the **one** index we are interested in keeping or resetting.
   directions are the eight indices around the position, clockwise. 0 is northwest.
   {min,max}_extents_index represent the valid index range for current row.
   position is the index on the board for the candidate letter.

   Return 0 on success, 1 if reset.
*/
int reset_direction_bound(int directions[], int position, int direction,
  int min_extents_index, int max_extents_index) {

  printf("min_extents_index=%d max_extents_index=%d\n", min_extents_index,
    max_extents_index);

  assert(direction >= 0 && direction <= 7);
  assert(position >= min_extents_index && position <= max_extents_index);

  /* You must accept negative indices in order to reset them...
     Otherwise, caller must incorporate logic around "valid and invalid" directions,
     which is what this method should do. */
  /* assert(directions[direction] >= 0); */

  int change_count = 0;
  int orig_index = directions[direction];

  switch (direction) {
    case 0: /* northwest */
      break;
    case 1: /* north */
      break;
    case 2: /* northeast */
      break;
    case 3: /* east */
      printf("Check east.\n");
      if (directions[direction] > max_extents_index) {
        directions[direction] = -1;
        change_count++;
      }
      break;
    case 4: /* se */
      break;
    case 5: /* s */
      break;
    case 6: /* sw */
      break;
    case 7: /* w */
      if (position == min_extents_index && directions[direction] < min_extents_index) {
        directions[direction] = -1;
        change_count++;
      }
      break;
    /* Avoiding early exit above to stay in line with assertions. */
    default:
      if (directions[direction] < 0) {
        directions[direction] = -1;
        change_count++;
      } else {
        assert(1==2);
      }
  }

  if (directions[direction] < 0) {
    printf("Reset direction %d at %d.\n", direction, orig_index);
  }

  assert(change_count <= 1);
  assert(directions[direction] == orig_index || directions[direction] == -1);

  return change_count;
}

/* A "unit ruler" based on the first row */
/* Treated as an offset, it becomes a window like so:

               0             1             2             3             4
   A A A A A A _ x x x x x _ _ x x x x _ _ _ x x x _ _ _ _ x x _ _ _ _ _ x
   A A A A A A             x           x x         x x x       x x x x
   A A A A A A
   A A A A A A           5
               _ _ _ _ _ _
               x x x x x

   Underscores are before the given index, inclusive of index.
   The x's always cover or highlight one before the index, in the next row.

   (The last row is a special case.)

   Now we can build up some semantics. Based on surroundings,

     0 1 2 Direction
     7   3   0 (nw): ?
     6 5 4   1 (up): ?
             2 (ne): ?
             3 (right): index+1
             4 (se): offset+2
             5 (down): offset+1
             6 (sw): offset
             7 (left): index-1

   What does the window look like as a negative offset?

     x x x x x     x x x x       x x x         x x           x
   0             1             2             3             4             5
   _           x _         x x _       x x x _     x x x x _   x x x x x _

   We can fill in the rest, with a caveat (*) for last position in a row:

     0 (nw): -offset-2
     1 (up): -offset-1
     2 (ne): -offset (*)

   At the last position in the row, ne is invalid (and technically se too).

   The last index in a row should always be equal to

     row-size = offset + 1
     try-last-row-index = ([0, 1, ..., LEN-1] * row-size) + offset

   Where we would loop over 0..LEN-1.

   Concretely, our sample board above has last-row indices of 5, 11, 17, and 23.
*/
int row_offset() {
  assert(LEN > 1); /* Avoid pathological size-1 "boards." */
  assert(adjacents[0] == '3'); /* All boards start with a "nw corner." */
  int i;
  for (i = 1; i < LEN; i++) {
    if (adjacents[i] == '3') {
      break;
    }
  }
  assert(i < LEN); /* All boards end with a "se corner." */
  return i;
}
