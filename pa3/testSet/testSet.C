MAIN_ENV

typedef struct {
  int *a; 
  long int *start, *end;
  int p;
  LOCKDEC(lock);
} GM;

GM *gm;

void pbksb(void) {
  register int i,j,q,p,N,k,M;
  int pid;
 
  GET_PID(pid)
  //printf("Proc ID: %u\n",pid);
  CLOCK(gm->start[pid])
  N = 1500000;
  k = 1000;
  M = 76;

  q = 0;
  p = 0;

  for (i = 0; i < (N-1); i++) {
    ACQUIRE(gm->lock)
    for (j = 0; j < (k-1); j++) q++;
    RELEASE(gm->lock)
    for (j = 0; j < (M-1); j++) p++;
  }
  CLOCK(gm->end[pid])
  gm->a[pid] = p+q;
}

int main(int argc,char **argv) {
  int i,j,p,n;
  int *a;
  unsigned int t1,t2;

  MAIN_INITENV
  gm = (GM*)G_MALLOC(sizeof(GM));
  LOCKINIT(gm->lock);
  p = gm->p = atoi(argv[1]);
  assert(p > 0);
  assert(p <= 8);

  gm->a     = (int*)G_MALLOC(p*sizeof(int));
  for(i = 0; i < p; i++) gm->a[i] = 0;
  gm->start = (long int*)G_MALLOC(p*sizeof(long int));
  gm->end   = (long int*)G_MALLOC(p*sizeof(long int));

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
