/* * * * * * *
 * Text Analysis module for Assignment 2.
 *
 * created for COMP20007 Design of Algorithms 2020
 * template by Tobias Edwards <tobias.edwards@unimelb.edu.au>
 * implementation by <Insert Name Here>
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>  
#include "text_analysis.h"

#define ALPHABET 27
#define MAX_CHAR 100
#define TRUE 1
#define FALSE 0
#define MAX_COMPLETION 5
#define INT_MAX 2147483647


// typedef struct trie TrieNode;
typedef struct trieNode TrieNode;
typedef struct settings Settings;
typedef struct config Config;
typedef struct map Map;

struct config {
  int total;
  int lastMapIndex;
};


struct map{
  char* key;
  int value;
  int visited;
};

struct settings{
  int prefixLength;
  int noOfString;
  int prefixKeys;
  int lastPrefixArrIndex;
};

struct trieNode{
  char data;
  TrieNode* keys[ALPHABET];
  int count;
};

/** Trie Utils
 * **/
TrieNode* new_TrieNode(char data){
  int i;
  TrieNode* trieNode = (TrieNode*)malloc(sizeof(TrieNode));
  for(i=0;i<ALPHABET;i++){
    trieNode->keys[i]= NULL;
  }
  trieNode->data = data;
  trieNode->count=0;

  return trieNode;
}


char getCharFromIndex(int idx){
  return (char)(idx+96);
}
int getNormalisedIndex(char c){
  return (int)c - 96;
}
void printKeys(TrieNode* trieNode){
  int i;
  if(trieNode == NULL){
    printf("[]");
  }else{
     printf("Keys: \n[");

    for(i=0; i< ALPHABET ;i++){
      if(trieNode->keys[i] != NULL){
        printf("%c", trieNode->keys[i]->data);
      }
    }
    printf("]\n");
  }
}
char* formString(){
  char *buffer = (char*)malloc(MAX_CHAR*sizeof(char));
  int count = 0;
  char temp;

  while((temp = getchar()) != '\n' ){
    buffer[count] = temp;
    count++;
  }
  return buffer;
}


void insertString(TrieNode* trie, char* string){
  int i= 0;
  TrieNode *cursor = trie;

  for(i=0 ; i< strlen(string); i++){
    char c = string[i];
    int idx = getNormalisedIndex(c);

    if(cursor->keys[idx] == NULL){
      cursor->keys[idx] = new_TrieNode(c);
      cursor = cursor->keys[idx];
    }
    else{
      cursor = cursor->keys[idx];
    }
  }
  

  if(cursor->keys[0] == NULL){
    cursor->keys[0] = new_TrieNode('$');
  }
  cursor->keys[0]->count++;
  // printf("S: %s %d\n",string, cursor->keys[0]->count);
}

void preOrderTraversal(TrieNode *trie){
  printf("%c\n", trie->data);
  int i;

  for(i=0; i< ALPHABET; i++){
    if(trie->keys[i]!= NULL){
      preOrderTraversal(trie->keys[i]);
    }
  }


}

// Build a character level trie for a given set of words.
//
// The input to your program is an integer N followed by N lines containing
// words of length < 100 characters, containing only lowercase letters.
//
// Your program should built a character level trie where each node indicates
// a single character. Branches should be ordered in alphabetic order.
//
// Your program must output the pre-order traversal of the characters in
// the trie, on a single line.
void problem_2_a() {
  int noOfStrings,i;


  scanf("%d \n", &noOfStrings);

  TrieNode *trie = new_TrieNode('^');

  for(i=0; i <noOfStrings; i++){
    char *temp = formString();

    insertString(trie,temp);
  }

  preOrderTraversal(trie);
}

void getPrefixUtil(TrieNode *node, char *word, int index, Settings *settings, char* prefixArr){
  // TODO: create prefixArr for prefix and add count
  int i;
  word[index] = node->data;

  if((settings->prefixLength -1) == index){
    int i;

    for(i=0; i<settings->prefixLength; i++){
      prefixArr[settings->lastPrefixArrIndex] = word[i];
      settings->lastPrefixArrIndex = settings->lastPrefixArrIndex+1;
    }
    settings->prefixKeys++;
    return;
  }



  for(i=1;i< ALPHABET;i++){
    if(node->keys[i]!= NULL){
      getPrefixUtil(node->keys[i], word, index+1,settings,prefixArr);
    }
  }

}

void getAllPrefix(TrieNode *trie, Settings *settings, char* prefixArr){
  int index = 0, i=0;
  
  char *word = (char*)malloc(settings->prefixLength*sizeof(char));
  char *combine = (char*)malloc(settings->noOfString*settings->prefixLength*sizeof(char)); 

  for(i=1;i< ALPHABET;i++){
    if(trie->keys[i]!= NULL){
      getPrefixUtil(trie->keys[i], word, index, settings, combine);
    }
  }

  strcpy(prefixArr,combine);
}

void printMap(Map** map, int prefixKeysCount){
  int i;
  for(i=0; i< prefixKeysCount ; i++){
    printf("%s: %d\n", map[i]->key, map[i]->value);
  }
}

Map** createMap(char* combine, Settings *settings){
  int i;
  Map **map = (Map**)malloc(settings->prefixKeys*sizeof(Map*));


  for(i=0 ;i< (settings-> prefixKeys); i++){
    Map* mapItem = (Map*)malloc(sizeof(Map*));
    mapItem->value = 0;
    mapItem->key = (char*)malloc(settings->prefixLength *sizeof(char));
    map[i] = mapItem;
  }

  int j=0, k=0;

  for(i=0;i<(settings->prefixKeys* settings->prefixLength);i++){
    
    if(i > 0 && (( (i+1)%settings->prefixLength) == 0)){
      map[k]->key[j]= combine[i];
      k++;
      j=0;
    }
    else{
      map[k]->key[j] = combine[i];
      j++;
    }
  }

  return map;
}

void getCountUnit(Map **map,TrieNode* node, Settings* settings, int keyIndex){

  int i;
  if(node->keys[0] != NULL){
    map[keyIndex]->value = map[keyIndex]->value + node->keys[0]->count;
  } 
  
  for(i=1;i<ALPHABET;i++){
    if(node->keys[i] != NULL){
      getCountUnit(map,node->keys[i],settings,keyIndex);
    }
  }

}

void getCountUtil(Map **map,TrieNode* trie, Settings* settings, int keyIndex){
  int i, prefixLength = settings->prefixLength;
  TrieNode *node = trie;
  char temp;

  //Traverse to prefix node
  for(i=0; i< prefixLength; i++){
    temp = map[keyIndex]->key[i];
    int idx = getNormalisedIndex(temp);
    node = node->keys[idx];
  }
  
  getCountUnit(map, node, settings, keyIndex);
}

void getCount(Map **map, TrieNode* trie, Settings* settings){
  // Traverse to prefix
  int i;
  

  for(i=0; i<settings->prefixKeys; i++){
    getCountUtil(map,trie,settings,i);
  }

  printMap(map,settings->prefixKeys);
}


// Using the trie constructed in Part (a) this program should output all
// prefixes of length K, in alphabetic order along with their frequencies
// with their frequencies. The input will be:
//   n k
//   str_0
//   ...
//   str_(n-1)
// The output format should be as follows:
//   an 3
//   az 1
//   ba 12
//   ...
//   ye 1
void problem_2_b() {
  int noOfStrings, prefixLength, i;
  scanf("%d %d \n", &noOfStrings, &prefixLength);
  
  // printf("No Of Strings: %d %d \n", noOfStrings, prefixLength);
  TrieNode *trie = new_TrieNode('^');


  Settings *settings = (Settings*)malloc(sizeof(Settings*));
  settings->noOfString = noOfStrings;
  settings->prefixLength = prefixLength;
  settings->prefixKeys =0;

  char* prefixArr = (char*)malloc(settings->noOfString*settings->prefixLength*sizeof(char));
  

  for(i=0;i<noOfStrings;i++){
    char *temp = formString();
    insertString(trie,temp);
  }


  /** Ideally, we should store the prefix in some sort of dictionary or use HashTable created in p1
   * We could also do memoization to reduce recursion calls at the expense of space complexity
   * However, there is no built-in dict in C
   * Prefix Array is used in this case e.g. ["aga","alb","art"] 
   * **/ 
  getAllPrefix(trie, settings, prefixArr);


  /** Counter for all prefix
   * {"aga": 1}
   * **/
  
  Map **map = createMap(prefixArr, settings);  
  getCount(map, trie, settings);
}

int getStringCount(char* s){
  int i, count=0, len = strlen(s);
  for(i=0; i< len ; i++){
    count += (int)s[i];
  }
  return count;
}



void addMapItem(Map** map, char* word, int count, Config *config){
  Map* mapItem = (Map*)malloc(sizeof(Map*));
  mapItem->key = (char*)malloc(MAX_CHAR*sizeof(char));
  strcpy(mapItem->key, word);
  mapItem->value = count;
  mapItem->visited = FALSE;

  map[config->lastMapIndex] = mapItem;
  config->lastMapIndex++;
}

void scoreUtil(TrieNode *node, char* word, int idx, Config *config, Map** map){
  int i;

  char *new = (char*)malloc(MAX_CHAR*sizeof(char*));
  word[idx] = node->data;
  strcpy(new, word);

  if(node->keys[0] != NULL){
    config->total = config->total + node->keys[0]->count;
    addMapItem(map, word, node->keys[0]->count, config);


  } 

  for(i=1;i<ALPHABET;i++){
    if(node->keys[i] != NULL){
      scoreUtil(node->keys[i], new, idx+1,config, map);
    }
  }
}


void getStringAndScore(TrieNode* trie, char* stub, Config *config, Map** map){
  TrieNode *node = trie;
  int i, idx=0;

  config->total = 0;

  char *word = (char*)malloc(MAX_CHAR*sizeof(char*));

  int stublen = strlen(stub);

  for(i=0; i< stublen; i++){
    int nIdx = getNormalisedIndex(stub[i]);
    if(i < stublen -1){
      word[i] = stub[i];
      idx++;
    }
    node = node->keys[nIdx];
  }
  scoreUtil(node, word, idx, config, map);
}

// void addMapItem(Map** map){
  
// }

// Again using the trie data structure you implemented for Part (a) you will
// provide a list (up to 5) of the most probable word completions for a given
// word stub.
//
// For example if the word stub is "al" your program may output:
//   0.50 algorithm
//   0.25 algebra
//   0.13 alright
//   0.06 albert
//   0.03 albania
//
// The probabilities should be formatted to exactly 2 decimal places and
// should be computed according to the following formula, for a word W with the
// prefix S:
//   Pr(word = W | stub = S) = Freq(word = W) / Freq(stub = S)
//
// The input to your program will be the following:
//   n
//   stub
//   str_0
//   ...
//   str_(n-1)
// That is, there are n + 1 strings in total, with the first being the word
// stub.
//
// If there are two strings with the same probability ties should be broken
// alphabetically (with "a" coming before "aa").
void problem_2_c() {

  int noOfStrings,i;
  char *stub;
  scanf("%d \n", &noOfStrings);
  stub = formString();


  TrieNode *trie = new_TrieNode('^');

  for(i=0;i<noOfStrings;i++){
    char *temp = formString();
    insertString(trie,temp);
  }
  int j,k,h;
  Map **map = (Map**)malloc(noOfStrings*sizeof(Map*));

  Config* config = (Config*)malloc(sizeof(Config*));
  config->total = 0;
  config->lastMapIndex = 0;

  getStringAndScore(trie,stub,config, map);


  Map **finalMap = (Map**)malloc(noOfStrings*sizeof(Map*));

  int totalResult = config->total, toFind = 0;
  int lowest = INT_MAX;

  for(i=0; i<MAX_COMPLETION; i++){
    int highestIndex, highest = 0;

    for(h=0; h<config->lastMapIndex ;h++){
      if((map[h]->value > highest) && map[h]->visited == FALSE){
        highestIndex = h;
        highest = map[h]->value;
      }
    }
    
    if(toFind <= MAX_COMPLETION - 1){
      finalMap[toFind] = map[highestIndex];
      finalMap[toFind]->visited = TRUE;
      toFind++;
    }
    
  }

  for(k=0 ; k< MAX_COMPLETION; k++){
    float probability = (((float)finalMap[k]->value / (float)totalResult));
    printf("%.2f %s \n",probability, finalMap[k]->key) ;
  }
}


