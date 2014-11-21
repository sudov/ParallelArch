MAIN_ENV

typedef struct {
  int *a, *queue; 
  float *start, *end;
  int p,current;
  LOCKDEC(lock);
  LOCKDEC(insertLock);
} GM;

GM *gm;

void pbksb(int pid) {
  register int i,j,q,p,N,k,M;
  N = p = k = M = gm->p;

  ACQUIRE(gm->insertLock)
  if (gm->current == -1) {
    gm->current = pid;
  }
  for (i = 0; i < p; i++) {
    if (gm->queue[i] == -1) {
      gm->queue[i] = pid;
      break;
    }
  }
  RELEASE(gm->insertLock)

  CLOCK(gm->start[pid])
    for (i = 0; i < (N-1); i++) {
      printf("Proc: %d\n", pid);
      int inserted = 0;
      for (j = 0; j < p; j++) {
        if (gm->queue[j] == pid) {
          inserted = 1;
          break;
        } else if (gm->queue[j] == -1) {
          gm->queue[j] = pid;
          inserted = 1;
          break;
        }
      }
      while (gm->current != gm->queue[gm->current]);
      // Critical Section
      ACQUIRE(gm->lock)
      q = 0;
      for (j = 0; j < (k-1); j++) q++;
      RELEASE(gm->lock)
      gm->current = (gm->current + 1)%p;

      p = 0;
      for (j = 0; j < (M-1); j++) p++;
    }
  CLOCK(gm->end[pid])
  // printf("Queue: %d %d %d %d %d %d %d %d\n", gm->queue[0], gm->queue[1], gm->queue[2], gm->queue[3], gm->queue[4], gm->queue[5], gm->queue[6], gm->queue[7]);
  gm->a[pid] = p+q;
}

void pbksb_wrapper(void) {
  int pid;
  GET_PID(pid);
  // printf("Wrapper Proc: %d:", pid);
  pbksb(pid);
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

  for(i = 0; i < p; i++) gm->a[i]= 0;
  for(i = 0; i < p; i++) gm->queue[i] = -1;

  gm->start   = (float*)G_MALLOC(p*sizeof(float));
  gm->end     = (float*)G_MALLOC(p*sizeof(float));
  gm->current = (int)G_MALLOC(sizeof(int));
  gm->current = -1;

  for(i = 0; i < p; i++)
    CREATE(pbksb_wrapper)

  CLOCK(t1)
  WAIT_FOR_END(p)
  CLOCK(t2)
  
  printf("Elapsed: %u us\n",t2-t1);

  for(i = 0; i < p; i++) printf("Proc %d finished at %d\n", i, gm->end[i]);

  MAIN_END
  return 0;
}
