#include "typedef.h"
#include "gmp.h"
#include "longtools.h"
#include "matrix.h"
#include "bravais.h"

/***************************************************************************
@
@ --------------------------------------------------------------------------
@
@ FILE: row_spin.c
@
@ --------------------------------------------------------------------------
@
****************************************************************************/

static void product_of_stairs(MP_INT *x,MP_INT **A,int rows,int cols)
{
   int i,
       j;

   mpz_set_si(x,1);

   for (i=0;i<rows;i++){
      for (j=0;j<cols && (mpz_cmp_si(A[i]+j,0) ==0);j++);
      mpz_mul(x,x,A[i]+j);
   }

   mpz_abs(x,x);

   return;
}

/*************************************************************************
@
@-------------------------------------------------------------------------
@
@ matrix_TYP *row_spin(matrix_TYP *x,matrix_TYP **G,int no,int option)
@
@ The function calculates a basis for the R module <x_i>_RG, where
@ x_i are the rows of x, G is assumed to be generated by the matrices G[i]
@ for 0 <= i <= no, and R is one of the rings Z or Q.
@ NOTE: this function will handle non integral matrices in G[i] as well.
@
@ matrix_TYP  *x: see above. x is changed via kgv2rat and long_row_hnf.
@ matrix_TYP **G: see above. It is also changed by rat2kgv for each entry in G.
@ int no        : will not be changed.
@ option        : drives the behaviour of the function. in case
@                 option % 2 == 0 it performs a Z-spinning algorithm,
@                 option % 2 == 1 it performs a Q-spinning algorithm,
@                 option % 4 == 0 perform a pair_red after spinning
@                                 the lattice. ONLY IN CONJUNCTION WITH
@                                 Z-spinning
@-------------------------------------------------------------------------
@
*************************************************************************/
matrix_TYP *row_spin(matrix_TYP *x,matrix_TYP **G,int no,int option)
{
  int i,
      j,
      k,
      l,
      dim = x->cols,
      rows = x->rows,
      new_rows;

  matrix_TYP *res;

  long quot;

  MP_INT **A,
         **Gram,
         **TR,
          *merk,
           tmp,
           prod,
           denominator;

  /* check trivialities */
  rat2kgv(x);
  for (i=0;i<no;i++){
     rat2kgv(G[i]);
     if (G[i]->cols != dim || G[i]->rows!= dim){
         fprintf(stderr,"error in row_spin:\n");
         fprintf(stderr,"The %d-th given matrix is %dx%d,\n",
                 i,G[i]->rows,G[i]->cols);
         exit(3);
     }
  }

  /* inserted tilman 22/07/97 */
  long_row_hnf(x);

  /* prepare all the things */
  if (x->rows>dim){
     A = init_MP_mat(x->rows+1,dim);
  }
  else{
     A = init_MP_mat(dim+1,dim);
  }
  for (i=0;i<rows;i++)
    for (j=0;j<dim;j++)
      mpz_set_si(A[i]+j,x->array.SZ[i][j]);
  mpz_init(&tmp);
  mpz_init_set_ui(&prod,0);
  mpz_init_set_si(&denominator,x->kgv);

  for (i=0;i<rows;i++){
     for (j=0;j<no;j++){
        /* multiply the the i-th row in A with the j-th generator in G,
           and stick it in the rows-th row */
        for (k=0;k<dim;k++){
           mpz_set(A[rows]+k,0);
           for (l=0;l<dim;l++){
              /* A[rows][k] += A[i][l] * G[j]->array.SZ[k][l] */
              mpz_set_si(&tmp,G[j]->array.SZ[l][k]);
              mpz_mul(&tmp,&tmp,A[i]+l);
              mpz_add(A[rows]+k,A[rows]+k,&tmp);
           }
        }

        /* bare in mind what with the denominator happen */
        if (G[j]->kgv != 1 && (option % 2 == 0)){
           /* calculate the gcd of the rows-th row of A with G[j]->kgv */
           mpz_set_si(&tmp,G[j]->kgv);
           for (k=0;k<dim && mpz_cmp_si(&tmp,1)!=0;k++)
              mpz_gcd(&tmp,&tmp,A[rows]+k);

           /* divide all entries in the rows-th row of A by this gcd */
           if (mpz_cmp_si(&tmp,1) !=0)
              for (k=0;k<dim;k++)
                 mpz_div(A[rows]+k,A[rows]+k,&tmp);

           /* now we have to destinguish two cases, the first (and
              good) of which is that G[j]->kgv/tmp == 1, then the
              denominator will not be changed. otherwise multiply
              denominator by this quotient and multiply the rest of
              A by it */
           quot = abs(G[j]->kgv/mpz_get_si(&tmp));
           if (quot != 1)
              for (k=0;k<rows;k++)
                 for (l=0;l<dim;l++)
                    mpz_mul_ui(A[k]+l,A[k]+l,(unsigned long) quot);

           mpz_mul_ui(&denominator,&denominator,(unsigned long)quot);
        }

        /* apply gauss */
        new_rows = MP_row_gauss(A,rows+1,dim);
        /* dump_MP_mat(A,new_rows,dim,"A vor"); */

        /* make manhattan as far as feassible */
        MP_row_gauss_reverse(A,new_rows,dim,option % 2);

        /* dump_MP_mat(A,new_rows,dim,"A nach");
        mpz_out_str(stdout,10,&denominator);
        fprintf(stdout,"\n");
        fflush(stdout); */

        /* for a Q-spin the dimension is sufficient to describe the space */
        if ((option % 2 == 1) && new_rows != rows){
           i = 0;
           j = 0;
        }

        /* for a Z-spin, compare the product of the elements of A
           which are stair indices */
        if (option == 0){
           product_of_stairs(&tmp,A,new_rows,dim);
           /* mpz_out_str(stdout,10,&prod);
           fprintf(stdout,"\n");
           mpz_out_str(stdout,10,&tmp);
           fprintf(stdout,"\n");
           fflush(stdout); */
           if (mpz_cmp_si(&prod,0) == 0 ||
              mpz_cmp(&prod,&tmp) != 0 ||
              new_rows != rows){
              i = 0;
              j = 0;
           }
           mpz_set(&prod,&tmp);
        }

        rows = new_rows;
     }
  }

  /* I like a good basis, so what's up with pair_red */
  if (option % 2 == 0 || option % 4 == 0){
     /* set the gram matrix */
     Gram = init_MP_mat(rows,rows);
     for (i=0;i<rows;i++){
        for (j=0;j<rows;j++){
           for (k=0;k<dim;k++){
              /* Gram[i][j] += A[i][k]*A[j][k] */
              mpz_mul(&tmp,A[i]+k,A[j]+k);
              mpz_add(Gram[i]+j,Gram[i]+j,&tmp);
           }
        }
     }
     /* set the transformation matrix and do the pair_red */
     TR = init_MP_mat(rows,rows);
     for (i=0;i<rows;i++) mpz_set_si(TR[i]+i,1);
     MP_pair_red(Gram,TR,rows);

     free_MP_mat(Gram,rows,rows);
     Gram = init_MP_mat(rows,dim);

     /* multiply TR with A, stick it to Gram for a moment */
     for (i=0;i<rows;i++){
        for (j=0;j<dim;j++){
           for (k=0;k<rows;k++){
              /* Gram[i][j] += TR[i][k] * A[k][j] */
              mpz_mul(&tmp,TR[i]+k,A[k]+j);
              mpz_add(Gram[i]+j,Gram[i]+j,&tmp);
           }
        }
     }

     /* stick it back to A, and swap the pointers only */
     for (i=0;i<rows;i++){
        merk = A[i];
        A[i] = Gram[i];
        Gram[i] = merk;
     }

     /* we won't need TR and Gram anymore */
     free_MP_mat(TR,rows,rows);
     free_MP_mat(Gram,rows,dim);
  }

  res = init_mat(rows,dim,"");
  write_MP_mat_to_matrix(res,A);

  if (abs(denominator._mp_size>1)){
     fprintf(stderr,"denominator in row_spin to large!\n");
     fprintf(stderr,"(I thought this to be impossible)\n");
     exit(3);
  }

  if (option == 0){
     res->kgv = mpz_get_si(&denominator);
  }
  else{
     res->kgv = 1;
  }

  Check_mat(res);

  /* clear space */
  if (x->rows>dim){
     free_MP_mat(A,x->rows+1,dim);
  }
  else{
     free_MP_mat(A,dim+1,dim);
  }
  free(A);
  mpz_clear(&tmp);
  mpz_clear(&denominator);

  return res;
}

/**********************************************************************
@ bravais_TYP *representation_on_lattice(matrix_TYP *x,bravais_TYP *G,
@                                        int option)
@
@
***********************************************************************/
bravais_TYP *representation_on_lattice(matrix_TYP *x,bravais_TYP *G,
                                       int option)
{

  int i,
      j;

  bravais_TYP *H;

  matrix_TYP *xtr,
             *y,
             *z,
            **X;

  /* init H as far as possible in this stage */
  H = init_bravais(G->dim);
  H->gen = (matrix_TYP **) malloc(G->gen_no * sizeof(matrix_TYP *));
  H->gen_no = G->gen_no;

  xtr = tr_pose(x);

  for (i=0;i<H->gen_no;i++){
     /* map x by the first generator of H */
     y = mat_mul(x,G->gen[i]);
     z = tr_pose(y);
 
     /* firstly try to solve the GLS over a prime 
     p_lse_solve(); */

     /* if this goes wrong, to it the hard way and solve it via Z */
     X = long_solve_mat(xtr,z);

     /* paranoia */
     if (X[1] != NULL){
        if (X[1]->cols!=0){
           fprintf(stderr,"error in representation_on_lattice, X[1] wrong\n");
           exit(3);
        }
        free_mat(X[1]);
     }

     H->gen[i] = tr_pose(X[0]);

     free_mat(X[0]);
     free(X);
     free_mat(z);
     free_mat(y);

  }

  /* clear up */
  free_mat(xtr);

  return H;
}

/****************************************************************************
@
@----------------------------------------------------------------------------
@
@ matrix_TYP *translation_lattice(matrix_TYP **G,int number,matrix_TYP *P)
@
@ Calculates a basis of the translation lattice of the space group
@ generated by the matrices given in G[i], 0<= i < number.
@ This lattice is returned via a matrix containing the columns of a basis
@ of the lattice (without the affine '1').
@
@ matrix_TYP **G : Generators for the spacegroup.
@ int number     : see above
@ matrix_TYP *P  : a matrix containing a presentation for the point group
@                  to the space group. It has to be a presentation according
@                  to the generators G[i], i.e { G[i] * T }/T fullfill
@                  the relations given in P.
@
@----------------------------------------------------------------------------
@
*****************************************************************************/
matrix_TYP *translation_lattice(matrix_TYP **G,int number,matrix_TYP *P)
{

  int i,
      j,
      k,
      lcm;

  matrix_TYP **Gtr,
             **Ginv,
              *B,
              *X,
              *Y;

  /* check all the trivialities to avoid trouble */
  for (i=0;i<number;i++){
     rat2kgv(G[i]);
     if (G[i]->cols != G[0]->cols ||
         G[i]->rows != G[0]->cols){
        fprintf(stderr,"uncompatible dimensions in translation_lattice\n");
        exit(3);
     }
  }

  /* init all the needed stuff */
  Gtr = (matrix_TYP **) malloc(number * sizeof(matrix_TYP *));
  for (i=0;i<number;i++) Gtr[i]=tr_pose(G[i]);
  Ginv = (matrix_TYP **) calloc(number , sizeof(matrix_TYP *));
  Y = init_mat(P->rows,G[0]->cols-1,"");

  for (i=0;i<P->rows;i++){
     X = init_mat(G[0]->cols,G[0]->cols,"1");
     for (j=0;j<P->cols;j++){
        if (P->array.SZ[i][j] != 0){
           if (abs(P->array.SZ[i][j]) > number){
             fprintf(stderr,"translation_lattice: you asked for the %d-th"
               ,P->array.SZ[i][j]);
              fprintf(stderr," generator,\n but gave only %d\n",number);
              exit(3);
           }

           /* the generator should be there */
           if (P->array.SZ[i][j] > 0){
              mat_muleq(X,G[P->array.SZ[i][j]-1]);
           }
           else if (P->array.SZ[i][j] < 0){
              if (Ginv[-P->array.SZ[i][j]-1] == NULL)
                 Ginv[-P->array.SZ[i][j]-1] = mat_inv(G[-P->array.SZ[i][j]-1]);
              mat_muleq(X,Ginv[-P->array.SZ[i][j]-1]);
           }
           Check_mat(X);
        }

     }

     /* now we should have a trivial linear part in X, and the last
        column of X gives a translation */

     /* check whether the linear part is indeed trivial */
     rat2kgv(X);
     for (j=0;j<X->cols-1;j++){
        for (k=0;k<X->rows-1;k++){
           if ((k==j && X->array.SZ[k][j] != X->kgv) ||
               (k!=j && X->array.SZ[k][j] != 0)){
              fprintf(stderr,"translation_lattice: the linear part is not\n");
              fprintf(stderr,"trivial. exiting!\n");
              exit(3);
           }
        }
     }

     /* stick the resulting translation into Y */
     lcm = X->kgv * Y->kgv/ GGT(X->kgv,Y->kgv);
     if (lcm != Y->kgv)
        for (j=0;j<i;j++)
           for (k=0;k<Y->cols;k++)
              Y->array.SZ[j][k] *= (lcm/Y->kgv);

     Y->kgv = lcm;

     for (j=0;j<X->rows-1;j++)
        Y->array.SZ[i][j] = X->array.SZ[j][X->cols-1] * (lcm/X->kgv);

     free_mat(X);
  }

  /* spin these (row) vector by Gtr */
  for (i=0;i<number;i++) real_mat(Gtr[i],Gtr[i]->rows-1,Gtr[i]->cols-1);
  X = row_spin(Y,Gtr,number,0);

  B = tr_pose(X);

  /* clear all the stuff */
  for (i=0;i<number;i++) free_mat(Gtr[i]);
  free(Gtr);
  for (i=0;i<number;i++) if (Ginv[i] != NULL) free_mat(Ginv[i]);
  free(Ginv);
  free_mat(X);
  free_mat(Y);

  return B;

}
