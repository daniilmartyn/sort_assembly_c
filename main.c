#define _BSD_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>
#include <time.h>

#include "sort.h"

#define INITIAL_ARRAY_SIZE	500
#define GROWTH_MULTIPLIER	2
#define MAXIMUM_WORD_LENGTH	256

/* define type to hold a list of words in an array */

typedef struct WordArrayStruct {
	unsigned int count;		/* number of words in the list */
	unsigned int size;		/* size of the words[] array   */
	char** words;			/* point to array of pointers to strings */
} WordArray;


/* function makeList creates and initializes a new WordArray */
/* and optionally loads it with the contents of an existing  */
/* structure.  If present the existing structure is deleted. */

WordArray* makeList(WordArray* existingWords) {
	WordArray* newList;
	unsigned int index = 0;

	/* make a new list structure and initialize it */
	newList = malloc(sizeof(WordArray));
	assert(newList != NULL);
	if (existingWords) {
		assert(existingWords->size > 0);
		newList->size = existingWords->size * GROWTH_MULTIPLIER;
	} else {
		newList->size = INITIAL_ARRAY_SIZE;
	}
	newList->count = 0;
	newList->words = malloc(sizeof(char*) * newList->size);
	assert(newList->words != NULL);

	/* if there is an existing list, put the words in the new list */
	if (existingWords) {
		for (; index < existingWords->count; index++) {
			newList->words[index] = existingWords->words[index];
		}
		newList->count = existingWords->count;

		/* at this point we don't need the old list, so delete it */
		free(existingWords->words);
		free(existingWords);
	}

	/* initialize the remaining space to NULL */
	for (; index < newList->size; index++) {
		newList->words[index] = NULL;
	}

	return newList;
}


/* make an exact copy of a list and return it.  Strings are */
/* also duplicated and re-allocated.						*/

WordArray* duplicateList(WordArray* list) {
	WordArray* newList;
	unsigned int index;

	assert(list != NULL);
	newList = malloc(sizeof(WordArray));
	assert(newList != NULL);
	newList->size = list->size;
	newList->count = list->count;
	newList->words = malloc(sizeof(char*) * list->size);
	
	for (index = 0; index < list->count; index++) {
		newList->words[index] =  strdup(list->words[index]);
	}

	return newList;
}

/* return the next word from file fd, otherwise return NULL */

char* getNextWord(FILE* fd) {
	char wordBuffer[MAXIMUM_WORD_LENGTH];
	char ch;
	int putChar = 0;

	assert(fd != NULL);

	/* scan past any space characters */
	while ((ch = fgetc(fd)) != EOF) {
		if (isalnum(ch)) break;
	}
	if (ch == EOF) return NULL;

	/* we must be at the beginning of a word, */
	/* grab all the characters up to a space, */
	/* but throw away everything but alphanumeric chars */
	wordBuffer[putChar++] = tolower(ch);
	while ((ch = fgetc(fd)) != EOF) {
		if (isspace(ch) || putChar >= MAXIMUM_WORD_LENGTH - 1) break;
		if (isalnum(ch)) wordBuffer[putChar++] = tolower(ch);
	}
	wordBuffer[putChar] = '\0';		/* terminate the word */

	return strdup(wordBuffer);		/* re-allocate it off the heap */
}


/* readWords reads in all the words in file fd and places them into */
/* a WordArray structure which it returns as its value.  Words are  */
/* defined to be any strings in the file separated by white space   */
/* with all non-alphanumeric characters removed. Characters are all */
/* made lowercase.													*/

WordArray* readWords(FILE* fd) {
	WordArray* list;
	char* word;

	assert(fd != NULL);
	list = makeList(NULL);

	/* grab all the words we can from the file and put them */
	/* in the list.  Note that when adding words to the list */
	/* the list may need to grow, so we pass a pointer to the */
	/* list pointer so it can be assigned to a new list we that happens */
	while ((word = getNextWord(fd))) {
		
		/* if the list has run out of space, make a bigger one */
		if (list->count >= list->size) 
			list = makeList(list);

		/* put a pointer to the word into the next empty location */
		/* and increment the counter */
		list->words[list->count++] = word;
	}

	return list;
}


/* Neatly print out a list of words, prefixed by "name" */

void printList(char* name, WordArray* list) {
	unsigned int index;
	assert(list != NULL && name != NULL);
	assert(list->size >= list->count);
	if (list->count) {
		for (index = 0; index < list->count; index++) {
			assert(list->words[index] != NULL);
			printf("%s[%i] = %s\n",name,index,list->words[index]);
		}
	} else printf("List is empty.\n");
}


/* return true (1) if the list is sorted in ascending order */
/* return false (0) otherwise */

int testSort(WordArray* list) {
	unsigned int index;

	assert(list != NULL && list->size >= list->count);

	for (index = 1; index < list->count; index++) {
		char* word1 = list->words[index - 1];
		char* word2 = list->words[index];
		assert(word1 != NULL && word2 != NULL);
		assert(strlen(word1) > 0 && strlen(word2) > 0);
		if (strcmp(word1,word2) > 0) return 0;
	}
	return 1;
}


/* test if two arrays are identical in content and order */

int identicalLists(WordArray* list1, WordArray* list2) {
	unsigned int index;

	assert(list1 != NULL && list2 != NULL);
	if (list1->count != list2->count) return 0;

	for (index = 0; index < list1->count; index++) {
		char *word1 = list1->words[index];
		char *word2 = list2->words[index];
		assert(word1 != NULL && word2 != NULL);
		if (strcmp(word1,word2)) return 0;
	}
	return 1;
}

/* this routine tests that two lists contain exactly the  */
/* same set of words, regardless of the order.  Note that */
/* it is n-squared and is therefore SLOW on big lists.	  */

int sameElements(WordArray* list1, WordArray* list2) {
	unsigned int index1, index2, count1, count2;

	if (list1->count != list2->count) return 0;
	for (index1 = 0; index1 < list1->count; index1++) {
		char* word = list1->words[index1];
		assert(word != NULL);
		count1 = count2 = 0;
		for (index2 = 0; index2 < list1->count; index2++) {
			char* word1 = list1->words[index2];
			char* word2 = list2->words[index2];
			assert(word1 != NULL && word2 != NULL);
			if (!strcmp(word,word1)) count1++;
			if (!strcmp(word,word2)) count2++;
		}
		if (count1 != count2) return 0;
	}
	return 1;
}

/* compare function passed to qsort for comparing strings */

int compStrings(const void *a, const void *b) {
	return strcmp(*(char **) a, *(char **) b);
}

/* Sort a list with the C-library qsort */

void sortQ(char** array, unsigned int count) {
	qsort(array,count,sizeof(char *),compStrings);
}


int main(int argc, char* argv[]) {
	char* fileName;
	char fileNameBuffer[50];
	FILE* inputFile;
	WordArray* masterList;
	WordArray* list1;
	WordArray* list2;

	/* assure that we have at least a file name argument */
	if (argc >= 2) fileName = argv[1];
	else {
		printf("Enter File Name: ");
		scanf("%49s",fileNameBuffer);
		fileName = fileNameBuffer;
	}

	/* try opening the argument as a file */
	if ((inputFile = fopen(fileName,"r")) == NULL) {
		printf("Error: Unable to read file '%s'\n", fileName);
		exit(1);
	}

	/* read all the words in the file into a list structure */
	masterList = readWords(inputFile);
	fclose(inputFile);
	printf("%i words were read.\n",masterList->count);

	/* make a copies of the list so we can do three separate sorts */
	list1 = duplicateList(masterList);
	list2 = duplicateList(masterList);

	/* make sure we have something worthwhile to sort */
	if (list1->count > 1) {
		clock_t cputimeInC;
		clock_t cputimeInAssembly;
		clock_t cputimeInQsort;
		clock_t tempTime;
		float elapsedTimeC;		
		float elapsedTimeA;
		float elapsedTimeQ;
		char* success;

		/* sort the master list with qsort, we assume this works */
		tempTime = clock();
		sortQ(masterList->words,masterList->count);
		cputimeInQsort = clock() - tempTime;

		/* sort the two identical arrays using the C routine and    */
		/* using the assembly routine, recording the time intervals */

		tempTime = clock();
		sortC(list1->words,list1->count);
		cputimeInC = clock() - tempTime;
		tempTime = clock();
		sortA(list2->words,list2->count);
		cputimeInAssembly = clock() - tempTime;

		elapsedTimeC = (float) cputimeInC / (float) CLOCKS_PER_SEC;
		elapsedTimeA = (float) cputimeInAssembly / (float) CLOCKS_PER_SEC;
		elapsedTimeQ = (float) cputimeInQsort / (float) CLOCKS_PER_SEC;

		success = testSort(masterList) ? "SUCCEEDED" : "FAILED";
		printf("SortQ %s in %.2f seconds.\n",success,elapsedTimeQ);

		/* test the success of each sort */
		/* and display the elasped times */
		success = (testSort(list1) && identicalLists(masterList, list1)) 
					? "SUCCEEDED" : "FAILED";
		printf("SortC %s in %.2f seconds.\n",success,elapsedTimeC);
		success = (testSort(list2) && identicalLists(masterList,list2))
								  ? "SUCCEEDED" : "FAILED";
		printf("SortA %s in %.2f seconds.\n",success,elapsedTimeA);

		printf("Elapsed time in sortA was %.1f%% of that of sortC.\n",
			100.0*elapsedTimeA/elapsedTimeC);
		printf("Elapsed time in sortC was %.1f%% of that of sortQ.\n",
			100.0*elapsedTimeC/elapsedTimeQ);
	} else {
		printf("Need one or more strings to sort.\n");
	}
	return 0;
}
