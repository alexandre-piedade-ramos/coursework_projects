#include <stdio.h>
#include <stdlib.h>
#define MAX_INPUT 200

typedef struct{
    int x;
    int y;
    int val;
}item;

typedef struct{
    int val;
    int size;

}vSel;

int v(int masterSheetX, int masterSheetY);
int max(int a,int b);
int lookUpItemVal(int sizeX, int sizeY, int **itemLookUp);


item * items;
int itemsNum;
int masterSheetX;
int masterSheetY;


int main() {
    char input[MAX_INPUT];
    int i,a,b,c;
    int anwser;
    fgets(input, MAX_INPUT, stdin);
    sscanf(input, "%d %d",&masterSheetX,&masterSheetY);
    fgets(input, MAX_INPUT, stdin);
    sscanf(input, "%d",&itemsNum);
    items = (item *) malloc(itemsNum*sizeof(item));
    for(i=0;i<itemsNum;i++){
        fgets(input, MAX_INPUT, stdin);
        sscanf(input, "%d %d %d",&a, &b,&c);
        items[i].x=a;
        items[i].y=b;
        items[i].val=c;
    }
    anwser=v(masterSheetX,masterSheetY);
    printf("%d\n",anwser);
    free(items);
    

    return 0;
}

int v(int masterSheetX, int masterSheetY){
    int i,j,k,res;
    int bestCutX=0, bestCutY=0;
    int r, q;
    int ** dp= (int **) calloc(masterSheetX * sizeof(int*), sizeof(int*));

    for(i=0; i<masterSheetX;i++){
        dp[i]= (int *) calloc( masterSheetY*sizeof(int), sizeof(int));
    }
    int ** itemLookUp= (int **) malloc(masterSheetX * sizeof(int*));
    for(i=0; i<masterSheetX;i++){
        itemLookUp[i]= (int *) calloc( masterSheetY*sizeof(int),sizeof(int));
    }

    for(i=0;i<itemsNum;i++){
        r=items[i].x-1;
        q=items[i].y-1;
        if(r<masterSheetX && q<masterSheetY){
            if(itemLookUp[r][q]==0)
                itemLookUp[r][q]=items[i].val;
            else if(items[i].val>itemLookUp[r][q])
                itemLookUp[r][q]=items[i].val;
        }
        if(q<masterSheetX && r<masterSheetY){
            if(itemLookUp[q][r]==0)
                itemLookUp[q][r]=items[i].val;
            else if(items[i].val>itemLookUp[q][r])
                itemLookUp[q][r]=items[i].val;
        }
    }
    
    for(i=0;i<masterSheetX;i++)
        for(j=0;j<masterSheetY;j++){
            bestCutX=0;
            bestCutY=0;
            if(i+1>=2){
                for(k=1; k<(i+1)/2+1;k++)
                    if((dp[i-k][j] + dp[k-1][j])>bestCutX)
                        bestCutX=(dp[i-k][j] + dp[k-1][j]);
            }
            if(j+1>=2){
                for(k=1; k<(j+1)/2+1;k++)
                    if((dp[i][j-k] + dp[i][k-1])>bestCutY)
                        bestCutY=(dp[i][j-k] + dp[i][k-1]);
                
            }
            dp[i][j]=max(max(bestCutX, bestCutY), lookUpItemVal(i+1,j+1,itemLookUp) );
        }
    
    res=dp[masterSheetX-1][masterSheetY-1];

    for(i=0; i<masterSheetX;i++) free(dp[i]);
    free(dp);
    for(i=0; i<masterSheetX;i++) free(itemLookUp[i]);
    free(itemLookUp);

    return res;
}

int lookUpItemVal(int sizeX, int sizeY, int **itemLookUp){
    int a=0,b=0;
    if(sizeX<=masterSheetX && sizeY<=masterSheetY)
        a=itemLookUp[sizeX-1][sizeY-1];
    if(sizeY<=masterSheetX && sizeX<=masterSheetY)
        b=itemLookUp[sizeY-1][sizeX-1];
    return max(a,b);
}

int max(int a,int b){
    if(a>=b) return a;
    return b;
}
