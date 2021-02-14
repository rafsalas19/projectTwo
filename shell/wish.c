#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <assert.h>
#include <string.h>
#include <sys/stat.h>

#define VECTOR_SIZE 100

int vectorSize= VECTOR_SIZE;
char **pathVector;
int currentIndex =0;


int resizedVector(){
	char** temp = (char**)malloc(sizeof(char*)*2*vectorSize);
	if (temp ==NULL){
		fprintf(stderr, "Failed to resize pathVector array.\n");
		exit(1);
	}
	memcpy(temp, pathVector, vectorSize*sizeof(char**));
	vectorSize=2*vectorSize;
	pathVector =temp;
	temp=NULL;
	return 0;
}


int checkAccess(){
	
	for(int i=0;i<currentIndex;++i){
		int return access("/bin/ls", X_OK);
	}
	return -1;
}


int main(int argc, char*argv[]){
	//int size=0;
	char *buffer;
	size_t length=0; 
	
	path = (char**)malloc(sizeof(char*)*pathVectorSize);
	
	if(argc<2){
		while(1){
			printf("wish>");
			getline(&buffer,&length,stdin);
			
			if(strcmp(buffer,"exit\n") ==0 ){
				printf("\n");
				exit(0);
			}
		}
	}
	
	
	
	
	return 0;
}