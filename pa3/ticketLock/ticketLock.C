MAIN_ENV

typedef struct {
  int *a; 
  float *start, *end;
  int p,t;
  LOCKDEC(lock);
  LOCKDEC(getTicket);
} GM;

GM *gm;

void pbksb(int pid) {
  register int i,j,q,N,p,k,M, r;
  N = p = k = M = 16;

  ACQUIRE(gm->getTicket)
  r = pid;
  RELEASE(gm->getTicket)

  CLOCK(gm->start[pid])
  for (i = 0; i < N; i++) {
    while (r != gm->t);
    // CRITICAL SECITON
    ACQUIRE(gm->lock) 
    for (j = 0; j < k; j++) q++;
    RELEASE(gm->lock)
    gm->t += 1;
    for (j = 0; j < k; j++) p++;
    // Grab next ticket for processor
    while (gm->t < 7);
    ACQUIRE(gm->getTicket)
    gm->t += 1;
    r      = gm->t;
    RELEASE(gm->getTicket)
  }
  CLOCK(gm->end[pid])
  gm->a[pid] = p + q;
}

void pbksb_wrapper(void) {
  int pid;
  GET_PID(pid);
  pbksb(pid);
}

int main(int argc,char **argv) {
  int i,j,p,n;
  int *a, *queue;
  unsigned int t1,t2;

  MAIN_INITENV
  gm = (GM*)G_MALLOC(sizeof(GM));
  LOCKINIT(gm->lock);
  LOCKINIT(gm->getTicket);
  gm->a     = (int*)G_MALLOC(p*sizeof(int));
  gm->start   = (float*)G_MALLOC(p*sizeof(float));
  gm->end     = (float*)G_MALLOC(p*sizeof(float));
  gm->t = (int)G_MALLOC(sizeof(int));
  gm->t = 0;
  p = gm->p = atoi(argv[1]);
  for(i = 0; i < p; i++) gm->a[i]= 0;

  assert(p > 0);
  assert(p <= 8);

  // Create p Processes
  for(i = 0; i < p; i++)
    CREATE(pbksb_wrapper)

  CLOCK(t1)
  WAIT_FOR_END(p-1)
  CLOCK(t2)
  
  printf("Elapsed: %u us\n",t2-t1);

  for(i = 0; i < p; i++) printf("Proc %d finished at %d\n", i, gm->end[i]);

  MAIN_END
  return 0;
}
