/*
Azizul Hakim
az073410
COP 3502
Sean Szumlanski
TriePrediction
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "TriePrediction.h"



int main(int argc, char **argv)
{
	char filename1[1024], filename2[1024];
	TrieNode *root;
	
	// capture the file name
	strcpy(filename1, argv[1]);
	strcpy(filename2, argv[2]);
	
	root = buildTrie(filename1);
	processInputFile(root, filename2);
	destroyTrie(root);
	
	return 0;
}

// Helper function called by printTrie(). (Credit: Dr. S.)
void printTrieHelper(TrieNode *root, char *buffer, int k)
{
	int i;

	if (root == NULL)
		return;

	if (root->count > 0)
		printf("%s (%d)\n", buffer, root->count);

	buffer[k + 1] = '\0';

	for (i = 0; i < 26; i++)
	{
		buffer[k] = 'a' + i;
		printTrieHelper(root->children[i], buffer, k + 1);
	}

	buffer[k] = '\0';
}

// If printing a subtrie, the second parameter should be 1; otherwise, if
// printing the main trie, the second parameter should be 0. (Credit: Dr. S.)
void printTrie(TrieNode *root, int useSubtrieFormatting)
{
	char buffer[1026];

	if (useSubtrieFormatting)
	{
		strcpy(buffer, "- ");
		printTrieHelper(root, buffer, 2);
	}
	else
	{
		strcpy(buffer, "");
		printTrieHelper(root, buffer, 0);
	}
}

// got this from prof szumlanski's notes
TrieNode *createTrieNode(void)
{
	int i;
	TrieNode *n = malloc(sizeof(TrieNode));
	
	if(n == NULL)
		return NULL;
	
	for(i = 0; i < 26; i++)
		n->children[i] = NULL;
	
	n->subtrie = NULL;
	n->count = 0;
	return n;
}

// got this from prof szumlanski's notes
// and modified it slightly
TrieNode *insertString(TrieNode *root, char *str)
{
	int i, index, length = strlen(str); 
	TrieNode *wizard;
	
	wizard = root;
	
	for (i = 0; i < length; i++)
	{
		if (!isalpha(str[i]))
		{
			fprintf(stderr, "Error: \"%s\" contains non-alpha characters and will not be inserted.\n", str);
			return NULL;
		}
		index = tolower(str[i]) - 'a';

		if (wizard->children[index] == NULL)
			wizard->children[index] = createTrieNode();

		// Now the wizard is able to jump forward.
		wizard = wizard->children[index];
	}
	
	wizard->count++;
	return wizard;
}

// function to get rid of any punctuation
void stripPunctuation(char *str)
{
	int i, j = 0, length;
	char temp, holder[MAX_CHARACTERS_PER_WORD];
	
	length = strlen(str);
	
	for(i = 0; i < length; i++)
	{
		if(str[i] != ' ')
		{
			if(isalpha(str[i]))
			{
				holder[j] = str[i];
				holder[j+1] = '\0';
				j++;
			}
		}
	}
	strcpy(str, holder);
}

TrieNode *buildTrie(char *filename)
{
	TrieNode *tempNode = NULL, *root = NULL;
	int length;
	char buffer[1024], tempChar[1024];
	FILE *ifp;
	
	if((ifp = fopen(filename, "r")) == NULL)
	{
		fprintf(stderr, "Error, failed to open %s\n", filename);
		return NULL;
	}
	root = createTrieNode();
	
	// loop through the trie 
	while(fscanf(ifp, "%s", buffer) != EOF)
	{
		strcpy(tempChar, buffer);
		stripPunctuation(buffer);
		
		if(tempNode != NULL)
		{
			// create space for the subtrie
			if(tempNode->subtrie == NULL)
				tempNode->subtrie = createTrieNode();
			
			insertString(tempNode->subtrie, buffer);			
		}
		
		// insert the node into the trie
		tempNode = insertString(root, buffer);
		length = strlen(buffer);
		
		if(tempChar[length - 1] == '.' || tempChar[length - 1] == '!' || tempChar[length - 1] == '?')
			tempNode = NULL;
	}
	
	// clean up 
	fclose(ifp);
	return root;
}

int processInputFile(TrieNode *root, char *filename)
{
	FILE *ifp;
	int number, i;
	char str[1024], buffer[1024], holder[1024];
	TrieNode *temp;
	
	ifp = fopen(filename, "r");
		
	// check if the file is NULL
	if(ifp == NULL || root == NULL)
		return 1;
		
	while(fscanf(ifp, "%s", buffer) != EOF)
	{			
		switch(buffer[0])
		{
			case '@':
			fscanf(ifp, "%s %d", str, &number);
			printf("%s", str);
			strcpy(holder, str);
			stripPunctuation(str);
			
			temp = getNode(root, str);
			if(temp == NULL)
			{
				printf("\n");			
				break;
			}
			
			for(i = 0; i < number; i++)
			{
				// if the subtrie is null,
				// exit prematurely
				if(temp->subtrie == NULL)
					break;
				
				getMostFrequentWord(temp->subtrie, holder);
				printf(" %s", holder);
				temp = getNode(root, holder);
			}
			
			printf("\n");
			break;
			
			case '!':
			printTrie(root, 0);
			break;
			
			default:
			printf("%s\n", buffer);
				
			if(containsWord(root, buffer))
			{
				temp = getNode(root, buffer);
				
				if(temp->subtrie == NULL)
					printf("(EMPTY)\n");
				
				else if(temp->subtrie != NULL)
					printTrie(temp->subtrie, 1);
			}	
			
			else if(!containsWord(root, buffer))
				printf("(INVALID STRING)\n");
				
			break;
		}
	}
	
	fclose(ifp);
	return 0;
}

TrieNode *destroyTrie(TrieNode *root)
{
	int i;
	TrieNode *temp = root;
	
	if(root == NULL)
		return NULL;

	// destroy the trie
	for(i = 0; i < 26; i++)
	{
		if(temp->children[i] != NULL)
			destroyTrie(temp->children[i]);
	}
	
	// destroy the subtrie
	if(temp->subtrie != NULL)
		destroyTrie(temp->subtrie);
		
	free(temp);
	return NULL;
}

TrieNode *getNodeHelper(TrieNode *root, char *str, int k)
{
	int i, j, length;
	TrieNode *temp = NULL;
	
	// check for null
	if(root == NULL || str == NULL)
		return NULL;
	
	// if str is NULL, check to see if the count
	// is greater than 1, else return the root
	if(str[0] == '\0')
	{
		if(root->count >= 1)
			return root;
		
		else
			return NULL;
	}	
	
	temp = root;
	length = strlen(str);

	// loop through the trie 
	for(i = 0; i < length; i++)
	{
		j = str[i] - 'a';
		
		if(temp->children[j] == NULL)
			return NULL;
		
		// update temp
		temp = temp->children[j];
	}
	
	if(k)
		return temp;
	
	if(temp->count >= 1)
		return temp;
	
	return NULL;
}

TrieNode *getNode(TrieNode *root, char *str)
{	
	return getNodeHelper(root, str, 0);
}

void getMostHelper(TrieNode *root, char *buffer, int k, int *max, char *str)
{	
	int i;	

	// got this from prof szumlanski's 
	// notes and modified it
	if(root == NULL)
		return;
	
	if(root->count > *max)
	{
		*max = root->count;
		strcpy(str, buffer);
	}

	buffer[k + 1] = '\0';

	for(i = 0; i < 26; i++)
	{
		buffer[k] =  'a' + i;
		getMostHelper(root->children[i], buffer, k + 1, max, str);
	}
	buffer[k] = '\0';
}

void getMostFrequentWord(TrieNode *root, char *str)
{
	int max = 0, k = 0;
	char buffer[1024];

	if(root == NULL)
	{
		strcpy(str, buffer);
		return;
	}
	
	buffer[0] = '\0';
	getMostHelper(root, buffer, k, &max, str);
}

int containsWord(TrieNode *root, char *str)
{
	TrieNode *temp = getNode(root, str);
	
	if(temp == NULL)
		return 0;
	
	else if(temp->count >= 1)
		return 1;
	
	else 
		return 0;
}

int prefixHelper(TrieNode *root)
{
	int i, total = 0;
	
	if(root == NULL)
		return 0;
		
	total = root->count;
	
	// keep coutnt of how many times 
	// a word occurs in the trie 
	for(i = 0; i < 26; i++)
		total += prefixHelper(root->children[i]);
	
	return total;
}

int prefixCount(TrieNode *root, char *str)
{	
	TrieNode *temp;
	
	// check for null
	if(root == NULL || str == NULL)
		return 0;
		
	// check if str is garbage
	if(str[0] == '\0')
		temp = root;
	
	else
		temp = getNodeHelper(root, str, 1);
	
	return prefixHelper(temp);
}

double difficultyRating(void) 
{ 
	return 5;
}

double hoursSpent(void)
{
	return 35;
}
