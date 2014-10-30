

	































































#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <assert.h>

typedef struct simplelock lock;

typedef struct struct_ANL_Globals {
   int nextPid;	
   
	lock *anlLock;
;
} ANLGlobalsStruct;

ANLGlobalsStruct *ANLGlobals;

typedef struct anl_barrier {
    
	lock *lock;

    volatile int count;
    volatile int spin;
    volatile int inuse;
} anl_barrier;

int _anl_reserved_fd;
int _anl_reserved_pid;
int _anl_reserved_nprocs;



//========================================================================
// Global definitions
//========================================================================

typedef struct {
  int n;
  int p;
  int total;
  char **maxBoard;
  char **initialBoard;
} GM; 

GM *gm;
int global_max_profit;
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
  char **maxBoard, **initialBoard;
  n             = gm->n;
  total         = gm->total;
  
  register int i, j, k;

  for (i = 0; i < n; i++) {
    for (j = 0; j < n; j++) {
      if (board[i][j]) printf("|Q");
      else printf("| ");
      if (j == n-1) printf("|\n");
    }
  }

  printf("Total solutions: %d\n", total);
  printf("Maximum profit: %d\n", global_max_profit);
}
 
//========================================================================
// N-Queens Algorithm
//========================================================================
void nqueens(int i, char **currentBoard, int currentProfit, int numQ, int maxProfIndex, int maxProfit[]) {
  int n, total;
  char **maxBoard, **initialBoard;
  n             = gm->n;
  total         = gm->total;
  maxBoard      = gm->maxBoard;
  register int j;

  if (i < n) {
    for (j = 0; j < n; j++) {
      if (canPlace(i, j, currentBoard)) {
        // Creating a new board
        register int x,y;
        char **newBoard;
        newBoard = (char**)
   mmap(NULL, n*sizeof(char*), PROT_READ|PROT_WRITE, MAP_SHARED, _anl_reserved_fd, 0);
;
        for (x = 0; x < n; x++) {
          newBoard[x] = (char*)
   mmap(NULL, n*sizeof(char*), PROT_READ|PROT_WRITE, MAP_SHARED, _anl_reserved_fd, 0);
;
          for (y = 0; y < n; y++) newBoard[x][y] = currentBoard[x][y];
        }

        newBoard[i][j] = 1;
        int profitAdd = abs(i - j);
        nqueens(i + 1, newBoard, currentProfit + profitAdd, numQ + 1, maxProfIndex, maxProfit);
      }
    }
  }
  else {
    total++;
    if (currentProfit > maxProfit[maxProfIndex]) {
      maxBoard = currentBoard;
      maxProfit[maxProfIndex] = currentProfit;
    }
  }
}

 
void nqueens_wrapper(void) {
  int n, total; 
  char **maxBoard, **initialBoard;
  
  n             = gm->n;
  total         = gm->total;
  maxBoard      = gm->maxBoard;
  initialBoard  = gm->initialBoard;
  int i;
  int maxProfit[n];
  for (i = 0; i < n; i++) {
    maxProfit[i] = 0;
  }

  for (i = 0; i < n; i++) {
    initialBoard[i][0] = 1;
    nqueens(i+1,initialBoard,0,1,i, maxProfit);
    initialBoard[i][0] = 0;
  }

  int max = maxProfit[0];
  for (i = 0; i < n; i++) {
    if (max < maxProfit[i]) {
      max = maxProfit[i];
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

  
{
   if ((_anl_reserved_fd=open("/dev/zero", O_RDWR)) == -1) {
      perror("ANL Init cannot open /dev/zero!");
      exit(99);
   }

   if ((ANLGlobals = (ANLGlobalsStruct *)mmap(NULL, 
	sizeof(ANLGlobalsStruct), PROT_READ|PROT_WRITE, MAP_SHARED, 
	_anl_reserved_fd, 0)) == MAP_FAILED) {
     perror ("ANL Global Initialization failed!");
     exit(99);
   }

   
   if ((ANLGlobals->anlLock = (lock*)mmap(NULL, getpagesize(), PROT_READ|PROT_WRITE, 
       	MAP_SHARED, _anl_reserved_fd, 0)) == MAP_FAILED) {
      perror ("LockInit failed!");
   }
   s_lock_init (ANLGlobals->anlLock);
;
   _anl_reserved_nprocs = 1;
}
  //Enforce arguments
  if (argc != 2) {
    printf("Usage: nqueens-seq <N>\nAborting.\n");
    exit(0);
  }
  gm = (GM*)
   mmap(NULL, sizeof(GM), PROT_READ|PROT_WRITE, MAP_SHARED, _anl_reserved_fd, 0);
;
  // p = gm->p = atoi(argv[2]);
  gm->n = atoi(argv[1]);
  n = gm->n;
  global_max_profit = 0;
  // assert(p > 0);
  // assert(p <= 8);

  total = gm->total = 0;
  maxBoard     = gm->maxBoard = (char**)
   mmap(NULL, n*sizeof(char*), PROT_READ|PROT_WRITE, MAP_SHARED, _anl_reserved_fd, 0);
;
  initialBoard = gm->initialBoard = (char**)
   mmap(NULL, n*sizeof(char*), PROT_READ|PROT_WRITE, MAP_SHARED, _anl_reserved_fd, 0);
;
  for (i = 0; i < n; i++) {
    maxBoard[i] = (char*)
   mmap(NULL, n*sizeof(char), PROT_READ|PROT_WRITE, MAP_SHARED, _anl_reserved_fd, 0);
;
    initialBoard[i] = (char*)
   mmap(NULL, n*sizeof(char), PROT_READ|PROT_WRITE, MAP_SHARED, _anl_reserved_fd, 0);
;
    for (j = i; j < gm->n; j++) {
      maxBoard[i][j] = 0;
      initialBoard[i][j] = 0;
    }
  }

  for(i = 0; i < 8; i++)
    
{
   _anl_reserved_nprocs++;
   if ((_anl_reserved_pid = fork()) == -1) {
     perror ("Create failed!");
     exit (99);
   }

   if (!_anl_reserved_pid) {
      // child
      nqueens_wrapper();
      exit(0);
   }
}
  
{
   struct timeval tv;
   gettimeofday(&tv,NULL);
   t1 = ((tv.tv_sec & 0x7ff) * 1000000) + tv.tv_usec;
}
  nqueens_wrapper();
  
{
   struct timeval tv;
   gettimeofday(&tv,NULL);
   t2 = ((tv.tv_sec & 0x7ff) * 1000000) + tv.tv_usec;
}
  printf("Printing maximum profit board\n");
  printBoard(maxBoard, global_max_profit);
  
{
   struct timeval tv;
   gettimeofday(&tv,NULL);
   t3 = ((tv.tv_sec & 0x7ff) * 1000000) + tv.tv_usec;
}
  printf("Computation time: %u microseconds\n", t2-t1);
  printf("Printing time:    %u microseconds\n", t3-t2);
  {
  fflush(stdout); 
  fflush(stderr); 
  exit(0);
}
  return 0;
}
