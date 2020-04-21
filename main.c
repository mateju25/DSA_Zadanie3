#include <stdio.h>
#include <stdlib.h>

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

/*---------------------------------------------------------------------------------------------*/
//region Halda

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

int* createRoute(VERTEX* paVertex, int* dlzka_cesty) {
    int* result = malloc((paVertex->val)*2 * sizeof(int));
    int i = (paVertex->val)*2-1;
    *dlzka_cesty = paVertex->val;
    while (paVertex != NULL) {
        result[i--] = paVertex->y;
        result[i--] = paVertex->x;
        paVertex = paVertex->before;
    }
    return result;
}

int* findRoute(char **mapa, int n, int m, int t, int *dlzka_cesty, int paFinX, int paFinY, int paStaX, int paStaY) {
    MIN_HEAP* heap = NULL;
    VERTEX* mapOfV[100][100];
    for (int j = 0; j < n; j++) {
        for (int i = 0; i < m; i++) {
            mapOfV[j][i] = (VERTEX*)malloc(sizeof(VERTEX));
            mapOfV[j][i]->val = -1;
            mapOfV[j][i]->x = i;
            mapOfV[j][i]->y = j;
            mapOfV[j][i]->seen = 0;
            mapOfV[j][i]->indexInHeap = 0;
            mapOfV[j][i]->before = NULL;
        }
    }
    VERTEX* temp = mapOfV[paStaX][paStaY];
    temp->val = 0;
    insertHeap(&heap, temp);

    while (heap->size >= 2) {
        VERTEX* temp = popFromHeap(&heap);
        mapOfV[temp->y][temp->x]->seen = 1;
        if ((temp->x == paFinX) && (temp->y == paFinY))
        {
            return createRoute(temp, dlzka_cesty);
        }
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
/*---------------------------------------------------------------------------------------------*/

int *zachran_princezne(char **mapa, int n, int m, int t, int *dlzka_cesty) {

    for (int j = 0; j < n; j++) {
        for (int i = 0; i < m; i++) {
            if (mapa[j][i] == 'D') return findRoute(mapa, n, m, t, dlzka_cesty, i, j, 0, 0);
        }
    }
}

int main()
{
    char **mapa;
    int i, test, dlzka_cesty, cas, *cesta;
    int n=0, m=0, t=0;
    FILE* f;
    while(1){
        printf("Zadajte cislo testu (0 ukonci program):\n");
        //scanf("%d",&test);
        test = 2;
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
                n = 5;
                m = 5;
                t = 12;
                mapa = (char**)malloc(n*sizeof(char*));
                mapa[0]="CCCHH";
                mapa[1]="HHCHH";
                mapa[2]="CCCNH";
                mapa[3]="CHNCC";
                mapa[4]="CCCCD";
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
