#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "getword.h"

#define MAXLN		1024
#define MAXWORD		100

/* TODO: find out how to just declare a structure and not define it. */
/* struct pgnode: contains page number and pointer to next node containing next
 * page number */
struct lnode  
{
	int lnum;
	struct lnode *nxtlnode;
};

/* struct word: string of word and a pointer to (flat tree) of page numbers 
 * last page is there to make it so every time we add a page we don't have to
 * go through a lot of page nodes to get to the end when it gets big. lastpg is 
 * like a bookmark*/
struct wnode  
{
	char *word;
	struct lnode *firstln;
	struct lnode *lastln;
	struct wnode *left;
	struct wnode *right;
} ;

int strcmpwrapper(const void *, const void *);
struct wnode *addwordandline(char *word, int ln, struct wnode *node);
struct lnode *addlnode(struct wnode *node, int ln);
void insertlnode(struct lnode *prevnode, int ln);

int main(int argc, char **argv)
{
	/* STUFF FOR EXCLUDED WORDS FROM LIST */
	/* default excluded words 
	 * TODO: FIND MORE TO ADD */
	char *defaultexclwords[] = {
		"a", "an", "and", "are", "as", "at", "be", "but", "by", "for", "if", "in", 
		"into", "is", "it", "no", "not", "of", "on", "or", "such", "that", "the", 
		"their", "then", "there", "these", "they", "this", "to", "was", "will", 
		"with"
	};
	
	/* get list of excluded words (if requested) */
	int comp;
	size_t arrsz = 0;
	if(argc > 1 && (comp = strcmp(argv[1], "-l")) == 0)
		arrsz = argc - 2;
	else if(argc > 1 && !comp)
		arrsz = 0;

	char *userexclwords[arrsz];
	/* TODO: try with *** instead to see if it works
	 * exclwords is a pointer to which array of string pointers we will be using 
	 * I know this is probably an overcomplication just more for learning */
	//char *(*exclwords[]);
	char ***exclwords;
	if(arrsz == 0)
	{
		exclwords = &defaultexclwords;
		int i;
		for(i = 2; i < argc; ++i)
			userexclwords[i-2] = argv[i];
		qsort((void*) defaultexclwords, arrsz, sizeof(char*), strcmpwrapper); /* TODO: test this in isolation*/
	}
	else
	{
		int i;
		for(i = 2; i < argc; ++i)
			userexclwords[i-2] = argv[i];
		exclwords = &userexclwords;
	}

	/* Now scan text for words and get structure of page numbers for each word */
	char word[MAXWORD];
	struct attr wattr;
	int n = 0;
	while((wattr = getword(word, MAXWORD)).c != EOF)
	{
		/* count if new line */
		if(wattr.c == '\n')
			n++;

		/* TODO: CHECK IF WORD IS IN EXCLUSION LIST, REMEMBER CRAZY POINTER STUFF */
		/* TODO: insert function that adds word */
	}
}

/* strcmpwrapper: needed for qsort because function pointer requires a function
 * that takes (void*, void*) as input and puts into it the word list with is a 
 * char **. So qsort calls strcmp(word_list, word_list+offset) not 
 * strcmp(word_list[0], word_list[i]). So strcmp gets the address word_list and
 * the word_list+offset not value at word_list[0] which is address of string and
 * word_list[i] which is address of other string */
int strcmpwrapper(const void *x, const void *y)
{
	/* (char **) tells compiler memory address is char ** so 
	 * first dereference gives us another address and second one 
	 * gives us a character. We then dereference to get just address
	 * of string strcmp is looking for */
	strcmp(*((char **)x), *((char **)y));
}

struct wnode *addwordandline(char *word, int ln, struct wnode *node)
{
	int diff;
	if(node == NULL)
	{
		node = malloc(sizeof(struct wnode));
		node->word = strdup(word);
		node->firstln = malloc(sizeof(struct lnode));
		node->firstln->lnum = ln;
		node->firstln->nxtlnode = NULL;
		node->left = NULL;
		node->right = NULL;
		return node;
	}
	else if((diff = strcmp(word, node->word)) < 0)
		addwordandline(word, ln, node->left);
	else if((diff = strcmp(word, node->word)) > 0)
		addwordandline(word, ln, node->right);
	/* matching word */
	else
		addlnode(node, ln);
}

/* addpgnode: adds pagenode at end. Assumes we are reading book from page 1 to 
 * end. Returns address of lnode with where page was added or found matching */
struct lnode *addlnode(struct wnode *node, int ln)
{
	/* line isn't present because it's greater than the biggest one we have, 
	 * most probable scenario */
	if(ln > node->lastln->lnum)
	{
		struct lnode *newlnode = malloc(sizeof(struct lnode)); /* create new lnode */
		newlnode->lnum = ln; /* add page number to it */
		newlnode->nxtlnode = NULL; /* make pointer to next lnode null */
		node->lastln->nxtlnode = newlnode; /* go to last lnode and update pointer to new lnode */
		node->lastln = newlnode; /* update bookmark pointer in wnode */
	}
	/* We have to start at the beginning and find where to insert it into. Much
	 * less probable scenario if we are reading where line numbers are increasing.
	 * We could add another pointer to lnode to previous page number but this would
	 * take a lot of memory for a scenario that probably wouldn't happen a lot in
	 * a real world scenario. */
	else if(ln < node->lastln->lnum)
	{
		/* It is possible that line numbers in order. In case they are we will
		 * insert it at the end. If it fits neatly in the middle of two we will
		 * put it there */
		struct lnode *line = node->firstln;
		while(line->nxtlnode != NULL && ln > line->nxtlnode->lnum)
		{
			if(ln == line->lnum)
				return line;
			line = line->nxtlnode;
		}
		insertlnode(line, ln);
	}
}

/* inserts lnode after specified point and fixes pointers so the previous ones all 
 * reference the next ones */
void insertlnode(struct lnode *prevnode, int ln)
{
	struct lnode *newnode = malloc(sizeof(struct lnode));
	newnode->lnum = ln;
	newnode->nxtlnode = prevnode->nxtlnode;
	prevnode->nxtlnode = newnode;
}
