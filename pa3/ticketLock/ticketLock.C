MAIN_ENV

typedef struct {
  int *a; 
  long int *start, *end;
  int p,t,r;
  LOCKDEC(getTicket);
} GM;

GM *gm;

void ticketLock(void) {
  register int i,j,q,p,myTicket;
  int pid;
  int N = 1500000;
  int k = 0;
  int M = 0;

  q = 0;
  p = 0;

  GET_PID(pid)
  CLOCK(gm->start[pid])
  for (i = 0; i < N; i++) {
    //Get ticket
    LOCK(gm->getTicket)
    myTicket = gm->t;
    gm->t += 1;
    UNLOCK(gm->getTicket)

    //Wait for processor's turn in critical section
    while (myTicket != gm->r);
    //Execute critical section, then increment number being serviced
    for (j = 0; j < k; j++) q++;
    gm->r++;

    //Simulate non-critical section
    for (j = 0; j < M; j++) p++;
  }
  CLOCK(gm->end[pid])
  gm->a[pid] = p+q;
}

int main(int argc,char **argv) {
  int i,j,p,n;
  int *a, *queue;
  unsigned int t1,t2;

  MAIN_INITENV
  gm = (GM*)G_MALLOC(sizeof(GM));
  LOCKINIT(gm->getTicket);
  gm->a     = (int*)G_MALLOC(p*sizeof(int));
  gm->start   = (long int*)G_MALLOC(p*sizeof(long int));
  gm->end     = (long int*)G_MALLOC(p*sizeof(long int));
  gm->t = (int)G_MALLOC(sizeof(int));
  gm->t = 0;
  gm->r = 0;
  p = gm->p = atoi(argv[1]);
  for(i = 0; i < p; i++) gm->a[i]= 0;

  assert(p > 0);
  assert(p <= 8);

  // Create p Processes
  for(i = 0; i < p; i++)
    CREATE(ticketLock)

  CLOCK(t1)
  WAIT_FOR_END(p-1)
  CLOCK(t2)
  
  printf("Elapsed: %u us\n",t2-t1);

  for(i = 0; i < p; i++) printf("Proc %d finished at %d\n", i, gm->end[i]);

  MAIN_END
  return 0;
}
