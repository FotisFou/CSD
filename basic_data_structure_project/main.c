
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


/**
 * @brief Optional function to initialize data structures that 
 *        need initialization
 *
 * @return 1 on success
 *         0 on failure
 */
int initialize (void)
{
	Species_head=NULL;	
	Species_tail=NULL;
	
	continents[0] = (struct ContinentList*)malloc(sizeof(struct ContinentList));
	continents[1] = (struct ContinentList*)malloc(sizeof(struct ContinentList));
	continents[2] = (struct ContinentList*)malloc(sizeof(struct ContinentList));
	continents[3] = (struct ContinentList*)malloc(sizeof(struct ContinentList));
	continents[4] = (struct ContinentList*)malloc(sizeof(struct ContinentList)); 
	
	continents[0]->populationList =NULL; 
	continents[1]->populationList =NULL; 
	continents[2]->populationList =NULL; 
	continents[3]->populationList =NULL; 
	continents[4]->populationList =NULL; 
	
	return 1;
}

/**
 * @brief insert new species in Species' list with ID <sid>
 *
 * @return 1 on success
 *         0 on failure
 */
 int insert_species (int sid)
{	
	struct Species *newSpecies = NULL;
	newSpecies = (struct Species *)malloc(sizeof(struct Species));
		
	if(newSpecies==NULL){
		printf("Error.Not enough memory");
		return 0;
	}
	
	newSpecies->sid =sid;
	newSpecies->Population_head=NULL;
	newSpecies->Population_tail=NULL;

	
	//if we are creating the first node
	if(Species_head==NULL ){
		
		newSpecies->next = NULL;
		newSpecies->prev = NULL;
		Species_head = newSpecies;
		Species_tail = newSpecies;
			
		printf("<%d>\n",newSpecies->sid);
		printf("This spiece does not have any population\n\n");
		return 1;		
	}else{
		
	    struct Species *current=Species_head;
		Species_tail->next=NULL;
		Species_head->prev=NULL;
		
		if(Species_head->sid > newSpecies->sid){
			newSpecies->prev=NULL;
			Species_head->prev = newSpecies;
			newSpecies->next=Species_head;
			Species_head=newSpecies;
			
			
		}else if(newSpecies->sid > Species_tail->sid){
				newSpecies->prev=Species_tail;
				Species_tail->next=newSpecies;
				Species_tail=newSpecies;
				
		}else{ 
			while(current->sid < newSpecies->sid) {
				current = current->next;
							
			}
			current->prev->next = newSpecies;
			newSpecies->prev = current->prev;
			current->prev = newSpecies;
			newSpecies->next=current;
				
		}
	
	}
	                                                                             
	printf("\n");	
	return 1;
}

/**
 * @brief insert new population with ID <gid> in Species' with ID <sid>
 *
 * @return 1 on success
 *         0 on failure
 */
int insert_population(int gid, int sid, int cid){
	
	struct Population *newPopulation;
	struct Species    *specie = Species_head;
	
	newPopulation = (struct Population*)malloc(sizeof(struct Population));
	
	if(newPopulation==NULL){
	printf("Error.Not enough memory");
	return 0;
	}
	
	newPopulation->gid=gid;
	newPopulation->cid=cid;
	newPopulation->sid=sid;
	newPopulation->next_species=NULL;
	
	
	//tranversing the dll to find the specie we want
	while(specie->sid != sid){
		specie=specie->next;
	}
	
	//if its the first element
	if(specie->Population_head==NULL){
		
		specie->Population_head = specie->Population_tail = newPopulation;
		newPopulation->next=NULL;
		
	}else{
		struct Population *current = specie->Population_head;
		
		if(specie->Population_head->gid > newPopulation->gid){
			newPopulation->next =current;
			specie->Population_head = newPopulation;
			
			
		}else if(specie->Population_tail->gid < newPopulation->gid ){
			current=specie->Population_tail;
			current->next = newPopulation;
			specie->Population_tail = newPopulation;
			
			
		}else{	
			while(current->next->gid < newPopulation->gid){
				current = current->next;
			}		
			newPopulation->next = current->next;
			current->next = newPopulation;		
		}	
	}

	print_populations();	
	return 1;
}

/**
 * @brief merge species with IDs <sid1> and <sid2> into new species with ID <sid3>
 *
 * @return 1 on success
 *         0 on failure
 */
int merge_species(int sid1, int sid2, int sid3){
	
	insert_species(sid3); //inserting the new specie to the dll.
	struct Species *tmp = Species_head , *specie1=NULL, *specie2=NULL, *specie3=NULL;
	print_species();
	while(tmp->sid != sid1){
		tmp = tmp->next;	
	}

	specie1=tmp;
	tmp=Species_head;
	
	while(tmp->sid != sid2){
		tmp = tmp->next;
	}
	
	specie2=tmp;
	tmp=Species_head;
	
	while(tmp->sid !=sid3){
		tmp=tmp->next;
	}
	specie3=tmp;
	
	// *********  MERGING THE TWO SORTED LISTS.  ************
	
	struct Population* current=NULL;
	
	
	if(specie1->Population_head==NULL && specie2->Population_head==NULL){
		specie3->Population_head= NULL;
	}else{
		
	if(specie1->Population_head==NULL){
		specie3->Population_head= specie2->Population_head;
	}
	
	if(specie2->Population_head==NULL){
		specie3->Population_head= specie1->Population_head;
	}
	
	
	while(specie1->Population_head !=NULL && specie2->Population_head !=NULL){
		
		if(specie1->Population_head->gid < specie2->Population_head->gid){
			current = specie1->Population_head;
			specie1->Population_head= specie1->Population_head->next;
			
		}else{
			current = specie2->Population_head;
			specie2->Population_head= specie2->Population_head->next;
		}	
		
		if(specie3->Population_head==NULL){
			specie3->Population_head = (struct Population*)malloc(sizeof(struct Population));
			specie3->Population_tail = (struct Population*)malloc(sizeof(struct Population));
				
			specie3->Population_head = specie3->Population_tail = current;
			
		}else{
			struct Population *tmp;
			tmp = (struct Population*)malloc(sizeof(struct Population));
			current->next =tmp;
			tmp = current;
		}
		
		 
	}
		

	if(specie1->Population_head==NULL){
		current->next = specie2->Population_head;
	}else{
		current->next = specie1->Population_head;
	}
	}
	
//  ****************** END OF MERGING  ********************* 	
	
//	SpecieToDelete(sid1);   /*KANE COMMENT TA DELETE GIA TO SEGFAULT*/
// 	SpecieToDelete(sid2);
	print_populations();
	print_continents();
	printf("\n");
	 
	return 1;
}

/**
 * @brief Distribute species' population in continents array
 *
 * @return 1 on success
 *         0 on failure
 */
int distribute(){
	struct Species* specie = Species_head;
	struct Population * speciePopulation= NULL;
	
	
	if(specie == NULL){
		printf("Error.No species found");
		return 0;
	}
	
	while(specie!=NULL){
		
		speciePopulation = specie->Population_head;
		struct ContinentPopulation *tmp0,*tmp1,*tmp2,*tmp3,*tmp4;
		
		while(speciePopulation !=NULL){
		
			struct ContinentPopulation* newPopulation;
			newPopulation = (struct ContinentPopulation*)malloc(sizeof(struct ContinentPopulation));
			
			newPopulation->gid = speciePopulation->gid;
			newPopulation->next= NULL;
		
			if(newPopulation==NULL){
			printf("Error.Not enough memory.");
			return 0;
			}
			
			if(speciePopulation->cid == 0){
					
				if(continents[0]->populationList == NULL){  //the first element of the population List.
					continents[0]->populationList = newPopulation;
					tmp0 = continents[0]->populationList; 					
				}else{
					tmp0->next = newPopulation;
					tmp0 = tmp0->next;
				}
			}	
			if(speciePopulation->cid == 1){
				
				if(continents[1]->populationList == NULL){  //the first element of the population List.
					continents[1]->populationList = newPopulation;
					tmp1 = continents[1]->populationList; 	
				}else{
					tmp1->next = newPopulation;
					tmp1 = tmp1->next;
				}	
			}
			if(speciePopulation->cid == 2){
					
				if(continents[2]->populationList == NULL){  //the first element of the population List.
					continents[2]->populationList = newPopulation;
					tmp2 = continents[2]->populationList; 
										
				}else{
					tmp2->next = newPopulation;
					tmp2 = tmp2->next;
				}	
			}
			
			if(speciePopulation->cid == 3){
	
				if(continents[3]->populationList == NULL){  //the first element of the population List.
					continents[3]->populationList = newPopulation;
					tmp3 = continents[3]->populationList; 	
				}else{
					tmp3->next = newPopulation;
					tmp3 = tmp3->next;
				}
			}
			
			if(speciePopulation->cid == 4){

				if(continents[4]->populationList == NULL){  //the first element of the population List.
					continents[4]->populationList = newPopulation;
					tmp4 = continents[4]->populationList; 		
				}else{
					tmp4->next = newPopulation;
					tmp4 = tmp4->next;
				}	
			}
		speciePopulation = speciePopulation->next;	
		}
		
		specie=specie->next;	
	}

		print_continents();
		printf("\n");
	return 1;
}

/**
 * @brief delete population with ID <gid> from species with ID <sid>
 *
 * @return 1 on success
 *         0 on failure
 */
int delete_population(int gid, int sid){
	
	struct Species* specie = Species_head;
	struct Population* population;
	struct Population* tmp = NULL;
	
	while(specie->sid !=sid){
		specie= specie->next;
	}
	
	if(specie== NULL){
		printf("Error. no specie with sid-> <%d> found" , sid);
		return 0;
	}
	
	population = specie->Population_head;
	
	while(population->gid != gid ){
		tmp = population;
		population = population->next;
	} 
	
	if(population == NULL){
		printf("Error. No population with gid-> <%d> found", gid);
		return 0;
	}else{
		if(population == specie->Population_head){
			specie->Population_head = specie->Population_head->next;
		}else{
			if(population->next !=NULL){
				tmp->next=population->next;	
			}else{
				specie->Population_tail =tmp;
			}
			
		}
	}

	struct ContinentPopulation* continentPop = continents[population->cid]->populationList;
	struct ContinentPopulation* tmp1;
	
	if(continents[population->cid]->populationList!=NULL){
		
		while(continentPop->gid != gid){
			tmp1 = continentPop;
			continentPop=continentPop->next;
		}
		
		if(continentPop->next !=NULL){
			tmp1->next=continentPop->next;
		}else{
			tmp1->next=NULL;
			free(continentPop);
		}
	}
	
	// ************** PRINTS ***************
	printf("\nSPECIES\n");
	printf("<%d>\n", specie->sid);
	
	struct Population* counter = specie->Population_head;
	while(counter !=NULL){
		printf("<%d>,", counter->gid);
		counter=counter->next;
	}
	
	printf("\n\nCONTINENTS\n");
	printf("Continent <%d>: ",population->cid);
	
	struct ContinentPopulation* Counter = continents[population->cid]->populationList;
	while(Counter !=NULL){
		printf("<%d>,", Counter->gid);
		Counter = Counter->next;
	}
	
	//******* END OF PRINTS ****************
	
	printf("\n");
	return 1;
}



/**
 * @brief delete species with ID <sid> and its populations
 *
 * @return 1 on success
 *         0 on failure
 */
int delete_species(int sid){

	SpecieToDelete(sid);
	
	/*********** PRINTS *****************/
	printf("\nSPECIES\n");
	print_populations();
	
	printf("\nCONTINENTS\n");
	print_continents();
	printf("\n");
	
	
	return 1;
}

int SpecieToDelete(int sid){
	struct Species *specie =Species_tail;
				
	while(specie->sid != sid){
		specie = specie->prev;		
	}
		
	if(specie == NULL){
		printf("Error. no specie with sid-> <%d> found" , sid);
		return 0;
	}
	
	if(specie->Population_head !=NULL){
		
		struct Population *population = specie->Population_head;
		struct ContinentPopulation *continent=NULL;
		struct ContinentPopulation *current =NULL;
		struct Population *next=NULL;
		
		while(population != NULL){
			
			if(continents[population->cid]->populationList !=NULL){
			
			continent = continents[population->cid]->populationList;
			
			if(continent->gid == population->gid){
				continent=continent->next;
				
			}else{
				while(continent->gid != population->gid && continent !=NULL){
					current=continent;
					continent=continent->next;
				}
				current->next=continent->next;
				free(continent);				
			}
			}
			
			next = population->next;
			free(population); 
			population=next;
		}
	}
		
	if(specie == Species_head){
		Species_head = Species_head->next;
	
	}else{
	
		if( specie->next !=NULL){
			specie->prev->next = specie->next;
			specie->next->prev = specie->prev;
		}else{
			Species_tail = specie->prev;
			specie->prev->next = NULL;
		}

	}
	
	
	free(specie); 
	return 1;	
		
}

/**
 * @brief Remaining species evolve into homo sapiens.
 *
 * @return 1 on success
 *         0 on failure
 */
int evolution(){
	struct Species* specie = Species_head;
	struct Population* tmp, *tmp2;
	
	/*initializing next_species pointer */
	while(specie !=NULL){	
		if(specie->next != NULL){
			if(specie->Population_head !=NULL && specie->next->Population_head !=NULL){
				specie->Population_head->next_species = specie->next->Population_head;
			}
		}else{
			specie->Population_head->next_species =NULL;
			break;
		}	
	//	printf("next specie -- <%d>\n", specie->Population_head->next_species->sid);	
		specie=specie->next;
	}
	
	specie =Species_head;
	
	while(specie->Population_head->next_species !=NULL){
		
	if(hs_L==NULL){
			hs_L=specie->Population_head;
			specie->Population_tail->next = specie->Population_head->next_species;
			tmp=specie->next->Population_tail;
		}else{
			tmp->next= specie->Population_head;
		}
	//	specie->Population_tail->next=specie->Population_head->next_species;
		specie=specie->next;
	}
	
	
   // print_homo_sapiens();
	
	return 1;
}

/**
 * @brief Gather statistics on species from homo_sapiens list
 *
 * @return 1 on success
 *         0 on failure
 */
int species_statistics(){
	struct Population *counter=hs_L->next_species;
	int num=0;
	
	if(hs_L==NULL){
		return 0;
	}
	
	while(counter!=NULL){
		num++;
		counter=counter->next;
	}
	
	printf("Homo Sapiens species: <%d>\n",num);
	
	return 1;
}

/**
 * @brief Gather statistics on population belonging to species with ID <sid> from homo_sapiens list
 *
 * @return 1 on success
 *         0 on failure
 */
int population_statistics(int sid){
	return 1;
}

/**
 * @brief Print species' list
 *
 * @return 1 on success
 *         0 on failure
 */
int print_species(){
		
		struct Species *count = Species_head;
		 
		if(count ==NULL){
			printf("Error.No species found");
			return 0;
		}
	
		while(count != NULL){
			printf("<%d>,", count->sid);
			count=count->next;
		}
	   printf("\n");	
	   return 1;
}

/**
 * @brief Print species' list with population lists
 *
 * @return 1 on success
 *         0 on failure
 */
int print_populations(){
	
	struct Species *specie = Species_head;
	struct Population *count = specie->Population_head;

	if(specie == NULL){
		printf("Error.No species found");
		return 0;
	}
	
	while(specie !=NULL){
	
	printf("<%d>\n",specie->sid);
	
	if(count == NULL){
		printf("This specie does not have any population\n");			
	}else{
		while(count !=NULL){
			printf("<%d,%d>,", count->gid, count->cid);
			count=count->next;
		}
	}
		
	specie= specie->next;	
	
	if(specie !=NULL){
	count=specie->Population_head;
	}
	
	printf("\n\n");
	
    }
    return 1;
 
}

/**
 * @brief Print continents array with each continent's population list
 *
 * @return 1 on success
 *         0 on failure
 */
int print_continents(){
	int i=0;
	
	for(i=0;i<5;i++){
		
		struct ContinentPopulation* tmp = continents[i]->populationList;
		printf("\n");
		printf("Continent %d: ", i);
		while(tmp !=NULL){
			printf("<%d>,", tmp->gid);
			tmp=tmp->next;
		}
		
	}
	return 1;
}

/**
 * @brief Print homo_sapiens list
 *
 * @return 1 on success
 *         0 on failure
 */
int print_homo_sapiens(){
	
	if(hs_L==NULL){
		return 0;
	}
	
	printf("Homo Sapiens: ");
	
	struct Population *current = hs_L;
	int sid = current->sid;
	
		while(current !=NULL){
			printf("[");
			while(current->sid == sid){
				printf("<%d,%d>,",current->gid,current->sid);
				current=current->next;
			}
			sid=current->sid;
		printf("]");
			
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
			//		print_species();
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
				 * F <sid> */
			case 'F':
				{
					int sid;

					sscanf(buff, "%c %d", &event, &sid);
					DPRINT("%c %d\n", event, sid);

					if (delete_species(sid)) {
						DPRINT("%c %d succeeded\n", event, sid);
					} else {
						fprintf(stderr, "%c %d failed\n", event, sid);
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
