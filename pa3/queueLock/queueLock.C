MAIN_ENV

typedef struct {
  int *a, *queue;
  long int *start, *end;
  int p, index;
  LOCKDEC(indexLock);
} GM;

GM *gm;

void queueLock(void) {
  register int i,j,q,p;
  int N = 1500000;
  int k = 0;
  int M = 0;
  int pid;

  q = 0;
  p = 0;
  int myIndex;

  GET_PID(pid)
  CLOCK(gm->start[pid])
  for (i = 0; i < N; i++) {
    //Acquire lock.
    LOCK(gm->indexLock)
    myIndex = gm->index;

    if (myIndex == (gm->p-1)*64) gm->index = 0;
    else gm->index += 64;
    UNLOCK(gm->indexLock)

    while(gm->queue[myIndex]);
    gm->queue[myIndex] = 1;

    //Execute critical section
    for (j = 0; j < k; j++) q++;

    //Unlock:
    //If this index points to the last spot in the queue, index=0 can go
    //Otherwise, myIndex+1 can go next
    if (myIndex == (gm->p-1)*64) gm->queue[0] = 0;
    else gm->queue[myIndex+64] = 0;
  }
  CLOCK(gm->end[pid])
  gm->a[pid] = p + q;
}

int main(int argc,char **argv) {
  int i,j,p,n;
  int *queue;
  unsigned int t1,t2;

  MAIN_INITENV
  gm = (GM*)G_MALLOC(sizeof(GM));
  LOCKINIT(gm->indexLock);
  p = gm->p   = atoi(argv[1]);
  gm->a       = (int*)G_MALLOC(p*sizeof(int));
  gm->start   = (long int*)G_MALLOC(p*sizeof(long int));
  gm->end     = (long int*)G_MALLOC(p*sizeof(long int));
  gm->queue   = (int*)G_MALLOC(256*p);
  gm->index   = (int)G_MALLOC(sizeof(int));
  gm->index = 0;
  for(i = 0; i < p; i++) gm->a[i]= 0;
  for(i = 0; i < (p*256/sizeof(int)); i++) {
    if (i == 0) gm->queue[i]= 0;
    else gm->queue[i] = 1;
  }

  assert(p > 0);
  assert(p <= 8);

  // Create p Processes
  for(i = 0; i < p; i++)
    CREATE(queueLock)

  CLOCK(t1)
  WAIT_FOR_END(p-1)
  CLOCK(t2)
  
  printf("Elapsed: %u us\n",t2-t1);

  for(i = 0; i < p; i++) printf("Proc %d finished at %d\n", i, gm->end[i]);
  G_FREE(gm->queue,p*64*sizeof(int))
  G_FREE(gm->start,p*sizeof(long int))
  G_FREE(gm->end,p*sizeof(long int))
  MAIN_END
  return 0;
}
