#ifndef HEADER_FILE
#define HEADER_FILE


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXINPUT 65535 /*Maximum size of stdin buffer*/ /*ALTERAR*/

#define INVERT_KW_LEN 8 /*size of the "inverso" keyword + null, doubles to try for a remote test*/
#define INVERT_KW "inverso\0" /*"inverso" keyword*/

#define MALLOC_INCREMENT 32

#define NO_MEMORY -4
#define DUPLICATE -3
#define BAD_INPUT -2 
#define N_A -1
#define TRUE 1
#define FALSE 0

#define COORDINATES_PRINT_FORMAT "16.12f"

/*Error messages*/
#define INVERT_KW_ERROR "incorrect sort option.\n"
#define STOP_DUPLICATE_ERROR "%s: stop already exists.\n"
#define NO_SUCH_STOP_ERROR "%s: no such stop.\n"
#define NO_SUCH_ROUTE_ERROR "%s: no such line.\n"
#define BAD_LINK_ERROR "link cannot be associated with bus line.\n"
#define BAD_COST_OR_TIME_ERROR "negative cost or duration.\n"
#define BAD_INPUT_FORMAT "input parameters not respected.\n"
#define NO_MEMORY_ERROR "No memory.\n"
#define BAD_LOC_ERROR "invalid location.\n"

/*
    Defines the coordinate structure by latitude (lat) and longitude (lon)

*/
typedef struct coordinates {
    double lat;
    double lon;
}Coordinates;

/*
    Defines the Stop structure
*/
typedef struct stop{
    char *name;
    unsigned short routesNum;
    /*
        number of routes which pass through this Stop
    */
    int routesMax;
    /*
        number of routes which may pass through this Stop before stops.routes
        memory reallocation
    */
    struct route ** routes; 
    /*
        An array of pointers to the routes which pass through this Stop,
        sorted alphanumerically by Route name
    */
    struct coordinates location;
}Stop;

/*
    Defines the Route structure
*/
typedef struct route{ 
    char * name;
    int stopsNum;
    /*number of stops through which this route passes*/
    int linksNum;
    /*number of links this route travels along*/
    int linksMax;
    /*
        number of links which this route may travel along before route.links
        memory reallocation
    */
    struct link ** links;
    /*
        An array of pointers to the links along this Route, 
        by the order in which they are travelled
    */
    double costTotal;
    double timeTotal;
}Route;

/*
    Defines the Link structure, for links between stops along a Route
*/
typedef struct link{
    struct route * r;
    /*the route which travels this link*/
    struct stop * first;
    /*the origin of this link*/
    struct stop * last;
    /*the destination of this link*/
    double cost;
    double time;
}Link;

typedef struct network{
    /*network Arrays*/

    Stop ** _stops;
    Link ** _links;
    Route ** _routes;

    /*network Arrays' sizes*/

    int _stopsSize;
    int _linksSize;
    int _routesSize;

    int _stopsMax;
    int _linksMax;
    int _routesMax;

}Network;

/*
*******************************************************************************
    Prototypes
*******************************************************************************
*/

/*main.c prototypes*/
int main();
int readArgByIndex(char request[], int start, char *dest[], int maxSize);
int noExtraArgs(char request[], int start);
int validateInvertKW(char *kw[]);
void * noMemoryExit(Network * network);


/*network.c prototypes*/
Network * newNetwork();
void freeNetwork(Network * network);

void printRoutes(Network * network);
Route * findRoute(char *name[], Network * network);
void addRoute(Route * route, Network * network);
void printRoutes(Network * network);
void removeRoute(Network * network, char * request);

void printStops(Network *network);
void addStop(Stop * stop, Network * network);
Stop * findStop(char *name[], Network * network);
void intersections(Network * network, char request[]);
void removeStop(Network * network,char * request);

void addLink(Link * link, Network * network);
void removeLink(Link * link, Network * network);

/*routes.c prototypes*/
int routesCommand(Network * network, char * request);
int routeArgsParser(char request[], char *routeName[], char *invertKW[]);
void printRoute(Route * route);
void printRouteStops(Route * route, int order);
Stop * getRouteStop(Route * route, int nthStop);
Route * newRoute(char *name[], Network * network);
void insertLinkIntoRoute(Route * route, Link * link);
void freeRoute(Route * route);
void showRoutesByLastStop(Network * network, char * request);

/*stops.c prototypes*/
int stopsCommands(Network * network, char * request);
int stopArgsParser(char request[], char *stopName[], Coordinates * coord);
void printStopLocation(Stop * stop);
Stop * newStop(char *name[], Coordinates loc, Network * network);
int findStopRoutePosition(Stop * stop, char routeName[]);
void addStopRoute(Route * route, Stop * stop);
void freeStop(Stop * stop);


/*link.c prototypes*/
int linksCommands(Network * network, char * request);
int linkArgsParser(char request[], char *routeName[], char *firstStop[], 
    char *lastStop[], double * cost, double * time);
Stop * getStopFromLink(Link * l, int first);
int validateLink(char *routeName[], char *firstStopName[], char *lastStopName[],
    double cost, double time, Route * r, Stop * firstStop, Stop * lastStop);
Link * newLink(Route * r, Stop * firstStop, Stop * lastStop,
    double cost, double time, Network * network);
void freeLink(Link * link);
#endif