#include <stdio.h>
#include <stdlib.h>

/*---------------------------------------------------------------------------------------------*/
//region Halda

typedef struct vertex {
    int val;
    int indexInHeap;
    char seen;
    char x, y;
    struct vertex* before;
}VERTEX;

typedef struct heap {
    int size;
    VERTEX** arrOfVer;
}MIN_HEAP;

//vytvori minimal heap
MIN_HEAP* createHeap() {
    MIN_HEAP* tempHeap = (MIN_HEAP*) malloc(sizeof(MIN_HEAP));
    tempHeap->arrOfVer  = (VERTEX**) malloc(10000*sizeof(VERTEX*));
    tempHeap->size = 1;
}
//vymeni dve hodnoty v heape
void swapInHeap(MIN_HEAP** root, int indexA, int indexB) {
    VERTEX* temp = (*root)->arrOfVer[indexA];
    (*root)->arrOfVer[indexA] = (*root)->arrOfVer[indexB];
    (*root)->arrOfVer[indexB] = temp;
    (*root)->arrOfVer[indexA]->indexInHeap = indexA;
    (*root)->arrOfVer[indexB]->indexInHeap = indexB;
}
//presetri ci je splnena vlastnost heapu
void heapify(MIN_HEAP** root, int index) {
    if (index <= 1) return;
    while ((*root)->arrOfVer[index]->val < (*root)->arrOfVer[index/2]->val) {
        swapInHeap(root, index, index/2);
        index /= 2;
        if (index <= 1) break;
    }
}
//vlozi do heapu
void insertHeap(MIN_HEAP** root, VERTEX* paNew) {
    if (*root == NULL)
        *root = createHeap();
    (*root)->arrOfVer[(*root)->size] = paNew;
    (*root)->arrOfVer[(*root)->size]->indexInHeap = (*root)->size;
    heapify(root, (*root)->size);
    (*root)->size++;
}
//vytiahne najmensi prvok a presetri vlastnost heapu
VERTEX* popFromHeap(MIN_HEAP** root) {
    VERTEX* first = (*root)->arrOfVer[1];
    (*root)->arrOfVer[1] = (*root)->arrOfVer[(*root)->size-1];
    (*root)->arrOfVer[(*root)->size-1] = NULL;
    (*root)->size--;
    for (int i = 1; i < (*root)->size; i++) {
        (*root)->arrOfVer[i]->indexInHeap = i;
    }
    int index = 1;
    if ((*root)->size <= 2) {
        return first;
    }
    if ((*root)->size == 3) {
        swapInHeap(root, 1, 2);
        return first;
    }
    while (((*root)->arrOfVer[index]->val >= (*root)->arrOfVer[index*2]->val) || ((*root)->arrOfVer[index]->val > (*root)->arrOfVer[index*2+1]->val)) {
        if ((*root)->arrOfVer[index*2+1] != NULL) {
            if ((*root)->arrOfVer[index * 2]->val >= (*root)->arrOfVer[index * 2 + 1]->val)
                swapInHeap(root, index, index * 2 + 1);
            else
                swapInHeap(root, index, index * 2);
        } else {
            swapInHeap(root, index, index * 2);
        }
        index *= 2;
        if (index*2 >= (*root)->size-1) break;
    }
    return first;
}
//endregion
/*---------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------*/
//region Dijkstra


//vlozi alebo updatne vrchol v halde
void relax(char **mapa, MIN_HEAP** heap, VERTEX** paNew, VERTEX** paTemp) {
    if (mapa[(*paTemp)->y][(*paTemp)->x] == 'H') {
        if (((*paTemp)->val + 2 < (*paNew)->val) || ((*paNew)->val == -1)) {
            (*paNew)->val = (*paTemp)->val + 2;
            (*paNew)->before = (*paTemp);
        }
    }
    else {
        if (((*paTemp)->val + 1 < (*paNew)->val) || ((*paNew)->val == -1)) {
            (*paNew)->val = (*paTemp)->val + 1;
            (*paNew)->before = (*paTemp);
        }
    }
    if ((*paNew)->indexInHeap != 0)
        heapify(heap, (*paNew)->indexInHeap);
    else
        insertHeap(heap, (*paNew));
}
//vytvori cestu z daneho vrchola k zaciatku na zaklade predchadzajucich vrcholov
int* createRoute(VERTEX* paVertex, int* dlzka_cesty) {
    VERTEX* temp = paVertex;
    int amount = 0;
    while (temp != NULL) {
        amount++;
        temp = temp->before;
    }
    int* result = malloc((amount)*2 * sizeof(int));
    int i = amount*2-1;
    *dlzka_cesty = amount;
    while (paVertex != NULL) {
        result[i--] = paVertex->y;
        result[i--] = paVertex->x;
        paVertex = paVertex->before;
    }
    return result;
}
//ohodnoti celu mapu z daneho bodu
void setMap(VERTEX* mapOfV[100][100], char **mapa, int n, int m, int paStaX, int paStaY) {
    MIN_HEAP* heap = NULL;
    VERTEX* temp = mapOfV[paStaX][paStaY];
    temp->val = 0;
    insertHeap(&heap, temp);
    while (heap->size >= 2) {
        VERTEX* temp = popFromHeap(&heap);
        mapOfV[temp->y][temp->x]->seen = 1;
        if ( (temp->x+1 < m) &&(mapOfV[temp->y][temp->x+1]->seen == 0) && (mapa[temp->y][temp->x+1] != 'N')) {
            VERTEX* new = mapOfV[temp->y][temp->x+1];
            relax(mapa, &heap, &new, &temp);
        }
        if ((temp->x-1 >= 0) &&(mapOfV[temp->y][temp->x-1]->seen == 0) &&(mapa[temp->y][temp->x-1] != 'N')) {
            VERTEX* new = mapOfV[temp->y][temp->x-1];
            relax(mapa, &heap, &new, &temp);
        }
        if ((temp->y+1 < n) &&(mapOfV[temp->y+1][temp->x]->seen == 0) &&(mapa[temp->y+1][temp->x] != 'N')) {
            VERTEX* new = mapOfV[temp->y+1][temp->x];
            relax(mapa, &heap, &new, &temp);
        }
        if ((temp->y-1 >= 0) && (mapOfV[temp->y-1][temp->x]->seen == 0) && (mapa[temp->y-1][temp->x] != 'N')) {
            VERTEX* new = mapOfV[temp->y-1][temp->x];
            relax(mapa, &heap, &new, &temp);
        }
    }
}
//endregion
/*---------------------------------------------------------------------------------------------*/



typedef struct edge {
    int val;
    int* line;
}EDGE;

typedef struct node {
    int x, y;
    EDGE* edges[7];
}NODE;

typedef struct bestRoute {
    int val;
    int* path;
}BEST_ROUTE;

void initializeMap(VERTEX* mapOfV[100][100], int n, int m) {
    for (int j = 0; j < n; j++) {
        for (int i = 0; i < m; i++) {
            mapOfV[j][i]->val = -1;
            mapOfV[j][i]->x = i;
            mapOfV[j][i]->y = j;
            mapOfV[j][i]->seen = 0;
            mapOfV[j][i]->indexInHeap = 0;
            mapOfV[j][i]->before = NULL;
        }
    }
}

void WriteArr(NODE* paPoints[7], BEST_ROUTE* paBest, int* paArr, int* paN){
    int val = 0;
    for (int i = 0; i < *paN - 1; i++){
        val += paPoints[paArr[i]]->edges[paArr[i+1]]->val;
    }
    if (val < paBest->val) {
        paBest->val = val;
        for (int i = 0; i < *paN; i++){
            paBest->path[i] = paArr[i];
        }
    }

}

EDGE* mergePaths(EDGE* paFirst, EDGE* paSecond) {
    EDGE* temp = malloc(sizeof(EDGE));
    temp->line = malloc((paFirst->val + paSecond->val-1)*2*sizeof(int));
    temp->val = paFirst->val + paSecond->val-1;
    int i = 0;
    for (i = 0; i < paFirst->val*2-2; i++) {
        temp->line[i] = paFirst->line[i];
    }
    for (i = 0; i < paSecond->val*2; i++) {
        temp->line[paFirst->val*2-2 + i] = paSecond->line[i];
    }
    return temp;
}

void Generate(NODE* paPoints[7], BEST_ROUTE* paBest, int* paArr, int* paN, int* nam){
    for (int i = 1; i <= *paN; i++){
        if (*paN != 1){
            (*paN)--;
            Generate(paPoints, paBest, paArr, paN, nam);
            (*paN)++;
            if (*paN != i){
                for (int j = 0; j <= (*paN / 2 + *paN % 2)-1; j++){
                    if (j != *paN - j - 1){
                        int help = paArr[j];
                        paArr[j] = paArr[*paN - j - 1];
                        paArr[*paN - j - 1] = help;
                    }
                }
            }
        }
        else WriteArr(paPoints, paBest, paArr, nam);
    }
}

int *zachran_princezne(char **mapa, int n, int m, int t, int *dlzka_cesty) {
    EDGE* pathToDragon = malloc(sizeof(EDGE));
    pathToDragon->line = malloc(sizeof(int));
    NODE* points[7];
    int akt = 0;
    int dragonX = 0, dragonY = 0;

    VERTEX* mapOfV[100][100];
    for (int j = 0; j < n; j++) {
        for (int i = 0; i < m; i++) {
            mapOfV[j][i] = (VERTEX*)malloc(sizeof(VERTEX));
        }
    }
    initializeMap(mapOfV, n, m);
    setMap(mapOfV, mapa, n, m, 0, 0);
    for (int j = 0; j < n; j++) {
        for (int i = 0; i < m; i++) {
            if (mapa[j][i] == 'D') {
                dragonX = i;
                dragonY = j;
                pathToDragon->line = createRoute(mapOfV[dragonY][dragonX], dlzka_cesty);
                pathToDragon->val = *dlzka_cesty;
            }
            if (mapa[j][i] == 'P') {
                points[akt]= (NODE*) malloc(sizeof(NODE));
                points[akt]->x = j;
                points[akt]->y = i;
                akt++;
            }
        }
    }
    points[akt]= (NODE*) malloc(sizeof(NODE));
    points[akt]->x = dragonY;
    points[akt]->y = dragonX;
    akt++;
    points[akt]=NULL;
    for (int i = 0; i < akt; i++) {
        if (points[i] == NULL) break;
        initializeMap(mapOfV, n, m);
        setMap(mapOfV, mapa, n, m, points[i]->x, points[i]->y);
        for (int j = 0; j < akt; j++) {
            points[i]->edges[j] = NULL;
            if (points[i] != points[j]) {
                points[i]->edges[j] = (EDGE*)malloc(sizeof(EDGE));
                points[i]->edges[j]->line = createRoute(mapOfV[points[j]->x][points[j]->y], dlzka_cesty);
                points[i]->edges[j]->val = *dlzka_cesty;
            }
        }
        points[i]->edges[akt] = NULL;
    }

    int* arr = malloc(akt*sizeof(int));
    for (int i = 0; i < akt; i++){
        arr[i] = i;
    }
    int* num;
    num = malloc(sizeof(int));
    *num = akt;
    BEST_ROUTE* best = malloc(sizeof(BEST_ROUTE));
    best->val = 10000;
    best->path = (int*) malloc(akt*sizeof(int));
    Generate(points, best, arr, num, &akt);

    for (int i = akt-1; i > 0; i--) {
        pathToDragon = mergePaths(pathToDragon, points[best->path[i]]->edges[best->path[i-1]]);
    }

    *dlzka_cesty = pathToDragon->val;
    return pathToDragon->line;
}

int main()
{
    char **mapa;
    int i, test, dlzka_cesty, cas, *cesta;
    int n=0, m=0, t=0;
    FILE* f;
    while(1){
        printf("Zadajte cislo testu (0 ukonci program):\n");
        scanf("%d",&test);
        dlzka_cesty = 0;
        n=m=t=0;
        switch(test){
            case 0://ukonci program
                return 0;
            case 1://nacitanie mapy zo suboru
                f=fopen("test.txt","r");
                if(f)
                    fscanf(f, "%d %d %d", &n, &m, &t);
                else
                    continue;
                mapa = (char**)malloc(n*sizeof(char*));
                for(i=0; i<n; i++){
                    mapa[i] = (char*)malloc(m*sizeof(char));
                    for (int j=0; j<m; j++){
                        char policko = fgetc(f);
                        if(policko == '\n') policko = fgetc(f);
                        mapa[i][j] = policko;
                    }
                }
                fclose(f);
                cesta = zachran_princezne(mapa, n, m, t, &dlzka_cesty);
                break;
            case 2://nacitanie preddefinovanej mapy
                n = 10;
                m = 10;
                t = 12;
                mapa = (char**)malloc(n*sizeof(char*));
                mapa[0]="CCHCNHCCHN";
                mapa[1]="NNCCCHHCCC";
                mapa[2]="DNCCNNHHHC";
                mapa[3]="CHHHCCCCCC";
                mapa[4]="CCCCCNHHHH";
                mapa[5]="PCHCCCNNNN";
                mapa[6]="NNNNNHCCCC";
                mapa[7]="CCCCCPCCCC";
                mapa[8]="CCCNNHHHHH";
                mapa[9]="HHHPCCCCCC";
                cesta = zachran_princezne(mapa, n, m, t, &dlzka_cesty);
                break;
            case 3: //pridajte vlastne testovacie vzorky
            default:
                continue;
        }
        cas = 0;
        for(i=0; i<dlzka_cesty; i++){
            printf("%d %d\n", cesta[i*2], cesta[i*2+1]);
            if(mapa[cesta[i*2+1]][cesta[i*2]] == 'H')
                cas+=2;
            else
                cas+=1;
            if(mapa[cesta[i*2+1]][cesta[i*2]] == 'D' && cas > t)
                printf("Nestihol si zabit draka!\n");
            if(mapa[cesta[i*2+1]][cesta[i*2]] == 'N')
                printf("Prechod cez nepriechodnu prekazku!\n");
            if(i>0 && abs(cesta[i*2+1]-cesta[(i-1)*2+1])+abs(cesta[i*2]-cesta[(i-1)*2])>1)
                printf("Neplatny posun Popolvara!\n");
        }
        printf("%d\n",cas);
        free(cesta);
        for(i=0; i<n; i++){
            free(mapa[i]);
        }
        free(mapa);
        break;
    }
    return 0;
}
