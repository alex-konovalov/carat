#include <typedef.h>
#include <getput.h>
#include <matrix.h>
#include <base.h>

int INFO_LEVEL;
extern int SFLAG;

main(int argc,char **argv){

  bravais_TYP *G,
              *N;

  matrix_TYP **normal,
             **x;

  bahn **strong;

  int i,
      j,
      l,
      anz,
      finite;

  read_header(argc,argv);

  if ((is_option('h') && optionnumber('h')==0) || (FILEANZ < 2)){
     printf("usage: \n");
     printf("   Normalizer_in_N file1 file2 \n");
     printf("\n");
     printf("Where file1 and file2 contain a bravais_TYP. Calculates the\n");
     printf("normalizer of the group in file1 in the group generated by\n");
     printf("the matrices of file2->gen, file2->cen and file2->normal.\n");
     printf("\n");
     printf("Options:\n");
     printf("\n");
     printf("-f     :  The program assumes that the calculated normalizer\n");
     printf("          is finite, and performs an base/strong generator\n");
     printf("          algorithm on it. In effect you will get less\n");
     printf("          generators for it. IT WILL WORK IF AND ONLY IF\n");
     printf("          THE CALCULATED NORMALIZER IS INDEED FINITE.\n");
     printf("\n");
     if (is_option('h')){
        exit(0);
     }
     else{
        exit(31);
     }
  }

  INFO_LEVEL = optionnumber('h');
  if (INFO_LEVEL & 8){
     SFLAG = 1;
  }
  finite = FALSE;
  if (is_option('f')) finite = TRUE;

  G = get_bravais(FILENAMES[0]);
  N = get_bravais(FILENAMES[1]);

  normal = normalizer_in_N(G,N,&anz,finite);

  printf("#%d\n",anz);
  for (i=0;i<anz;i++){
     put_mat(normal[i],NULL,NULL,2);
  }

  /* cleaning up memory for pointer_statistics */
  free_bravais(G);
  free_bravais(N);
  for (i=0;i<anz;i++) free_mat(normal[i]);
  free(normal);

  printf("end of Normalizer_in_N\n");

  if (INFO_LEVEL & 8){
     pointer_statistics(0,0);
  }

  exit(0);
} /* main */

