/*
*   CS345- EX01
*   FOTIS FOUSKAS
*   4066
*   C SHELL IMPLEMENTATION
*
*/






#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pwd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <dirent.h>

#define CMD_SIZE 1024
#define TOK_DELIM " \t\r\n\a"
#define TRUE 1



void display_shell();
char *read_input(int display_switch); 
void shell();
void execute(char *command);
void execute_pipe(char **commands);
int check_for_redirection(char *input);
void redirect(char **commands);
void redirect_back(char **commands);
void redirect_double(char **commands);
char *pwd();
char **tokenize_input(char *input, char *delim);
char *cmd, *path, **cmd_tokenized, **commands;

int main(){

    shell();

}

void shell(){
    
    while(TRUE){
        char *input= (char *)malloc(CMD_SIZE*sizeof(char));
        cmd_tokenized= (char **)malloc(CMD_SIZE*sizeof(char));
        commands= (char **)malloc(CMD_SIZE*sizeof(char));
        int index= 0, redirection= 0;

        input = read_input(1);
        redirection= check_for_redirection(input);

        if(strchr(input, '|')!=NULL){ /*pipes*/
            
            commands= tokenize_input(input, "|");                           
            execute_pipe(commands); 

        }else if(redirection!=0){

            if(redirection==1){
               
                commands= tokenize_input(input, ">");
                redirect(commands);
            }
            if(redirection==2){
              
                commands= tokenize_input(input, "<");
                redirect_back(commands);
            }
            if(redirection==3){
               
                commands= tokenize_input(input, ">>");
                redirect_double(commands);
            }
            


        }else{

            commands= tokenize_input(input, ";");
            while(commands[index]!=NULL){
                execute(commands[index]);
                index++;
            }
               
        }
        index=0;
        free(commands);
        free(input);
    }
}

void execute(char *command){

    cmd_tokenized= (char **)malloc(CMD_SIZE*sizeof(char));
    int index=0;

    cmd_tokenized = tokenize_input(command, TOK_DELIM);
        
        pid_t pid= fork();

        if(pid>0){
            wait(NULL);
           
            
        }else if (pid<0)
        {
            fprintf(stderr, "Error forking\n");
            exit(EXIT_FAILURE);
        }else{
            
            if (strcmp(cmd_tokenized[index], "exit")==0)
            {
                kill(pid, SIGKILL);
            }
                
            if(strcmp(cmd_tokenized[index], "cd")==0)
            {
                if(cmd_tokenized[index++]!=NULL)
                {
                    path = cmd_tokenized[index];
                    chdir(path);
                    index--;            
                }
            }
            execvp(cmd_tokenized[0], cmd_tokenized);

        }
}



void execute_pipe(char **commands){
    int pipes=0, index=0,i=0, j=0;
    pid_t pid, pid2;
    char **argv;

    while(commands[index]!=NULL) index++; /*posa pipes exoume*/
    pipes= index-1;
    index=0;

    int fd[2*pipes];

    for(i=0; i< 2*pipes; i++){
        if(pipe(fd + i*2)<0){
            fprintf(stderr, "Error piping\n");
            exit(EXIT_FAILURE);
        }
    }
    i=0;

    while(commands[i]!=NULL){ 
		
		pid=fork();
		if(pid==0){
		
			if(i!=0){   /*not first*/
				dup2(fd[(i-1)*2],0);
			}
			
			if(i!=pipes){       /*not last because we need og fd*/
				dup2(fd[(i*2)+1],1);
			}	

			for(j=0;j<4*pipes;j++){
				close(fd[j]);
			}
            argv= tokenize_input(commands[i], TOK_DELIM);

			if(execvp(argv[0],argv)==-1){
				fprintf(stderr, "Error executing\n");
				exit(EXIT_FAILURE);
			}
		}
		else if (pid<0){
			fprintf(stderr, "Error forking\n");
			exit(EXIT_FAILURE);
		}
		i++;		
	}

    for(i=0;i<2*pipes;i++){
		close(fd[i]);
	}

	for(i=0;i<pipes+1;i++){
		wait(NULL);
	}
	
	return;

}

int check_for_redirection(char *input){

    char *str;
    if((str=strstr(input, ">\0"))!=NULL){
       
        if(strstr(str, ">")==NULL) return 1;
        return 3;
        
    } 
    if(strchr(input, '<')!=NULL) return 2;
 

    return 0;
}

void redirect(char **commands){
    pid_t pid;
    char  **argv;
    int fd;
    int index=0;

    if(commands[1]== NULL){
        fprintf(stderr, "Error redirecting\n");
        return;
    }
    pid= fork();

    if(pid==-1){
        fprintf(stderr, "Error forking\n");
        exit(EXIT_FAILURE);
    }else if(pid==0){

        if(commands[1]==NULL){
            fprintf(stderr, "Error redirecting\n");
            return;
        }

        if((fd= open(commands[1], O_CREAT | O_WRONLY | O_TRUNC ,1000))<0){ /*O_CREAT-If pathname does not exist, create it as a regular file*/
        fprintf(stderr, "Error redirecting\n");                            /*O_TRUNC-If file already exists, it will be truncated to len 0*/
        return ;                                                           /*O_WRONLY-Open for writing only*/
        }

        dup2(fd, 1);
        close(fd);

       
        // while(commands[index]!=NULL){
        //     printf("commands[index]----%s\n", commands[index]);
        //     index++;
        // }
        argv= tokenize_input(commands[0], TOK_DELIM);
    
        if(execvp(argv[0], argv)==-1){
            fprintf(stderr, "Error executing\n");
            exit(EXIT_FAILURE);
        }

    }else{
        wait(NULL);
    }
    return;
}



void redirect_back(char **commands){
    pid_t pid;
    char **argv, *file;
    int fd, i=0, j=0;
    

    if(commands[1]== NULL){
        fprintf(stderr, "Error redirecting\n");
        return;
    }
    pid= fork();

    if(pid==-1){
        fprintf(stderr, "Error forking\n");
        exit(EXIT_FAILURE);
    }else if(pid==0){

        if(commands[1]!=NULL){
          /*  printf("commands[1]----%s\n", commands[1]); */
        }

        stpcpy(file, commands[1]);

       while (file[i]){
        
        if (file[i]!= ' '){
            file[j] = file[i];
            j++;
        }  
		i++;
	    }
	    file[j] = '\0';
        printf("file ---%s\n", file);
        printf("cmd1 ---%s\n", commands[1]);
        
        if((fd= open(file, O_RDONLY, 0))<0){ 
        fprintf(stderr, "Error redirecting\n");                            
        return;                                                           
        }

        dup2(fd, STDIN_FILENO);
        close(fd);

       
        argv= tokenize_input(commands[0], TOK_DELIM);
    
        if(execvp(argv[0], argv)==-1){
            fprintf(stderr, "Error executing\n");
            exit(EXIT_FAILURE);
        }

    }else{
        wait(NULL);
    }
    return;
}

void redirect_double(char **commands){
    pid_t pid;
    char  **argv;
    int fd;
    int index=0;

    if(commands[1]== NULL){
        fprintf(stderr, "Error redirecting\n");
        return;
    }
    pid= fork();

    if(pid==-1){
        fprintf(stderr, "Error forking\n");
        exit(EXIT_FAILURE);
    }else if(pid==0){

        if(commands[1]==NULL){
            fprintf(stderr, "Error redirecting\n");
            return;
        }

        if((fd= open(commands[1], O_APPEND | O_WRONLY, 0644))<0){ /*O_CREAT-If pathname does not exist, create it as a regular file*/
        fprintf(stderr, "Error redirecting\n");                            /*O_TRUNC-If file already exists, it will be truncated to len 0*/
        return ;                                                           /*O_WRONLY-Open for writing only*/
        }

        dup2(fd, 1);
        close(fd);

       
        // while(commands[index]!=NULL){
        //     printf("commands[index]----%s\n", commands[index]);
        //     index++;
        // }
        argv= tokenize_input(commands[0], TOK_DELIM);
    
        if(execvp(argv[0], argv)==-1){
            fprintf(stderr, "Error executing\n");
            exit(EXIT_FAILURE);
        }

    }else{
        wait(NULL);
    }
    return;
}

char **tokenize_input(char *input, char *delim){
    char **tokens= (char **)malloc(CMD_SIZE*sizeof(char));
    char **commands= (char **)malloc(CMD_SIZE*sizeof(char));
    char *buf= (char *)malloc(CMD_SIZE*sizeof(char));
    char *current_token, *current_cmd;
    int i=0;

    buf= strdup(input);
    if(strchr(buf, ';')!=NULL || strchr(buf, '|')!=NULL){                /*einai sequence apo cmds*/
        
       

        current_cmd= strtok(input, delim);       /*xwrizw ta commands me delim - ;*/
        while(current_cmd !=NULL){             /*kai meta to kathe cmd xwrista*/
            commands[i]= current_cmd;
            current_cmd= strtok(NULL, delim);
            i++;
        }
      
        return commands;
        

    }else{

        current_token= strtok(input, delim);
        while(current_token !=NULL){

            tokens[i]= current_token;
            i++;
            current_token= strtok(NULL, delim);
        
        }

        return tokens;
    }
    
}

char *read_input(int display_switch){

    char *input, *buf;
  
    buf= (char *)malloc(CMD_SIZE*sizeof(char));
    input= (char *)malloc(CMD_SIZE*sizeof(char));
    cmd= (char *)malloc(CMD_SIZE*sizeof(char));
    path= (char *)malloc(CMD_SIZE*sizeof(char));
         
    if(display_switch==1) display_shell();

    fgets(input, CMD_SIZE*sizeof(char), stdin);

    return input;
}

char* pwd(){
    long size = pathconf(".", _PC_PATH_MAX);
    char *buf, *dir;

    if ((buf = (char *)malloc((size_t)size)) != NULL)
    dir = getcwd(buf, (size_t)size);
    
    return dir;
}

void display_shell(){
    char *user= getlogin();
    char *dir= pwd();

    printf("%s@cs345sh%s$ ", user, dir);
}
