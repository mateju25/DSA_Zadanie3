#include <stdio.h>
#include <stdlib.h>

typedef struct vertex {
    int val;
    struct vertex* before;
}VERTEX;

typedef struct heap {
    int size;
    VERTEX** arrOfVer;
}MIN_HEAP;

/*---------------------------------------------------------------------------------------------*/
/*---------------------------------------- HALDA ----------------------------------------------*/
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
    heapify(root, (*root)->size);
    (*root)->size++;
}
//vytiahne najmensi prvok a presetri vlastnost heapu
VERTEX* popFromHeap(MIN_HEAP** root) {
    VERTEX* first = (*root)->arrOfVer[1];
    (*root)->arrOfVer[1] = (*root)->arrOfVer[(*root)->size-1];
    (*root)->arrOfVer[(*root)->size-1] = NULL;
    (*root)->size--;
    int index = 1;
    while (((*root)->arrOfVer[index]->val > (*root)->arrOfVer[index*2]->val) || ((*root)->arrOfVer[index]->val > (*root)->arrOfVer[index*2+1]->val)) {
        if ((*root)->arrOfVer[index*2]->val > (*root)->arrOfVer[index*2+1]->val)
            swapInHeap(root, index, index*2+1);
        else
            swapInHeap(root, index, index*2);
        index *= 2;
        if (index*2 >= (*root)->size-1) break;
    }
    return first;
}
/*---------------------------------------------------------------------------------------------*/

int *zachran_princezne(char **mapa, int n, int m, int t, int *dlzka_cesty) {
    VERTEX* mapOfV[100][100];

    MIN_HEAP* heap = NULL;
    VERTEX* temp = malloc(sizeof(VERTEX));
    temp->val = 30;
    insertHeap(&heap, temp);
    temp = malloc(sizeof(VERTEX));
    temp->val = 20;
    insertHeap(&heap, temp);
    temp = malloc(sizeof(VERTEX));
    temp->val = 10;
    insertHeap(&heap, temp);
    temp = malloc(sizeof(VERTEX));
    temp->val = 5;
    insertHeap(&heap, temp);
    temp = malloc(sizeof(VERTEX));
    temp->val = 5;
    insertHeap(&heap, temp);
    temp = malloc(sizeof(VERTEX));
    temp->val = 25;
    insertHeap(&heap, temp);
    temp = malloc(sizeof(VERTEX));
    temp->val = 45;
    insertHeap(&heap, temp);
    temp = malloc(sizeof(VERTEX));
    temp->val = 1;
    insertHeap(&heap, temp);

    temp = popFromHeap(&heap);
    temp = popFromHeap(&heap);
    temp = popFromHeap(&heap);
    temp = popFromHeap(&heap);

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
    }
    return 0;
}
