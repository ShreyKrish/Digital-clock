#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

/* Markers used to bound trace regions of interest */
volatile char MARKER_START, MARKER_END;

int main(int argc, char* argv[])
{

    /* Record marker addresses */
    FILE* marker_fp = fopen(".marker","w");
    assert(marker_fp);
    fprintf(marker_fp, "%llx\n%llx",
        (unsigned long long int) &MARKER_START,
        (unsigned long long int) &MARKER_END );
    fclose(marker_fp);

    if (argc!=3) {
        printf("Usage: ./cacheBlocking <matrix_a_file> <matrix_b_file>\n");
        exit(EXIT_FAILURE);
    }

    FILE* matrix_a_fp = fopen(argv[1], "r");
    if (!matrix_a_fp) {
        perror("fopen failed");
        return EXIT_FAILURE;
    }
    size_t n;
    int ret = fscanf(matrix_a_fp, "%ld\n", &n);
    assert (ret==1);
    int* a = calloc( n*n, sizeof(int) );
    for ( size_t i=0; i<n; i++ ) {
        for ( size_t k=0; k<n; k++ ) {
            ret = fscanf(matrix_a_fp, "%d ", &a[i*n+k]);
            assert (ret==1);
        }
        ret = fscanf(matrix_a_fp, "\n");
    }
    fclose(matrix_a_fp);

    FILE* matrix_b_fp = fopen(argv[2], "r");
    if (!matrix_b_fp) {
        perror("fopen failed");
        return EXIT_FAILURE;
    }
    size_t m;
    ret = fscanf(matrix_b_fp, "%ld\n", &m);
    assert (ret==1);
    assert( n==m );
    int* b = calloc( n*n, sizeof(int) );
    for ( size_t k=0; k<n; k++ ) {
        for ( size_t j=0; j<n; j++ ) {
            ret = fscanf(matrix_b_fp, "%d ", &b[k*n+j]);
            assert (ret==1);
        }
        ret = fscanf(matrix_b_fp, "\n");
    }
    fclose(matrix_b_fp);

    int* c = calloc( n*n, sizeof(int) );
    MARKER_START = 211;

   
   size_t bSize;

    if (n>32) {
        bSize = n/2;
    } else {
        bSize = n/4;
    }


unsigned int i = 0;
while(i<n){
    unsigned int k = 0;
  while(k<n){
      unsigned int j = 0;
      while(j<n){
           unsigned int x = i; 
        while(x < i+ bSize){
           unsigned int y = k;
           while(y < k + bSize){
               unsigned int z  = j;
              while(z< j+ bSize){
                  c[x*n+z] += a[x*n+y] * b[y*n+z];
                  z++;
              }
               y++;
           }
            x++;
        }
          j = j+ bSize; 
      }
      k = k + bSize; 
  }
    i = i + bSize;
}

    // ijk
    // for ( size_t i=0; i<n; i++ ) {
    //     for ( size_t j=0; j<n; j++ ) {
    //         int sum = c[i*n+j];
    //         for ( size_t k=0; k<n; k++ ) {
    //             sum += a[i*n+k] * b[k*n+j];
    //         }
    //         c[i*n+j] = sum;
    //     }
    // }

    // jki
    // for ( size_t j=0; j<n; j++ ) {
    //     for ( size_t k=0; k<n; k++ ) {
    //         int r = b[k*n+j];
    //         for ( size_t i=0; i<n; i++ ) {
    //             c[i*n+j] += a[i*n+k] * r;
    //         }
    //     }
    // }
    MARKER_END = 211;

    for ( size_t i=0; i<n; i++ ) {
        for ( size_t j=0; j<n; j++ ) {
            printf( "%d ", c[i*n+j] );
        }
        printf( "\n" );
    }

    free(c);
    free(b);
    free(a);
    exit(EXIT_SUCCESS);

}
