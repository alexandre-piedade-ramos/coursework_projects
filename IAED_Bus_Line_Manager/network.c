/* iaed-23 - Alexandre Ramos - ist1102598 - project2 - Bus Line Manager */
#include "proj2.h"

/*
*******************************************************************************
    Functions for Network
*******************************************************************************

    A Network aggregates all data for managing a bus line network.
    The functions in this file are responsible for creating and destroying
    networks, as well as adding routes, stops and links to a network, and 
    finding them.

    User input parsing assumes input formats and sizes limits are respected
*******************************************************************************
*/

/*
    Creates a new network
*/
Network * newNetwork(){

    Network * network = (Network*) malloc(sizeof(Network));
    if(!network)
        exit(0);

    network->_routes = (Route**) malloc(sizeof(Route*)*MALLOC_INCREMENT);
    if(!network->_routes){
        free(network);
        exit(0);
    }

    network->_stops =  (Stop**) malloc(sizeof(Stop*)*MALLOC_INCREMENT);
    if(!network->_stops){
        free(network->_routes);
        free(network);
        exit(0);
    }

    network->_links = (Link**) malloc(sizeof(Link*)*MALLOC_INCREMENT);
    if(!network->_links){
        free(network->_stops);
        free(network->_routes);
        free(network);
        exit(0);
    }

    network->_stopsSize=0;
    network->_linksSize=0;
    network->_routesSize=0;

    network->_routesMax=MALLOC_INCREMENT;
    network->_stopsMax=MALLOC_INCREMENT;
    network->_linksMax=MALLOC_INCREMENT;

    return network;
}

/*
    Frees all the dynamically allocated components of a network structure
    and the network itself
*/
void freeNetwork(Network * network){
    int i;
    for(i=0; i < network->_routesSize; i++)
        freeRoute(network->_routes[i]);

    for(i=0; i < network->_stopsSize; i++)
        freeStop(network->_stops[i]);

    for(i=0; i < network->_linksSize; i++)
        freeLink(network->_links[i]);
    
    free(network->_routes);
    free(network->_stops);
    free(network->_links);

    free(network);
}

/*
*******************************************************************************
    Functions for Routes in a network
*******************************************************************************
*/

/*
    Adds a route to a Network
*/
void addRoute(Route * r, Network * network){
    int _routesMax=network->_routesMax, _routesSize=network->_routesSize;

    if(_routesSize == _routesMax){
        if(!(network->_routes=  realloc(
                                network->_routes,
                                sizeof(Route*)*(_routesMax + MALLOC_INCREMENT)
                                )
            )){
                freeRoute(r);
                noMemoryExit(network);
            }
        else network->_routesMax+=MALLOC_INCREMENT;
    }

    network->_routes[network->_routesSize]=r;
    network->_routesSize+=1;
    return;
}

/*
    Finds a route in a Network by name and returns a pointer to the route.
    If the name isn't found returns NULL.
*/
Route * findRoute(char *name[], Network * network){
    int i;
    for(i=0; i<network->_routesSize;i++){
        if(!strcmp(network->_routes[i]->name, *name))
            return network->_routes[i];
    }
    return NULL;
}

/*
    Prints all routes in a Network to stdout
*/
void printRoutes(Network * network){
    int i;
    for(i=0; i<network->_routesSize;i++){
        if(strlen(network->_routes[i]->name) < 6){
        printRoute(network->_routes[i]);
        putchar('\n');
        }
    }
    return;
}

/*
    Receives the name of a route in the network, and removes said route from
    the network, and from any stops through which the route passes.
    Prints an error if the route name doesn't match any route in the network
*/
void removeRoute(Network * network, char * request){
    Route * r; Stop * s;
    int i,j, reqI=0;
    char *routeName= NULL;

    if(fgets(request ,MAXINPUT+1,stdin)==NULL) return;
    reqI=readArgByIndex(request, reqI, &routeName, 0);
    if(reqI>0 && !noExtraArgs(request, reqI)) return;
    
    if(reqI==NO_MEMORY){
        printf(""NO_MEMORY_ERROR"");
        freeNetwork(network);
        exit(0);
    }

    r=findRoute(&routeName, network);
    
    if(r==NULL){
        printf(""NO_SUCH_ROUTE_ERROR"", routeName );
        free(routeName);
        return;
    }
    

    free(routeName);

    for(i=1; i<=r->stopsNum; i++){
        s=getRouteStop(r, i);
        for(j=0; j< s->routesNum; j++){
            if(s->routes[j]==r){ /*era aqui o typo*/
                if(s->routesNum > j+1)
                    memmove(
                            s->routes+j,
                            s->routes+j+1,
                            (s->routesNum-j+1) * sizeof(Route *)
                        );

                s->routesNum--;
                break;
            }
        }
    }
    for(i=0; i<r->linksNum; i++)
        removeLink(r->links[i], network);

    for(i=0; i< network->_routesSize;i++)
        if(network->_routes[i]==r)
            if(network->_routesSize>(i+1)){

                memmove(
                        network->_routes+i,
                        network->_routes+i+1,
                        (network->_routesSize-(i+1)) * sizeof(Route *)
                    );
                /*copies *(_routes+i) into *(_routes),
                so that [A,B] => [B,B]
                memmove(
                    network->_routes[i],
                    network->_routes[i+1],
                    (network->_routesSize-(i+1)) * sizeof(Route *)
                );
                Would copy **(_routes+1) into **(_routes)
                so that [A,B] => [A,B] where A and B
                point to the same block of memory
                In which case free(A) and free(B) would
                have the same effect
                */
                break;
            } 


    network->_routesSize--;

    freeRoute(r);    
}

/*
*******************************************************************************
    Functions for Stops in a network
*******************************************************************************
*/


void addStop(Stop * stop, Network * network){
    int _stopsMax=network->_stopsMax, _stopsSize=network->_stopsSize;
    if(_stopsSize == _stopsMax){
        if(!(network->_stops=realloc(network->_stops, sizeof(Link*)*(_stopsMax + MALLOC_INCREMENT)))){
                    freeStop(stop);
                    noMemoryExit(network);
                }
        else network->_stopsMax+=MALLOC_INCREMENT;
    }
    network->_stops[network->_stopsSize]=stop;
    network->_stopsSize+=1;
    return;
}
/*
    Finds a Stop in a Network by name and returns a pointer to the Stop.
    If the name isn't found returns NULL.
*/
Stop * findStop(char *name[], Network * network){
    int i;
    for(i=0; i<network->_stopsSize;i++){
        if(!strcmp(network->_stops[i]->name, *name)) 
            return network->_stops[i];
    }
    return NULL;
}

/*
    Prints all stops in the network to stdout
*/
void printStops(Network * network){
    int i;
    for(i=0; i<network->_stopsSize; i++){
        printf("%s: ", network->_stops[i]->name);
        printStopLocation(network->_stops[i]);
        printf(" %d\n", network->_stops[i]->routesNum);
    }
    return;
}

/*
    Prints each Stop in a Network through which multiple routes pass, along with 
    the respectives route's names
*/
void intersections(Network * network, char request[]){
    int i,j;
    Stop * stop;

    if(fgets(request ,MAXINPUT+1,stdin)==NULL) return;
    if(!noExtraArgs(request, 0)) return;

    for(i=0; i<network->_stopsSize;i++){
        stop=network->_stops[i];

        if(stop->routesNum > 1){

            printf("%s %d: ", stop->name, stop->routesNum );

            for(j=0; j < (stop->routesNum - 1); j++){
                printf("%s ", stop->routes[j]->name );
            }
            printf("%s\n", stop->routes[j]->name );
        }
    }
}

/*
    Receives the name of a stop in the network, and removes said stop from
    the network, as well as from any links from or to the stop, either by
    removing them from the network if they are the first along their route's
    path, or by collapsing themselves and the previous link into a single link.
    
    Prints an error if the route name doesn't match any route in the network
*/
void removeStop(Network * network, char * request){
    Route * r; Stop * s; Link * l;
    int i,j, reqI=0;
    char *stopName=NULL;

    if(fgets(request ,MAXINPUT+1,stdin)==NULL) return;
    reqI=readArgByIndex(request, reqI, &stopName, 0);
    if(reqI>0 && !noExtraArgs(request, reqI)) return;

    if(reqI==NO_MEMORY){
        printf(""NO_MEMORY_ERROR"");
        freeNetwork(network);
        exit(0);
    }

    s=findStop(&stopName, network);
    if(s==NULL){
        printf(""NO_SUCH_STOP_ERROR"", stopName );
        free(stopName);
        return;
    }

    free(stopName);
    
    for(i=0; i<s->routesNum; i++){
        r = s->routes[i];

        while(r->linksNum > 0 && r->links[0]->first == s){
            l=r->links[0];
            memmove( r->links, r->links+1, (r->linksNum - 1)*sizeof(Link*));        
            if(r->linksNum==1) r->stopsNum--;
            r->stopsNum--;
            r->linksNum--;
            r->costTotal-= l->cost;
            r->timeTotal-= l->time;
            removeLink(l, network);
        }

        if(r->linksNum > 0 &&r->links[r->linksNum-1]->last == s){
            r->costTotal-= r->links[r->linksNum-1]->cost;
            r->timeTotal-= r->links[r->linksNum-1]->time;
            removeLink(r->links[r->linksNum-1], network);
            if(r->linksNum==1) r->stopsNum--;
            r->stopsNum--;
            r->linksNum--;
        }
   
            for(j=1; j<r->linksNum; j++){
            
                if(r->links[j]->first==s){
                    l=r->links[j];
                    r->links[j-1]->last= l->last;
                    r->links[j-1]->cost+=l->cost;
                    r->links[j-1]->time+=l->time;
                    removeLink(l, network);
                    memmove(
                        r->links+j,
                        r->links+j+1,
                        (r->linksNum-(j+1)) * sizeof(Link *)
                    );
                    if(r->linksNum==1) r->stopsNum--;
                    r->stopsNum--;
                    r->linksNum--;
                }
            }
    }
        

    for(i=0; i< network->_stopsSize;i++)
        if(network->_stops[i]==s)
            if(network->_stopsSize>(i+1)){
                memmove(
                        network->_stops+i,
                        network->_stops+i+1,
                        (network->_stopsSize-(i+1)) * sizeof(Stop *)
                    );
                break;
            } 


    network->_stopsSize--;
    
    freeStop(s);

}

/*
*******************************************************************************
    Functions for Stops in a network
*******************************************************************************
*/

/*
    Adds a link to the network, and calls for adding the link to the route
    to which it belongs, and the route to the stops in the link.
    Is responsible for reallocating a route's links components, or a stops
    routes component, if size limits were to be excided with the links 
    addition to the network
*/
void addLink(Link * l, Network * network){
    int _linksMax=network->_linksMax, _linksSize=network->_linksSize;
    if(_linksSize == _linksMax){
        if(!(network->_links=realloc(
                                    network->_links,
                                    sizeof(Link*)*(_linksMax+MALLOC_INCREMENT)
                            )
            )){
                    freeLink(l);
                    noMemoryExit(network);
            }
        else network->_linksMax+=MALLOC_INCREMENT;
    }
    network->_links[network->_linksSize]=l;

    if(l->r->linksNum == l->r->linksMax){
        if(!(l->r->links= (Link **) realloc(
                            l->r->links,
                            sizeof(Link*)*(l->r->linksMax + MALLOC_INCREMENT)
                        )
            )){
                freeLink(l);
                noMemoryExit(network);
            }
        else l->r->linksMax+=MALLOC_INCREMENT;
        }

    if(l->first->routesNum == l->first->routesMax){
        if(!(l->first->routes= (Route **) realloc(
                                    l->first->routes,
                        sizeof(Route*)*(l->first->routesMax+ MALLOC_INCREMENT))
            )){
                freeLink(l);
                noMemoryExit(network);
            }
        else l->first->routesMax+= MALLOC_INCREMENT;
        }

    if(l->last->routesNum == l->last->routesMax){
        if(!(l->last->routes=
                (Route **) realloc(
                            l->last->routes,
                        sizeof(Route*)*(l->last->routesMax+ MALLOC_INCREMENT)
                    )
            )){
                freeLink(l);
                noMemoryExit(network);
            }
        else l->last->routesMax+= MALLOC_INCREMENT;
        }
  
    insertLinkIntoRoute( l->r, l);
    network->_linksSize+=1;
    return;
}

/*
    Removes a link from the network
*/
void removeLink(Link * l, Network * network){
    int i;
    for(i=0; i< network->_linksSize;i++)
        if(network->_links[i]==l)
            if(network->_linksSize>(i+1)){
                memmove(
                        network->_links+i,
                        network->_links+i+1,
                        (network->_linksSize-(i+1)) * sizeof(Link *)
                    );
                break;
            } 
    freeLink(l);
    network->_linksSize--;
}

