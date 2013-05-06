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
#include <time.h>  
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

int parseSourceFolder(int* nTotalFiles,  char* fNameHas, char* sPath2dir,sCustom* pSourceTree){
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
				parseSourceFolder(nTotalFiles, fNameHas, sNewFilePath, pSourceTree);
			}		
		}else{
			if((fNameHas == NULL) ||((fNameHas!=NULL) && (strstr(file.name,fNameHas)!=NULL))){
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
		}
		//printf("\n");
		tinydir_next(&dir);
	}
	tinydir_close(&dir);
	return 0;
}


void fnAddToTree(char* sFileName,char * sPath2dir, sCustom* pSourceTree){
	// store it
	char cFile = sFileName[0];
	if(cFile>='A' && cFile<='Z'){
		cFile = sFileName[0] + ('a'-'A');
	}if(cFile>='a' && cFile<='z'){				
	}else{
		cFile = 'z'+1;
	}
	sFile* newFile = (sFile*)malloc(sizeof(sFile));
	newFile->sFileName = (char*)malloc(sizeof(char)*(strlen(sFileName)+2));
	newFile->sPathName = (char*)malloc(sizeof(char)*(strlen(sPath2dir)+2));
	strcpy(newFile->sFileName,sFileName);
	strcpy(newFile->sPathName,sPath2dir);
	newFile->next = (pSourceTree[cFile -'a']).sElements;
	(pSourceTree[cFile -'a']).sElements = newFile;
	(pSourceTree[cFile -'a']).nTotal += 1;	
}

void fnPrintTree(sCustom* pSourceTree, char c){
	int i, j;
	int nBuckets = 'z'-'a'+2;	
	if(c != '?'){
		i = c - 'a';
	}
	for((c == '?')?(i = 0):(i = c - 'a'); (c == '?')?(i < nBuckets):(i <(c - 'a' + 1 )); i++){
		sFile* tempFile = pSourceTree[i].sElements;
		for( j = 0; j< pSourceTree[i].nTotal; j++){
			if(tempFile == NULL){
				printf("\n Invalid Termination!!!");
			}else{
				printf("\n %d - %s, %s", j, tempFile->sFileName, tempFile->sPathName);				
				tempFile = tempFile->next;									
			}
		}
	}
}

int fnIsFNameInTree(char* sFileName, char* sPath2dir, sCustom* pSourceTree){
	char* sSpecial = strstr(sFileName,"/");
	if(sSpecial!=NULL){
		//printf("\nFor / Case Changed Name from : %s to", sFileName);
		sFileName = sSpecial+1;
		//printf(" %s",sFileName);
	}
	int i, j, a, b;
	int nBuckets = 'z'-'a'+2;
	char cFile = sFileName[0];
	if(cFile>='A' && cFile<='Z'){
		cFile = sFileName[0] + ('a'-'A');
	}if(cFile>='a' && cFile<='z'){				
	}else{
		cFile = 'z'+1;
	}
	i = cFile - 'a';	
	sFile* tempFile = pSourceTree[i].sElements;
	for( j = 0; j< pSourceTree[i].nTotal; j++){
		if(tempFile == NULL){
			printf("\n Invalid Termination!!!");
		}else{
			//char* sSpecial = strstr(sFileName,"/");
			//if(sSpecial!=NULL){
			//	//printf("\n Processing Special Case");
			//	if(0 == strcmp(tempFile->sFileName,(sSpecial+1))){
			//		// will add path check later.
			//		if(sPath2dir) {
			//			printf("\n(%c) %d - %s, %s  for %s ",'a'+i, j, tempFile->sFileName, tempFile->sPathName, sFileName);
			//			strcpy(sPath2dir, tempFile->sPathName);
			//		}
			//		return 1;
			//	}
			//}
			//a = strlen(tempFile->sFileName);
			//b = strlen(sFileName);
			//if( a == b ) {
			//	printf("\n These have same length :");
			//	printf("\n %s (%d)",tempFile->sFileName, a);
			//	printf("\n %s (%d)",sFileName, b );
			//}
			//if(strstr(tempFile->sFileName,sFileName)!= NULL){
			//	printf("\n Names %s and %s are related ? ",tempFile->sFileName,sFileName);
			//	printf("\n Strcmp returns %d for those names ",strcmp(tempFile->sFileName,sFileName));
			//}
			if(0 == strcmp(tempFile->sFileName,sFileName)){
				if(sPath2dir) {
					strcpy(sPath2dir, tempFile->sPathName);
				}
				return 1;
			}			
			tempFile = tempFile->next;				
		}
	}	
	return 0;
}

void fnClearData(sCustom* pSourceTree){
	int i, j;
	int nBuckets = 'z'-'a'+2;
	for(i = 0; i < nBuckets; i++){		
		sFile* tempFile = pSourceTree[i].sElements;
		for( j = 0; j< pSourceTree[i].nTotal; j++){
			if(tempFile == NULL){
				printf("\n Invalid Termination!!!");
			}else{
				//printf("\n(%c) %d - %s, %s",'a'+i, j, tempFile->sFileName, tempFile->sPathName);					
				sFile* freeFile = tempFile;
				tempFile = tempFile->next;
				free(freeFile->sFileName);
				free(freeFile->sPathName);
				free(freeFile);
				freeFile = NULL;				
			}
		}
	}	
}

int fnPrepareData(int* nTotalFiles, char* fNameHas, char* sPersistData, char* sDemoPath, sCustom* pSourceTree){
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
		*nTotalFiles = nTotal;
		char demoChar;
		char sFileName[80];
		char sPathName[256];		
		for(i = 0; i < nBuckets; i++){
			fscanf(fp,"\n%c,%d", &demoChar, &pSourceTree[i].nTotal);
			//printf("\n The No of files starting with '%c' are %d", demoChar, pSourceTree[i].nTotal);
		}
		char* sTestString = NULL;
		for(i = 0; i < nBuckets; i++){
			for( j = 0; j< pSourceTree[i].nTotal; j++){
				fscanf(fp,"%s %[^\n]",sFileName, sPathName);
				sTestString = strstr(sFileName,"/");
				if(sTestString != NULL ){
					printf("\n Path and Name are being exchanged for : name : %s , path: %s" , sFileName, sPathName);
					printf("\n The Position in the file is %l, %d",ftell(fp), ftell(fp));
					printf("\n Ignoring file");
					//return -1;
				}else{
					sFile* newFile = (sFile*)malloc(sizeof(sFile));
					newFile->sFileName = (char*)malloc(sizeof(char)*(strlen(sFileName)+2));
					newFile->sPathName = (char*)malloc(sizeof(char)*(strlen(sPathName)+2));
					strcpy(newFile->sFileName, sFileName);
					strcpy(newFile->sPathName, sPathName);
					newFile->next = (pSourceTree[i]).sElements;
					(pSourceTree[i]).sElements = newFile;
				}
			}
		}
		fclose(fp);			
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
		nTotal = 0;
		parseSourceFolder(&nTotal, fNameHas, sSourcePath,pSourceTree);
		//printf("\n Total Files are : %d ",nTotal);
		*nTotalFiles = nTotal;
		fprintf(fp,"\n%d",nTotal);	// write the total no of files next
		for(i = 0; i < nBuckets; i++){
			//printf("\n The No of files starting with '%c' are %d", 'a'+i, pSourceTree[i].nTotal);
			fprintf(fp,"\n%c,%d",'a'+i, pSourceTree[i].nTotal); // write the element lists next
			//fflush(stdout);
		}
		for(i = 0; i < nBuckets; i++){		
			sFile* tempFile = pSourceTree[i].sElements;
			for( j = 0; j< pSourceTree[i].nTotal; j++){
				if(tempFile == NULL){
					printf("\n Invalid Termination!!!");
				}else{
					//printf("\n %d - %s, %s", j, tempFile->sFileName, tempFile->sPathName);
					fprintf(fp,"\n%s %s",tempFile->sFileName, tempFile->sPathName); // write the filenames and path names next
					//fflush(stdout);					
					tempFile = tempFile->next;									
				}
			}
		}
		fclose(fp);
	}	
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
			sFileName[i] == '.'|| sFileName[i] == '_' || sFileName[i] == '/'){			
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

int getHeaderFilesNeeded(char* sRequiredFile, int* nTotalFiles, sCustom* pReqdTree, sCustom* pSourceTree, sCustom* pHeaderTree){
	char sFileLine[32];
	char reqdFile[32];
	char reqdPath[256];
	// the following two operators will prevent it from parsing the entire file, will break after 15 non header lines after a header is found.
	int nBlanksAllowed = 50;
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
				//printf("\n The Required Header now is : %s with length : %d ",reqdFile, strlen(reqdFile));								
				if(1 == fnIsFNameInTree( reqdFile, NULL, pHeaderTree)){
					// Check if it is a standard header
					printf("\n The Required Header : %s is a standard Header",reqdFile);
				//}else if( 1 == fnIsFNameInTree(reqdFile, NULL, pReqdTree)){
				//	// skip if it is already included to prevent infinite loop
				//	printf("\n The Required Header : %s is already in Queue ",reqdFile);
				}else{
					// check if the header could be found in the source headers					
					if(1 == fnIsFNameInTree(reqdFile, reqdPath, pSourceTree)){
						printf("\n The Required Header : %s has been found In Src at %s ",reqdFile, reqdPath);
						fnAddToTree( reqdFile, reqdPath,pReqdTree);
					}else{
						printf("\n The Required Header : %s has been not been found in Src ",reqdFile);
						fnAddToTree( reqdFile, "?",pReqdTree);
					}
					*nTotalFiles += 1;
				}
				fflush(stdout);				
				// also 
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
	int k 				= 'z'-'a'+2;
	int nTotalFiles 	= 0;
	int nTotHeaders 	= 0;
	int nTotReqd		= 0;
	char* stdHeaderPath = "/home/bharat/../../cygdrive/c/ndk-r8d/sources";
	char* sDemoPath 	= "/home/bharat/../../cygdrive/u"; //"/home/bharat/bharat";  // "/home/bharat/../../cygdrive/c/VE40B";
	char* sPersistData 	= "savedData.dat";
	char* sHeaderData  	= "headerData.dat";
	char* sHeaderExt   	= ".h";
	char* sTestFile 	= "omx_vdec_test.cpp";//"gltest.c";
	time_t start, ends;
	double seconds;
	
	sCustom* pHeaderTree 	= NULL;
	sCustom* pSourceTree 	= NULL;
	sCustom* pReqdTree 		= NULL;
	
	printf("Total Array Values are : %d ",k);
	
	// creating header list
	time(&start);
	printf("\n Preparing Header Sources from: %s ",stdHeaderPath);
	pHeaderTree = (sCustom*)malloc(sizeof(sCustom)*k);	
	for(i = 0; i < k; i++){		
		pHeaderTree[i].nTotal = 0;
		pHeaderTree[i].sElements = NULL;
	}	
	if(2 == fnPrepareData(&nTotHeaders, sHeaderExt, sHeaderData, stdHeaderPath, pHeaderTree)){
		if( remove( sHeaderData ) != 0 ){
			printf( "Error deleting file" );
		} else{
			printf( "File successfully deleted" );
			fnPrepareData(&nTotHeaders, sHeaderExt, sHeaderData, stdHeaderPath, pHeaderTree);
		}		 
	}	
	time(&ends);
	seconds = difftime(ends, start);
	printf("\n Generating Headers took %lf seconds for %d files ", seconds, nTotHeaders);
	// ends header list
	
	// creating sources list	
	time(&start);
	printf("\n Preparing Sources from: %s ",sDemoPath);
	pSourceTree = (sCustom*)malloc(sizeof(sCustom)*k);	
	for(i = 0; i < k; i++){		
		pSourceTree[i].nTotal = 0;
		pSourceTree[i].sElements = NULL;
	}
	nTotalFiles 	= 0;
	int nValue = fnPrepareData(&nTotalFiles, sHeaderExt, sPersistData, sDemoPath, pSourceTree);
	if(2 == nValue){
		if( remove( sPersistData ) != 0 ){
			printf( "Error deleting file" );
		} else{
			printf( "File successfully deleted" );
			nTotalFiles 	= 0;
			fnPrepareData(&nTotalFiles, sHeaderExt, sPersistData, sDemoPath, pSourceTree);
		}		 
	}
	if(-1 == nValue){
		printf("\n Error in Program! ")	;		
		return -1;
	}
	time(&ends);
	seconds = difftime( ends, start);
	printf("\n Generating Sources DB for %d files took %lf seconds  ", nTotalFiles, seconds);
	// ends creating sources list
	
#if (1)	
	// get the list of header files needed
	time(&start);
	printf("\n Getting the headers needed ..");
	pReqdTree = (sCustom*)malloc(sizeof(sCustom)*k);	
	for(i = 0; i < k; i++){		
		pReqdTree[i].nTotal = 0;
		pReqdTree[i].sElements = NULL;
	}
	getHeaderFilesNeeded(sTestFile, &nTotReqd, pReqdTree, pSourceTree, pHeaderTree);
	
	printf("\n Non Standard Headers are (%d):",nTotReqd);
	fnPrintTree(pReqdTree, '?');
#endif	
//	fnPrintTree(pSourceTree, 'u');
	// reclaim all allocates spaces		
	if(pHeaderTree)	{
		fnClearData(pHeaderTree);
		free(pHeaderTree);
	}
	if(pSourceTree) {
		fnClearData(pSourceTree);
		free(pSourceTree);
	}
	if(pReqdTree) {
		fnClearData(pReqdTree);
		free(pReqdTree);
	}
}