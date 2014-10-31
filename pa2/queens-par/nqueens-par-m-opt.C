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
  int n = gm->n;

  for (i = 0; i < n; i++) {
    bit = board[i];
    row = bit ^ (bit & (bit - 1));

    for (j = 0; j < n; j++) {
      if ((row >> j) & 1) printf("Q ");
      else printf("_ ");
    }
    printf("\n");
  }
}


void printResults() {
  printf("Maximum profit board\n");
  printf("Max profit: %d\n", gm->maxProfit);
  printf("Total solutions: %d\n", gm->total);
  printf("Maximum profit board\n");
  printBoard(gm->maxBoard);
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

//========================================================================
// Parallel N-Queens Algorithm
//========================================================================
//
//  The sequential algorithm that the following code was derived from is
//  based heavily on optimizations published online by Jeff Somers, and
//  can be found at jsomers.com/nqueen_demo/nqueens.html
//
//========================================================================

void pnqueens(int bits, int n) {
  int localTotal = 0;
  int localMaxProfit = 0;
  int localMaxBoard[n];
  int results[n];

  int stack[n+2];
  register int* stackPointer;

  int columns[n];
  int updiags[n];
  int dndiags[n];

  register int row=1;
  register int lsb;

  int mask = (1 << n) - 1;

  stack[0] = -1;
  stackPointer = stack;
  *stackPointer = 0;
  stackPointer++;
  row = 1;

  results[0] = bits;
  columns[0] = 0;
  columns[1] = bits;
  updiags[0] = 0;
  updiags[1] = bits << 1;
  dndiags[0] = 0;
  dndiags[1] = bits >> 1;

  bits = mask & ~(columns[1] | updiags[1] | dndiags[1]);

  while(1) {
    if (!bits) {
      stackPointer--;
      if (stackPointer == stack) {
        break;
      }
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
        localTotal += 2;
        register int k;
        register int profit0 = 0;
        register int profit1 = 0;

        for (k = 0; k < n; k++) {
          profit0 += abs(k - bitsToValue(results[k]));
          profit1 += abs((n - 1 - k) - bitsToValue(results[k]));
        }

        if ((profit0 > localMaxProfit) && (profit0 >= profit1)) {
          localMaxProfit = profit0;
          for (k = 0; k < n; k++) {
            localMaxBoard[k] = results[k];
          }
        }
        else if (profit1 > localMaxProfit) {
          localMaxProfit = profit1;
          for (k = 0; k < n; k++) {
            localMaxBoard[k] = results[k];
          }
        }

        stackPointer--;
        bits = *stackPointer;
        row--;
      }
    }
  }

  // All solutions for this initial position found
  LOCK(gm->totalLock)
  gm->total += localTotal;
  UNLOCK(gm->totalLock)

  LOCK(gm->profitLock)
  if (localMaxProfit > gm->maxProfit) {
    register int k;

    gm->maxProfit = localMaxProfit;
    LOCK(gm->boardLock)
    for (k = 0; k < n; k++) {
      gm->maxBoard[k] = localMaxBoard[k];
    }
    UNLOCK(gm->boardLock)
  }
  UNLOCK(gm->profitLock)
}

void wrapper(void) {
  int pid;
  int n, p, i;

  GET_PID(pid);
  n = gm->n;
  p = gm->p;

  for (i = 0+pid; i < (n + 1)/2; i+=p) {
    int bits = (1 << i);
    pnqueens(bits, n);
  }
}

//========================================================================
// Main
//========================================================================

int main (int argc, char **argv) {
  int i, n, p, total, maxProfit;
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

  gm->total = 0;
  gm->maxProfit = 0;
  gm->maxBoard = (int*)G_MALLOC(n*sizeof(int));
  
  LOCKINIT(gm->totalLock);
  LOCKINIT(gm->profitLock);
  LOCKINIT(gm->boardLock);

  for (i = 0; i < p-1; i++) CREATE(wrapper)

  CLOCK(t1)
  wrapper();
  WAIT_FOR_END(p-1)
  CLOCK(t2)
  printResults();
  CLOCK(t3)
  printf("Computation time: %u microseconds\n", t2-t1);
  printf("Printing time:    %u microseconds\n", t3-t2);
  G_FREE(maxBoard,n*sizeof(int))
  MAIN_END
  return 0;
}
