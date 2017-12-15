#include <unistd.h>     /* Symbolic Constants */
#include <sys/types.h>  /* Primitive System Data Types */ 
#include <errno.h>      /* Errors */
#include <stdio.h>      /* Input/Output */
#include <stdlib.h>     /* General Utilities */
#include <pthread.h>    /* POSIX Threads */
#include <string.h>
#include <time.h>
#include <math.h>
#include <ctype.h>

#define FALSE 0
#define TRUE 1
#define MAX_LINE_LENGTH 100
#define MAX_WORDS_PER_LINE 6
#define MAX_WORD_LENGTH 30
#define MAX_PRINT_LENGTH 30
#define MAX_LABELS 30



#define CHECK_IF_VAR(token)  \
	if (check_if_var( token ) == 1) \
		return 1;


#define CHECK_IF_GLOBAL_VAR(token) \
	if (check_if_global_var( token ) == 1)  return 1;
     


#define CHECK_IF_VARVAL(token) \
	if (check_if_var( token ) == 1) \
		if ( checkIfInteger( token ) == FALSE )   return 1;


	
 

unsigned int labels[MAX_LABELS][2]; //arxikopoihsh me /0 ???
int labelCounter = 0;       
//find_pc_from_label(char *label, int **labels, int numOfLabels)



int checkIfInteger(char *str){//akurh????
         
    int valid = TRUE;
    int i = 0;
    
    if (str[0] == '-') {
	i++;
    }	

    for ( ; i < strlen(str); i++)
    {
        if (!isdigit(str[i]))
        {
            valid = FALSE;
            break;
        }
    }
    return(valid);
}

int countLines(char* file_arg){ //inline int checkSyntax
    
    char buf[200];
    int count=0;
    
    FILE* f = fopen(file_arg, "r");
    if(f == NULL) {
      perror("Error opening file");
      return(-1);
    }
    while(fgets(buf,sizeof(buf), f) != NULL){
        count++;
    }
    
    fclose(f);
    
    return(count);
}



int check_if_global_var( char *token )  {
   if(token[0] == '$') {
	if ( isalpha(token[1]) ) {
				
	  	
	/*
	 * insert here token[2]+1 to list with GLOBAL variables(not variables). If it is already declared, it is OK:)
	 */	
	}      
	else {
		return (1);  
	}
   }
 
   return (0);
}


int check_if_var( char *token )  {
   if(token[0] == '$'){
	if ( isalpha(token[1]) ) {
	/*
	 * insert here token[1]+1 to list with variables(not global variables). If it is already declared, it is OK:)
	 */
	}      
	else {
		return (1);  
	}
    }
    else {
	return (1);
    }
 
    return (0);
	
}

/*
 * - Find the hash of the label
 * - If the hash doesn't exist, return error code
 */
int find_pc_from_label(char *label)  {
	int i;
	unsigned long hash = 5381;
	char c;
 	int j=0;

 	while ((c = label[j])){
      		hash = ((hash << 5) + hash) + c; // hash * 33 + c 
      		j++;
 	}
 	
 	for (i = 0; i < labelCounter; i++)  {
   		if ( labels[i][0] == hash )
			return labels[i][1];//return the programm counter
	}


  	return -1;
}   


int checkSyntax(char token[MAX_WORDS_PER_LINE][MAX_WORD_LENGTH]){  //inline int checkSyntax || inline kai tis mikres sun?

    //int x,y;
    char instruction[MAX_WORD_LENGTH];
    strcpy(instruction, token[0]);
 
   
    if(strcmp(instruction, "LOAD") == 0){
	/*
	 * 1st parameter: var
	 */
	CHECK_IF_VAR( token[1] );

	/*
	 * 2nd parameter: globalVar
	 */
	CHECK_IF_GLOBAL_VAR( token[2] );


	/*
	 * edw kanonika prepei na mpei kapoios elegxos stin periptwsi: LOAD $r1 $r2 $r3
	 */
         
    }
    else if(strcmp(instruction, "STORE") == 0){
	/*
	 * 1st parameter: globalVar
	 */
	CHECK_IF_GLOBAL_VAR( token[1] );


	/*
	 * 2nd parameter: var or int. 
	 */
	CHECK_IF_VARVAL(token[2]);


	/*
	 * edw kanonika prepei na mpei kapoios elegxos stin periptwsi: STORE $r1 $r2 $r3
	 */
        
    }
    else if(strcmp(instruction, "SET") == 0){
    	/*
	 * 1st parameter: var
	 */
	CHECK_IF_VAR( token[1] );

	/*
	 * 2nd parameter: var or int. 
	 */
	CHECK_IF_VARVAL(token[2]);

	/*
	 * edw kanonika prepei na mpei kapoios elegxos stin periptwsi: SET $r1 $r2 $r3
	 */
        
    }
    else if(strcmp(instruction, "ADD") == 0 ||
 	    strcmp(instruction, "SUB") == 0 ||
	    strcmp(instruction, "MUL") == 0 || 
	    strcmp(instruction, "DIV") == 0 ||
	    strcmp(instruction, "MOD") == 0 )                  {


	/*
	 * 1st parameter: var
	 */
	CHECK_IF_VAR( token[1] );
     
	/*
	 * 2nd parameter: var or int. 
	 */
	CHECK_IF_VARVAL(token[2]);

	/*
	 * 3rd parameter: var or int. 
	 */
	CHECK_IF_VARVAL(token[3]);   
	
	/*
	 * edw kanonika prepei na mpei kapoios elegxos stin periptwsi: ADD $r1 $r2 $r3 $r4
	 */
    }
    else if(strcmp(instruction, "BRGT") == 0 ||
	    strcmp(instruction, "BRGE") == 0 ||
	    strcmp(instruction, "BRLT") == 0 || 
	    strcmp(instruction, "BRLE") == 0 || 
	    strcmp(instruction, "BREQ") == 0 )              {


	/*
	 * 1st parameter: var or int. 
	 */
	CHECK_IF_VARVAL(token[1]);


	/*
	 * 2nd parameter: var or int. 
	 */
	CHECK_IF_VARVAL(token[2]);


	/*
	 * 3rd parameter: label
	 */
	if (find_pc_from_label(token[3]) == -1 )  
		return 1;
        
    }
    else if  ( strcmp(instruction, "BRA") == 0 )  {
	/*
	 * 1st parameter: label
	 */
	 if (find_pc_from_label(token[1]) == -1 )  
		return 1;

    }
    else if(strcmp(instruction, "DOWN") == 0){
        
    }
    else if(strcmp(instruction, "UP") == 0){
        
    }
    else if(strcmp(instruction, "SLEEP") == 0){
        
    }
    else if(strcmp(instruction, "PRINT") == 0){
        
    }
    else if(strcmp(instruction, "RETURN") == 0){
        
    }
    else if(strcmp(instruction, "#PROGRAM") == 0){
        
    }
    else{
        return(1);
    }
    
    
    return(0);
}

int valueOfToken(char token[MAX_WORD_LENGTH]){
    
    int ret, valid;
    
    ret = 5; //...........
         
  
    valid = checkIfInteger(token); //checkIfInteger(ret);
    if(valid){
        return(ret);
    }else{
        printf("Error. The variable %s doesn't contain integer data. Exiting.", token);
        return(1);
    }
    
}

int getLabelAdress(char token[MAX_WORD_LENGTH]){
    
    int pc;
    
    pc = 5;//..............
    
    return(pc);
}

int main(int argc, char *argv[] ){

    int c, i, j, k, 
        pc,
        totalLines,
    //    labelCounter,
        arg1,
        arg2,
        arg3;
    //int **args;
    char line[MAX_LINE_LENGTH];     
    char instruction[MAX_WORD_LENGTH],
         temp_token[MAX_WORD_LENGTH];
    char *temp_string;                  
    char temp_pr[MAX_PRINT_LENGTH];  //malloc??      //prosorini metabliti gia print instruction
    const char s[] = " \t\r\n\v\f"; //delimeter lines by whitespace
    const char p[] = "\"";          //delimeter for printing
    unsigned long hash;
    //unsigned char str[MAX_WORD_LENGTH]; //malloc    //prosorini metabliti gia hasing(label)
    
    //metrame grammes arxeious
    totalLines = countLines(argv[1]); 
    char token[totalLines][MAX_WORDS_PER_LINE][MAX_WORD_LENGTH]; 
    //unsigned int labels[totalLines][2]; //arxikopoihsh me /0 ???
    
	FILE* fp = fopen(argv[1], "r");
    if(fp == NULL) {
      perror("Error opening file");
      return(-1);
   }
   
   //arguments
   /*
    for(i=2; i <= argc; i++{
        *args[i-2] = argv[i]; 
    }*/
    
   /******initialize memory *******/
    memset(temp_pr, '\0', sizeof(temp_pr));
    memset(temp_token, '\0', sizeof(temp_token));
    for(j=0; j < totalLines; j++){
        memset(token[j], '\0', sizeof(token[j]));
        memset(labels[j], '\0', sizeof(labels[j]));
    }



/******************** POLY SHMANTIKO VHMA: PRE-COMPILE:p **********/
/* - Prepei na xoume apomonwsei ta LABELS, prin ginei to compile!!!
 * - An to 1o token kathe grammis einai LABEL, to swzoume...
 */


    pc = 0;
    labelCounter = 0;
    while (pc <= totalLines)  {
    	if ( fgets(line, 200, fp) == NULL )
		break;//endOfFile

      	temp_string = strtok(line, s);    
	if( (temp_token[0] == 'L') && ( strcmp(temp_string, "LOAD") != 0) ){
           		
		// hash label to store it in "labels" table
      		hash = 5381;
       		j=0;
       		while ((c = temp_token[j])){
              		hash = ((hash << 5) + hash) + c; // hash * 33 + c 
               		j++;
       		}
       		labels[labelCounter][0] = hash%(65534); //unsigned integer table
       		labels[labelCounter][1] = pc;
       		labelCounter++;		
	}

	pc++;
    }

   	
    fseek( fp, 0, SEEK_SET );
/******************** COMPILE **********************************************************/
    pc = 0; //program counter
    //labelCounter = 0;
    while (pc <= totalLines){ //for every line
        
        if (fgets(line, 200, fp) == NULL)
            break;  //end of file
             
        /***** TOKENS ON THE SAME LINE********/
        i = 0; //iteration between tokens
        temp_string = strtok(line, s);  // get the first token 
        while( temp_string != NULL ) {  // walk through other tokens on the same line  
            strcpy(temp_token, temp_string);
            
            //case "print"
            if(temp_token[0] == '\"'){    
                strcpy(temp_pr, strtok(NULL, p));
                
                for(j = 1; temp_token[j] != '\0'; ++j){ //delete-> "
                    temp_token[j-1] = temp_token[j];
                }
                temp_token[j-1] = ' ';
                for(k = 0; temp_pr[k] != '\0'; ++k, ++j){ //olokliri print se 1 token
                    temp_token[j] = temp_pr[k];
                }
                temp_token[j] = '\0';
            }
            
            //assing 
            strcpy(token[pc][i] ,temp_token);
            
            //next token on the line
            i++;
            temp_string = strtok(NULL, s);
        }
        
        
        /***** STORE LABEL OF THE LINE ******************/
        if((token[pc][0][0] == 'L') && (strcmp(token[pc][0], "LOAD") != 0)){
       
            //delete label from instruction
            for(j=1; token[pc][j][0] != '\0'; j++){
                strcpy(token[pc][j-1], token[pc][j]);
            }
            memset(token[pc][j-1], '\0', sizeof(token[pc][j]));        
        }
        
        
	if ( checkSyntax(token[pc]) == 1 ) {
		return 1;
	}

        //debugging print
        for(j=0; token[pc][j][0] != '\0'; j++){
            printf("%d: %s\n", j, token[pc][j]);
        }
        
        
        
        //next line
        pc++;
        printf("*****************New line****************\n");
    }

 
    if(strcmp(token[0][0], "#PROGRAM")){
	printf("it is %s\n", token[0][0]);
        printf("Wrong syntax at line 0. \"#PROGRAM\" is needed. Exiting\n");
        return(1);
    }

/********************** RUN **********************************************************/
return 0;

pc = 1;

while(1){
    
    strcpy(instruction, token[pc][0]);
    if(token[pc][1][0] != '\0' && token[pc][0][0] != 'D' && token[pc][0][0] != 'U' && token[pc][0][0] != 'P'){
        arg1 = valueOfToken(token[pc][1]);
        if(token[pc][2][0] != '\0'){
            arg2 = valueOfToken(token[pc][2]);
            if(token[pc][3][0] != '\0' && token[pc][0][0] != 'B')//oxi BRGE BRGE etc
                arg3 = valueOfToken(token[pc][3]); 
        }
    }
    
/*
    if(strcmp(instruction, "LOAD") == 0){
        //multithread prob
        //loc_val(token[pc][1] = arg2);
    }
    else if(strcmp(instruction, "STORE") == 0){
        //multithread prob
        //glob_val(token[pc][1] = arg2);
    }
    else if(strcmp(instruction, "SET") == 0){
        //loc_val(token[pc][1] = arg2);
    }
    else if(strcmp(instruction, "ADD") == 0){
        //loc_val(token[pc][1] = arg2 + arg3
    }
    else if(strcmp(instruction, "SUB") == 0){
        //loc_val(token[pc][1] = arg2 - arg3
    }
    else if(strcmp(instruction, "MUL") == 0){
        //loc_val(token[pc][1] = arg2 * arg3
    }
    else if(strcmp(instruction, "DIV") == 0){
        //loc_val(token[pc][1] = arg2 / arg3
    }
    else if(strcmp(instruction, "MOD") == 0){
        //loc_val(token[pc][1] = arg2 % arg3
    }
    else if(strcmp(instruction, "BRGT") == 0){
        if(arg1 > arg2){
            pc = getLabelAdress(token[pc][3]);
            continue;
        }
    }
    else if(strcmp(instruction, "BRGE") == 0){
        if(arg1 >= arg2){
            pc = getLabelAdress(token[pc][3]);
            continue;
        }
    }
    else if(strcmp(instruction, "BRLT") == 0){
        if(arg1 < arg2){
            pc = getLabelAdress(token[pc][3]);
            continue;
        }
    }
    else if(strcmp(instruction, "BRLE") == 0){
        if(arg1 <= arg2){
            pc = getLabelAdress(token[pc][3]);
            continue;
        }
    }
    else if(strcmp(instruction, "BREQ") == 0){
        if(arg1 == arg2){
            pc = getLabelAdress(token[pc][3]);
            continue;
        }
    }
    else if(strcmp(instruction, "BRA") == 0){
        pc = getLabelAdress(token[pc][3]);
        continue;
    }
    else if(strcmp(instruction, "DOWN") == 0){
        
    }
    else if(strcmp(instruction, "UP") == 0){
        
    }
    else if(strcmp(instruction, "SLEEP") == 0){
        
    }
    else if(strcmp(instruction, "PRINT") == 0){
        printf("%s ", token[pc][1]);
        for(j=2; token[pc][j][0] != '\0'; j++)
            printf("%d ", valueOfToken(token[pc][j]));
        printf("\n");
    }
    else if(strcmp(instruction, "RETURN") == 0){
        return(0);
    }
    else{
        printf("Uknown instruction: %s\n", instruction);
        break;
    }
    */
    
    
    
    pc++;
}







	return(1);
}
