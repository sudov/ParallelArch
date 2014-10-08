MAIN_ENV

void bksb(double **a,double *b,int n) {
  register int i,j,end;
  register double sum;
  end = n-1;
  for(i = end; i >= 0; i--) {
    sum = b[i];
    for(j = end; j > i; j--)
      sum -= a[i][j]*b[j];
    b[i] = sum/a[i][i];
  }
}

int main(int argc, char **argv) {
  int n,i,j;
  double **a;
  double *b, count=1.0;
  unsigned int t1,t2;
  MAIN_INITENV
  if (argc!=2) {
     printf("Usage: bksb <size>\nAborting...\n");
     exit(0);
  }
  n = atoi(argv[1]);
  a = (double**)G_MALLOC(n*sizeof(double*));
  for(i = 0; i < n; i++) {
    a[i] = (double*)G_MALLOC(n*sizeof(double));
    for(j = i;j < n;j++) {
       a[i][j] = count;
       count++;
    }
  }
  b = (double*)G_MALLOC(n*sizeof(double));
  for(i = 0; i < n; i++) {
    b[i] = count;
    count++;
  }
  CLOCK(t1)
  bksb(a,b,n);
  CLOCK(t2)
  printf("Elapsed: %u microseconds\n",t2-t1);
  for(i = 0; i < n; i++) printf("%lf ", b[i]);
  printf("\n");
  MAIN_END
  return 0;
}

