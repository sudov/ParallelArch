MAIN_ENV

typedef struct {
  int *a; 
  double *start, *end, *queue;
  int p;
  LOCKDEC(lock);
  LOCKDEC(insertLock);
} GM;

GM *gm;

void pbksb(void) {
  register int i,j,q,p,N,k,M;
  int pid;
 
  GET_PID(pid)
  ACQUIRE(gm->insertLock)

  RELEASE(gm->insertLock)

  //printf("Proc ID: %u\n",pid);
  CLOCK(gm->start[pid])
  N = gm->p;
  for (i = 0; i < (N-1); i++) {
    ACQUIRE(gm->lock)
    q = 0;
    k = gm->p;
    M = gm->p;
    for (j = 0; j < (k-1); j++) q++;
    RELEASE(gm->lock)
    p = 0;
    for (j = 0; j < (M-1); j++) p++;
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
  LOCKINIT(gm->lock);
  LOCKINIT(gm->insertLock);
  p = gm->p = atoi(argv[1]);
  assert(p > 0);
  assert(p <= 8);

  gm->a     = (int*)G_MALLOC(p*sizeof(int));
  gm->queue = (int*)G_MALLOC(p*sizeof(int));

  for(i = 0; i < p; i++) gm->a[i]     = 0;
  for(i = 0; i < p; i++) gm->queue[i] = -1;

  gm->start = (double*)G_MALLOC(p*sizeof(double));
  gm->end   = (double*)G_MALLOC(p*sizeof(double));

  for(i = 0; i < p; i++)
    CREATE(pbksb)

  CLOCK(t1)
  WAIT_FOR_END(p)
  CLOCK(t2)
  
  printf("Elapsed: %u us\n",t2-t1);

  for(i = 0; i < p; i++) printf("Proc %d finished at %d\n", i, gm->end[i]);
  printf("\n");

  MAIN_END
  return 0;
}
