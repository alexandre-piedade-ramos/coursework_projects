/* iaed-23 - Alexandre Ramos - ist1102598 - project2 - Bus Line Manager */
#include "proj2.h"

/*
*******************************************************************************
    Functions for Links
*******************************************************************************

    Functions to handle links in a Network.
    User input parsing assumes input formats and sizes limits are respected
*******************************************************************************
*/



/*
    Handles the 'l' command at it's arguments
*/
int linksCommands(Network * network, char * request){
    char *routeName=NULL, *firstStopName=NULL, *lastStopName=NULL;
    double cost, time;
    short caseVal=0;
    Link * l;
    Route * r;
    Stop * firstStop, * lastStop;

    caseVal=linkArgsParser(
                request,&routeName,
                &firstStopName, &lastStopName,
                &cost, &time
            );

    if(caseVal== BAD_INPUT){
        printf(""BAD_INPUT_FORMAT"");
        return BAD_INPUT;
    }

    if(caseVal==NO_MEMORY)
        noMemoryExit(network);
    
    
    r=findRoute(&routeName, network);
    firstStop=findStop(&firstStopName, network);
    lastStop=findStop(&lastStopName, network);

    if(validateLink(
            &routeName,
            &firstStopName, &lastStopName,
            cost, time,
            r, firstStop, lastStop
            )
        ){
            l=newLink(r, firstStop, lastStop, cost, time, network);
            addLink(l, network);
    }

    free(routeName);
    free(firstStopName);
    free(lastStopName);
    return 0;
}



/*
Parses a string for the arguments allowed for the 'l' command
*/
int linkArgsParser(char request[], char *routeName[], char *firstStop[], 
    char *lastStop[], double * cost, double * time)
    {
        int argsRead=0, reqI=0, n;

        if(fgets(request ,MAXINPUT+1,stdin)==NULL) return BAD_INPUT;

        reqI=readArgByIndex(request, reqI, routeName, 0);
        if(reqI==NO_MEMORY) return NO_MEMORY;
        if(reqI==BAD_INPUT) return BAD_INPUT;

        argsRead++;
        reqI=readArgByIndex(request, reqI, firstStop, 0);
        if(reqI==NO_MEMORY){free(*routeName); return NO_MEMORY;}
        if(reqI==BAD_INPUT){free(*routeName); return BAD_INPUT;}

        argsRead++;
        reqI=readArgByIndex(request, reqI, lastStop, 0);
        if(reqI==NO_MEMORY){free(*routeName);free(*firstStop);return NO_MEMORY;}
        if(reqI==BAD_INPUT){free(*routeName);free(*firstStop);return BAD_INPUT;}

        argsRead++;
        if(2!=sscanf(&request[reqI],"%lf %lf%n",cost, time, &n)){ 
            /*%n doesnt count for the return value of sscanf*/
            free(*routeName);
            free(*firstStop);
            free(*lastStop);
            return BAD_INPUT;
        }
        reqI+=n;
        argsRead+=2;

        if(!noExtraArgs(request, reqI)){
            free(*routeName);
            free(*firstStop);
            free(*lastStop);
            return BAD_INPUT;
        }

        return argsRead;
}



/*
    Validates the user request for creating a new Link, and displays
    the appropriate errors if the request in invalid
*/
int validateLink(char *routeName[], char *firstStopName[], char *lastStopName[],
    double cost, double time, Route * r, Stop * firstStop, Stop * lastStop)
    {   
        
        if(r==NULL){
            printf(""NO_SUCH_ROUTE_ERROR"",*routeName);
            return 0;
        }
        if(firstStop==NULL){
            printf(""NO_SUCH_STOP_ERROR"",*firstStopName);
            return 0;
        }
        if(lastStop==NULL){
            printf(""NO_SUCH_STOP_ERROR"",*lastStopName);
            return 0;
        }
        if(r->stopsNum>0){
            firstStop=getRouteStop(r, 1);
            lastStop=getRouteStop(r, r->stopsNum);

            if(strcmp(firstStop->name, *lastStopName )!=0
                && strcmp(lastStop->name,*firstStopName )!=0)
                    {
                        printf(""BAD_LINK_ERROR"");
                        return 0;
            }
        }
        if(cost<0 || time<0){
            printf(""BAD_COST_OR_TIME_ERROR"");
            return 0;
        }
        return 1;
    }




/*
    Returns a link's first or last stop
*/
Stop * getStopFromLink(Link * l, int last){
    if(last) return l->last;
    return l->first;
}




/*
    Creates a new Link, doesn't add it to the network
*/
Link * newLink(Route * r, Stop * firstStop, Stop * lastStop,
    double cost, double time, Network * network)
    {
        Link * newLink = (Link *) malloc(sizeof(Link));
        if(!newLink) noMemoryExit(network);

        newLink->r=r;
        newLink->first=firstStop;
        newLink->last=lastStop;
        newLink->cost=cost;
        newLink->time=time;
        return newLink;
}




/*
    Frees all the dynamically allocated components of a link structure
    and the link itself
*/
void freeLink(Link * link){
    free(link);
}
