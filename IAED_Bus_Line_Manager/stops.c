/* iaed-23 - Alexandre Ramos - ist1102598 - project2 - Bus Line Manager */
#include "proj2.h"
/*
*******************************************************************************
    Functions for Stops
*******************************************************************************

    Functions to handle stops in a Network.
    User input parsing assumes input formats and sizes limits are respected
*******************************************************************************
*/



/*
    Handles the 'p' command at it's arguments
*/
int stopsCommands(Network * network, char * request){
    
    char *stopName=NULL;
    Coordinates coord; 
    short caseVal=0;
    Stop * s;

    caseVal=stopArgsParser(request, &stopName, &coord);

    switch (caseVal){
        case BAD_INPUT :
            return BAD_INPUT;
        case NO_MEMORY :
            noMemoryExit(network);
            break;
        case 1 :
            s=findStop(&stopName, network);
            if(!s) printf(""NO_SUCH_STOP_ERROR"",stopName);
            else{
                printStopLocation(s);
                putchar('\n');
            }
            free(stopName);
            break;
        case 2 : 
            if(findStop(&stopName, network)){
                printf(""STOP_DUPLICATE_ERROR"",stopName);
                free(stopName);
                break;
            }
            if(coord.lat>90 || coord.lat<(-90) || coord.lon<(-180) || coord.lon>180){
                printf(""BAD_LOC_ERROR"");
                free(stopName);
                break;
            }

            s=newStop(&stopName, coord, network);
            addStop(s, network);
            break;
        default : 
            printStops(network);
            break;
    }

    return 0;
}



/*
    Parses a string for the arguments allowed for the 'c' command
*/
int stopArgsParser(char request[], char *stopName[], Coordinates * coord){
    int argsRead=0, reqI=0;
    if(fgets(request ,MAXINPUT+1,stdin)==NULL) return BAD_INPUT;

    reqI=readArgByIndex(request, reqI, stopName, 0);
    if(reqI==NO_MEMORY) return NO_MEMORY;
    if(reqI==BAD_INPUT) return BAD_INPUT;

    if(reqI!=N_A){
        argsRead++;
        if(2==sscanf(&request[reqI],"%lf %lf",&coord->lat, &coord->lon))
            argsRead++;
        else if(!noExtraArgs(request, reqI)){free(stopName); reqI=BAD_INPUT;}
    }
    
    return argsRead;
}



/*
    Prints a Stop's location to stdout
    Doesnt print '\n'
*/
void printStopLocation(Stop * s){
    printf("%"COORDINATES_PRINT_FORMAT" %"COORDINATES_PRINT_FORMAT"", 
            s->location.lat, s->location.lon
    );
}



/*
    Creates a new Stop, doesn't add it to the network
*/
Stop * newStop(char *name[], Coordinates loc, Network * network){
    Stop * newStop = (Stop*) malloc(sizeof(Stop));
    if(!newStop){
        free(*name);
        noMemoryExit(network);
    }
    newStop->name=*name;
    newStop->location=loc;
    newStop->routesNum=0;
    newStop->routes = (Route **) malloc(sizeof(Route*)*MALLOC_INCREMENT);
    if(!newStop->routes){
        free(*name);
        free(newStop);
        noMemoryExit(network);
    }
    newStop->routesMax=MALLOC_INCREMENT;
    return newStop;
}




/*
    Finds where a Route should be inserted in the Stop's alphanumerically routes
    Returns the position for insertion if the Route isn't a duplicate,
    and DUPLICATE otherwise
*/
int findStopRoutePosition(Stop * s, char name[]){
    /*
    strcmp compares s1 and s2 index by index
    strcmp returns 0 if s1 and s2 are equal, >0 if s1>s2 or <0 if s2>s1
    */
    int routesNum=s->routesNum;
    int max=routesNum-1, min=0, cmpVal;
    int pivot= (max+min)/2;

    if(routesNum==0) return 0;

    while(max>=min){
        cmpVal=strcmp(name, s->routes[pivot]->name);
        if(cmpVal==0) return DUPLICATE;
        if(cmpVal>0)min=pivot+1;
        else max=pivot-1;
        pivot=(max+min)/2;
    }
    cmpVal=strcmp(name,s->routes[pivot]->name);
    if(cmpVal==0) return DUPLICATE;
    if(cmpVal>0) pivot++;

    return pivot;
}



/*
    Adds a new Route to a Stop's routes, or does nothing if the Route is
    already there
*/
void addStopRoute(Route * r, Stop * s){
    int insertionIndex, routesNum=s->routesNum, i;

    if((insertionIndex=findStopRoutePosition(s, r->name))!=DUPLICATE){
        if(routesNum>0 && insertionIndex!=(routesNum)){
            for(i=routesNum-1; i>=insertionIndex;i--){ /*TESTAR COM memmove*/
                s->routes[i+1]=s->routes[i];
            }
        }
        s->routes[insertionIndex]=r;
        s->routesNum++;
    }
}



/*
    Frees all the dynamically allocated components of a stop structure
    and the stop itself
*/
void freeStop(Stop * stop){
    free(stop->name);
    free(stop->routes);
    free(stop);
}