from pulp import LpProblem, LpVariable, lpSum, GLPK_CMD, value, LpMaximize
import time



def main():
    #Get Input
    numToys, numCombos, prodCap  = input().split(" ")

    numToys = int(numToys)
    numCombos = int(numCombos)
    prodCap = int(prodCap)

    toys=[None for i in range(numToys)]
    combos=[None for i in range(numCombos)]

    for i in range(0,numToys ):
        val, cap = input().split()
        toys[i]=( int(val), int(cap), [] )

    skipped=0
    for i in range(0, numCombos):
        id1, id2, id3, val=input().split(" ")
        #Skip pointless combo deals, no ideia why this is a possibility
        if(toys[int(id1)-1][0]+ toys[int(id2)-1][0]+toys[int(id3)-1][0] >= int(val)):
            skipped+=1
            continue
        combos[i-skipped]=(int(id1)-1, int(id2)-1, int(id3)-1, int(val))
        toys[int(id1)-1][2].append(i-skipped)
        toys[int(id2)-1][2].append(i-skipped)
        toys[int(id3)-1][2].append(i-skipped)

    numCombos-=skipped

    #Init LpProblem
    prob = LpProblem("MaxProfit", LpMaximize)

    #Init LpVariables
    #Glpk throws a tantrum if the names have digits as prefixes, idk why
    lpToys=[LpVariable( "b" + str(toyID), 0, toys[toyID][1], cat='Integer') for toyID in range(0,numToys)]
    lpCombos=[LpVariable( "p" + str(comboID+numToys), lowBound=0 , upBound=min(combos[comboID][0],combos[comboID][1],combos[comboID][2]), cat='Integer') for comboID in range(0,numCombos)]

    #Init linear combination to maximize
    linearComb=[ (lpToys[i] * toys[i][0]) for i in range(len(lpToys))]

    linearComb+=[lpCombo * combos[int(lpCombo.name[1:])-numToys][3] for lpCombo in lpCombos]
    prob+=lpSum(linearComb)

    #Limit to Production capacity
    lpToys.append([lpCombo*3 for lpCombo in lpCombos])

    prob += lpSum(lpToys) <= prodCap
    

    countIneq=0
    upperIndex=numToys
    for i in range(0,upperIndex):
        temp=[]
        for comboId in toys[i][2]:
            temp.append(lpCombos[comboId])
        if(len(temp)>0):
            countIneq+=1
            prob += lpToys[i] + temp <= lpToys[i].upBound

    #print(countIneq + numToys+numCombos)
    #print(numCombos + numToys)
    #print(prob)
    
    #Solve
    prob.solve(GLPK_CMD(msg=True))
    #Around 8x times slower than the built solver from my testing
    #The built in just won't terminate on some data sets for some reason
    #Setting mip=False runs way faster which makes sense
    #But it solves 20/21 problems correctly. I don't understand why.
    print(int(value(prob.objective)))


idk = time.perf_counter()
main()
idc=time.perf_counter()-idk
print(f"{idc:.2f}")