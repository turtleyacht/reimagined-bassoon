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
char adjacents[LEN] = "355553588885588885355553";

/* Unavoidably, retain state in another variable . */
/* Express positions clockwise:

     0 1 2                    0 1 2 3 4 5 6 7
     7 A 3   translates to    0 0 0 0 0 0 0 0
     6 5 4

   Which just happens to be eight bits, or a byte!
*/
short int states[LEN]    = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 1, 0, 0, 0, 0, 0, 0, 0 };

/* Of course, we'll need functions to initialize these.
   The board will be too large to do by hand.
*/

