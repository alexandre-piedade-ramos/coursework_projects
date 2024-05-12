/* iaed-23 - Alexandre Ramos - ist1102598 - project2 - Bus Line Manager */
#include "proj2.h"
/*
*******************************************************************************
    Functions for Routes
*******************************************************************************

    Functions to handle routes in a Network.
    User input parsing assumes input formats and sizes limits are respected
*******************************************************************************
*/



/*
    Handles the 'c' command at it's arguments
*/
int routesCommand(Network * network, char * request){
    char *routeName=NULL;
    char *invertKW=NULL;
    Route * r;
    int caseVal;

    caseVal=routeArgsParser( request, &routeName, &invertKW);
    switch (caseVal){ 
        case BAD_INPUT :
            return BAD_INPUT;
        case NO_MEMORY :
            noMemoryExit(network);
            break;
        case 1 : 
            r=findRoute(&routeName, network);
            if(r){
                free(routeName);
                printRouteStops(r, FALSE);
            }else{
                r=newRoute(&routeName, network);
                addRoute(r, network);
            }
            break;
        case 2 :
            if(validateInvertKW(&invertKW)){
                r=findRoute(&routeName, network);
                printRouteStops(r, TRUE);
            }
            free(invertKW);
            free(routeName);
            break;
        default : 
            printRoutes(network);
            break;
    }

    return 0;
}



/*
    Parses a string for the arguments allowed for the 'c' command
*/
int routeArgsParser(char request[], char *routeName[], char *invertKW[]){
    int argsRead=0, reqI=0;

    if(fgets(request ,MAXINPUT+1,stdin)==NULL) return BAD_INPUT;

    reqI=readArgByIndex(request, reqI, routeName, 0);
    if(reqI==NO_MEMORY) return NO_MEMORY;
    if(reqI==BAD_INPUT) return BAD_INPUT;

    if(reqI!=N_A){
        argsRead++;
        reqI=readArgByIndex(request, reqI, invertKW, INVERT_KW_LEN-1);

        if(reqI==NO_MEMORY){free(*routeName);return NO_MEMORY;}
        if(reqI==BAD_INPUT){free(*routeName);return BAD_INPUT;}

        if(reqI!=N_A){
            argsRead++;
            if(!noExtraArgs(request, reqI)){
                free(*routeName);
                free(*invertKW);
                return BAD_INPUT;}
        }
    }
    
    return argsRead;
}



/*
    Prints a route to stdout
*/
void printRoute(Route * r){

    printf("%s ", r->name);

    if(r->stopsNum>0){
        printf("%s %s ", 
            getRouteStop(r, 1)->name,
            getRouteStop(r, r->stopsNum)->name
        );
    }

    printf("%d %.2f %.2f",
            r->stopsNum, 
            r->costTotal, 
            r->timeTotal
        );

    return;
}



/*
    Returns the n-th Stop of the route, s in range [1,stopsNum]
    Returns Null if the route has no stops
*/
Stop * getRouteStop(Route * r, int nthStop){
    if(r->stopsNum<nthStop) return NULL;
    if(nthStop<=2) return getStopFromLink(r->links[0], nthStop-1);
    return getStopFromLink(r->links[nthStop-2], TRUE);
}



/*
    Prints a route's stops to stdout, by the order in which they are traversed
*/
void printRouteStops(Route * r, int reverse){
    int i=1,order=1, limit=r->stopsNum;

    if(r->stopsNum==0) return;

    if(reverse){
        order=-1;
        i=-1*r->stopsNum;
        limit=-1;
    }

    for(; i<limit; i++){
        printf("%s, ", getRouteStop(r, order*i)->name);
    }
    printf("%s\n", getRouteStop(r, order*i)->name);
    return;
}



/*
    Creates a new route
    Doesn't add it to a Network
*/
Route * newRoute(char *name[], Network * network){
    Route * newRoute = (Route*) malloc(sizeof(Route));
    if(!newRoute){
        free(*name);
        noMemoryExit(network);
    }
    newRoute->name=*name;
    newRoute->stopsNum=0;
    newRoute->linksNum=0;
    newRoute->costTotal=0;
    newRoute->timeTotal=0;
    newRoute->linksMax=MALLOC_INCREMENT;
    newRoute->links = (Link **) malloc(sizeof(Link*)*MALLOC_INCREMENT);
    if(!newRoute->links){
        free(*name);
        free(newRoute);
        noMemoryExit(network);
    }

    return newRoute;
}



/*
    Inserts a Link into a route
    Links must be validated previously
    Also calls the function for adding the route to the Stops' routes
*/
void insertLinkIntoRoute(Route * r, Link * l){
    int linksNum = r->linksNum;
    int stopsNum = r->stopsNum;
    int i;
    Stop * firstStop, * lastStop;


    if(linksNum==0){
        r->links[0]=l;
        r->stopsNum+=1;
    }
    else{

        firstStop=getRouteStop(r, 1);
        lastStop=getRouteStop(r, stopsNum);

        if(l->first==lastStop && l->last==firstStop){
            /*Rota circular*/
            r->links[linksNum]=l;
        }
        else if(l->first==lastStop){
            r->links[linksNum]=l;
        }
        else if(l->last==firstStop){
            for(i=r->linksNum-1; i>=0;i--)
                r->links[i+1]=r->links[i];

            r->links[0]=l;     
        }
    }

    r->stopsNum+=1;
    r->linksNum+=1;

    r->costTotal+=l->cost;
    r->timeTotal+=l->time;

    addStopRoute(r, l->first);
    addStopRoute(r, l->last);
    return;
}



/*
    Frees all the dynamically allocated components of a route structure
    and the route itself
*/
void freeRoute(Route * route){
    free(route->name);
    free(route->links);
    free(route);
}

void showRoutesByLastStop(Network * network, char * request){
    int reqI=0, i, found=FALSE;
    char * stopName=NULL;
    Stop * stop;
    if(fgets(request ,MAXINPUT+1,stdin)==NULL) return;

    reqI=readArgByIndex(request, reqI, &stopName, 0);
    if(reqI==NO_MEMORY){
        free(stopName);
        noMemoryExit(network);
        return;
    }
    if(reqI==BAD_INPUT){
        free(stopName);
        printf(""BAD_INPUT_FORMAT"");
        return;
    }
    stop=findStop(&stopName, network);
    if(!stop){
                printf(""NO_SUCH_STOP_ERROR"",stopName);
                free(stopName);
                return;
            }
    free(stopName);
    for(i=0; i<network->_routesSize; i++){
            if(network->_routes[i]->linksNum>0)
                if(stop==getRouteStop(network->_routes[i], network->_routes[i]->stopsNum)){
                    found=TRUE;
                    printf("%s ", network->_routes[i]->name);
                }
    }
    if(found) putchar('\n');

    return;
}