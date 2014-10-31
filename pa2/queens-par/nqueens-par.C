MAIN_ENV


//========================================================================
// Global definitions
//========================================================================

typedef struct {
  int p;
  int n;
  int total;
  int *maxProfit;
  char **maxBoard;
  char **initialBoard;
  int global_max_profit;
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

void printBoard(char **board, int global_max_profit) {
  int n, total; 
  n     = gm->n;
  total = gm->total;
  
  register int i, j;

  for (i = 0; i < n; i++) {
    for (j = 0; j < n; j++) {
      if (board[i][j])
        printf("|Q");
      else 
        printf("| ");
      if (j == n-1) 
        printf("|\n");
    }
  }

  printf("Total solutions: %d\n", total);
  printf("Maximum profit: %d\n", global_max_profit);
}

//========================================================================
// N-Queens Algorithm
//========================================================================

void nqueens(int i, char **currentBoard, int currentProfit, int numQ, int maxProfit[], int maxProfIndex) {
  register int j;
  int n = gm->n;
  char ** maxBoard = gm->maxBoard;

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
        nqueens(i + 1, newBoard, currentProfit + profitAdd, numQ + 1, maxProfit, maxProfIndex);
      }
    }
  }
  else {
    gm->total = gm->total + 1;
    if (currentProfit > maxProfit[maxProfIndex]) {
      maxProfit[maxProfIndex] = currentProfit;
      gm->global_max_profit = currentProfit;
      for (i = 0; i < n; i++)
        for (j = 0; j < n; j++)
          gm->maxBoard[i][j] = currentBoard[i][j];
    }
  }
}

void nqueens_wrapper(void){
  int i, j, n, p, pid;
  char **initialBoard;

  initialBoard = gm->initialBoard;
  
  int maxProfit[1];
  maxProfit[0] = 0;
  n = gm->n;
  p = gm->p;
  GET_PID(pid);

  for (i = pid; i < n; i += p) {
    // initialBoard[0][i] = 1;
    nqueens(0, initialBoard, 0, 0, maxProfit, 0);
    // initialBoard[0][i] = 0;
  }
}

// ========================================================================
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
  gm->p = 8;
  gm->n = atoi(argv[1]); 
  n = gm->n;
  gm->total = 0;
  gm->maxBoard = (char**)G_MALLOC(n*sizeof(char*));
  gm->initialBoard = (char**)G_MALLOC(n*sizeof(char*));
  gm->global_max_profit = 0;

  for (i = 0; i < n; i++) {
    gm->maxBoard[i] = (char*)G_MALLOC(n*sizeof(char));
    gm->initialBoard[i] = (char*)G_MALLOC(n*sizeof(char));
    for (j = i; j < n; j++) {
      gm->maxBoard[i][j] = 0;
      gm->initialBoard[i][j] = 0;
    }
  }

  CLOCK(t1)
  for(i = 0; i < n; i++)
    CREATE(nqueens_wrapper)
  WAIT_FOR_END(n);
  CLOCK(t2)
  
  printf("Printing maximum profit board\n");
  printBoard(gm->maxBoard, gm->global_max_profit);
  CLOCK(t3)
  
  printf("Computation time: %u microseconds\n", t2-t1);
  printf("Printing time:    %u microseconds\n", t3-t2);
  MAIN_END
  return 0;
}
