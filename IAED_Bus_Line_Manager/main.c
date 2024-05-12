/* iaed-23 - Alexandre Ramos - ist1102598 - project2 - Bus Line Manager */
#include "proj2.h"

/*
*******************************************************************************
    The functions in this file are responsible for retrieving user
    commands, and calling the funcions responsible for parsing and handling
    the request.
*******************************************************************************
*/

/*
    Reads the user's command and calls the appropriate command handler
    As the start of the program, allocates a new Network to the heap,
    and a string to the stack for receiving user input safely 
*/
int main(){
    Network * network= newNetwork();
    char request[MAXINPUT+1];
    char c;
    while(scanf("%c", &c) && c!=EOF) /*doesnt consume white space*/
        switch (c) {
            case 'q' : freeNetwork(network); return 0;
            case 'c' : routesCommand(network, request); break;
            case 'p' : stopsCommands(network, request); break;
            case 'l' : linksCommands(network, request); break;
            case 'i' : intersections(network, request); break;
            case 'r' : removeRoute(network, request); break;
            case 'e' : removeStop(network, request); break;
            case 'a' : freeNetwork(network);
                        network= newNetwork();
                        break;
            case 'f' : showRoutesByLastStop(network, request); break;
        }
    return 0;
}



/*
    Reads an arguments provided by the user of up to maxSize chars into dest.
    If maxSize is 0, maxSize defaults to MAXINPUT-start, and dest size is
    assumed to be dynamic.
    Returns the index of the next char to be read from the user request
    Valid argument delimiters are ' ', '\t' and '"'
    Arguments must be separated by ' ' or '\t'  
    Commands must end with '\n'
*/
int readArgByIndex(char request[], int start, char *dest[], int maxSize){
    char c,space=' ', tab='\t', quote='"', newLine='\n';
    int inWS=0, inWord=1, inQuote=2, state=inWS;
    int i,j;
    int allocCount=0;

    if(request[start]==space||request[start]==tab||request[start]==newLine){

        if(maxSize==0) maxSize=MAXINPUT-start;

        *dest= malloc(sizeof(char)*MALLOC_INCREMENT);
        if(!*dest) return NO_MEMORY;
        allocCount++; 
        
        
        for(i=start, j=0, c=request[i]; c!='\0';i++, c=request[i]){
            
            if( j+1 > allocCount*MALLOC_INCREMENT)
                if(!(*dest=realloc(*dest, ++allocCount*MALLOC_INCREMENT))){
                    free(*dest);
                    return NO_MEMORY;
                }
            if(j>maxSize && !(state==inQuote && j==maxSize))
                break;

            switch (state){
                case 0:
                    if(c==space || c==tab) continue;
                    else if(c==newLine)
                        {(*dest)[j]='\0'; free(*dest); return N_A;}
                    else{
                        if(c==quote){state=inQuote; continue;}
                        state=inWord;
                        (*dest)[j]=c;
                        j++;
                    }
                    break;
            
                case 1:
                    if(c==space ||c==tab|| c==newLine){
                        (*dest)[j]='\0'; return i;
                    }
                    (*dest)[j]=c;
                    j++;
                    break;

                case 2:
                    if(c==quote){(*dest)[j]='\0'; return i+1;}
                    (*dest)[j]=c;
                    j++;
                    break;
            }
        }
    }

    if(allocCount) free(*dest);
    printf(""BAD_INPUT_FORMAT"");
    return BAD_INPUT;
}



/*
    Check a string for any non white space characters
*/
int noExtraArgs(char request[], int start){
    char check[2];
    if(sscanf(&request[start]," %1s ",check)!=EOF) 
        return FALSE;
    return TRUE;
}



/*
    Validates the "inverso" keyword and it's abbreviations
    The accepted formats are ^inv(e$|er$|ers$|erso$)
    Returns TRUE if the kw is valid, or prints an error and returns false
    otherwise
*/
int validateInvertKW(char *kw[]){
    int i; char * key = *kw;
    for(i=0;i<INVERT_KW_LEN;i++){
        if(INVERT_KW[i]!=key[i]){
            if(key[i]=='\0' && i>2) return TRUE;
            printf(""INVERT_KW_ERROR"");
            return FALSE;
        }
    }
    return TRUE;
}



/*
    Called when memory allocation fails, exits the program safelly
*/
void * noMemoryExit(Network * network){
    printf(""NO_MEMORY_ERROR"");
    freeNetwork(network);
    exit(0);
}