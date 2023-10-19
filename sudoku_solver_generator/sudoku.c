/*********************************
sudoku.c
@author fotis fouskas 4066

*/
#include "sudoku.h"
#include <assert.h>
#include <ctype.h>

#define RANGE 9
#define CELLS 81

Grid_T sudoku_read(void){
  Grid_T g;

  char str[2];
  int n;
  int count = 0;
  int i = 0, j = 0;
  str[1] = '\0';
  while ((str[0] = getchar()) != EOF) {
    if (str[0] == '\n') {             /* EPOMENI SEIRA */
      i++;                            
      j = 0;                          
      continue;                       
    }
    if (isdigit((int)str[0])) {       /* if number */
      n = atoi(str);                  
      if (n < 0 || n > RANGE) {         /* check int range */
        fprintf(stderr, "wrong input\n");
        exit(EXIT_FAILURE);
      }
      grid_update_value(&g, i, j, n);  
      j++;                            
      count++;                        
    } else if (str[0] != ' ') {       /* check if not space */
      fprintf(stderr, "wrong input\n");
      exit(EXIT_FAILURE);
    }
  }
  if (count != CELLS) {               /* check total inputs */
    fprintf(stderr, "wrong input\n");
    exit(EXIT_FAILURE);
  }
  return g;
}

void sudoku_print(FILE *s, Grid_T g){
    int i,j;
    for(i=0;i<RANGE;i++){
        for(j=0;j<RANGE;j++){
            fprintf(s, "%d ", grid_read_value(g,i,j));
        }
        fprintf(s, "\n");
    }
}

void print_errors(Grid_T g){
    int i, j, k, l, num;
    for (i = 0; i<RANGE; i++){
        for (j = 0; j<RANGE; j++){
            num = grid_read_value(g,i,j);
            if (num == 0)
                continue;
            for (k = 0; k<RANGE; k++){
                if (num == grid_read_value(g, i, k) && k!=j){/*    ROW    */
                    printf("fault in pair: [%d , %d] , [%d , %d]\n",i,j,i,l);
                }
                if (num == grid_read_value(g, k, j) && k!=i){/*  COLUMN  */
                    printf("fault in pair: [%d , %d] , [%d , %d]\n",i,j,k,j);
                }
            }
            for (k = (i/3)*3; k<(i/3)*3+3; k++){
                for (l = (j/3)*3; l<(j/3)*3+3; l++){
                    if ( (((k!=i) && (l==j))||((k==i) && (l!=j)))
                            && num == grid_read_value(g, k, l)){/* SMALL BLOCK */
                        printf("fault in pair: [%d,%d] - [%d,%d]\n",i, j, k, l);
                    }
                }
            }
        }
    }
}
/* RETURN 1 IF THERE IS AN ERROR */
/* idios tropos me to panw */
int has_errors(Grid_T g)
{
    int i, j, k, l, num;
    int flag = 0;
    for (i = 0; i<RANGE; i++)
        for (j = 0; j<RANGE; j++){
            num = grid_read_value(g,i,j);
            if (num == 0)
                continue;
            for (k = 0; k<RANGE; k++){
                if (num == grid_read_value(g, i, k) && k!=j)
                    flag = 1;
                if (num == grid_read_value(g, k, j) && k!=i)
                    flag = 1;
            }
	    k = (i/3)*3;
            while (k < (i/3)*3+3)
	    {
		l = (j/3)*3;
                while (l < (j/3)*3+3)
		{
                    if ( (((k!=i) && (l==j))||((k==i) && (l!=j)))
                            && num == grid_read_value(g, k, l))
                        flag = 1;
		    l++;
		}
		k++;
	    }
        }
    return flag;
}
/*RETURN 1 IF ITS CORRECT.
*/
int sudoku_is_correct(Grid_T g){
	if(has_errors(g)==1){
		return 0;
	}else{
		return 1;
	}
}

static void sudoku_apply_choice(Grid_T *g, int i, int j, int n){
  grid_update_value(g, i, j, n);
  return;
}
int sudoku_solution_is_unique(Grid_T g){
    return (grid_read_unique(g));
}
/*
 * fix all choices for every cell in Grid
 */
void sudoku_init_choices(Grid_T *g){
    int i, j, k, l, m;
    grid_set_unique(g);
    for (i = 0; i<RANGE; i++) {
        for (j = 0; j<RANGE; j++){
            grid_set_count(g,i,j);
            grid_clear_choice(g,i,j,0);
            for (m = 1; m<=RANGE; m++)
                grid_set_choice(g, i, j, m);
        }
    }/*all choices set*/
    for (i = 0; i<RANGE; i++) {
        for (j = 0; j<RANGE; j++){
            m = grid_read_value(*g,i,j);
            if (m != 0){      /* cell not empty */
                grid_clear_count(g,i,j);
                for (m=1; m<RANGE + 1; m++)
                    grid_clear_choice(g, i, j, m);
                continue;
            } /* if cell is taken clear all choices */
            else{           /* cell is empty */
                for (k = 0; k<RANGE; k++){
                    m = grid_read_value(*g, k, j);
                    grid_remove_choice(g, i, j, m); /* clear all in COLUMN */
		    m = grid_read_value(*g, i, k);
                    grid_remove_choice(g, i, j, m); /* clear all in ROW */
                }
                for (k = (i/3)*3; k < (i/3)*3+3; k++)
                    for (l = (j/3)*3; l < (j/3)*3+3; l++){
                        m = grid_read_value(*g, k, l);
                        grid_remove_choice(g, i, j, m);/* remove all in 3x3 */
                    }
            }
        }
    }return;
}

/* epistrefei mia tuxaia epilogh apo ta cells pou exoun monadikh epilogh */
static int sudoku_try_next_random(Grid_T g, int *row, int *col)
{
    int i, j, k, n;
    int nchoice = RANGE + 1;

    for (i = 0; i < RANGE; i++)
    {
	for (j = 0; j < RANGE; j++)
	{
	    n = grid_read_count(g, i, j);
	    if (n > 0 && n < nchoice)
	        nchoice = n;
	}
    }
    /*twra to nchoice einai to mikrotero dunato */
    while (nchoice < RANGE + 1)
    {
	i = rand()%RANGE;
	j = rand()%RANGE;
	n = grid_read_count(g, i, j);/*pairnw random cells mexri na isxuei n == nchoice*/
	if (n == nchoice) {
	    while (1) {
		k = rand()%RANGE + 1;/* dinei random nums mexri vrei kapoion pou ontws uparxei*/
		if (grid_choice_is_valid(g, i, j, k))
		{/*ekxwrhsh timhs*/
		    *row = i;
		    *col = j;
		    return k;
		}
	    }
	}
    }
    return 0;/*alliws to afhnei 0*/
}

/* Remove value from grid */
static int sudoku_remove_choice(Grid_T *g, int i, int j, int n)
{/*removes mia choice kai kanei return ton ari8mo twn choices prin kanei remove*/
  int nchoice;

  nchoice = grid_read_count(*g, i, j);
  grid_remove_choice(g, i, j, n);

  return nchoice;
}

/*remove to n apo, thn r, thn c kai to upo-grind */
static void sudoku_eliminate_choice(Grid_T *g, int r, int c, int n)
{
  int k, l;
  for (k = 0; k < RANGE; k++)
  {
    grid_remove_choice(g, k, c, n);        /* diagrafh tou n apo th sthlh*/
    grid_remove_choice(g, r, k, n);        /* diagrafh tou n apo th grammh */
  }

  k = (r/3)*3;
  while (k < (r/3)*3+3)
  {
    l = (c/3)*3; 
    while (l < (c	 /3)*3+3)
    {
      grid_remove_choice(g, k, l, n);      /* diagrafh tou n apo to ypo-grid*/
       l++;
    }
    k++;
  }

  return;
}

Grid_T sudoku_generate(void){
  Grid_T new_g;
  int i, j, n, stop;
  int ready_puzzle[RANGE*RANGE] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 
                              4, 5, 6, 7, 8, 9, 1, 2, 3, 
                              7, 8, 9, 1, 2, 3, 4, 5, 6, 
                              2, 3, 4, 5, 6, 7, 8, 9, 1, 
                              5, 6, 7, 8, 9, 1, 2, 3, 4, 
                              8, 9, 1, 2, 3, 4, 5, 6, 7, 
                              3, 4, 5, 6, 7, 8, 9, 1, 2, 
                              6, 7, 8, 9, 1, 2, 3, 4, 5, 
                              9, 1, 2, 3, 4, 5, 6, 7, 8 };
  stop = RANGE + 1;
  while ((stop--) != 0) {               /* not guarranteed so limit tries */
    for (i = 0; i < RANGE; i++) {                          /* access */
      for (j = 0; j < RANGE; j++){                         /* every element */
        grid_update_value(&new_g, i, j, 0);              /* init zero */
      }
    }
    sudoku_init_choices(&new_g);
    while ((n = sudoku_try_next_random(new_g, &i, &j))) {
      sudoku_remove_choice(&new_g, i, j, n);
      sudoku_apply_choice(&new_g, i, j, n);
      sudoku_eliminate_choice(&new_g, i, j, n);
    }
    if (sudoku_is_correct(new_g))
      return new_g;
  }
  n = 0;
  for (i = 0; i < RANGE; i++) {                          /* access */
    for (j = 0; j < RANGE; j++){                         /* every element */
      grid_update_value(&new_g, i, j, ready_puzzle[n++]);/* init ready */
    }
  }
  sudoku_init_choices(&new_g);
  return new_g;
}

Grid_T sudoku_solve(Grid_T g){
  Grid_T tmp_g, sol;
  int i, j;
  int n;
  int nchoice;
  while ((n = sudoku_try_next_random(g, &i, &j))) { /* tsekarei diathesimes epiloges */
    nchoice = sudoku_remove_choice(&g, i, j, n);
    if (nchoice == 1) {
      sudoku_apply_choice(&g, i, j, n);
      sudoku_eliminate_choice(&g, i, j, n);
    } else {
      grid_clear_unique(&g);
      tmp_g = g;
      sudoku_apply_choice(&tmp_g, i, j, n);
      sol = sudoku_solve(tmp_g);
      if (sudoku_is_correct(sol))
        return sol;
    }
  }
  return g;
}


int main(int argc, char **argv){
  Grid_T init, final;
  int n, number=0;
  
  srand(getpid());
  switch (argc) {
    case 1:         /* sudoku */
      init = sudoku_read();
      sudoku_print(stderr, init);
      sudoku_init_choices(&init);
      final = sudoku_solve(init);
      if (sudoku_is_correct(final)) {
        if (sudoku_solution_is_unique(final))
          fprintf(stderr, "puzzle has a unique solution:\n");
        else
          fprintf(stderr, "puzzle has a solution:\n");
      } else {
        fprintf(stderr, "puzzle does not have solution:\n");
      }
      sudoku_print(stdout, final);
      
      break;
    case 2:         /* check solution */
      if (argv[1][0] == '-' && 
          argv[1][1] == 'c' && 
          argv[1][2] == '\0') {
        init = sudoku_read();
        sudoku_print(stderr, init);
        if (sudoku_is_correct(init))
            fprintf(stderr, "puzzle solution is correct.\n");
        else
          fprintf(stderr, "puzzle solution is not correct.\n");
        print_errors(init);
      }
      break;
    case 4:         /* generate puzzle */
      if (argv[3][0] == '-' && 
          argv[3][1] == 'u' && 
          argv[3][2] == '\0')
        ;
      else
        break;
    case 3:
      if (argv[1][0] == '-' && 
          argv[1][1] == 'g' && 
          argv[1][2] == '\0') {
        n = 0;
        number = 1;
        while (argv[2][n] != '\0')
          if (!isdigit((int)argv[2][n++])) {
            number = 0;
        }
        if (number==1) {
          n = atoi(argv[2]);
          if (argc == 4)
            init = sudoku_generate();
          else
            init = sudoku_generate();
          sudoku_print(stdout, init);
        }
      }
      break;
    default:
      fprintf(stderr,
"wrong number of arguments\n\
available options:\n\
sudoku [-c] / [-g nelts [-u]]\n");
  }
  
  return 0;
}
