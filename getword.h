#if !defined GETWORD_H
#define GETWORD_H	1

struct attr
{
	int comm, str, c, func;
};

struct attr getword(char *word, int lim);


#endif /* GETWORD_H */
