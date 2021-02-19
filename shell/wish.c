#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <assert.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/wait.h> 
#define VECTOR_SIZE 100

int pathVectorSize= VECTOR_SIZE;
int tokenVectorSize= VECTOR_SIZE;
char **pathVector;
char **tokenVector;
char curDir[]="~/";
int currentPathIndex =0;
int currentTokIndex =0;

int error(){
	char error_message[30] = "An error has occurred\n";
	write(STDERR_FILENO, error_message, strlen(error_message));
	return 0;
}

int clearVector(char** vector, int *index){
	// for(int i =0;i<(*index);++i){
		// free(vector[i]);
		
	// }
	*index =0;
	return 0;
}

int resizedVector(char ** vector,int *size){
	char** newVec = (char**)malloc(sizeof(char*)*2*(*size));
	
	
	if (newVec ==NULL ){
		return -1;
	}
	memcpy(newVec, vector, (*size)*sizeof(char**));
	
	*size=2*(*size);
	char** swap= vector;
	vector =newVec;
	free(swap);
	
	return 0;
}


char* checkAccess(char* command){
	int ret = access(command, X_OK);
	printf("check %s %d\n",command,ret);
	if (ret==0) {return command;}
	for(int i=0;i<currentPathIndex;++i){
		int size =  strlen(command)+strlen(pathVector[i])+1;
		char* fullpath = (char*)malloc(sizeof(char)*size);
		//char slash ='/';
		printf("check path %s\n",pathVector[i] );
		memcpy(fullpath,pathVector[i] , strlen(pathVector[i])*sizeof(char));
		fullpath[strlen(pathVector[i])]='/';
		
		//memcpy(fullpath+pathSizeVector*sizeof(char),&slash , sizeof(char));
		memcpy(fullpath+strlen(pathVector[i])+1, command , strlen(command)*sizeof(char));
		printf("check %s\n",fullpath);
		ret = access(fullpath, X_OK);
		if (ret==0) {return fullpath;}
	}
	return NULL;
}


int redirect(char* fname, char* buffer){
	FILE *outputf = fopen(fname,"w");
	if(outputf==NULL){
		return -1;
	}
	fputs(buffer,outputf);
	fclose(outputf);
	return 0;
}

int checkForRedirect(char* command,char **args){return 0;}

int execSingleCmd(char* command,char **args){
	command=checkAccess(command);
	printf("exec %s\n",command);
	if(command==NULL){
		return -1;
	}
	int pid = fork();
	if(pid==0){
		//child
		execv(command, args);
	}
		return 0;
}



int execute(){
	//check built insline
	if(strcmp(tokenVector[0],"exit") ==0 ){//||strcmp(tokenVector[0],"exit\n") ==0){
		printf("\n");
		if(currentTokIndex>1){
			error();
			return-1;
		}
		exit(0);
	}
	else if(strcmp(tokenVector[0],"cd") ==0){
		if(currentTokIndex!=2){
			error();
			return-1;
		}
		//change dir
		if (chdir(tokenVector[1]) != 0){
			error();
			return-1;
		} 
		return 0;
	}
	else if(strcmp(tokenVector[0],"path") ==0){
		clearVector(pathVector,&currentPathIndex);
		int i= 0;
		for( i=1;i<currentTokIndex;++i){
			pathVector[i-1]= tokenVector[i];
			currentPathIndex++;
		}

		// printf("\nNew Path: ");
		for(int i=0;i<currentPathIndex;++i){
			
			printf("%s ",pathVector[i]);
		}
		// printf("\n");
		return 0;
	}
	else if(strcmp(tokenVector[0],"\n") ==0){
		return 0;
	}


	//char ** commands = (char**)malloc(sizeof(char*)*(currentTokIndex+1));
	// char* arguments[]={"-l", "./",NULL};
	// char* command="ls";
	int tokIter=0; //helps iterate for command and argument
	while(tokIter<currentTokIndex){
		if(strcmp(tokenVector[tokIter],"&")==0){
			tokIter++;
			if(tokIter>=currentTokIndex){ break;}
		}

		char *command=tokenVector[tokIter];
		tokIter++;
		char** arguments = (char**)malloc(sizeof(char*)*currentTokIndex);
		// arguments[0]= "-l";
		// arguments[1]="./";
		// arguments[2]=NULL;
		for(int i=0;i<currentTokIndex-1;++i){
			arguments[i]=tokenVector[tokIter];
			printf("arg %s\n", arguments[i]);
			tokIter++;
			if(i==currentTokIndex-2){//set last element to NULL
				arguments[i+1]=NULL;
				printf("arg %s\n", arguments[i+1]);
			}
		}
		
		printf("command %s\n", command);
		// for(int i=0;i<=tokIter-1;++i){
			// printf("arg %s\n", arguments[i]);
		// }
		if(execSingleCmd(command,arguments)<0){
			error();
			return -1;
		}
	}
	

	
	int status;
	while (wait(&status) >=0)
    {
		//printf("wait");
    }
	return 0;
}


void parseAndExecute(char* cmdLine){
		
	char * token;
	while( ( token= strsep(&cmdLine," ")) != NULL ){
       // printf("%s\n",token);
	   if(token==NULL){break;}
		if(currentTokIndex-1 == tokenVectorSize){
			if(resizedVector(tokenVector,&tokenVectorSize)<0){ 
				error();
				clearVector(tokenVector,&currentTokIndex);
				return;
			}
		}
		//take care of \n
		int tokenSize = strlen(token);
		if(token[tokenSize-1]=='\n' &&tokenSize>1){
			token[tokenSize-1] = '\0';
			
		}
		tokenVector[currentTokIndex] = token;
		currentTokIndex++;	

	}
	
	// for(int i =0;i<currentTokIndex;i++){
		// printf("%s",tokenVector[i]);		
	// }
	if(currentTokIndex>=1){
		execute();
	}
	clearVector(tokenVector,&currentTokIndex);
}


int main(int argc, char*argv[]){
	//int size=0;
	char *buffer;
	size_t length=0; 
	
	pathVector = (char**)malloc(sizeof(char*)*pathVectorSize);
	tokenVector = (char**)malloc(sizeof(char*)*tokenVectorSize);
	//pathSizeVector = (int*)malloc(sizeof(int)*pathVectorSize);
	

	// char *tmp = (char*)malloc(sizeof(char)*5);
	// //print("fst size:%d \n",strlen(tmp));
	// for(int i=0;i<5;++i){
		
		// tmp[i] = 'a';
	// }
	// printf("fst size:%ld \n",strlen(tmp));
	// tmp[4]='\0';
	// printf("snd size:%ld \n",strlen(tmp));
	
	if(argc<2){
		while(1){
			printf("wish>");
			getline(&buffer,&length,stdin);
			parseAndExecute(buffer);
		}
	}
	
	
	
	
	return 0;
}