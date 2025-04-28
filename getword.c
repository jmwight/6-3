#include <ctype.h>
#include <stdio.h>
#include "getch.h"
#include "getword.h"

/* getword: get next word or character from input */
struct attr getword(char *word, int lim)
{
	int c, nxt_c, firstch; 
	static int onelncom, multilncom;
	static struct attr wattr;

	int getch(void);
	void ungetch(int);   //try this later & remove getch.h header for learning	

	char *w = word;
	wattr.func = 0;

	while(isspace(c = getch()))
		if(c == '\n')
			onelncom = 0;

	for(firstch = 1; --lim > 0; c = getch())
	{
		switch(c) 
		{
			case '"':
				/* flip quote on or off */
				if(!wattr.str)
					wattr.str = 1;
				else
					wattr.str = 0;
				break;
			case '/':
				if((nxt_c = getch()) == '*')
					multilncom = 1;
				else if(nxt_c == '/')
					onelncom = 1;
				else
					ungetch(nxt_c);
				if(multilncom || onelncom)
					wattr.comm = 1;
				break;
			case '*':
				if((nxt_c = getch()) == '/')
				{
					multilncom = 0;
					wattr.comm = 0;
				}
				else
					ungetch(nxt_c);
				break;
			case '#':
				/* if at beginning of word */
				if(w == word)
					*w++ = c;
				else
					wattr.c = *word;
				break;
			case EOF:
				*w = '\0';
				if(firstch)
				{
					wattr.c = c;
					ungetch(c);
				}
				else
					wattr.c = *word;
				return wattr;

			case '_':
				*w++ = c;
				if(firstch)
					firstch = 0;
				break;
			case '(':
				wattr.func = 1;
				/* zoom forward */
				/*while(!isspace(c = getch()))
					;
				ungetch(c);*/
				return wattr;
			case '\n':
				*w = '\0';
				ungetch(c);
				wattr.c = *word;
				return wattr;
			default:
				*w = c;
				if(firstch && !isalpha(*w) || !firstch && !isalnum(*w))
				{
					*w = '\0';
					wattr.c = *word;
					return wattr;
				}
				if(firstch)
					firstch = 0;
				w++;
				break;
		}

	}
}
