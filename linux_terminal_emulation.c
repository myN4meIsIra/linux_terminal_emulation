//Ira Garrett

// import statements/headers
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <dirent.h>
#include<malloc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

//defining what TRUE means
#define TRUE 1

//struct for 'storing' the tokenated string
struct Tokens {
	int num_tokens;
	char **tokens;
};

//buffer for cwd 
char cwd[255]; 
int n = 256;

//function called to determine the size of a file (for determining a proper buffer size)
int sizeOf(FILE *file){
	int length = 0;
	while(getc(file) != EOF){length++;}
	return length;
}


int isIn(char* ls){
	char* tokLS =  ls;
	char* options[] = {"0","l","a","h"};
	printf("size of options %ld\n",(sizeof(options)/sizeof(char *)));	
	for(int i = 0; i < (sizeof(options)/sizeof(char *)-1); i++){
		if(strchr(ls,'/')!= NULL){tokLS = strtok(ls,"/");}
		if(strchr(ls,'-')!= NULL){tokLS = strtok(ls,"-");}
		if(strcmp((char*)tokLS,(char*)options[i])==0){
			
			return i;
			
		}
	}
	
	printf("\"%s\" Not found in list\n",ls);	
	return -1;
}

void execute(struct Tokens tokens) {
	
	// Get command. Arguments are in tokens.tokens[1...]
	char* cmd = tokens.tokens[0];

  	//CD////////////////////////////////////////////////////////////
  	if (strcmp(cmd, "cd") == 0) {
		//change directory using term #2 as input
		chdir(tokens.tokens[1]);
		
		//if not term #2, go to home directory
		if(tokens.tokens[1]==NULL){chdir(getenv("HOME"));}
		printf("\n");	
  	} 
	
	//PWD///////////////////////////////////////////////////////////
	else if (strcmp(cmd, "pwd") == 0) {
    		//pwd is the current working directory
		char* pwd = getcwd(cwd,255);
		
		printf("%s\n",pwd);
  	}


	//CAT///////////////////////////////////////////////////////////
	else if (strcmp(cmd, "cat") == 0) {
    		//file location defined by user
		char* file = tokens.tokens[1];

		//find size of file (from function before this one)
		int length = sizeOf(fopen(file,"r"));
		
	
		//array of chars length terms long, each of size for char of the system
		char* c = (char *)calloc(length, sizeof(char));
		int fd,sz; 
		
		//open, read, and then close... then print
		fd = open(file,O_RDONLY);
		sz = read(fd, c, length);
		close(fd);
		printf("%s\n",c);
		
  	} 

	//ls///////////////////////////////////////////////////////////
	else if (strcmp(cmd, "ls") == 0) {
    		//get current working directory
		char* pwd = getcwd(cwd,255);
		
		DIR *dirstream = opendir(pwd);
		
		int a = 1;
		
		while(TRUE){
			if(tokens.tokens[a] == NULL){break;}
			printf("testing term: %s\n",tokens.tokens[a]);

			if((tokens.tokens[a] != NULL) && (isIn(tokens.tokens[a])==-1) ){
				printf("address changed");
				dirstream = opendir(tokens.tokens[a]);
			}
			
			a++; 
		}

		
		struct dirent *dir_entry = NULL;
		int iteration = 0;
		
		//if(tokens.tokens[1] == NULL){
			while((dir_entry = readdir(dirstream)) != NULL){
				printf("%s",dir_entry->d_name);
				int tabs = (strlen(dir_entry->d_name))/8;
				if(strlen(dir_entry->d_name) < 8){tabs = 0;}
				
				//print the directory enteries in as much of an "ordered" layout as possible
				for(int l = 0; l<(4-tabs); l++){printf("\t");}
				//compensate for going past the edge of the screen
				if(iteration++ == 4){printf("\n");iteration = 0;}			
			}
			printf("\n");
			closedir(dirstream);
		//}

		int b = 1;
		/*while(tokens.tokens[b] != NULL && b<a){
			if(tokens.tokens[b] != NULL && isIn(tokens.tokens[b])!=-1){
				
				if(tokens.tokens[b] == "-l"){printf("-l");}
				else if(tokens.tokens[b] == "-a"){printf("-a");}
				
				else{printf("error: command \"%s\" not found\n",tokens.tokens[b]);}
				
			}
			b++;
		}*/


  	}

	//exit///////////////////////////////////////////////////////////
	else if (strcmp(cmd, "exit") == 0) {
		//exit with user defined code if offered
    		if(tokens.tokens[1]==NULL){exit(1);}
		if(tokens.tokens[1]!=NULL){exit(atoi(tokens.tokens[1]));}
  	}


	//(unknown)///////////////////////////////////////////////////////////
	else {
    		printf("%s: command not found\n", tokens.tokens[0]);
  	}
  
  // All done, free heap memory
  free(tokens.tokens);

}


//mostly, the magic code 
void main() {
	char* home;
	
  // Shell loop; Press ctrl c to exit like a real coder
  while (TRUE) {

	//have terminal line show current directory
	home = getcwd(cwd,100);
	printf("%s$ ",home);

    struct Tokens tokens;
    // Get input
    char* line = NULL;
    size_t n = 0;
    getline(&line, &n, stdin);

    int token_count = 1;
    char* strptr = line;
    while (*strptr != '\0') {
	if (*strptr == ' ')
	    token_count++;
	if (*strptr == '\n')
	    *strptr = '\0';
	strptr++;
    }
    tokens.num_tokens = token_count;
    tokens.tokens = malloc(token_count * sizeof(char*));
    tokens.tokens[0] = strtok(line, " ");
    for (int i = 1; i < token_count; i++) {
	tokens.tokens[i] = strtok(NULL, " ");
    }
    // Handle the command 
    execute(tokens);
  }
}
