

	































































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
int n, total, maxProfit;
char **maxBoard, **initialBoard;

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

  n = atoi(argv[1]);
  total = 0;
  maxBoard     = (char**)
   mmap(NULL, n*sizeof(char*), PROT_READ|PROT_WRITE, MAP_SHARED, _anl_reserved_fd, 0);
;
  initialBoard = (char**)
   mmap(NULL, n*sizeof(char*), PROT_READ|PROT_WRITE, MAP_SHARED, _anl_reserved_fd, 0);
;
  for (i = 0; i < n; i++) {
    maxBoard[i] = (char*)
   mmap(NULL, n*sizeof(char), PROT_READ|PROT_WRITE, MAP_SHARED, _anl_reserved_fd, 0);
;
    initialBoard[i] = (char*)
   mmap(NULL, n*sizeof(char), PROT_READ|PROT_WRITE, MAP_SHARED, _anl_reserved_fd, 0);
;
    for (j = i; j < n; j++) {
      maxBoard[i][j] = 0;
      initialBoard[i][j] = 0;
    }
  }

  
{
   struct timeval tv;
   gettimeofday(&tv,NULL);
   t1 = ((tv.tv_sec & 0x7ff) * 1000000) + tv.tv_usec;
}
  nqueens(0,initialBoard,0,0);
  
{
   struct timeval tv;
   gettimeofday(&tv,NULL);
   t2 = ((tv.tv_sec & 0x7ff) * 1000000) + tv.tv_usec;
}
  printf("Printing maximum profit board\n");
  printBoard(maxBoard);
  
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
