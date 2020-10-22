#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define SIZE 20

struct SymbolTable {
   char *key;   
   char *value;
};

struct SymbolTable* hashArray[SIZE]; 
struct SymbolTable* dummyItem;
struct SymbolTable* item;

int hashFunction(char *key) {
    // TODO: implement hash function for string
    return 1; 
}

int hashCode(char *key) {
   return hashFunction(key);
}

struct SymbolTable *search(char *key) {
   //get the hash 
   int hashIndex = hashCode(key);  
	
   //move in array until an empty 
   while(hashArray[hashIndex] != NULL) {
	
      if(hashArray[hashIndex]->key == key)
         return hashArray[hashIndex]; 
			
      //go to next cell
      ++hashIndex;
		
      //wrap around the table
      hashIndex %= SIZE;
   }        
	
   return NULL;        
}

void insert(char *key, char *value) {
   struct SymbolTable *item = (struct SymbolTable*) malloc(sizeof(struct SymbolTable));
   item->key = key;  
   item->value = value;

   //get the hash 
   int hashIndex = hashCode(key);

   //move in array until an empty or deleted cell
   while(hashArray[hashIndex] != NULL && hashArray[hashIndex]->key != "dummy") {
      //go to next cell
      ++hashIndex;
		
      //wrap around the table
      hashIndex %= SIZE;
   }
	
   hashArray[hashIndex] = item;
}

void display() {
   int i = 0;
	
   for(i = 0; i<SIZE; i++) {
	
      if(hashArray[i] != NULL)
         printf(" (%s,%s)",hashArray[i]->key,hashArray[i]->value);
      else
         printf(" ~~ ");
   }
	
   printf("\n");
}

int main() {
   insert("MethodAccess", "(location: location; , getMethod: string)");

   display();
}