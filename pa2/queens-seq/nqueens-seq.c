MAIN_ENV

//========================================================================
// Global definitions
//========================================================================

int n;
int total;
char **maxBoard;
int maxProfit;

//========================================================================
// Helper methods
//========================================================================

int getQueenColumn (int row, char **currentBoard) {
  register int col;

  for (col = 0; col < n; col++) {
    if (currentBoard[row][col]) {
      return col;
    }
  }

  return -1;
}

char canPlace (int row, int col, char **currentBoard) {
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
 
void nqueens(int i, char **currentBoard, int currentProfit, int numQ) {
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
  int i, j;
  char **initialBoard;
  unsigned int t1, t2, t3;

  MAIN_INITENV

  //Enforce arguments
  if (argc != 2) {
    printf("Usage: nqueens-seq <N>\nAborting.\n");
    exit(0);
  }

  n = atoi(argv[1]);
  total = 0;

  maxBoard = (char**)G_MALLOC(n*sizeof(char*));
  initialBoard = (char**)G_MALLOC(n*sizeof(char*));
  for (i = 0; i < n; i++) {
    maxBoard[i] = (char*)G_MALLOC(n*sizeof(char));
    initialBoard[i] = (char*)G_MALLOC(n*sizeof(char));
    for (j = i; j < n; j++) {
      maxBoard[i][j] = 0;
      initialBoard[i][j] = 0;
    }
  }

  CLOCK(t1)
  nqueens(0,initialBoard,0,0);
  CLOCK(t2)
  printf("Printing maximum profit board\n");
  printBoard(maxBoard);
  CLOCK(t3)
  printf("Computation time: %u microseconds\n", t2-t1);
  printf("Printing time:    %u microseconds\n", t3-t2);
  MAIN_END
  return 0;
}
