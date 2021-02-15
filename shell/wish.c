#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <assert.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define VECTOR_SIZE 100

int pathVectorSize= VECTOR_SIZE;
char **pathVector;
char curDir[]="~/";

int currentIndex =0;
//int *pathSizeVector;




int resizedVector(){
	char** temp = (char**)malloc(sizeof(char*)*2*pathVectorSize);
	//int* tempSize= (int*)malloc(sizeof(int)*2*pathVectorSize);
	
	if (temp ==NULL ){
		fprintf(stderr, "Failed to resize pathVector array.\n");
		exit(1);
	}
	memcpy(temp, pathVector, pathVectorSize*sizeof(char**));
	//memcpy(tempSize, pathSizeVector, pathVectorSize*sizeof(int*));
	pathVectorSize=2*pathVectorSize;
	pathVector =temp;
	//pathSizeVector = tempSize;
	//tempSize=NULL;
	temp=NULL;
	return 0;
}


int isBuiltIn(char* command,int length){return 0;}

char* checkAccess(char* command,int length){
	int ret = access(command, X_OK);
	if (ret==0) {return 0;}
	for(int i=0;i<currentIndex;++i){
		int size =  strlen(command)+strlen(pathVector[i])+1;
		char* fullpath = (char*)malloc(sizeof(char)*size);
		//char slash ='/';
		
		memcpy(fullpath,pathVector[i] , strlen(pathVector[i])*sizeof(char));
		fullpath[strlen(pathVector[i])]='/';
		//memcpy(fullpath+pathSizeVector*sizeof(char),&slash , sizeof(char));
		memcpy(fullpath+strlen(pathVector[i])+1, command , strlen(command)*sizeof(char));
		
		ret = access(fullpath, X_OK);
		if (ret==0) {return 0;}
	}
	return NULL;
}

void parseAndExecute(char* cmdLine){
	
	
	char * token;
	while( ( token= strtok(cmdLine," ")) != NULL ){
        //printf("%s\n",token);
		
		if(strcmp(cmdLine,"exit") ==0 ||strcmp(cmdLine,"exit\n") ==0){
			printf("\n");
			exit(0);
		}	
		
	}
	
	

	
}


int main(int argc, char*argv[]){
	//int size=0;
	char *buffer;
	size_t length=0; 
	
	pathVector = (char**)malloc(sizeof(char*)*pathVectorSize);
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