MAIN_ENV


//========================================================================
// Global definitions
//========================================================================

typedef struct {
  int n;
  int p;
  int total;
  char **maxBoard;
  char **initialBoard;
  int maxProfit;
} GM;

GM *gm;

//========================================================================
// Helper methods
//========================================================================

int getQueenColumn (int row, char **currentBoard) {
  int n;
  n = gm->n;

  register int col;

  for (col = 0; col < n; col++) {
    if (currentBoard[row][col]) {
      return col;
    }
  }
  return -1;
}

char canPlace (int row, int col, char **currentBoard) {
  int n;
  n = gm->n;

  register int i, j;

  for (i = 0; i < n; i++) {
    j = getQueenColumn(i, currentBoard);
    if (j == -1) continue;

    if ((col==j) || (row==i) || ((j-i)==(col-row)) || ((i+j)==(row+col)))
      return 0;
  }

  return 1;
}

void printBoard(char **board) {
  int n, total, maxProfit;
  char **maxBoard, **initialBoard;
  n             = gm->n;
  total         = gm->total;
  maxProfit     = gm->maxProfit;
  
  register int i, j, k;

  for (i = 0; i < n; i++) {
    for (j = 0; j < n; j++) {
      if (board[i][j]) printf("|Q");
      else printf("| ");
      if (j == n-1) printf("|\n");
    }
  }

  printf("Total solutions: %d\n", total);
  printf("Maximum profit: %d\n", maxProfit);
}
 
//========================================================================
// N-Queens Algorithm
//========================================================================
 
void nqueens_wrapper(void) {
  int n, total, maxProfit;
  char **maxBoard, **initialBoard;
  
  n             = gm->n;
  total         = gm->total;
  maxBoard      = gm->maxBoard;
  initialBoard  = gm->initialBoard;
  maxProfit     = gm->maxProfit;

  int i;
  for (i = 0; i < n; i++) {
    initialBoard[i][0] = 1;
    nqueens(i+1,initialBoard,0,1);
    initialBoard[i][0] = 0;
  }
}

void nqueens(int i, char **currentBoard, int currentProfit, int numQ) {
  int n, total, maxProfit;
  char **maxBoard, **initialBoard;
  n             = gm->n;
  total         = gm->total;
  maxBoard      = gm->maxBoard;
  maxProfit     = gm->maxProfit;
  register int j;

  if (i < n) {
    for (j = 0; j < n; j++) {
      if (canPlace(i, j, currentBoard)) {
        // Creating a new board
        register int x,y;
        char **newBoard;
        newBoard = (char**)G_MALLOC(n*sizeof(char*));
        for (x = 0; x < n; x++) {
          newBoard[x] = (char*)G_MALLOC(n*sizeof(char*));
          for (y = 0; y < n; y++) newBoard[x][y] = currentBoard[x][y];
        }

        newBoard[i][j] = 1;
        int profitAdd = abs(i - j);
        nqueens(i + 1, newBoard, currentProfit + profitAdd, numQ + 1);
      }
    }
  }
  else {
    total++;
    if (currentProfit > maxProfit) {
      maxBoard = currentBoard;
      maxProfit = currentProfit;
    }
  }
}

//========================================================================
// Main
//========================================================================

int main (int argc, char **argv) {
  int i, j, p, n;
  int total;
  char **maxBoard;
  char **initialBoard;
  unsigned int t1, t2, t3;

  MAIN_INITENV
  //Enforce arguments
  if (argc != 2) {
    printf("Usage: nqueens-seq <N>\nAborting.\n");
    exit(0);
  }
  gm = (GM*)G_MALLOC(sizeof(GM));
  p = gm->p = atoi(argv[2]);
  gm->n = atoi(argv[1]);
  n = gm->n;
  
  assert(p > 0);
  assert(p <= 8);

  total = gm->total = 0;
  maxBoard     = gm->maxBoard = (char**)G_MALLOC(n*sizeof(char*));
  initialBoard = gm->initialBoard = (char**)G_MALLOC(n*sizeof(char*));
  for (i = 0; i < n; i++) {
    maxBoard[i] = (char*)G_MALLOC(n*sizeof(char));
    initialBoard[i] = (char*)G_MALLOC(n*sizeof(char));
    for (j = i; j < gm->n; j++) {
      maxBoard[i][j] = 0;
      initialBoard[i][j] = 0;
    }
  }

  for(i = 0; i < 8; i++)
    CREATE(nqueens_wrapper)
  CLOCK(t1)
  nqueens_wrapper();
  CLOCK(t2)
  printf("Printing maximum profit board\n");
  printBoard(maxBoard);
  CLOCK(t3)
  printf("Computation time: %u microseconds\n", t2-t1);
  printf("Printing time:    %u microseconds\n", t3-t2);
  MAIN_END
  return 0;
}
