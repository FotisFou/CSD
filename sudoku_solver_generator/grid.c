#include <stdio.h>
#include <stdlib.h>
#include "grid.h"



/* update value of i,j to n */
void grid_update_value( Grid_T *g, int i, int j, int n){
    g->elts[i][j].val = n;
    return;
}

/* return value of i,j */
int grid_read_value(Grid_T g, int i, int j){
	
    return g.elts[i][j].val;
}

/* set to 1 choice n for elt i,j */
void grid_set_choice(Grid_T *g, int i, int j, int n){
    g->elts[i][j].choices.num[n] = 1;
    return;
}

/* clear (set to 0) choice n for elt i,j */
void grid_clear_choice(Grid_T *g, int i, int j, int n){

    g->elts[i][j].choices.num[n] = 0;
    return;
}

int grid_choice_is_valid(Grid_T g, int i, int j, int n){
    return g.elts[i][j].choices.num[n] == 1;
}

void grid_remove_choice(Grid_T *g, int i, int j, int n){
    if(grid_choice_is_valid(*g,i,j,n)) g->elts[i][j].choices.count--; 
    grid_clear_choice(g,i,j,n);
}

int grid_read_count(Grid_T g, int i, int j){
    return  g.elts[i][j].choices.count;
}

void grid_set_count(Grid_T *g, int i, int j){
    g->elts[i][j].choices.count = 9;
}

void grid_clear_count(Grid_T *g, int i, int j){
    g->elts[i][j].choices.count = 0;
}

int grid_read_unique(Grid_T g){
    return g.unique;
}

void grid_set_unique(Grid_T *g){
    g->unique = 1;
}

void grid_clear_unique(Grid_T *g){
    g->unique = 0;
}

