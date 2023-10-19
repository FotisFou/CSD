
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Darwin.h"

#define BUFFER_SIZE 1024  /* Maximum length of a line in input file */

/* Uncomment the following line to enable debugging prints 
 * or comment to disable it */
#define DEBUG

#ifdef DEBUG
#define DPRINT(...) fprintf(stderr, __VA_ARGS__);
#else  /* DEBUG */
#define DPRINT(...)
#endif /* DEBUG */
int print=1;//variable to control the prints of the functions(only if print==1 print)

/**
 * @brief Optional function to initialize data structures that 
 *        need initialization
 *
 * @return 1 on success
 *         0 on failure
 */
int initialize (void)
{
	Species_root=NULL;
	
	int i=0;
	for(i=0;i<5;i++){
		continents[i]=(struct ContinentTree*)malloc(sizeof(struct ContinentTree));
		continents[i]->population_root=NULL;
		continents[i]->sentinel=(struct ContinentPopulation*)malloc(sizeof(struct ContinentPopulation));
		continents[i]->sentinel->gid=-1;
		continents[i]->sentinel->lc=continents[i]->sentinel->rc=NULL;	
	}
	
	return 1;
}

//recursive function to return the first smaller node of the node with sid key.
struct Species*  returnNode(struct Species *species,int sid){
	if(species->lc==NULL||species->sid<sid){
		return species;
	}
	if(sid>species->lc->sid &&species->rc!=NULL){
		return returnNode(species->rc,sid);	
	}else{
		return returnNode(species->lc,sid);
	}
}

//function to return the most left element on tree
struct Species *returnLeftNode(struct Species *root){
	if(root->lc==NULL){
		return root;
	}
	returnLeftNode(root->lc);
}
 
/**
 * @brief insert new species in Species' list with ID <sid>
 *
 * @return 1 on success
 *         0 on failure
 */
int insert_species (int sid)
{
	struct Species *newSpecies = (struct Species*)malloc(sizeof(struct Species));
	if(newSpecies==NULL){
		printf("Error,not enough memory");
		return 0;
	}
	newSpecies->sid=sid;
	newSpecies->population_root=NULL;
	newSpecies->lc=newSpecies->rc=NULL;
	
	//if we are creating the first node
	if(Species_root==NULL){
		Species_root =newSpecies;	
	}else{
		
		//if sid is bigger than root sid
	if(sid>Species_root->sid){
			if(Species_root->lc==NULL){
				newSpecies->lc=Species_root;
				Species_root=newSpecies;
			}else{
				newSpecies->lc=Species_root->lc;
				newSpecies->rc=Species_root;
				Species_root->lc=Species_root->rc;
				Species_root->rc=NULL;
				Species_root=newSpecies;
		}
		}else if(Species_root->lc==NULL || Species_root->rc==NULL){
			if(Species_root->lc==NULL &&Species_root->rc==NULL){
				Species_root->lc=newSpecies;			
			}else{
				if(sid>Species_root->lc->sid){
					Species_root->rc=newSpecies;
				}else{
					Species_root->rc=Species_root->lc;
					Species_root->lc=newSpecies;
				}
			}
			
		}
		else{//general case
			struct Species *tmp,*root, *leftnode;
			root=Species_root;
			tmp=returnNode(Species_root,sid);
			
			if(tmp->sid>sid){	
			//	if(tmp->rc==NULL){
					printf("check4\n");
				tmp->lc=newSpecies;			
			/*	}else{
					printf("check5\n");
					tmp->lc=tmp->rc;
					tmp->rc=newSpecies;
				} */
														
			}else{
			
				if(tmp->lc==NULL){
					printf("check1\n");
					if(tmp->sid<sid){
					newSpecies->sid=tmp->sid;
					tmp->lc=tmp->rc;
					tmp->rc=newSpecies;
					tmp->sid=sid;	
					}
				}else if(tmp->rc==NULL){
					printf("check2\n");
					newSpecies->sid=tmp->sid;
					tmp->rc=newSpecies;
					tmp->sid=sid;								
				}else{
					printf("check3\n");
					newSpecies->sid=tmp->sid;
					newSpecies->lc=tmp->lc;
					newSpecies->rc=tmp->rc;
					tmp->sid=sid;
					tmp->lc=newSpecies;
					tmp->rc=NULL;
				}
			}
		}	
	}
	if(print==1){
		printf("\n");	
		print_species();
		printf("\n");
	}	
	return 1;
}

//function to search the species tree.
struct Species *searchSpecies(struct Species *root,int sid){
	
	if(sid==root->sid || root->lc==NULL){
		return root;
	}
	if(sid>root->lc->sid){
		return searchSpecies(root->rc,sid);	
	}
	return searchSpecies(root->lc,sid);	
}

//function to find the parent of a population.
struct Population *findParent(struct Population *root,int gid,struct Population *parent){
	if(root==NULL){
		return root; 
	}
	if(root->gid==gid){
		return parent;
	}else{
		findParent(root->lc,gid,root);
		findParent(root->rc,gid,root);
	}	
}
//function to insert a new population 
struct Population *insert(struct Population *root,struct Population *new){
		if(root==NULL) {
			return new;	
		}
		if(new->gid<root->gid){
			root->lc=insert(root->lc,new);
		}else if (new->gid>root->gid){
			root->rc=insert(root->rc,new);
		}
		return root;
}

//function to print a population tree of a specie.
void printInorder(struct Population *root){
	if(root==NULL){
		return;
	}
	printInorder(root->lc);
	printf("<%d,%d>,",root->gid, root->cid);
	printInorder(root->rc);
}


/**
 * @brief insert new population with ID <gid> in Species' with ID <sid>
 *
 * @return 1 on success
 *         0 on failure
 */
int insert_population(int gid, int sid, int cid){
	struct Population *newPopulation=(struct Population*)malloc(sizeof(struct Population));
	struct Population *proot=(struct Population*)malloc(sizeof(struct Population));
	struct Species *sroot=Species_root,*currentSpecie=NULL;
	if(newPopulation==NULL){
		printf("Error,not enough memory");
		return 0;
	}
	
	newPopulation->gid=gid;
	newPopulation->sid=sid;
	newPopulation->cid=cid;
	newPopulation->lc=newPopulation->rc=newPopulation->parent=NULL;
	
	currentSpecie=searchSpecies(sroot,sid);
	
	if(currentSpecie==NULL){
		printf("Error,no specie with <%d> sid found.",sid);
		return 0;
	}
	if(currentSpecie->population_root==NULL){//if root is null
		currentSpecie->population_root=newPopulation;	
	}else{
		proot=currentSpecie->population_root;
		insert(proot,newPopulation);
		newPopulation->parent=findParent(proot,gid,proot);	
	}
	if(print==1){
	
	printf("\n");
	printf("<%d>\n",currentSpecie->sid);
	printInorder(currentSpecie->population_root);
	printf("\n");		
	}
	return 1;
}

//function to search the population bst.
struct Population *searchPopulations(struct Population *root,int gid){
	if(root==NULL || root->gid==gid){
		return root;
	}
	if(root->gid<gid){
		return searchPopulations(root->rc,gid);
	}
	return searchPopulations(root->lc,gid);
}

//function to find the lowest common ancestor of the population bst.
int findAncestor(struct Population *root,int gid1,int gid2){
	if(root==NULL){
		return 0;
	}
	if(root->gid<gid1&&root->gid<gid2){
		return findAncestor(root->rc,gid1,gid2);
	}
	
	if(root->gid>gid1 && root->gid>gid2){
		return findAncestor(root->lc,gid1,gid2);
	}
	return root->gid;
}

/**
 * @brief find the lowest (earliest) common ancestor of populations with ID <gid1> and <gid2> of species with ID <sid>
 *
 * @return 1 on success
 *         0 on failure
 */
int lowest_common_ancestor(int sid, int gid1, int gid2){
	
	struct Species *specie=searchSpecies(Species_root,sid);
	struct Population *pop1,*pop2;
	pop1=searchPopulations(specie->population_root,gid1);
	pop2=searchPopulations(specie->population_root,gid2);
	if(pop1==NULL || pop2==NULL){
		printf("Error,one population does not exist");
		return 0;
	}
	int ancestor=findAncestor(specie->population_root,pop1->gid,pop2->gid);
	if(ancestor==0){
		return 0;
	}else{
		printf("Lowest Common Ancestor:<%d>\n", ancestor);	
	}
	return 1;
}

//function to count all the nodes in a population bst.
int countNodes(struct Population *root){
	int size=1;
	if(root->lc !=NULL){
		size+=countNodes(root->lc);
	}
	if(root->rc !=NULL){
		size+=countNodes(root->rc);
	}	
	return size;		
}

//function to store the sorted nodes of population bst in an array
int bst2arrayCounter=0;
void bst2array(struct Population *root,struct Population *arr[]){
	if(root==NULL){
		return;
	}
	bst2array(root->lc,arr);
	arr[bst2arrayCounter++]=root;
	bst2array(root->rc,arr);	
}

//function to convert the sorted array to bst.
struct Population *array2bst(struct Population *arr[],int start,int end){
	if(start>end){
		return NULL;
	}
	//visit
	int middle=(start+end)/2;
	struct Population *arrayMiddle=(struct Population*)malloc(sizeof(struct Population));
	arrayMiddle=arr[middle];
	struct Population *root=arrayMiddle;
	
	root->lc=array2bst(arr,start,middle-1);
	root->rc=array2bst(arr,middle+1,end);
}

//search species for merge
struct Species *searchSpeciesMerge(struct Species *root,int sid){
	
	if(sid==root->sid || root->lc==NULL){
		
		return root;
	}
	
	if(sid>root->lc->sid){
		return searchSpeciesMerge(root->rc,sid);	
	}else{
		return searchSpeciesMerge(root->lc,sid);	
	}	
}

/**
 * @brief merge species with IDs <sid1> and <sid2> into new species with ID <sid3>
 *
 * @return 1 on success
 *         0 on failure
 */
int merge_species(int sid1, int sid2, int sid3){
	print=0;//variable to disable the prints from the function insert species.
	insert_species(sid3);
	print=1;
	struct Species *sp1,*sp2;
	sp1=searchSpeciesMerge(Species_root,sid1);
	printf("sp1----<%d>\n",sp1->sid);
	sp2=searchSpeciesMerge(Species_root,sid2);
	printf("sp2----<%d>\n",sp2->sid);
	if(sp1->sid !=sid1 || sp2->sid !=sid2){
		printf("Error,one specie does not exist\n");
		return 0;
	}
	
	if(sp1->population_root==NULL  || sp2->population_root==NULL){
	
		if(sp1->population_root==NULL&& sp2->population_root==NULL){
			searchSpecies(Species_root,sid3)->population_root=NULL;
		}else{
			if(sp1->population_root==NULL){
				searchSpecies(Species_root,sid3)->population_root=sp2->population_root;
			}else{
				searchSpecies(Species_root,sid3)->population_root=sp1->population_root;
			}
		}
		
	}else{
		
		int size1=countNodes(sp1->population_root)-1;
		int size2=countNodes(sp2->population_root)-1;
		int size3=size1+size2+1;
		struct Population *arr1[size1],*arr2[size2], *arr3[size3];
		bst2array(sp1->population_root,arr1);
		bst2arrayCounter=0;
		bst2array(sp2->population_root,arr2);
		bst2arrayCounter=0;
		
		//*************Merging the arrays******************************
		int index1=0,index2=0,index3=0;
		
		while(index1<=size1 && index2<=size2){
			if(arr1[index1]->gid<arr2[index2]->gid){
				arr3[index3++]=arr1[index1++];
			}else{
				arr3[index3++]=arr2[index2++];
			}
		}
		while(index1<=size1){
			arr3[index3++]=arr1[index1++];
		}
		while(index2<=size2){
			arr3[index3++]=arr2[index2++];
		}
		//***********end of merging***********************************
	
		searchSpecies(Species_root,sid3)->population_root=array2bst(arr3,0,size3);
		
		//delete
	//	deleteSpeciePopulations(sp1->population_root);
	//	deleteSpeciePopulations(sp2->population_root);                   
		/*
		int i1=0;
		for(i1=0;i1<=size1;i1++){
			printf("arr1[%d]= <%d>\n", i1, arr1[i1]->gid);
		}
		printf("\n");
		int z1=0;
		for(z1=0;z1<=size2;z1++){
			printf("arr2[%d]= <%d>\n", z1, arr2[z1]->gid);
		}
		printf("\n");
		int j1=0;
		for(j1=0;j1<=size3;j1++){
			printf("arr3[%d]= <%d>\n",j1,arr3[j1]->gid);
		} */
	}
	deleteSpeciePopulations(sp1->population_root);
	deleteSpeciePopulations(sp2->population_root);
	print_populations(); 
	
	return 1;
}
//function to insert a population to a Continent.
struct ContinentPopulation *insertContinentPopulation(struct ContinentPopulation *croot,int gid){

	if(croot==NULL){
		struct ContinentPopulation *new=(struct ContinentPopulation*)malloc(sizeof(struct ContinentPopulation));
		new->gid=gid;
		new->lc=new->rc=NULL;
		return new; 
	}
	if(gid<croot->gid){
		croot->lc=insertContinentPopulation(croot->lc,gid);
	}else if(gid>croot->gid){
		croot->rc=insertContinentPopulation(croot->rc,gid);
	}
	return croot;
}


//function traversing Population tree in preorder needed for distribute.
void PopulationPreorder(struct Population *proot){
	if(proot==NULL){
		return;
	}
	//visit
	struct ContinentPopulation *croot=(struct ContinentPopulation*)malloc(sizeof(struct ContinentPopulation));
	croot=continents[proot->cid]->population_root;
	if(croot==NULL){
		continents[proot->cid]->population_root=insertContinentPopulation(croot,proot->gid);		
	}
	insertContinentPopulation(croot,proot->gid);
	
	
	PopulationPreorder(proot->lc);
	PopulationPreorder(proot->rc);
}

//function traversing Species tree in preorder.
void SpeciesPreorder(struct Species *sroot){
	if(sroot==NULL){
		return;
	}
	//visit
	struct Population *proot=sroot->population_root;
	PopulationPreorder(proot);
	
	SpeciesPreorder(sroot->lc);
	SpeciesPreorder(sroot->rc);	
}

/**
 * @brief Distribute species' population in continents array
 *
 * @return 1 on success
 *         0 on failure
 */
int distribute(){
	if(Species_root==NULL){
		printf("Error,no species found\n");
		return 0;
	}
	struct Population *proot=Species_root->population_root;
	SpeciesPreorder(Species_root);
	print_continents();
	return 1;
}

//function to return smallest gid in the population bst.
struct Population *returnSmallest(struct Population *root){
	if(root->lc==NULL){
		return root;
	}
	returnSmallest(root->lc);
}

//function to delete a population from the population bst.
struct Population *deletePopulation(struct Population *root,int gid){
	if(root==NULL){
		return root;
	}
	
	if(gid<root->gid){
		root->lc= deletePopulation(root->lc,gid);
	}else if(gid>root->gid){
		root->rc=deletePopulation(root->rc,gid);
	}else if(gid==root->gid){
		
		if(root->lc==NULL){
			if(root->rc==NULL){
				free(root);
				return NULL;
			}
			struct Population *tmp=root->rc;
			free(root);
			return tmp;
		}else if(root->rc==NULL){
			struct Population *tmp=root->lc;
			free(root);
			return tmp;
		}
		//if node to be deleted has two children.
		struct Population *tmp=returnSmallest(root->rc);
		root->gid= tmp->gid;
		root->rc=deletePopulation(root->rc,tmp->gid);
		
	}
}
//function to search a continent bst.
struct ContinentPopulation *searchContPopulations(struct ContinentPopulation *root,int gid){
	if(root==NULL || root->gid==gid){
		return root;
	}
	if(root->gid<gid){
		return searchContPopulations(root->rc,gid);
	}
	return searchContPopulations(root->lc,gid);
}

//function to return smallest gid in the continent bst.
struct ContinentPopulation *returnCSmallest(struct ContinentPopulation *root){
	if(root->lc==NULL){
		return root;
	}
	returnCSmallest(root->lc);
}

//function to return smallest gid in the population bst.
struct Population *returnPSmallest(struct Population *root){
	if(root->lc==NULL){
		return root;
	}
	returnPSmallest(root->lc);
}

//function to delete a population from the continent bst.
struct ContinentPopulation *deleteContPopulation(struct ContinentPopulation *root,int gid){
	if(root==NULL){
		return root;
	}
	
	if(gid<root->gid){
		root->lc= deleteContPopulation(root->lc,gid);
	}else if(gid>root->gid){
		root->rc=deleteContPopulation(root->rc,gid);
	}else if(gid==root->gid){
		
		if(root->lc==NULL){
			struct ContinentPopulation *tmp=root->rc;
			free(root);
			return tmp;
		}else if(root->rc==NULL){
			struct ContinentPopulation *tmp=root->lc;
			free(root);
			return tmp;
		}
		//if node to be deleted has two children.
		struct ContinentPopulation *tmp=returnCSmallest(root->rc);
		root->gid= tmp->gid;
		root->rc=deleteContPopulation(root->rc,tmp->gid);	
	}
}

//function to print a continent bst.
void printContinentInorder(struct ContinentPopulation *root){
	if(root==NULL){
		return;
	}
	printContinentInorder(root->lc);
	printf("<%d>,",root->gid);
	printContinentInorder(root->rc);
}


/**
 * @brief delete population with ID <gid> from species with ID <sid>
 *
 * @return 1 on success
 *         0 on failure
 */
int delete_population(int gid, int sid){
	struct Species* specie=	searchSpecies(Species_root,sid);
	struct Population *pop2delete=searchPopulations(specie->population_root,gid);
	if(pop2delete==NULL){
		printf("Error,population not found\n");
		return 0;
	}	
	//delete from population bst
	struct Population* poproot=searchSpecies(Species_root,sid)->population_root;
	if(gid==poproot->gid){
		poproot=deletePopulation(poproot,gid);
		specie->population_root=poproot; 
	}else{
		deletePopulation(poproot,gid);
	}
	
	if(deletePopulation(poproot,gid)==NULL){//ama exei mono to population_root
		searchSpecies(Species_root,sid)->population_root=NULL; 
	}
	if(print==1){
	printf("SPECIES\n<%d>\n",sid);
	printInorder(searchSpecies(Species_root,sid)->population_root);
	}
	//delete from continent bst.
	deleteContPopulation(continents[pop2delete->cid]->population_root,gid);
	if(print==1){	
	printf("\nCONTINENTS\nContinent <%d>: ",pop2delete->cid);
	printContinentInorder(continents[pop2delete->cid]->population_root);
	printf("\n");
	}
	return 1;
}

//function to delete all the populations of a Specie.
void deleteSpeciePopulations(struct Population *proot){
	if(proot==NULL){
		return;
	}
	deleteSpeciePopulations(proot->lc);
	deleteSpeciePopulations(proot->rc);
	//visit
	print=0;
	delete_population(proot->gid , proot->cid);
	print=1;	
}

//function to find the oldest specie(smallest sid) in species tree.

struct Species *FindOldestSpecie(struct Species *root){
	if(root->lc==NULL){	
		return root;
	}
	FindOldestSpecie(root->lc);
}

//function to find the parent of a specie.
struct Specie *prnt=NULL;
struct Species *FindParent(struct Species *root,int sid){
	if(root->sid==sid){
		return prnt;
	}
	prnt=root;	
	return FindParent(root->lc,sid);
}

/**
 * @brief delete the species with lowest id and its populations
 *
 * @return 1 on success
 *         0 on failure
 */
int delete_species(){
	if(Species_root==NULL){
		printf("Error,specie tree not found");
		return 0;
	}

	struct Species *OldestSpecie=FindOldestSpecie(Species_root);
	deleteSpeciePopulations(OldestSpecie->population_root);
	struct Species *Parent=FindParent(Species_root,OldestSpecie->sid);
		
//	printf("parent---<%d>\n",Parent->sid);
//	printf("child----<%d>\n",OldestSpecie->sid);
	if(Parent->lc==NULL){
		Parent->lc=Parent->rc;
		Parent->rc=NULL;
	//	free(FindOldestSpecie(Species_root));
	}else{
		Parent->lc=Parent->rc;
		Parent->rc=NULL;
//		free(FindOldestSpecie(Species_root));
	}
				
	
	printf("SPECIES\n");
	print_populations();
	printf("\nCONTINENTS\n");
	print_continents();		
	
	return 1;
}
//function to return the most left node in HomoSapiens Tree.
struct HomoSapiens *returnMostLeft(struct HomoSapiens *root){
	if(root->lc==NULL){
		return root;
	}
//	printf("MostLeft");
	returnMostLeft(root->lc);	
}


//function to return the most right node in HomoSapiens Tree.
struct HomoSapiens *returnMostRight(struct HomoSapiens *root, int sid){
	if(root->lc->sid<sid){
		return root->rc;
	}
//	printf("root->rc---<%d>",root->rc->sid);
	returnMostRight(root->lc, sid);	
}

//function to insert to HomoSapiens Tree

void insertHomoSapiens(struct HomoSapiens *root){
	if(root==NULL){
		return;
	}
	if(Homo_Sapiens_root==NULL){
		Homo_Sapiens_root=root;
		return;
	}
		
	if(Homo_Sapiens_root->lc==NULL){
		struct HomoSapiens *copy=Homo_Sapiens_root;
		Homo_Sapiens_root->lc=root;
		Homo_Sapiens_root->rc=copy;
		Homo_Sapiens_root->lc->next=Homo_Sapiens_root->rc;
		root=Homo_Sapiens_root;
//		printf("HomoSapiens----<%d>\n",root->sid);
//		printf("HomoSapiens->lc----<%d>\n",root->lc->sid);
//		printf("HomoSapiens->rc----<%d>\n\n",root->rc->sid);
		return;
	}
//	printf("root-><%d>\n",root->sid);
//	printf("HomoSapiens-><%d>\n",Homo_Sapiens_root->lc->sid);
	struct HomoSapiens *MostLeftLeaf=returnMostLeft(Homo_Sapiens_root->lc);
//	printf("Most left leaf-----<%d>\n",MostLeftLeaf->sid);
	struct HomoSapiens *copy;
	if(MostLeftLeaf->sid>root->sid){
	//	printf("check 1\n");
		copy=MostLeftLeaf;
		MostLeftLeaf->lc=root;
		MostLeftLeaf->rc=copy;
		MostLeftLeaf->lc->next=MostLeftLeaf->rc;
		MostLeftLeaf->rc->next=MostLeftLeaf->next;
		MostLeftLeaf->next=NULL;
	//	printf("MostLeftLeaf->rc----<%d>\n",MostLeftLeaf->rc->sid);
		return;
		
	}else if(MostLeftLeaf->sid<root->sid){
		
		 if(root->sid>Homo_Sapiens_root->lc->sid){//when we add the Species_root->rc subtree.
		//	printf("check 2\n");
		//	printf("CHECKK-----<%d>",Homo_Sapiens_root->rc->lc->sid);
		
			MostLeftLeaf=returnMostLeft(Homo_Sapiens_root->rc);
			copy=MostLeftLeaf;
			MostLeftLeaf->lc=root;
			MostLeftLeaf->rc=copy;
			MostLeftLeaf->lc->next=MostLeftLeaf->rc;
			MostLeftLeaf->rc->next=MostLeftLeaf->next;
			MostLeftLeaf->next=NULL;
			return;
		
		}else{
		//	printf("check 3\n");
			struct HomoSapiens *MostRightLeaf=returnMostRight(Homo_Sapiens_root->lc,root->sid);
		//	printf("Most right leaf--><%d>\n",MostRightLeaf->sid);
			copy=MostRightLeaf;
			MostRightLeaf->lc=root;
			MostRightLeaf->rc=copy;
			MostRightLeaf->lc->next=MostRightLeaf->rc;
			MostRightLeaf->rc->next=MostRightLeaf->next;
			MostRightLeaf->next=NULL;
		}
	} 
		
	

	
	return;
}


//function to traverse Species tree in preorder.
struct Species *speciesPreorder(struct Species *root){
	if(root==NULL){
		return root;
	}
	//visit
	struct HomoSapiens *hs=(struct HomoSapiens*)malloc(sizeof(struct HomoSapiens));
	hs->lc=hs->rc=hs->next=NULL;
	hs->population_root=root->population_root;
	hs->sid=root->sid;
	insertHomoSapiens(hs);
	
	speciesPreorder(root->lc);
	speciesPreorder(root->rc);
	
}
void printHomo(struct HomoSapiens *root){
	if(root==NULL){
		return;
	}
	
	printHomo(root->lc);
	printf("<%d>\n",root->sid);
	printHomo(root->rc);
}

/**
 * @brief Remaining species evolve into homo sapiens.
 *
 * @return 1 on success
 *         0 on failure
 */
int evolution(){
	if(Species_root==NULL){
		printf("Error,Species tree not found");
		return 0;
	}	
	speciesPreorder(Species_root);
	
	print_homo_sapiens();
//	printHomo(Homo_Sapiens_root);
	
	return 1;
}

/**
 * @brief Gather statistics on species from homo_sapiens tree
 *
 * @return 1 on success
 *         0 on failure
 */
int species_statistics(){
	struct HomoSapiens *homo=returnMostLeft(Homo_Sapiens_root);
	int num=1;
	printf("HomoSapiens:");
	while(homo!=NULL){
		num=num+1;
		printf("<%d>",homo->sid);
		homo=homo->next;	
	}
	printf("\n");
	printf("Homo Sapiens Species :<%d>\n",num-1);
	
	
	return 1;
}

/**
 * @brief Gather statistics on population belonging to species with ID <sid> from homo_sapiens tree
 *
 * @return 1 on success
 *         0 on failure
 */
int population_statistics(int sid){
	struct HomoSapiens *homo=returnMostLeft(Homo_Sapiens_root);
	
	while(homo!=NULL &&homo->sid!=sid){
		homo=homo->next;
	}
	printf( "Homo Sapiens populations:");
	if(homo!=NULL && homo->population_root!=NULL){
	printf( " %d",countNodes(homo->population_root));
	}
	
	printf("\n");
	return 1;
}
//function to print a bst in postorder.
void printPostorder(struct Species *root){
	if(root==NULL){
		return;
	}
	printPostorder(root->lc);
	printPostorder(root->rc);
	
	printf("<%d>",root->sid);
}

/**
 * @brief Print species' leaves list
 *
 * @return 1 on success
 *         0 on failure
 */
int print_species(){
	if(Species_root==NULL){
		printf("Error, tree not found");
		return 0;
	}
	printPostorder(Species_root);
	return 1;
	
}



//recursive function to print all the populations.
void printPopulations(struct Species *sroot){
	if(sroot==NULL){
		return;
	}
	printPopulations(sroot->lc);
	printPopulations(sroot->rc);
	
	printf("<%d>\n",sroot->sid);
	if(sroot->population_root !=NULL){
		printInorder(sroot->population_root);
		
	}else{
		printf("This specie does not have any populations");
	}
	printf("\n");
}

/**
 * @brief Print species' tree with population trees
 *
 * @return 1 on success
 *         0 on failure
 */
int print_populations(){
	struct Species *sroot=Species_root;
	if(sroot==NULL){
		printf("Error,no species found");
		return 0;
	}
	printPopulations(sroot);
	
	return 1;
}

//function to print a continent population inorder.
void printContinentsInorder(struct ContinentPopulation *root){
	if(root==NULL){
		return;
	}
	printContinentsInorder(root->lc);
	printf("<%d>,",root->gid);
	printContinentsInorder(root->rc);
}


/**
 * @brief Print continents array with each continent's population tree
 *
 * @return 1 on success
 *         0 on failure
 */
int print_continents(){
	int i=0;
	for(i=0;i<5;i++){
		struct ContinentPopulation *tmp=continents[i]->population_root;
		printf("\n");
		printf("Continent %d: ",i);
		printContinentsInorder(tmp);
	}
	printf("\n");
	return 1;
}
//print homo sapiens specie population.
void printHomoSapiensPopulation(struct Population *root,int sid){
	if(root==NULL){
		return;
	}
	printHomoSapiensPopulation(root->lc,sid);
	//visit
	printf("<%d,%d>",root->gid,sid);
	
	printHomoSapiensPopulation(root->rc,sid);
	
}

/**
 * @brief Print homo_sapiens tree
 *
 * @return 1 on success
 *         0 on failure
 */
int print_homo_sapiens(){
	if(Homo_Sapiens_root==NULL){
		printf("Error,no HomoSapiens tree");
		return 0;
	}
	struct HomoSapiens *homo=returnMostLeft(Homo_Sapiens_root);
	printf("Homo Sapiens:");
	
	while(homo->next!=NULL){
		printf("|");
		printHomoSapiensPopulation(homo->population_root,homo->sid);
		printf("|");
		homo=homo->next;
	}

	
	return 1;
}

/**
 * @brief Free resources
 *
 * @return 1 on success
 *         0 on failure
 */
int free_all(void)
{
	return 1;
}




/**
 * @brief The main function
 *
 * @param argc Number of arguments
 * @param argv Argument vector
 *
 * @return 0 on success
 *         1 on failure
 */
int main(int argc, char** argv)
{
	FILE *fin = NULL;
	char buff[BUFFER_SIZE], event;

	/* Check command buff arguments */
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <input_file> \n", argv[0]);
		return EXIT_FAILURE;
	}

	/* Open input file */
	if (( fin = fopen(argv[1], "r") ) == NULL ) {
		fprintf(stderr, "\n Could not open file: %s\n", argv[1]);
		perror("Opening test file\n");
		return EXIT_FAILURE;
	}

	/* Initializations */
	initialize();

	/* Read input file buff-by-buff and handle the events */
	while (fgets(buff, BUFFER_SIZE, fin)) {

		DPRINT("\n>>> Event: %s", buff);

		switch(buff[0]) {

			/* Comment */
			case '#':
				break;

				/* Insert Species
				 * S <sid> */
			case 'S':
				{
					int sid;

					sscanf(buff, "%c %d", &event, &sid);
					DPRINT("%c %d\n", event, sid);

					if (insert_species(sid)) {
						DPRINT("%c %d succeeded\n", event, sid);
					} else {
						fprintf(stderr, "%c %d failed\n", event, sid);
					}

					break;
				}

				/* Insert population
				 * G <gid> <sid> <cid> */
			case 'G':
				{
					int gid, sid, cid;

					sscanf(buff, "%c %d %d %d", &event, &gid, &sid, &cid);
					DPRINT("%c %d %d %d\n", event, gid, sid, cid);

					if (insert_population(gid, sid, cid)) {
						DPRINT("%c %d %d %d succeeded\n", event, gid, sid, cid);
					} else {
						fprintf(stderr, "%c %d %d %d failed\n", event, gid, sid, cid);
					}

					break;
				}

				/* Lowest Common Ancestor
				 * L <sid> <gid1> <gid2> */
			 case 'L':
				{
					int sid, gid1, gid2;

					sscanf(buff, "%c %d %d %d", &event, &sid, &gid1, &gid2);
					DPRINT("%c %d %d %d\n", event, sid, gid1, gid2);

					if (lowest_common_ancestor(sid, gid1, gid2)) {
						DPRINT("%c %d %d %d succeeded\n", event, sid, gid1, gid2);
					} else {
						fprintf(stderr, "%c %d %d %d failed\n", event, sid, gid1, gid2);
					}

					break;
				}

				/* Merge species
				 * M <sid1> <sid2> <sid3> */
			case 'M':
				{
					int sid1, sid2, sid3;

					sscanf(buff, "%c %d %d %d", &event, &sid1, &sid2, &sid3);
					DPRINT("%c %d %d %d\n", event, sid1, sid2, sid3);

					if (merge_species(sid1, sid2, sid3)) {
						DPRINT("%c %d %d %d succeeded\n", event, sid1, sid2, sid3);
					} else {
						fprintf(stderr, "%c %d %d %d failed\n", event, sid1, sid2, sid3);
					}

					break;
				}

				/* Distribute species
				 * D */
			case 'D':
				{
					sscanf(buff, "%c", &event);
					DPRINT("%c\n", event);

					if (distribute()) {
						DPRINT("%c succeeded\n", event);
					} else {
						fprintf(stderr, "%c failed\n", event);
					}

					break;
				}

				/* Delete population
				 * K <gid> <sid> */
			case 'K':
				{
					int gid, sid;

					sscanf(buff, "%c %d %d", &event, &gid, &sid);
					DPRINT("%c %d %d\n", event, gid, sid);

					if (delete_population(gid, sid)) {
						DPRINT("%c %d %d succeeded\n", event, gid, sid);
					} else {
						fprintf(stderr, "%c %d %d failed\n", event, gid, sid);
					}

					break;
				}

				/* Delete species
				 * F */
			case 'F':
				{
					DPRINT("%c \n", event);

					if (delete_species()) {
						DPRINT("%c succeeded\n", event);
					} else {
						fprintf(stderr, "%c failed\n", event);
					}

					break;
				}

				/* Evolution to homo sapiens
				 * E */
			case 'E':
				{
					sscanf(buff, "%c", &event);
					DPRINT("%c\n", event);

					if (evolution()) {
						DPRINT("%c succeeded\n", event);
					} else {
						fprintf(stderr, "%c failed\n", event);
					}

					break;
				}

				/* Species' statistics
				 * N */
			case 'N':
				{
					sscanf(buff, "%c", &event);
					DPRINT("%c\n", event);

					if (species_statistics()) {
						DPRINT("%c succeeded\n", event);
					} else {
						fprintf(stderr, "%c failed\n", event);
					}

					break;
				}

				/* Population statistics
				 * J <sid> */
			case 'J':
				{
					int sid;

					sscanf(buff, "%c %d", &event, &sid);
					DPRINT("%c %d\n", event, sid);

					if (population_statistics(sid)) {
						DPRINT("%c %d succeeded\n", event, sid);
					} else {
						fprintf(stderr, "%c %d failed\n", event, sid);
					}

					break;
				}

				/* Print species
				 * P */
			case 'P':
				{
					sscanf(buff, "%c", &event);
					DPRINT("%c\n", event);

					if (print_species()) {
						DPRINT("%c succeeded\n", event);
					} else {
						fprintf(stderr, "%c failed\n", event);
					}

					break;
				}

			/* Print populations
				 * X */
			case 'X':
				{
					sscanf(buff, "%c", &event);
					DPRINT("%c\n", event);

					if (print_populations()) {
						DPRINT("%c succeeded\n", event);
					} else {
						fprintf(stderr, "%c failed\n", event);
					}

					break;
				}

			/* Print continents
				 * C */
			case 'C':
				{
					sscanf(buff, "%c", &event);
					DPRINT("%c\n", event);

					if (print_continents()) {
						DPRINT("%c succeeded\n", event);
					} else {
						fprintf(stderr, "%c failed\n", event);
					}

					break;
				}

			/* Print homo sapiens
				 * W */
			case 'W':
				{
					sscanf(buff, "%c", &event);
					DPRINT("%c\n", event);

					if (print_homo_sapiens()) {
						DPRINT("%c succeeded\n", event);
					} else {
						fprintf(stderr, "%c failed\n", event);
					}

					break;
				}

				/* Empty line */
			case '\n':
				break;

				/* Ignore everything else */
			default:
				DPRINT("Ignoring buff: %s \n", buff);

				break;
		}
	}

	free_all();
	return (EXIT_SUCCESS);
}
