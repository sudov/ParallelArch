MAIN_ENV

//========================================================================
// Global definitions
//========================================================================

typedef struct {
  int n;
  int total;
  LOCKDEC(totalLock)
  int maxProfit;
  LOCKDEC(profitLock)
  int* maxBoard;
  LOCKDEC(boardLock)
  int p;
} GM;

GM *gm;

//========================================================================
// Helper methods
//========================================================================

void printBoard(int* board) {
  register int i, j;
  int row, bit;

  for (i = 0; i < n; i++) {
    bit = board[i];
 //   printf("results[%d] = %d\n", i, board[i]);
    row = bit ^ (bit & (bit - 1));

    for (j = 0; j < n; j++) {
      if ((row >> j) & 1) printf("Q ");
      else printf("_ ");
    }
    printf("\n");
  }
}

void printResults() {
  printBoard(maxBoard);
  printf("Maximum profit board\n");
  printf("Max profit: %d\n", maxProfit);
  printf("Total solutions: %d\n", total);
}



int bitsToValue(int bits){
  if (!bits) return 0;

  int counter = -1;

  while (bits > 0) {
    bits = bits >> 1;
    counter++;
  }
  return counter;
}




void updateProfit(int* results) {
  register int i;
  register int profit0 = 0;
  register int profit1 = 0;

  for (i = 0; i < n; i++) {
    profit0 += abs(i - bitsToValue(results[i]));
    profit1 += abs((n - 1 - i) - bitsToValue(results[i]));
  }

  if ((profit0 > maxProfit) && (profit0 >= profit1)) {
    maxProfit = profit0;
    for (i = 0; i < n; i++) {
      maxBoard[i] = results[i];
    }
  }
  else if ((profit1 > maxProfit) && (profit1 > profit0)) {
    maxProfit = profit1;
    for (i = 0; i < n; i++) {
      maxBoard[i] = results[n-1-i];
    }
  }
}

//========================================================================
// Parallel N-Queens Algorithm
//========================================================================

// options: split up work within the algorithm OR split up on outside,
// each processor maintains individual maxProfit and such

void pnqueens(void) {
  int pid;
  int n, p;

  GET_PID(pid);
  n = gm->a;
  p = gm->p

  int results[n];

  int stack[n+2];
  register int* stackPointer;

  int columns[n];
  int updiags[n];
  int dndiags[n];

  register int row = 0;
  register int lsb;
  register int bits;

  int i;
  int oddN = n & 1;
  int mask = (1 << n) - 1;

  stack[0] = -1;

  for (i = 0; i < (1 + oddN); i++) {
    bits = 0;

    if (!i) {
      bits = (1 << (n >> 1)) - 1;
      stackPointer = stack + 1;

      results[0] = 0;
      columns[0] = 0;
      updiags[0] = 0;
      dndiags[0] = 0;
    }
    else {
      bits = 1 << (n >> 1);
      row = 1;

      results[0] = bits;
      columns[0] = 0;
      columns[1] = bits;
      updiags[0] = 0;
      updiags[1] = bits << 1;
      dndiags[0] = 0;
      dndiags[1] = bits >> 1;

      *stackPointer = 0;
      stackPointer++;
      bits = (bits - 1) >> 1;
    }
    
    //possibly begin splitting up work here
    while(1) {
      if (!bits) {
        stackPointer--;
        if (stackPointer == stack) break;
        bits = *stackPointer;
        row--;
      }
      else {
        lsb = bits ^ (bits & (bits - 1));
        bits &= ~lsb;

        results[row] = lsb;

        if (row < n-1) {
          int rowLast = row;
          row++;
          columns[row] = columns[rowLast] | lsb;
          updiags[row] = ( updiags[rowLast] | lsb ) << 1;
          dndiags[row] = ( dndiags[rowLast] | lsb ) >> 1;
          
          *stackPointer = bits;
          stackPointer++;

          bits = mask & ~(columns[row] | updiags[row] | dndiags[row]);
        }
        else {
          total++;
          updateProfit(results);

          stackPointer--;
          bits = *stackPointer;
          row--;
        }
      }
    }
  }
  total *= 2;
}

//========================================================================
// Main
//========================================================================

int main (int argc, char **argv) {
  int n, p, total, maxProfit;
  int* maxBoard;
  unsigned int t1, t2, t3;

  MAIN_INITENV

  //Enforce arguments
  if (argc != 3) {
    printf("Usage: nqueens-seq <P> <N>\nAborting.\n");
    exit(0);
  }

  gm = (GM*)G_MALLOC(sizeof(GM));
  p = gm->p = atoi(argv[1]);
  n = gm->n = atoi(argv[2]);

  assert(p > 0);
  assert(p <= 8);

  total = 0;
  maxProfit = 0;
  maxBoard = (int*)G_MALLOC(n*sizeof(int));

  CLOCK(t1)
  pnqueens();
  WAIT_FOR_END(p-1)
  CLOCK(t2)
  printResults();
  CLOCK(t3)
  printf("Computation time: %u microseconds\n", t2-t1);
  printf("Printing time:    %u microseconds\n", t3-t2);
  MAIN_END
  return 0;
}
