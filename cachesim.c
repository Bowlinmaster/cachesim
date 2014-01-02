#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "cachesim.h"

counter_t accesses = 0, hits = 0, misses = 0, writebacks = 0;
int numWays; //The number of ways
int numSets = 0; //The number of sets
int numEntriesInCache = 0; //Number of lines essentially
int shiftAmount = 0; //used for shifting for masks
int bitsPerSet; //num of bits taking up set number
int bitsPerBlock; //num of bits taking up data stuff (the right part)
typedef struct cacheEntry {
    int tag;
    int valid;
    int dirty;
    counter_t lruAge;
} cacheEntry1;
cacheEntry1 *theCache;

void cachesim_init(int blocksize, int cachesize, int ways) {
    numWays = ways;
    numSets = (cachesize/blocksize)/(ways); //Get the number of sets in the cache
    numEntriesInCache = numSets*ways; //The total number of "entries" in the cache
    bitsPerSet = log2(numSets); //num of bits taking up set number (5)
    bitsPerBlock = log2(blocksize); //num of bits taking up data stuff (the right part) //6
    shiftAmount = bitsPerSet+bitsPerBlock; //11
    theCache = malloc(numEntriesInCache*sizeof(cacheEntry1)); //dynamically create and array for the appropriate size of structs
    int x;
    //initialize all the values in the array
    for(x=0; x<numEntriesInCache;x++){
        theCache[x].dirty = 0;
        theCache[x].tag = -1;
        theCache[x].valid = 1;
        theCache[x].lruAge = 1;
    }
}

void cachesim_access(addr_t physical_addr, int write) {
    accesses = accesses+1;
    addr_t thisIsTheTag = physical_addr >> (shiftAmount); //tag of the PA

    //Extract the set from the PA
    addr_t thisIsTheSet = ((physical_addr - (thisIsTheTag<<(bitsPerBlock+bitsPerSet))) >> bitsPerBlock);
    int setLoopLow = thisIsTheSet*numWays; //Get the lower bound to loop for the certain set in the cache array
    int setLoopHigh = setLoopLow+(numWays-1); //Get the upper bound to loop for the certain set in the cache array
    int j;
    int hit = 0;
    for(j=setLoopLow;j<=setLoopHigh;j++){
        if(theCache[j].tag == thisIsTheTag){
            //We have a hit!
            hit = 1;
            hits=hits+1;
            int k;
            //Increase the age of everything
            for(k=setLoopLow;k<=setLoopHigh;k++){
                theCache[k].lruAge++;
            }
            theCache[j].lruAge = 0; //Set the age of the thing we hit to 0
            break;
        }else{
            //Check the next spot
        }
    }
    if(hit == 0){
        //We have a miss :'(
        misses=misses+1;
        if(write){
            writebacks++;
        }

        int m;
        int highestAge = 0;
        int highestSpot = 0;
        //Loop through the set and find the oldest element
        for(m=setLoopLow;m<=setLoopHigh;m++){
            if(theCache[m].lruAge>highestAge){
                highestAge = theCache[m].lruAge; //This is the oldest age
                highestSpot = m; //This is the location of the oldest thing
            }else{
            }
        }

        //Replace the oldest element with the new tag
        theCache[highestSpot].tag = thisIsTheTag;

        //Increase the age of each element
        for(m=setLoopLow;m<=setLoopHigh;m++){
            theCache[m].lruAge++;
        }
        theCache[highestSpot].lruAge = 0; //reset the age of the thing we just added to 0

    }

}

void cachesim_print_stats() {
  printf("%llu, %llu, %llu, %llu\n", accesses, hits, misses, writebacks);
}
