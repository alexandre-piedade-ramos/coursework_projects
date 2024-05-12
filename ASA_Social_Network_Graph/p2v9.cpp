#include <iostream>
#include <vector>
using namespace std;

struct vertex{
    int id;
    int weight;
    int index;
    int lowlink;
    bool onTarjanStack;
    //id of scc this vertex belongs to
    int belongsToSCC;
    vector<int> * out;
    vector<int> * in;
};

struct tarjanState{
    int head;
    long unsigned int it;
    bool inCond;
    bool stateStackEmpty;
};

void getArgs();
void tarjanSCC();
tarjanState tarjanVisit(int v, int * index, long unsigned int it, bool inCond);
void newSCC(vector<int> idsForNewSCC);
void buildSCCNodesAdjTable();
void longestPath();
vector<vertex> nodes;
long unsigned int nodesNum;

vector<tarjanState> tarjanStates;
vector<int> tarjanStack;

//keep in mind sssNodes[0] is the last on topo order
vector<vertex *> sccNodes;

int numEdgesG;
int main(){
    std::ios::sync_with_stdio(false);
    getArgs();
    tarjanSCC();
    buildSCCNodesAdjTable();
    longestPath();
    return 0;
}

void getArgs(){
    int newVertexId, newAdjVertexId;
    int numEdges;
    //scanf("%lu %d", &nodesNum, &numEdges);
    cin >> nodesNum;
    cin >> numEdges;
    numEdgesG=numEdges;
    nodes.resize(nodesNum+1); //+1 so id:1 is in nodes[1]
    for(long unsigned int i=1; i<=nodesNum;i++){
        nodes[i].id=i;
        nodes[i].weight=1;
        nodes[i].index=-1;
        nodes[i].lowlink=-1;
        nodes[i].onTarjanStack=false;
        nodes[i].belongsToSCC=-1;
        nodes[i].out = new vector<int>();
        nodes[i].in = new vector<int>();
    }

    for(int i=0; i<numEdges;i++){
        //scanf("%d %d",&newVertexId, &newAdjVertexId);
        cin >> newVertexId;
        cin >> newAdjVertexId;
        nodes[newVertexId].out->push_back(newAdjVertexId);
        nodes[newAdjVertexId].in->push_back(newVertexId);
    }

}


void tarjanSCC(){
    int index=0;
    long unsigned int i=1;

    tarjanState nextCall;

    nextCall.head=nodes[i].id;
    nextCall.it=0;
    nextCall.inCond=false;
    i++;
    

    do{ 
        //printf("head: %d, it: %d, inCond: %d, ssEmpty: %d\n", nextCall.head,*nextCall.it,nextCall.inCond,nextCall.stateStackEmpty);
        nextCall = tarjanVisit(nextCall.head, &index, nextCall.it,nextCall.inCond);
        
        if(!(tarjanStack.size()==0))
            continue;

        for(; i<=nodesNum; i++){
            if(nodes[i].index == -1){
                nextCall.head=nodes[i].id;
                nextCall.it=0;
                nextCall.inCond=false;
                break;
            }
        }
        
    }while(i<=nodesNum);
}

tarjanState tarjanVisit(int v, int * index, long unsigned int it, bool inCond){
    tarjanState thisCallState;
    tarjanState nextCallState;
    
    
    //cout << v << '\n';
    if(!nodes[v].onTarjanStack){

        nodes[v].index=*index;
        nodes[v].lowlink=*index;
        (*index)++;
        tarjanStack.push_back(v);
        nodes[v].onTarjanStack=true;
    } 


    if(inCond){
        nodes[v].lowlink = min(nodes[v].lowlink, nodes[nodes[v].out->at(it)].lowlink);
        it++;
        tarjanStates.pop_back();
    }


    if(nodes[v].out->size()>0){ 
        for(; it < nodes[v].out->size();it++){
            //if j.index is undefined
            int j=nodes[v].out->at(it);
            if(nodes[j].index==-1){
                nextCallState.head=j;
                nextCallState.it = 0;
                nextCallState.inCond=false;

                thisCallState.head=v;
                thisCallState.it=it;
                thisCallState.inCond=true;
                tarjanStates.push_back(thisCallState);
                return nextCallState;
                //nodes[v].lowlink = min(nodes[v].lowlink, nodes[*it].lowlink);
            }
            else if(nodes[j].onTarjanStack){
                nodes[v].lowlink=min(nodes[v].lowlink, nodes[j].index);
            }
        }
    }

    if(nodes[v].index==nodes[v].lowlink){
        int j;
        vector<int> idsForNewSCC;
        do{
            j=tarjanStack.back();
            tarjanStack.pop_back();
            nodes[j].onTarjanStack=false;
            idsForNewSCC.push_back(j);
        }while(v!=j);

        newSCC(idsForNewSCC);
    }
    //if(inCond) tarjanStates.pop_back(); //pop self once done
    if(tarjanStates.size()==0){
        nextCallState.stateStackEmpty=true;
        return nextCallState;
    }
    return tarjanStates.back(); //undefined behaviour on calling with empty state stack
}

//newSCCs are created in reverse topo order, and form a DAG
void newSCC(vector<int> idsForNewSCC ){
    if((idsForNewSCC.size()==1) && 
        (nodes[idsForNewSCC.back()].out->size()==0) && 
        (nodes[idsForNewSCC.back()].in->size()==0)
    ) return;
    

    vertex * scc = new vertex;
    scc->id=sccNodes.size() + 1 ;
    scc->weight=1;
    scc->index=-1;
    scc->lowlink=-1;
    scc->onTarjanStack=false;
    scc->belongsToSCC=-1;
    scc->out = new vector<int>();
    scc->in = new vector<int>();

    for(int i : idsForNewSCC)
        nodes[i].belongsToSCC=scc->id;


    sccNodes.push_back(scc);
    /*
    for(int i : idsForNewSCC)
        printf("Node %d belongs to SCC:%d\n", nodes[i].id, scc->id);
    */
}

void buildSCCNodesAdjTable(){

    vector<int> nextList;
    nextList.reserve(numEdgesG*2);

    for(long unsigned int i=1; i<=nodesNum;i++){
        for(int j : *(nodes[i].out)){
            if((nodes[i].belongsToSCC != nodes[j].belongsToSCC)){
                nextList.push_back(nodes[i].belongsToSCC);
                nextList.push_back(nodes[j].belongsToSCC);
            }
        }
        delete nodes[i].out;
    }

    nodes.clear();
    nodes.shrink_to_fit();

    int kys = nextList.size();
    for(int i=0; i<kys; i+=2){
        int j=i+1;
        sccNodes[nextList[i]-1]->out->push_back(nextList[j]);
        sccNodes[nextList[j]-1]->in->push_back(nextList[i]);
    }


}


void longestPath(){

    vector<int> longestPathEndingAt(sccNodes.size(),0);

    for(int i=sccNodes.size(); i>0;i--){
        int max=0;
        for(int j : *(sccNodes[i-1]->in)){
            if(longestPathEndingAt[j-1] > max) max=longestPathEndingAt[j-1];
        }
        longestPathEndingAt[i-1]=max+1;
    }
    int max =0;
    for(int i :longestPathEndingAt ) if(i>max) max=i;
    max--; //idk man
    //printf("%d\n", max);
    cout << max << endl;
}

