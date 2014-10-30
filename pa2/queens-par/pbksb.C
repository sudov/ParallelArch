MAIN_ENV

typedef struct {
  double **a;
  double *b;
  int n,p;
  char *pse;
} GM;

GM *gm;

void pbksb(void) {
  register int i,j,start,end;
  register double sum;
  int pid,block;
  double **a,*b;
  int n,p;
  GET_PID(pid);
  a = gm->a;
  b = gm->b;
  n = gm->n;
  p = gm->p;
  block = n/p;
  start = block*pid;
  end = start+block-1;
  for(j = n-1; j > end; j--)
    WAITPAUSE(gm->pse[j])
  for(i = end; i >= start; i--) {
    sum = b[i];
    for(j = n-1; j > i; j--)
      sum -= a[i][j]*b[j];
    b[i] = sum/a[i][i];
    SETPAUSE(gm->pse[i])
  }
}

int main(int argc,char **argv) {
  int i,j,p,n;
  double **a,*b, count=1.0;
  unsigned int t1,t2;
  MAIN_INITENV
  if (argc!=3) {
     printf("Usage: pbksb P N\nAborting...\n");
     exit(0);
  }
  gm = (GM*)G_MALLOC(sizeof(GM));
  p = gm->p = atoi(argv[1]);
  gm->n = atoi(argv[2]);
  assert(p > 0);
  assert(p <= 8);
  n = gm->n;
  a = gm->a = (double**)G_MALLOC(n*sizeof(double*));
  for(i = 0; i < n; i++) {
    a[i] = (double*)G_MALLOC(n*sizeof(double));
    for(j = i;j < n;j++){
       a[i][j] = count;
       count++;
    }
  }
  b = gm->b = (double*)G_MALLOC(n*sizeof(double));
  for(i = 0; i < n; i++) {
    b[i] = count;
    count++;
  }
  gm->pse = (char*)G_MALLOC(n*sizeof(char));
  for(i = 0; i < n; i++)
    CLEARPAUSE(gm->pse[i])
  for(i = 0; i < p-1; i++)
    CREATE(pbksb)
  CLOCK(t1)
  pbksb();
  WAIT_FOR_END(p-1)
  CLOCK(t2)
  printf("Elapsed: %u us\n",t2-t1);
  for(i = 0; i < n; i++) printf("%lf ", gm->b[i]);
  printf("\n");
  for(i = 0; i < n; i++)
    G_FREE(a[i],n*sizeof(double))
  G_FREE(a,n*sizeof(double*))
  G_FREE(b,n*sizeof(double))
  MAIN_END
  return 0;
}

