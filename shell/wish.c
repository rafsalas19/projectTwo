#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <assert.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/wait.h> 

#define VECTOR_SIZE 100

int pathVectorSize= VECTOR_SIZE;
int tokenVectorSize= VECTOR_SIZE;
//int fileVectorSize= VECTOR_SIZE;
//char **fileVector;
char **pathVector;
char **tokenVector;
char curDir[]="~/";
int currentPathIndex =1;
int currentTokIndex =0;
//int currentFileIndex =0;

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

	if (ret==0) {return command;}

	for(int i=0;i<currentPathIndex;++i){
		int size =  strlen(command)+strlen(pathVector[i])+1;
		char* fullpath = (char*)malloc(sizeof(char)*size);
		memcpy(fullpath,pathVector[i] , strlen(pathVector[i])*sizeof(char));
		fullpath[strlen(pathVector[i])]='/';
		memcpy(fullpath+strlen(pathVector[i])+1, command , strlen(command)*sizeof(char));

		ret = access(fullpath, X_OK);
		if (ret==0) {return fullpath;}
	}
	return NULL;
}


int execSingleCmd(char* command,char **args, bool redirect,char* fname ){
	args[0] =command;
	
	char * fullpath=checkAccess(command);

	if(fullpath==NULL){

		return -1;
	}
	int pid = fork();
	if(pid==0)
	{//child
		if(redirect == true){
			if(fname==NULL){
				exit( -1);
			}
			FILE *outfile= fopen(fname,"w");
			//struct stat ofstat;
			int fno = fileno(outfile);//open(fname, O_TRUNC); //
			if(fno <0){
				exit( -1);
			}
			close(1);// close stdout
			dup2(fno,1);
		}


		//fullpath
		int ret = execv(fullpath, args);
		// char * tmparg[]= {"ls",NULL};
		// int ret = execv("/bin/ls", tmparg);
		exit( ret);
		
	}
	else{
		int status;
		//wait(&status);
		while (wait(&status) >=0)
		{
			
		}
		return status;
		// if (WIFEXITED(status)) 
			// printf("Exit status: %d\n", WEXITSTATUS(status)); 
		// else if (WIFSIGNALED(status)) 
			// psignal(WTERMSIG(status), "Exit signal");
		//printf("return status %d\n",status);
	}
	return 0;
}



int execute(){
	//check built insline
	if(strcmp(tokenVector[0],"exit") ==0 ){//||strcmp(tokenVector[0],"exit\n") ==0){
		if(currentTokIndex>1){
			return-1;
		}
			// int pid = getpid();
		// printf("my pid1=%d\n",pid);
		exit(0);
		//printf("exit2\n");
	}
	else if(strcmp(tokenVector[0],"cd") ==0){
		if(currentTokIndex!=2){
			//error();
			return-1;
		}
		//change dir
		if (chdir(tokenVector[1]) != 0){
			//error();
			return-1;
		} 
		return 0;
	}
	else if(strcmp(tokenVector[0],"path") ==0){
		clearVector(pathVector,&currentPathIndex);
		//currentPathIndex++;
		int i= 0;
		
		//pathVector[0]="/bin";
		for( i=1;i<currentTokIndex;++i){
			if(currentPathIndex-1 == pathVectorSize){
				if(resizedVector(pathVector,&pathVectorSize)<0){ 				
					clearVector(pathVector,&currentPathIndex);
					return -1;
				}
			}
			pathVector[i-1]= tokenVector[i];
			currentPathIndex++;
		}

		return 0;
	}
	else if(strcmp(tokenVector[0],"\n") ==0){
		return 0;
	}


	int tokIter=0; //helps iterate for command and argument

	while(tokIter<currentTokIndex){
		bool redirect =false;
		char* fname=NULL;
		if(strcmp(tokenVector[tokIter],"&")==0){//parallel | look for more commands

			tokIter++;//move on to next command
			if(tokIter>=currentTokIndex){ break;}//no more commands
		}
		else if(strcmp(tokenVector[tokIter],">")==0){//redirect given, this should have been handled by last iteration let break
			if(tokIter==0){
				return -1;
			}
			tokIter++;
			tokIter++;
			
			if(tokIter>=currentTokIndex){
				break;
			}else if(strcmp(tokenVector[tokIter],"&")==0){
				continue;
			}
			else{
			
				return -1;
			}			
		}

		char *command=tokenVector[tokIter];
		tokIter++;
		
		char** arguments = (char**)malloc(sizeof(char*)*(currentTokIndex+1));//free this later
		int parseLimit = currentTokIndex - (tokIter -1); //the limit changes with iteration
		
		for(int i=1;i<parseLimit;++i){
			if(strcmp(tokenVector[tokIter],"&")==0){//if & finish this command so we can move on to next			
				arguments[i] =NULL;
				break;
			}
			else if(strcmp(tokenVector[tokIter],">")==0){//redirect to file
				//printf("here\n");
				if(!(tokIter+1 < currentTokIndex)){//no output file name given
					//error();
					return -1;
				}
				//printf("here\n");
				if(tokIter+2<currentTokIndex){
					if(strcmp(tokenVector[tokIter+2],"&")!=0){
							return -1;
					}
				}
				redirect=true;
				
				fname = tokenVector[tokIter+1];
			//	printf("here %s\n",fname);
				break;
			}

			//default action
			arguments[i]=tokenVector[tokIter];			
			tokIter++;
			if(i==currentTokIndex-2){//set last element to NULL
				arguments[i+1]=NULL;
			}
			else if(tokIter >= currentTokIndex)//hacky but makes sure we are within bounds
			{
				arguments[i+1]=NULL;
				break;
			}
		}
		
		if(execSingleCmd(command,arguments,redirect,fname)<0){

			return -1;
		}
	}

	return 0;
}


char * preparse(char* cmdLine){
	int index = 0;
	int length = strlen(cmdLine);
	while( index < length){
		index = strcspn(cmdLine, "\t");
		if(!(index < strlen(cmdLine))){
			break;
		}
		cmdLine[index] = ' ';
	}
	index = 0;
	while( index < length){
		index = strcspn(cmdLine, "\n");
		if(!(index < strlen(cmdLine))){
			break;
		}

		cmdLine[index] = '\0';
	}
	
	//parse & and >
	char * tempParse = (char*)malloc(sizeof(char)* (length*2));
	int tmp_iter=0;
	for(int i =0;i<length;++i){
		if(cmdLine[i]=='&' ||cmdLine[i]=='>'){
			if(tmp_iter!=0){
				tempParse[tmp_iter] = ' ';
				tmp_iter++;
			}
			tempParse[tmp_iter] = cmdLine[i];
			tmp_iter++;
			tempParse[tmp_iter] = ' ';
			tmp_iter++;
		}else{
			tempParse[tmp_iter] = cmdLine[i];
			tmp_iter++;
		}	
	}
	
	tempParse[tmp_iter] = '\0';
	length = strlen(tempParse);
	//get rid of extra space
	char *parseStr= (char*)malloc(sizeof(char)* (length+1));
	bool spaceLastchar = false;
	int parseIter=0;
	for(int i =0;i<length;++i){	
		if((tempParse[i]==' ') && (spaceLastchar==false)){
			spaceLastchar = true;
		}
		else if((tempParse[i]==' ') && (spaceLastchar==true)){
			continue;
		}
		else{
			spaceLastchar =false;
		}
		if(parseIter == 0&& tempParse[i]==' '){
			continue;
		}
		parseStr[parseIter] = tempParse[i];
		parseIter++;
	}
	if(parseStr[parseIter-1]==' '){
		parseStr[parseIter-1] ='\0';
	}else{
		parseStr[parseIter] ='\0';
	}

	
	//printf("%s\n",parseStr);
	free(tempParse);
	return parseStr;
}

void parseAndExecute(char* cmdLine){
	char* prep= preparse(cmdLine);
	char * token;
	
	while( ( token= strsep(&prep," ")) != NULL ){
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
		
		char * pushToken= (char*)malloc(sizeof(char)*strlen(token));
		*pushToken = *token;
		memcpy(pushToken,token , strlen(token)*sizeof(char));
		tokenVector[currentTokIndex] = pushToken;		
		currentTokIndex++;	

	}

	if(currentTokIndex>=1){
		if(execute()<0){
			error();
			clearVector(tokenVector,&currentTokIndex);
			return;
		}
	}
	
	clearVector(tokenVector,&currentTokIndex);
}


int main(int argc, char*argv[]){
	//int size=0;
	char *buffer;
	size_t length=0; 
	
	pathVector = (char**)malloc(sizeof(char*)*pathVectorSize);
	tokenVector = (char**)malloc(sizeof(char*)*tokenVectorSize);
	//fileVector = (int*)malloc(sizeof(int)*fileVectorSize);
	
	pathVector[0]="/bin";

	if(argc<2){
		while(1){
			printf("wish>");
			getline(&buffer,&length,stdin);
			//preparse(buffer);
			parseAndExecute(buffer);
		}
	}
	else if (argc <3){
		
		FILE *inputf = (FILE *)malloc(sizeof(FILE));

		inputf= fopen(argv[1],"r");//try to open input file	
		//printf("file: %s\n",argv[i+1]);
		if(inputf==NULL){
			error();
			exit(1);
		}	
		//getlines
	
		while(getline(&buffer,&length,inputf)!=-1){			
			parseAndExecute(buffer);
		}
		fclose(inputf);
		
	}
	else{
		error();
		exit(1);
	}
	
	return 0;
}