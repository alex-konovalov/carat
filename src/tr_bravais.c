#include"typedef.h"

#include"getput.h"
#include"bravais.h"
#include"matrix.h"

main (int argc, char *argv[])
{
  int anz,
      i;

  rational *r;

  matrix_TYP **F;

  bravais_TYP *G,
              *H;

  read_header(argc, argv);
  if(FILEANZ != 1)
  {
    printf("Usage: %s 'file' [-n]\n",argv[0]);
    printf("\n");
    printf("file: bravais_TYP of the finite unimodular group G.\n");
    printf("\n");
    printf("Calculates the transposed group of G.\n");
    printf("\n");
    printf("Options:\n");
    printf("-n    : The space of invariant forms of the transposed group is not\n");
    printf("        calculated.\n");
    printf("-i    : Invert the generators of G, G->normal, G->cen to give an isomorphism.\n");
    printf("\n");
    printf("Cf. Tr.\n");
    if (is_option('h')){
       exit(0);
    }
    else{
       exit(31);
    }
  }

  G = get_bravais(FILENAMES[0]);
  H = tr_bravais(G,!is_option('n'),is_option('i'));

  put_bravais(H,NULL,NULL);

  exit(0);
}
