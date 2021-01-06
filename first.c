#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


struct block{
    unsigned long int tag;
    int blocks;
    int index;
    int count;
    struct block* next;
};

//prototypes
void push(struct block* head, unsigned long int tag, int hit, int miss);
struct block* pop(struct block* head);
struct block* getDecimalBinary(double num, struct block* stacc);
int checkIfPowerOf2(int num);
int doesFileExist(char *file);
int calcLogBase2(int num);
int findLowest(struct block* cache);
void removeLRU(struct block* cache, int lowest);
void freeCache(struct block* cache, int sets);

void push(struct block* head, unsigned long int tag, int hit, int miss){
    //if no blocks have been added yet
    if (head->count == 0){
        head->tag = tag;
        head->next = NULL;
        head->count = hit + miss;
        //return head;
    }else{
        struct block* temp = malloc(sizeof(struct block));
        temp->tag = head->tag;
        temp->next = head->next;
        temp->count = head->count;
        temp->index = head->index;
        temp->blocks = head->blocks;

        head->tag = tag;
        head->next = temp;
        head->count = hit + miss;
        head->blocks = head->blocks;
        //return temp;
    }
    
}

void dequeue(struct block* head){
    if(head->next == NULL){
        head->tag = 0;
        head->next = NULL;
        head->count = 0;
        return;
    }

    struct block* curr = head;
    struct block* prev;
    while(curr->next != NULL){
        prev = curr;
        curr = curr->next;
    }
    prev->next = NULL;
    free(curr);
    //return head;

}


struct block* pop(struct block* head){
    if(head == NULL){
        return head;
    }
    struct block* temp = head;
    head = head->next;
    free(temp);
    return head;
}

int calcPowerOf2(int power){
    int result = 1;
    if(power == 0){
        return 1;
    }
    while (power != 0){
        result = result * 2;
        power = power -1;
    }
    return result;
}

int findSizeOfStack(struct block* head){
    if(head ==NULL){
        return 0;
    }
    int count = 0;
    struct block* curr = head;
    while(curr != NULL){
        curr = curr->next;
        count++;
    }
    return count;
}



void freeStack(struct block* stacc){
    if(stacc != NULL){
        struct block* curr = stacc->next;
        struct block* prev;
        while(curr != NULL){
            prev = curr;
            curr = curr->next;
            free(prev);
        }
    }
}

void freeCache(struct block* cache, int sets){
    for(int i = 0; i<sets; i++){
        freeStack(&cache[i]);
    }
    free(cache);
}


int checkIfPowerOf2(int num){
    while (num != 1){
        if (num % 2 != 0){
            return 0;
        }
        num = num/2;
    }
    return 1;
}

int doesFileExist(char *temp)
{
    FILE* file = fopen(temp, "r");
    if (file)
    {
        fclose(file);
        return 1;
    }
    return 0;
}

int calcLogBase2(int num){
    int result = 0;
    int temp = 1;
    while(temp != num){
        temp = temp *2;
        result = result + 1;
    }
    return result;
}


int checkCache(unsigned long int target, int index, struct block* cache, int size){
    //loop thru blocks in set
    struct block* curr = &cache[index];
    while(curr != NULL){
        if (curr->tag == target){
            return index;
        }
        curr = curr->next;
    }
    return -1; 


}

int isCacheFull(struct block* cache, int position){
    int space = cache[position].blocks;
    struct block* curr = &cache[position];
    if(curr->count == 0){
        return 1;
    }
    while(curr != NULL){
        curr = curr->next;
        space = space - 1;
    }
    if(space <= 0){
        return 0;
    }else{
        return 1;
    }

}

void evict(struct block* cache, int policy){

    if(policy == 1){
        //if fifo, just pop off the tail
        dequeue(cache);

    }else if(policy == 0){
        //if lru, find and remove block w lowest count
        int lowest = findLowest(cache);
        removeLRU(cache, lowest);
    }

    
}

int findLowest(struct block* cache){
    struct block* curr = cache;
    int lowest = curr->count;
    while(curr != NULL){
        if (curr->count < lowest){
            lowest = curr->count;
        }
        curr = curr->next;
    }
    return lowest;

}

void removeLRU(struct block* cache, int lowest){
    struct block* curr = cache;
    struct block* prev;
    if(curr->next == NULL){
        curr->tag = 0;
        curr->next = NULL;
        curr->count = 0;
        return;

    }
    if(curr->count == lowest){
        curr->tag = curr->next->tag;
        curr->count = curr->next->count;
        curr->index = curr->next->index;
        curr->blocks = curr->next->blocks;
        prev = curr->next;
        curr->next = curr->next->next;
        free(prev);
        return;
    }
    while(curr->count != lowest){
        prev = curr;
        curr = curr->next;
    }
    prev->next = curr->next;
    free(curr);
    //return cache;
}

void updateCount(struct block* cache, unsigned long int tag, int hit, int miss){
    struct block * curr = cache;
    while (curr->tag != tag){
        curr = curr->next;
    }
    curr->count = hit + miss;
}


int main(int argc, char* argv[argc+1]){
    //check for valid inputs
    if(argc != 6){
        printf("error");
        return EXIT_SUCCESS;
    }

    int cachesize = atoi(argv[1]);
    int blocksize = atoi(argv[4]);

    if(checkIfPowerOf2(cachesize) != 1 || checkIfPowerOf2(blocksize) != 1){
        printf("error");
        return EXIT_SUCCESS;
    }

    if(doesFileExist(argv[5]) == 0){
        printf("error");
        return EXIT_SUCCESS;
    }

    //extract associativity and cache policy

    int policyflag;
    int assoflag;
    char* asso = argv[2];

    if(strcmp(asso, "direct") == 0){
        assoflag = 1;
    }else if(strcmp(asso, "assoc") == 0){
        assoflag = 0;
    }else{
        assoflag = atoi(&asso[6]);
    }

    //calc # of sets, or number of blocks if full associative
    int sets = 0;
    if(assoflag != 0){
        sets = (cachesize) / (assoflag * blocksize);
    }else{
        sets = 1;
        assoflag = cachesize / blocksize;
    }

    if(strcmp(argv[3], "fifo") == 0){
        policyflag = 1;
    }else if(strcmp(argv[3], "lru") == 0){
        policyflag = 0;
    }


    FILE* fp = fopen(argv[5], "r");

    //calculate size of block bits, index bits, and tag bits
    int indexbits = 0;
    if(sets != 1){
        indexbits = calcLogBase2(sets);
    }
    int blockbits = calcLogBase2(blocksize);

    //create cache

    struct block* cache = malloc(sizeof(struct block)*sets);
    for(int i = 0; i< sets; i++){
        cache[i].tag = 0;
        cache[i].index = i;
        cache[i].count = 0;
        cache[i].next = NULL;
        //set the blocks to the associativity
        cache[i].blocks = assoflag;
    }
    //create counters
    int hit = 0;
    int miss = 0; 
    int memread = 0;
    int memwrite = 0;
    
    //begin scanning file
    char* op = malloc(sizeof(char));
    while(fscanf(fp, "%c", op) != EOF){
        unsigned long int address;

        fscanf(fp, "%lx\n", &address);

        int index;
        unsigned long int tag;

        //calculate set index bits and tag bits
        if(indexbits != 0){
            index = (address >> blockbits) & (calcPowerOf2(indexbits) - 1);
        }else{
            index = 0;
        }
        tag = address >> (blockbits + indexbits);

        //check cache for the current input
        int position = checkCache(tag, index, cache, sets);
        

        //if miss
        if (position == -1){
            if(op[0] == 'W'){
                miss = miss+1;
                memwrite = memwrite + 1;
                memread = memread+1;
                //check if space
                if(isCacheFull(cache, index) == 0){
                    //no space, so must evict
                    evict(&cache[index], policyflag);
                    push(&cache[index], tag, hit, miss);
                }else if(isCacheFull(cache, index) ==1){
                    //is space, so just add
                    push(&cache[index], tag, hit, miss);
                }
            }else{
                miss = miss+1;
                memread = memread+1;
                //check if space
                if(isCacheFull(cache, index) == 0){
                    //no space, so must evict
                    evict(&cache[index], policyflag);
                    push(&cache[index], tag, hit, miss);
                }else if(isCacheFull(cache, index) ==1){
                    //is space, so just add
                    push(&cache[index], tag, hit, miss);
                }
            }
        }else{
            //if hit
            if(op[0] == 'W'){
                hit = hit+1;
                memwrite = memwrite + 1;
                //update count for hit block
                updateCount(&cache[index], tag, hit, miss);
            }else{
                hit = hit+1;
                //update count for hit block
                updateCount(&cache[index], tag, hit, miss);
            }
        }
    }
    printf("memread:%d\n", memread);
    printf("memwrite:%d\n", memwrite);
    printf("cachehit:%d\n", hit);
    printf("cachemiss:%d\n", miss);

    free(op);
    freeCache(cache, sets);
    fclose(fp);
        
    return EXIT_SUCCESS;
}