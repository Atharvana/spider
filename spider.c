// Spider v 1.0
// Bharat Kumar M.
// Experimental Android Source crawler.

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
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

		printf("%s %s", sPath2dir, file.name);
		if (file.is_dir)
		{
			printf("/");
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
			printf("\n file name[0] is %c",file.name[0]);
			printf("\n saved array is %c -> %d",cFile, cFile - 'a');
			fflush(stdout);
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
		printf("\n");
		tinydir_next(&dir);
	}
	tinydir_close(&dir);
	return 0;
}

int main( int argc, char** argv){
	FILE* fp;
	char* sFileName = "omx_vdec_test.cpp";	
	//char* sSourcePath = "/home/bharat/../../cygdrive/c/VE40B";
	char* sSourcePath = "/home/bharat/bharat";
	char* sPersistData = "savedData.dat";
		
	// let us have 65 for each letter of the alphabet
	int nTotal = 0;
	int i, j, k;
	k = 'z'-'a'+2;
	printf("Total Array Values are : %d ",k);
	sCustom* pSourceTree = (sCustom*)malloc(sizeof(sCustom)*k);	
	for(i = 0; i < k; i++){		
		pSourceTree[i].nTotal = 0;
		pSourceTree[i].sElements = NULL;
	}
	parseSourceFolder(&nTotal, sSourcePath,pSourceTree);
	
	printf("\n Total Files are : %d ",nTotal);
	for(i = 0; i < k; i++){
		printf("\n The No of files starting with '%c' are %d", 'a'+i, pSourceTree[i].nTotal);
		fflush(stdout);
		sFile* tempFile = pSourceTree[i].sElements;
		for( j = 0; j< pSourceTree[i].nTotal; j++){
			if(tempFile == NULL){
				printf("\n Invalid Termination!!!");
			}else{
				printf("\n %d - %s, %s", j, tempFile->sFileName, tempFile->sPathName);
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
	if(pSourceTree) free(pSourceTree);
}