#include <stdio.h>
#include <stdlib.h>

/*---------------------------------------------------------------------------------------------*/
//region Halda

//struktura vrcholu
typedef struct vertex {
    int val;    //hodnota pre ohodnotenie pomocou dijkstru
    int indexInHeap;    //index v halde potrebny na update haldy
    char seen;  //ukazuje ci uz som vrchol vyvazil
    char x, y;  //suradnice v mape
    struct vertex* before;  //ukazovatel na vrchol, z ktoreho som sa dostal do tohto vrchola
}VERTEX;

typedef struct heap {
    int size;   //velkost haldy
    VERTEX** arrOfVer;  //zoznam prvkov v halde
}MIN_HEAP;

//vytvori minimal heap
MIN_HEAP* createHeap() {
    MIN_HEAP* tempHeap = (MIN_HEAP*) malloc(sizeof(MIN_HEAP));
    tempHeap->arrOfVer  = (VERTEX**) malloc(200*sizeof(VERTEX*));
    tempHeap->size = 0;
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
    (*root)->arrOfVer[(*root)->size+1] = paNew;
    (*root)->arrOfVer[(*root)->size+1]->indexInHeap = (*root)->size;
    heapify(root, (*root)->size+1);
    (*root)->size++;
}
//vytiahne najmensi prvok a presetri vlastnost heapu
VERTEX* popFromHeap(MIN_HEAP** root) {
    VERTEX* first = (*root)->arrOfVer[1];
    (*root)->arrOfVer[1] = (*root)->arrOfVer[(*root)->size];
    (*root)->arrOfVer[(*root)->size] = NULL;
    (*root)->size--;
    for (int i = 1; i < (*root)->size+1; i++) {
        (*root)->arrOfVer[i]->indexInHeap = i;
    }
    int index = 1;
    if ((*root)->size <= 1) {
        return first;
    }
    if ((*root)->size == 2) {
        if((*root)->arrOfVer[1]->val > (*root)->arrOfVer[2]->val) swapInHeap(root, 1, 2);
        return first;
    }
    while (((*root)->arrOfVer[index]->val >= (*root)->arrOfVer[index*2]->val) || ((*root)->arrOfVer[index]->val >= (*root)->arrOfVer[index*2+1]->val)) {
        if ((*root)->arrOfVer[index*2+1] != NULL) {
            if ((*root)->arrOfVer[index * 2]->val > (*root)->arrOfVer[index * 2 + 1]->val) {
                swapInHeap(root, index, index * 2 + 1);
                index = index * 2 + 1;
            }
            else {
                swapInHeap(root, index, index * 2);
                index = index * 2;
            }
        } else {
            swapInHeap(root, index, index * 2);
            index = index * 2;
        }

        if (index*2 >= (*root)->size) break;
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
    else {
        insertHeap(heap, (*paNew));
    }
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
void setMap(VERTEX*** mapOfV, char **mapa, int n, int m, int paStaX, int paStaY) {
    MIN_HEAP* heap = NULL;
    VERTEX* temp = mapOfV[paStaX][paStaY];
    temp->val = 0;
    insertHeap(&heap, temp);
    while ((temp = popFromHeap(&heap))!=NULL) {
        mapOfV[temp->y][temp->x]->seen = 1;
        if ( (temp->x+1 < m) &&((mapOfV[temp->y][temp->x+1]->seen == 0)&&(mapa[temp->y][temp->x+1] != 'N'))) {
            VERTEX* new = mapOfV[temp->y][temp->x+1];
            relax(mapa, &heap, &new, &temp);
        }
        if ((temp->x-1 >= 0) &&((mapOfV[temp->y][temp->x-1]->seen == 0)&&(mapa[temp->y][temp->x-1] != 'N'))) {
            VERTEX* new = mapOfV[temp->y][temp->x-1];
            relax(mapa, &heap, &new, &temp);
        }
        if ((temp->y+1 < n) &&((mapOfV[temp->y+1][temp->x]->seen == 0)&&(mapa[temp->y+1][temp->x] != 'N'))) {
            VERTEX* new = mapOfV[temp->y+1][temp->x];
            relax(mapa, &heap, &new, &temp);
        }
        if ((temp->y-1 >= 0) && ((mapOfV[temp->y-1][temp->x]->seen == 0)&&(mapa[temp->y-1][temp->x] != 'N'))) {
            VERTEX* new = mapOfV[temp->y-1][temp->x];
            relax(mapa, &heap, &new, &temp);
        }
        mapOfV[temp->y][temp->x]->seen = 1;
    }
}
//endregion
/*---------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------*/
//region Finding Path

typedef struct edge {
    int val;
    int cost;
    int* path;
}EDGE;

typedef struct node {
    int x, y;
    EDGE* edges[7];
}NODE;

//nastavi zakladne hodnoty do mapy vrcholov
void initializeMap(VERTEX*** mapOfV, int n, int m) {
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

//uvolni mapu vrcholov
void freeMap(VERTEX*** mapOfV, int n, int m) {
    for (int j = 0; j < n; j++) {
        for (int i = 0; i < m; i++) {
            free(mapOfV[j][i]);
        }
        free(mapOfV[j]);
    }
    free(mapOfV);
    mapOfV = NULL;
}

//zjednoti dve cesty do jednej a aj ich dlzky
EDGE* mergePaths(EDGE* paFirst, EDGE* paSecond) {
    EDGE* temp = malloc(sizeof(EDGE));
    temp->path = malloc((paFirst->val + paSecond->val-1)*2*sizeof(int));
    temp->val = paFirst->val + paSecond->val-1;
    int i = 0;
    for (i = 0; i < paFirst->val*2-2; i++) {
        temp->path[i] = paFirst->path[i];
    }
    for (i = 0; i < paSecond->val*2; i++) {
        temp->path[paFirst->val*2-2 + i] = paSecond->path[i];
    }
    return temp;
}
//aktualizuje najkratsiu cestu ak je dana cesta kratsia
void isPathBetter(NODE** paPoints, EDGE* paBest, int* paArr, int* paN){
    int val = 0;
    if (paArr[*paN - 1] != *paN - 1) return;
    for (int i = 0; i < *paN - 1; i++){
        val += paPoints[paArr[i]]->edges[paArr[i+1]]->cost;
    }
    if (val < paBest->cost) {
        paBest->cost = val;
        for (int i = 0; i < *paN; i++){
            paBest->path[i] = paArr[i];
        }
    }
}
//vytvara permutacie, ktore sa vyuzivaju ako indexy v poli vrcholov
void generatePermutation(NODE** paPoints, EDGE* paBest, int* paArr, int* paN, int* nam){
    for (int i = 1; i <= *paN; i++){
        if (*paN != 1){
            (*paN)--;
            generatePermutation(paPoints, paBest, paArr, paN, nam);
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
        else isPathBetter(paPoints, paBest, paArr, nam);
    }
}
//vrati celu cestu od zaciatku az po poslednu princeznu
int *zachran_princezne(char **mapa, int n, int m, int t, int *dlzka_cesty) {
    EDGE* finalPath = malloc(sizeof(EDGE));
    finalPath->path = malloc(sizeof(int));
    finalPath->path = NULL;
    NODE* points[7];
    int akt = 0;
    int dragonX = 0, dragonY = 0;

    VERTEX*** mapOfV = malloc(n*sizeof(VERTEX**));
    for (int j = 0; j < n; j++) {
        mapOfV[j] = malloc(m*sizeof(VERTEX*));
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
                finalPath->path = createRoute(mapOfV[j][i], dlzka_cesty);
                if (finalPath->path == NULL) {
                    *dlzka_cesty = 0;
                    return NULL;
                }
                finalPath->cost = mapOfV[j][i]->val;
                if (finalPath->cost > t) {
                    *dlzka_cesty = 0;
                    return NULL;
                }
                finalPath->val = *dlzka_cesty;
            }
            if (mapa[j][i] == 'P') {
                points[akt]= (NODE*) malloc(sizeof(NODE));
                points[akt]->x = i;
                points[akt]->y = j;
                akt++;
            }
        }
    }
    points[akt]= (NODE*) malloc(sizeof(NODE));
    points[akt]->x = dragonX;
    points[akt++]->y = dragonY;
    points[akt]=NULL;

    for (int i = 0; i < akt; i++) {
        if (points[i] == NULL) break;
        initializeMap(mapOfV, n, m);
        setMap(mapOfV, mapa, n, m, points[i]->y, points[i]->x);
        for (int j = 0; j < akt; j++) {
            points[i]->edges[j] = NULL;
            if (points[i] != points[j]) {
                points[i]->edges[j] = (EDGE*)malloc(sizeof(EDGE));
                points[i]->edges[j]->path = createRoute(mapOfV[points[j]->y][points[j]->x], dlzka_cesty);
                if (points[i]->edges[j]->path == NULL) {
                    *dlzka_cesty = 0;
                    return NULL;
                }
                points[i]->edges[j]->cost = mapOfV[points[j]->y][points[j]->x]->val;
                points[i]->edges[j]->val = *dlzka_cesty;
            }
        }
        points[i]->edges[akt] = NULL;
    }

    int* arr = malloc(akt*sizeof(int));
    for (int i = 0; i < akt; i++){
        arr[i] = i;
    }
    int* num = malloc(sizeof(int));
    *num = akt;
    EDGE* best = malloc(sizeof(EDGE));
    best->val = 0;
    best->cost = 10000;
    best->path = (int*) malloc(akt*sizeof(int));
    generatePermutation(points, best, arr, num, &akt);

    for (int i = akt-1; i > 0; i--) {
        finalPath = mergePaths(finalPath, points[best->path[i]]->edges[best->path[i-1]]);
    }

    *dlzka_cesty = finalPath->val;
    freeMap(mapOfV, n, m);
    return finalPath->path;
}

//endregion
/*---------------------------------------------------------------------------------------------*/

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
                srand(3);
                n = 50;
                m = 100;
                t = 120;
                printf("%d %d %d\n", n, m, t);
                mapa = (char**)malloc(n*sizeof(char*));
                for (int i = 0; i < n; i++) {
                    mapa[i] = (char*)malloc(m*sizeof(char));
                    for (int j = 0; j < m; j++) {

                        int x = rand() % 10;
                        switch (x) {
                            case 0 ... 3:
                                mapa[i][j] = 'C';
                                break;
                            case 4 ... 7:
                                mapa[i][j] = 'H';
                                break;
                            case 8 ... 9:
                                mapa[i][j] = 'N';
                                break;
                            default:break;
                        }
                    }
                }
                mapa[0][0]='C';
                mapa[rand() % n][rand() % m] = 'D';
                for (int i = 0; i < 5; i++) {
                    mapa[rand() % n][rand() % m] = 'P';
                }

                for (int i = 0; i < n; i++) {
                    for (int j = 0; j < m; j++) {
                        printf("%c", mapa[i][j]);
                    }
                    printf("\n");
                }
                cesta = zachran_princezne(mapa, n, m, t, &dlzka_cesty);
                break;
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