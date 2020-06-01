/* * * * * * *
 * Hashing module for Assignment 2.
 *
 * created for COMP20007 Design of Algorithms 2020
 * template by Tobias Edwards <tobias.edwards@unimelb.edu.au>
 * implementation by <Insert Name Here>
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "hash.h"


#define BUF_SIZE 256
#define BIN_SIZE 6
#define CONFLICT 1
#define NO_CONFLICT 0

/**Problem 1a**/
typedef struct stringClass StringClass;

struct stringClass
{
  int length;
  char* string;
  /* data */
};


StringClass* readString(int mod);
int hashFunction(char* s, int strlen, int mod);
int charUtil(char c);
char* toBinAndAppend(int code, char* binResult, int binIndex);
int hashUtil(char *bin, int binLength, int mod);



/**Problem 1b**/
typedef struct hashTable HashTable;
typedef struct hashItem HashItem;

struct hashTable{
  HashItem** items;
  int *keys;
  int lastKeyIndex;
  int slots;
  int stepSize;
};

struct hashItem{
  int key;
  char *string;
};


HashTable* new_HashTable(int slots);
int insertItem(HashTable* hashTable,StringClass* strObj);
char* getString();
void printHashTable(HashTable* hashTable);
HashTable* resizeTable(HashTable* oldTable, StringClass *strObj);
int* new_boolArray(int slots);



// Implements a solution to Problem 1 (a), which reads in from stdin:
//   N M
//   str_1
//   str_2
//   ...
//   str_N
// And outputs (to stdout) the hash values of the N strings 1 per line.
void problem_1_a() {
  // TODO: Implement Me!
  
  int mod,i;
  int noOfString;
  scanf("%d %d\n", &noOfString, &mod);

  for(i=0; i < noOfString;i++){
    StringClass *strObj = readString(mod);
    int key = hashFunction(strObj->string, strObj->length, mod);
    printf("%d\n",key);
  }
}

StringClass* readString(int mod){
  StringClass *strClass = (StringClass*)malloc(sizeof(StringClass));
  char *buffer = (char*)malloc(BUF_SIZE*sizeof(char));
  int count = 0;
  char temp;


  while((temp = getchar()) != '\n' ){
    buffer[count] = temp;
    count++;
  }
  buffer[count] = '\0';
  
  strClass->string = buffer;

  strClass->length = count;

  return strClass;
}

int hashFunction(char* s, int strlen, int mod){
  int i, ASCIICode;
  char *binResult = (char*)malloc(6*sizeof(char)*strlen);
  
  for(i=0; i< strlen ; i++){
    ASCIICode = charUtil(s[i]);

    toBinAndAppend(ASCIICode, binResult, i*BIN_SIZE);
  }
  return hashUtil(binResult, BIN_SIZE*strlen, mod);
}
/** This function assumes the computer are using ASCII. 
 * Might be different due to computer locale settings
**/
int charUtil(char c){
  int temp = (int)c;
 

  if(temp >= 97 && temp <=122 ){
    //lowercase => a in ASCII starts at 97
    temp = temp - 97;
  }
  else if(temp >= 65 && temp <=90){
    //uppercase => A in ASCII starts at 65
    temp = (temp - 65) + 26;
  }else if(temp >= 48 && temp <=57){
    //0 in ASCII starts at 52
    temp = temp + 4;
  }
  else{
    printf("INVALID CHARACTER! ");
    exit(0);
  }

  return temp;
}

char* toBinAndAppend(int code, char* binResult, int binIndex){
  char *container = (char*)malloc(6*sizeof(char));
  int i, j;
  for (i = BIN_SIZE - 1; i >= 0; i--) { 
    int k = code >> i; 
    (k & 1) ? (container[BIN_SIZE - 1 - i] = '1'): (container[BIN_SIZE - 1 - i] = '0');
  } 

  //Append to original bin
  for(j=0; j< BIN_SIZE; j++){
    binResult[binIndex] = container[j];
    binIndex++;
  }
  
  return binResult;
}

int hashUtil(char *bin, int binLength, int mod){
  /** Horner's Method
   * (x + y) mod m = ((x mod m) + (y mod m)) mod m
   * (x × y) mod m = ((x mod m) × (y mod m)) mod m
   * E.g. 2^15 + 2^14 + 2^13 mod 17
   * (2^15) + (2^14) = (2+1)2+0)2+0)2+0)...2+0)
   *  x = 2 | y = 1 
   * ((2^15) + (2^14)) %17 = ((2%17) + (1%17)) % 17  => 3
   * Next Step: 
   * X = 3  | Y = 2
   * (3 x 2)%17 = ((3%17) x (2%17))%17
  **/
  
  int i;
  //Initialise
  int alt = bin[0] == '1'? 1 : 0, result;
  
  for(i=1; i < binLength - 1; i++){
    int x1;
    (bin[i] == '1') ? (x1 = 1): (x1 = 0);
    alt = (alt + x1%mod)%mod;
    alt = (alt%mod * 2%mod)%mod;
  }

  //Special case for 2^0;
  bin[binLength-1] == '1'? (result = ((alt%mod) + (1%mod))%mod): (result = ((alt%mod) + (0%mod))%mod);

  return result;
}


// Implements a solution to Problem 1 (b), which reads in from stdin:
//   N M K
//   str_1
//   str_2
//   ...
//   str_N
// Each string is inputed (in the given order) into a hash table with size
// M. The collision resolution strategy must be linear probing with step
// size K. If an element cannot be inserted then the table size should be
// doubled and all elements should be re-hashed (in index order) before
// the element is re-inserted.
//
// This function must output the state of the hash table after all insertions
// are performed, in the following format
//   0: str_k
//   1:
//   2: str_l
//   3: str_p
//   4:
//   ...
//   (M-2): str_q
//   (M-1):
void problem_1_b() {

  int noOfStrings,slots,stepSize,i;
  scanf("%d %d %d \n", &noOfStrings, &slots, &stepSize);

  HashTable* table = new_HashTable(slots);
  table->stepSize = stepSize;
 
  for(i=0; i<noOfStrings;i++){
    StringClass *strObj = readString(slots);
    int shouldResize = insertItem(table, strObj);
    if (shouldResize == 1){
      table = resizeTable(table,strObj);
    }
  }
 
  printHashTable(table);
}

int* new_boolArray(int slots){
  int *arr = (int*)malloc(slots*sizeof(int)), i; 
  for(i=0; i<slots; i++){
    arr[i] = 0;
  }
  return arr;
}

/** hashFunction(char* s, int strlen, int mod);
 * **/
int insertItem(HashTable* hashTable,StringClass* strObj){

  int hashKey = hashFunction(strObj->string,strObj->length, hashTable->slots); 
  int *boolArray = new_boolArray(hashTable->slots);
  
  while(hashTable->items[hashKey]->string != NULL){
    if(boolArray[hashKey] == 1) {
      return CONFLICT;
    }
    else{
      boolArray[hashKey] = 1;
      hashKey = (hashKey + hashTable->stepSize) % hashTable->slots;
    }
  }


  hashTable->items[hashKey]->string = strObj->string;
  hashTable->keys[hashTable->lastKeyIndex] =hashKey; 
  hashTable->lastKeyIndex++;
    
  return NO_CONFLICT;
}

HashTable* resizeTable(HashTable* oldTable, StringClass *strObj){
  HashTable *newTable = new_HashTable((oldTable->slots)*2);
  newTable->stepSize = oldTable->stepSize;

  // printf("To be Added: %s \n", strObj->string);
  printf("To be Added: %s \n", strObj->string);

  int j;
  //Add Existing String
  for(j=0; j < oldTable->slots; j++ ){
    char *curr = oldTable->items[j]->string;
    int count = 0, k=0;

    if(curr != NULL){
      StringClass *temp = (StringClass*)malloc(sizeof(StringClass*));
      temp->string = curr;
      while(curr[k] != '\0'){
        k++;
        count++;
      }
      temp->length = count;
      insertItem(newTable, temp);
    }
  }

  //Add Current string
  insertItem(newTable, strObj);

  return newTable;
}

void printHashTable(HashTable* hashTable){
  int slots = hashTable->slots, i;
  printf("\n");
  for(i=0; i<slots;i++){
    char *result;
    hashTable->items[i]->string == NULL? (result = "") : (result =hashTable->items[i]->string);
    printf(" %d: %s \n", i, result);
  }

}

HashTable* new_HashTable(int slots){
  HashTable *instance = (HashTable*)malloc(sizeof(HashTable*));
  instance->keys = (int*)malloc(slots*sizeof(int*));
  instance->slots = slots;
  instance->items = (HashItem**)malloc(slots*sizeof(HashItem*));
  
  int i;
  
  for(i=0; i < slots; i++){
    HashItem *item = (HashItem*)malloc(sizeof(HashItem*));
    item->key= i;
    instance->items[i] = item;  
  }


  return instance;
}

char* getString(){
  char *buffer = (char*)malloc(BUF_SIZE*sizeof(char));
  int count = 0;
  char temp;

  while((temp = getchar()) != '\n' ){
    buffer[count] = temp;
    count++;
  }
  buffer[count] = '\0';
  return buffer;
}
