#ifdef __cplusplus
extern "C" {
#endif


#ifndef _TSUBGROUPS_H_
#define _TSUBGROUPS_H_

#ifndef _CARAT_TYPEDEF_H_
#include"typedef.h"
#endif

#ifndef __GMP_H__
#include"gmp.h"
#endif

#ifndef _NAME_H_
#include"name.h"
#endif


typedef struct{
   bravais_TYP *R;	/* translationengleich maximal subgroup */
   bravais_TYP *P;	/* point group of R */
   int orbitlength;     /* length of the orbit */
   int pointgrouporder;	/* order of P */
} TSubgroup_TYP;
 
typedef struct{
   char qname[1024];	/* name of the Q-class */
   int zname[2];	/* name of the Z-class */
   MP_INT aff_name;	/* name of the affine class */
   matrix_TYP *trafo;	/* transformation matrix, conjugating the given group in CARAT rep. */
   int order;		/* order of the point group */
} CARATname_TYP;

typedef struct{
   matrix_TYP **words;	/* subgroupwords */
   int word_no;		/* number of words */
} TSUB_TYP;


/* -------------------------------------------------------------- */
#ifdef __STDC__

TSubgroup_TYP **tsubgroup(bravais_TYP *R,
                           bravais_TYP *P,
                           matrix_TYP *pres,
                           matrix_TYP **gapwords,
                           int *no,
                           boolean aflag,
			   boolean cflag);

TSubgroup_TYP *ite_gruppe(bravais_TYP *R,
                          bravais_TYP *P,
			  bravais_TYP *Rinv,
			  bravais_TYP *Pinv,
                          matrix_TYP *mat,
			  boolean aflag);

TSubgroup_TYP **tsubgroup_db(bravais_TYP *R,
                             boolean aflag,
                             int *anzahl);

void free_TSubgroup_TYP(TSubgroup_TYP *sbg);

matrix_TYP **PoaN(bravais_TYP *R,
                  bravais_TYP *P,
		  matrix_TYP *pres,
		  int *anz);

CARATname_TYP name_fct(bravais_TYP *R,
                       database *database);

void free_CARATname_TYP(CARATname_TYP Name);

bravais_TYP *get_std_rep(char *filename,
                         CARATname_TYP Name);

matrix_TYP **get_words(char *pfad,
                       CARATname_TYP Name,
		       int aff_class_no,
		       boolean aflag,
		       int *anzahl);

bravais_TYP **tsupergroups(bravais_TYP *R,
                           int *anzahl);

/* -------------------------------------------------------------- */
#else

TSubgroup_TYP **tsubgroup();

TSubgroup_TYP *ite_gruppe();

TSubgroup_TYP **tsubgroup_db();

void free_TSubgroup_TYP();

matrix_TYP **PoaN();

void name_fct();

void free_CARATname_TYP();

bravais_TYP *get_std_rep();

matrix_TYP **get_words();

bravais_TYP **tsupergroups();


/* -------------------------------------------------------------- */
#endif
#endif

#ifdef __cplusplus
}
#endif

