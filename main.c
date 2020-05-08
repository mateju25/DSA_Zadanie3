/**
 * Autor: Matej Delincak
 * Datum poslednej upravy: 14.5.2020
 */
#include <stdio.h>
#include <stdlib.h>

/*---------------------------------------------------------------------------------------------*/
//region Halda

//struktura vrcholu
typedef struct vertex {
    int cost;    //hodnota pre ohodnotenie pomocou dijkstru
    int length;    //dlzka cesty
    int indexInHeap;    //index v halde potrebny na update haldy
    int x, y;  //suradnice v mape
    struct vertex* before;  //ukazovatel na vrchol, z ktoreho som sa dostal do tohto vrchola
}VERTEX;

typedef struct heap {
    int size;   //velkost haldy
    VERTEX** arrOfVer;  //zoznam prvkov v halde
}MIN_HEAP;

//vytvori minimal heap
MIN_HEAP* createHeap() {
    MIN_HEAP* tempHeap = (MIN_HEAP*) malloc(sizeof(MIN_HEAP));
    tempHeap->arrOfVer  = (VERTEX**) malloc(1000*sizeof(VERTEX*));
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
    while ((*root)->arrOfVer[index]->cost < (*root)->arrOfVer[index/2]->cost) {
        swapInHeap(root, index, index/2);
        //posun o parenta vyssie
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
    if ((*root)->size == 0) return NULL;
    VERTEX* first = (*root)->arrOfVer[1];
    first->indexInHeap = -1;
    (*root)->arrOfVer[1] = (*root)->arrOfVer[(*root)->size];
    (*root)->arrOfVer[1]->indexInHeap = 1;
    (*root)->arrOfVer[(*root)->size] = NULL;
    (*root)->size--;
    int index = 1;
    if ((*root)->size <= 1) {
        return first;
    }
    if ((*root)->size == 2) {
        if((*root)->arrOfVer[1]->cost > (*root)->arrOfVer[2]->cost) swapInHeap(root, 1, 2);
        return first;
    }
    while (((*root)->arrOfVer[index]->cost >= (*root)->arrOfVer[index*2]->cost) || ((*root)->arrOfVer[index]->cost >= (*root)->arrOfVer[index*2+1]->cost)) {
        if ((*root)->arrOfVer[index*2+1] != NULL) {
            if ((*root)->arrOfVer[index * 2]->cost > (*root)->arrOfVer[index * 2 + 1]->cost) {
                //vymen praveho potomka
                swapInHeap(root, index, index * 2 + 1);
                index = index * 2 + 1;
            }
            else {
                //vymen laveho potomka
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

typedef struct edge {
    int length; //dlzka cesty
    int cost;   //cas cesty
    int* path;  //suradnice cesty
}EDGE;

//vlozi alebo updatne vrchol v halde
void relax(char **mapa, MIN_HEAP** heap, VERTEX** paNew, VERTEX** paTemp) {
    if (mapa[(*paTemp)->y][(*paTemp)->x] == 'H') {
        if (((*paTemp)->cost + 2 < (*paNew)->cost) || ((*paNew)->cost == -1)) {
            //ohodnot vrchol ak som v hustine
            (*paNew)->cost = (*paTemp)->cost + 2;
            (*paNew)->length = (*paTemp)->length + 1;
            (*paNew)->before = (*paTemp);
        }
    }
    else {
        if (((*paTemp)->cost + 1 < (*paNew)->cost) || ((*paNew)->cost == -1)) {
            //ohodnot vrchol ak som na ceste
            (*paNew)->cost = (*paTemp)->cost + 1;
            (*paNew)->length = (*paTemp)->length + 1;
            (*paNew)->before = (*paTemp);
        }
    }
    if ((*paNew)->indexInHeap > 0)
        //ak uz vrchol je v halde
        heapify(heap, (*paNew)->indexInHeap);
    else
        //ak sa v halde nenachadza
        insertHeap(heap, (*paNew));
}
//vytvori cestu z daneho vrchola k zaciatku na zaklade predchadzajucich vrcholov
EDGE* createRoute(char **mapa, VERTEX* paVertex) {
    EDGE* temp = (EDGE*) malloc(sizeof(EDGE));
    temp->path = (int*) malloc((paVertex->length+1)*2*sizeof(int));
    temp->length = paVertex->length+1;
    temp->cost = paVertex->cost;
    if (paVertex->before == NULL) {
        //ak sa k vrcholu nemozem dostat
        return NULL;
    }
    int i = temp->length*2-1;
    while (paVertex != NULL) {
        //vytvor suradnice cesty
        temp->path[i--] = paVertex->y;
        temp->path[i--] = paVertex->x;
        paVertex = paVertex->before;
    }
    return temp;
}
//ohodnoti celu mapu z daneho bodu
void setMap(VERTEX*** mapOfV, char **mapa, int n, int m, int paStaY, int paStaX) {
    MIN_HEAP* heap = NULL;
    VERTEX* temp = mapOfV[paStaY][paStaX];
    if (mapa[paStaY][paStaX] == 'H')
        temp->cost = 2;
    else
        temp->cost = 1;
    if (mapa[paStaY][paStaX] == 'N') return;
    insertHeap(&heap, temp);

    //popni z haldy a zacni dijkstru
    while ((temp = popFromHeap(&heap))!=NULL) {
        if ( (temp->x+1 < m) &&((mapOfV[temp->y][temp->x+1]->indexInHeap >= 0)&&(mapa[temp->y][temp->x+1] != 'N'))) {
            //pozri praveho suseda
            VERTEX* new = mapOfV[temp->y][temp->x+1];
            //zrelaxuj
            relax(mapa, &heap, &new, &temp);
        }
        if ((temp->x-1 >= 0) &&((mapOfV[temp->y][temp->x-1]->indexInHeap >= 0)&&(mapa[temp->y][temp->x-1] != 'N'))) {
            //pozri laveho suseda
            VERTEX* new = mapOfV[temp->y][temp->x-1];
            //zrelaxuj
            relax(mapa, &heap, &new, &temp);
        }
        if ((temp->y+1 < n) &&((mapOfV[temp->y+1][temp->x]->indexInHeap >= 0)&&(mapa[temp->y+1][temp->x] != 'N'))) {
            //pozri dolneho suseda
            VERTEX* new = mapOfV[temp->y+1][temp->x];
            //zrelaxuj
            relax(mapa, &heap, &new, &temp);
        }
        if ((temp->y-1 >= 0) && ((mapOfV[temp->y-1][temp->x]->indexInHeap >= 0)&&(mapa[temp->y-1][temp->x] != 'N'))) {
            //pozri horneho suseda
            VERTEX* new = mapOfV[temp->y-1][temp->x];
            //zrelaxuj
            relax(mapa, &heap, &new, &temp);
        }
    }
}
//endregion
/*---------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------*/
//region Hladanie cesty

typedef struct node {
    int x, y;   //suradnice doleziteho vrchola
    EDGE* edges[7];    //jeho cesty k inym vrcholom
}NODE;

//nastavi zakladne hodnoty do mapy vrcholov
void initializeMap(VERTEX*** mapOfV, char **mapa, int n, int m) {
    for (int j = 0; j < n; j++) {
        for (int i = 0; i < m; i++) {
            if (mapa[j][i]!='N') {
                //nastavi mapu vrcholov na zakladne hodnoty
                mapOfV[j][i]->cost = -1;
                mapOfV[j][i]->length = 0;
                mapOfV[j][i]->x = i;
                mapOfV[j][i]->y = j;
                mapOfV[j][i]->indexInHeap = 0;
                mapOfV[j][i]->before = NULL;
            }
        }
    }
}

//uvolni mapu vrcholov
void freeMap(VERTEX*** mapOfV, int n, int m) {
    for (int j = 0; j < n; j++) {
        for (int i = 0; i < m; i++) {
            if (mapOfV[j][i] != NULL)
                free(mapOfV[j][i]);
        }
        free(mapOfV[j]);
    }
    free(mapOfV);
}

//zjednoti dve cesty do jednej a aj ich dlzky
EDGE* mergePaths(EDGE* paFirst, EDGE* paSecond) {
    EDGE* temp = (EDGE*)malloc(sizeof(EDGE));
    //vytvor novu cestu
    temp->path = (int*)malloc((paFirst->length + paSecond->length-1)*2*sizeof(int));
    temp->length = paFirst->length + paSecond->length-1;
    int i = 0;
    for (i = 0; i < paFirst->length*2-2; i++) {
        //nakopiruj prvu cestu
        temp->path[i] = paFirst->path[i];
    }
    for (i = 0; i < paSecond->length*2; i++) {
        //nakopiruj druhu cestu
        temp->path[paFirst->length*2-2 + i] = paSecond->path[i];
    }
    free(paFirst->path);
    free(paFirst);
    free(paSecond->path);
    free(paSecond);
    return temp;
}
//aktualizuje najkratsiu cestu ak je dana cesta kratsia
void isPathBetter(NODE** paPoints, EDGE* paBest, int* paArr, int* paN){
    int val = 0, i;
    if (paArr[*paN - 1] != *paN - 1) return;
    for (i = 0; i < *paN - 1; i++){
        //spocitaj cas
        val += paPoints[paArr[i]]->edges[paArr[i+1]]->cost;
    }
    if (val < paBest->cost) {
        //nasla sa lepsia, tak si ju zapamataj
        paBest->cost = val;
        for (i = 0; i < *paN; i++){
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
                        //swapovanie hodnot
                        int help = paArr[j];
                        paArr[j] = paArr[*paN - j - 1];
                        paArr[*paN - j - 1] = help;
                    }
                }
            }
        }
        //nasla sa permutacia
        else isPathBetter(paPoints, paBest, paArr, nam);
    }
}
//vrati celu cestu od zaciatku az po poslednu princeznu
int *zachran_princezne(char **mapa, int n, int m, int t, int *dlzka_cesty) {
    EDGE* finalPath = NULL;
    NODE* points[7]; //dolezite vrcholy na mape (P, D)
    int akt = 0, i, j;
    int dragonX = 0, dragonY = 0;

    //alokuj miesto pre mapu
    VERTEX*** mapOfV = (VERTEX***)malloc(n*sizeof(VERTEX**));
    for (j = 0; j < n; j++) {
        mapOfV[j] = (VERTEX**)malloc(m*sizeof(VERTEX*));
        for (i = 0; i < m; i++) {
            mapOfV[j][i] = (VERTEX*)malloc(sizeof(VERTEX));
        }
    }

    initializeMap(mapOfV, mapa, n, m);
    setMap(mapOfV, mapa, n, m, 0, 0);
    for (j = 0; j < n; j++) {
        for (i = 0; i < m; i++) {
            if (mapa[j][i] == 'D') {
                dragonX = i;
                dragonY = j;
                if ((i == 0) && (j == 0)) {
                    //specialny pripad
                    //ked drak je tam kde sa zacinam
                    finalPath = (EDGE*) malloc(sizeof(EDGE));
                    finalPath->path = (int*) malloc(2*sizeof(int));
                    finalPath->cost = 0;
                    finalPath->length = 1;
                    finalPath->path[1] = 0;
                    finalPath->path[0] = 0;
                    continue;
                }
                finalPath = createRoute(mapa, mapOfV[j][i]);
                if ((finalPath == NULL) || (finalPath->cost > t)) {
                    //ak neni cesta, alebo je malo casu na zabitie draka
                    printf("Cesta neexistuje.\n");
                    *dlzka_cesty = 0;
                    return NULL;
                }
            }
            if (mapa[j][i] == 'P') {
                //nasla sa princezna
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

    for (i = 0; i < akt; i++) {
        if (points[i] == NULL) break;
        initializeMap(mapOfV, mapa, n, m);
        setMap(mapOfV, mapa, n, m, points[i]->y, points[i]->x);
        for (j = 0; j < akt; j++) {
            points[i]->edges[j] = NULL;
            if (points[i] != points[j]) {
                //vytvor cesty od kazdeho doleziteho vrchola ku kazdemu inemu
                points[i]->edges[j] = createRoute(mapa, mapOfV[points[j]->y][points[j]->x]);
                if (points[i]->edges[j] == NULL) {
                    //ak cesta neexistuje
                    printf("Cesta neexistuje.\n");
                    *dlzka_cesty = 0;
                    return NULL;
                }
            }
        }
        points[i]->edges[akt] = NULL;
    }

    int* arr = (int*)malloc(akt*sizeof(int));
    for (i = 0; i < akt; i++){
        //vytvor pole pre permutacie
        arr[i] = i;
    }
    int* num = (int*)malloc(sizeof(int));
    *num = akt;
    //vytvorenie najlepsej cesty
    EDGE* best = malloc(sizeof(EDGE));
    best->length = 0;
    best->cost = 10000;
    best->path = (int*) malloc(akt*sizeof(int));
    generatePermutation(points, best, arr, num, &akt);

    //spoji cesty na zaklade najlepsej vygenerovanej cesty
    for (i = akt-1; i > 0; i--) {
        finalPath = mergePaths(finalPath, points[best->path[i]]->edges[best->path[i-1]]);
    }

    *dlzka_cesty = finalPath->length;
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
    char s[50];
    while(1){
        printf("Zadajte cislo testu (0 ukonci program):\n");
        scanf("%d",&test);
        dlzka_cesty = 0;
        n=m=t=0;
        switch(test){
            case 0://ukonci program
                return 0;
            case 1://nacitanie mapy zo suboru
                printf("Zadaj meno suboru: ");
                scanf("%s", s);
                f=fopen(s,"r");
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
            case 3: {//pridajte vlastne testovacie vzorky
                unsigned int seed = 0;
                printf("Seed: ");
                scanf("%d", &seed);
                srand(seed);
                scanf("%d %d %d", &n, &m, &t);
                mapa = (char **) malloc(n * sizeof(char *));
                for (i = 0; i < n; i++) {
                    mapa[i] = (char *) malloc(m * sizeof(char));
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
                            default:
                                break;
                        }
                    }
                }
                mapa[0][0] = 'C';
                mapa[rand() % n][rand() % m] = 'D';
                for (i = 0; i < 5; i++) {
                    mapa[rand() % n][rand() % m] = 'P';
                }

                for (i = 0; i < n; i++) {
                    for (int j = 0; j < m; j++) {
                        printf("%c", mapa[i][j]);
                    }
                    printf("\n");
                }
                cesta = zachran_princezne(mapa, n, m, t, &dlzka_cesty);
                break;
            }
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