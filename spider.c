// Spider v 2.0
// Bharat Kumar M.
// Experimental Android Source crawler.
// TODO: Should add another set of buckets with fileName sizes instead of starting chars.
// TODO: Add persist and load method.

/* 
// FILE structure
path of the folder that will be persisted
total no of files
'a',<no of files>
.
.
'z',<no of files>
'{'<no of files>
all files names with paths
filename1.c path2
filename2.c path2
.
.
.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "tinydir.h"

const char* sFolderToIgnore[5] = {".","..","CVS"};
int    nSizeFolder2Ignore = 3;

typedef struct sFileStruct{
	char* sFileName;
	char* sPathName;
	struct sFileStruct* next;
}sFile;

typedef struct{
	int nTotal;
	sFile* sElements;
}sCustom;

int parseSourceFolder(int* nTotalFiles, char* sPath2dir,sCustom* pSourceTree){
	tinydir_dir dir;
	if (tinydir_open(&dir, sPath2dir) == -1)
	{
		printf("\n Error opening file %s",sPath2dir);
		tinydir_close(&dir);
		return 1;
	}
	while (dir.has_next)
	{
		tinydir_file file;
		if (tinydir_readfile(&dir, &file) == -1)
		{
			printf("Error getting file");
			tinydir_close(&dir);
			return 1;
		}

		//printf("%s %s", sPath2dir, file.name);
		if (file.is_dir)
		{
			//printf("/");
			int i = 0, bCheck = 1;
			// check if the folder is to be ignored
			for(i = 0; i< nSizeFolder2Ignore; i++){
				if(strcmp(file.name,sFolderToIgnore[i])==0 ) bCheck = 0;				
			}
			if(bCheck == 1){
				char sNewFilePath[strlen(sPath2dir)+strlen(file.name)+5];
				sprintf(sNewFilePath,"%s/%s",sPath2dir,file.name);
				parseSourceFolder(nTotalFiles, sNewFilePath, pSourceTree);
			}		
		}else{
			// store it
			char cFile = file.name[0];
			if(cFile>='A' && cFile<='Z'){
				cFile = file.name[0] + ('a'-'A');
			}if(cFile>='a' && cFile<='z'){				
			}else{
				cFile = 'z'+1;
			}
			//printf("\n file name[0] is %c",file.name[0]);
			//printf("\n saved array is %c -> %d",cFile, cFile - 'a');
			//fflush(stdout);
			sFile* newFile = (sFile*)malloc(sizeof(sFile));
			newFile->sFileName = (char*)malloc(sizeof(char)*(strlen(file.name)+2));
			newFile->sPathName = (char*)malloc(sizeof(char)*(strlen(sPath2dir)+2));
			strcpy(newFile->sFileName,file.name);
			strcpy(newFile->sPathName,sPath2dir);
			newFile->next = (pSourceTree[cFile -'a']).sElements;
			(pSourceTree[cFile -'a']).sElements = newFile;
			(pSourceTree[cFile -'a']).nTotal += 1;			
			*nTotalFiles += 1;
		}
		//printf("\n");
		tinydir_next(&dir);
	}
	tinydir_close(&dir);
	return 0;
}

int fnPrepareData(int* nTotalFiles, char* sPersistData, char* sDemoPath, sCustom* pSourceTree){
	FILE* fp;
	//char* sFileName = "omx_vdec_test.cpp";	
	//char* sSourcePath = "/home/bharat/../../cygdrive/c/VE40B";
	//char* sDemoPath = "/home/bharat/bharat";	
	char sSourcePath[256];
	// let us have 65 for each letter of the alphabet
	int nTotal = 0;
	int i, j, nBuckets;
	nBuckets = 'z'-'a'+2;
	*nTotalFiles = 0;
	/*
	You can also use R_OK, W_OK, and X_OK in place of F_OK to check for read permission, write permission, and execute permission (respectively) rather than 
	existence, and you can OR any of them together (i.e. check for both read and write permission using R_OK|W_OK)
	*/
	if( access( sPersistData, R_OK ) != -1 ) { 
		// file exists
		fp = fopen(sPersistData,"r");
		fscanf(fp,"%s",sSourcePath);
		printf("\n Reading Persistance DB : %s",sSourcePath);
		if(strcmp(sSourcePath,sDemoPath)!=0){
			printf("\nError! Persistance file content path: %s is different from the path needed: %s", sSourcePath, sDemoPath);
			return 2;
		}
		fscanf(fp,"%d",&nTotal);
		printf("\n Total Files are : %d ",nTotal);
		char demoChar;
		char sFileName[80];
		char sPathName[256];		
		for(i = 0; i < nBuckets; i++){
			fscanf(fp,"\n%c,%d", &demoChar, &pSourceTree[i].nTotal);
			printf("\n The No of files starting with '%c' are %d", demoChar, pSourceTree[i].nTotal);
		}
		for(i = 0; i < nBuckets; i++){
			for( j = 0; j< pSourceTree[i].nTotal; j++){
				fscanf(fp,"%s %s",sFileName, sPathName);
				sFile* newFile = (sFile*)malloc(sizeof(sFile));
				newFile->sFileName = (char*)malloc(sizeof(char)*(strlen(sFileName)+2));
				newFile->sPathName = (char*)malloc(sizeof(char)*(strlen(sPathName)+2));
				strcpy(newFile->sFileName, sFileName);
				strcpy(newFile->sPathName, sPathName);
				newFile->next = (pSourceTree[i]).sElements;
				(pSourceTree[i]).sElements = newFile;				
			}
		}
		fclose(fp);
		for(i = 0; i < nBuckets; i++){		
			sFile* tempFile = pSourceTree[i].sElements;
			for( j = 0; j< pSourceTree[i].nTotal; j++){
				if(tempFile == NULL){
					printf("\n Invalid Termination!!!");
				}else{
					printf("\n(%c) %d - %s, %s",'a'+i, j, tempFile->sFileName, tempFile->sPathName);					
					sFile* freeFile = tempFile;
					tempFile = tempFile->next;
					free(freeFile->sFileName);
					free(freeFile->sPathName);
					free(freeFile);
					freeFile = NULL;				
				}
			}
		}		
	} else {
		// file doesn't exist, generate the file
		strcpy(sSourcePath,sDemoPath);
		fp = fopen(sPersistData,"w");
		if(fp == NULL){
			printf("\n Unable to Write Persistance Data");			
			return -1;
		}
		printf("\n Writing Persistance DB : %s",sSourcePath);
		// write path first
		fprintf(fp,"%s",sSourcePath);
		parseSourceFolder(&nTotal, sSourcePath,pSourceTree);
		printf("\n Total Files are : %d ",nTotal);
		fprintf(fp,"\n%d",nTotal);	// write the total no of files next
		for(i = 0; i < nBuckets; i++){
			printf("\n The No of files starting with '%c' are %d", 'a'+i, pSourceTree[i].nTotal);
			fprintf(fp,"\n%c,%d",'a'+i, pSourceTree[i].nTotal); // write the element lists next
			fflush(stdout);
		}
		for(i = 0; i < nBuckets; i++){		
			sFile* tempFile = pSourceTree[i].sElements;
			for( j = 0; j< pSourceTree[i].nTotal; j++){
				if(tempFile == NULL){
					printf("\n Invalid Termination!!!");
				}else{
					printf("\n %d - %s, %s", j, tempFile->sFileName, tempFile->sPathName);
					fprintf(fp,"\n%s %s",tempFile->sFileName, tempFile->sPathName); // write the filenames and path names next
					fflush(stdout);				
					sFile* freeFile = tempFile;
					tempFile = tempFile->next;
					free(freeFile->sFileName);
					free(freeFile->sPathName);
					free(freeFile);
					freeFile = NULL;				
				}
			}
		}
		fclose(fp);
	}
	*nTotalFiles = nTotal;
}

void formatToFileName(char* sFileName){
	int x = strlen(sFileName);
	int i = 0, j = 0;
	int nBeginBlock = 0;
	char* reqdChars;
	char  sOutName[32];
	for(i = 0; i<x ; i++){
		//printf("\n %d is %d , %c ", i, sFileName[i], sFileName[i]);
		if((sFileName[i]>='a' && sFileName[i]<='z') || 
			(sFileName[i]>='A' && sFileName[i]<='Z')||
			(sFileName[i]>='0' && sFileName[i]<='9')||
			sFileName[i] == '.'|| sFileName[i] == '_'){			
			sOutName[j++] = sFileName[i];
			nBeginBlock = 1;
		}
		if(nBeginBlock == 1 && (sFileName[i] == ' '||sFileName[i] == '\t'||sFileName[i]=='\n'||sFileName[i]=='\r')){
			//printf("\n Breaking");
			break;
		}
	}
	sOutName[j] = '\0';
	reqdChars = strstr(sOutName,".h");
	if(reqdChars==NULL){
		// append  .h to this
		sOutName[j] = '.';
		sOutName[j+1] = 'h';
		sOutName[j+2] = '\0';
	}
	strcpy(sFileName,sOutName);	
}

int getHeaderFilesNeeded(char* sRequiredFile, int* nTotalFiles, sCustom* pSourceTree){
	//sCustom* stdHdrList = NULL;
	//fnGetStdHdrs(stdHdrList);
	char sFileLine[32];
	char reqdFile[32];
	int nBlanksAllowed = 15;
	int bClearBlanks = 0;
	char* filteredWord = "#include";	
	FILE* fp = fopen(sRequiredFile,"r");
	if(fp){
		while((!feof(fp)&&(fgets(sFileLine,32,fp)!=NULL))&&(nBlanksAllowed>0)){
			char* reqdChars;
			reqdChars = strstr(sFileLine,filteredWord);
			if(reqdChars!=NULL){
				// strip "" or <> and if the name does'nt have a .h append it							
				strncpy(reqdFile, reqdChars+(strlen(filteredWord)), (strlen(reqdChars) - strlen(filteredWord)));
				//printf("\n The Required Header now was : %s with length : %d ",reqdFile, strlen(reqdFile));
				formatToFileName(reqdFile);
				printf("\n The Required Header now is : %s with length : %d ",reqdFile, strlen(reqdFile));
				fflush(stdout);
				// also check if it is a standard header
				// also skip if it is already included to prevent infinite loop
				/*
				sFile* newFile = (sFile*)malloc(sizeof(sFile));
				newFile->sFileName = (char*)malloc(sizeof(char)*(strlen(file.name)+2));
				newFile->sPathName = (char*)malloc(sizeof(char)*(strlen(sPath2dir)+2));
				strcpy(newFile->sFileName,file.name);
				strcpy(newFile->sPathName,sPath2dir);
				newFile->next = (pSourceTree[cFile -'a']).sElements;
				(pSourceTree[cFile -'a']).sElements = newFile;
				(pSourceTree[cFile -'a']).nTotal += 1;			
				*nTotalFiles += 1;
				*/
				bClearBlanks = 1;
			}else{
				if(1 == bClearBlanks)
					nBlanksAllowed--;
			}
		}
	}else{
		printf("\n Unable to Open the Source file : %s",sRequiredFile);
	}
}


int main( int argc, char** argv){
	int i;
	int nTotalFiles = 0;
	int k = 'z'-'a'+2;
	char* sDemoPath = "/home/bharat/bharat";
	char* sPersistData = "savedData.dat";
	char* sTestFile = "gltest.c";
	printf("Total Array Values are : %d ",k);
	sCustom* pSourceTree = (sCustom*)malloc(sizeof(sCustom)*k);	
	for(i = 0; i < k; i++){		
		pSourceTree[i].nTotal = 0;
		pSourceTree[i].sElements = NULL;
	}
	if(2 == fnPrepareData(&nTotalFiles, sPersistData, sDemoPath, pSourceTree)){
		if( remove( sPersistData ) != 0 ){
			printf( "Error deleting file" );
		} else{
			printf( "File successfully deleted" );
			fnPrepareData(&nTotalFiles, sPersistData, sDemoPath, pSourceTree);
		}		 
	}
	getHeaderFilesNeeded(sTestFile,NULL,NULL);
	if(pSourceTree) free(pSourceTree);	
}