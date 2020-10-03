/**************************************************************************
*                     X J D X G E N
*                                                   Author: Jim Breen
*           Index (.xjdx) generator program fron XJDIC
*
*		V2.3 - indexes JIS X 0212 (3-byte EUC) kanji
*		Copyright 1998 Jim Breen <jwb@csse.monash.edu.au>
***************************************************************************/
/*  This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 1, or (at your option)
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.     */

/* Changed: ignore all rc stuff. use args 1 and 2 for input/output file.
  -- jason */

/* Heavily commented, removed the unused header file, split off the
	readDictionary function, removed unused functions and variables... cleaned
	up the code in general.	Preparing for integration to the rest of the program

	Note that this indexer has been hacked off of Jim Breen's xjdic program,
	and a lot of the things which have been removed were relevant to that
	program, but not to this one.
	--Joe
	*/

#include "xjdxgen.h"

#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define TRUE 1
#define FALSE 0
#define SPTAG '@'
#define TOKENLIM 40
#define INDEX_VERSION 14;		/*The last time the index structure changed was Version1.4*/

unsigned char *db;
uint32_t  *jindex;
uint32_t indlen;

/*====function to Load Dictionary and load/create index table=======*/
int __indexer_start(int argc, char **argv)
{
	const char *Dname;
	const char *JDXname;
	FILE *fp;
	uint32_t diclen;
	uint32_t indptr;

	printf("\nNOTE: running this program by itself is never necessary. Kiten will run it automatically.\n");
	printf("\nXJDXGEN V2.3 Index Table Generator for XJDIC. \n      Copyright J.W. Breen, 1998\n");

	if (argc < 3)
	{
		printf("\nUSAGE: kitengen input output.xjdx\n");
		exit(2);
	}

	Dname = argv[1]; /*Name of the dictionary being scanned */
	JDXname = argv[2]; /*Name of the output file */
	printf("Commandline request to use files %s and %s \n", Dname, JDXname);
	printf("\nWARNING!!  This program may take a long time to run .....\n");

	db = readDictionary(Dname,&diclen); /*Reads the dict, but leaves a space at the beginning*/
	diclen++; /*add one to the number of bytes considered in the file */
	db[diclen] = 10;  /*set the first and final entry in the database to 10 */
	db[0] = 10;
	printf("Dictionary size: %d bytes.\n",diclen);


	indlen = (diclen * 3)/4; /*Make a wild guess at the index file length */
	jindex = (uint32_t *)malloc(indlen); /* and allocate it */
	if(jindex == NULL)
	{
		fprintf(stderr,"malloc() for index table failed.\n");
		exit(1);
	}

	printf("Parsing.... \n");
  /*this is the dictionary parser. It places an entry in jindex for every
   kana/kanji string and every alphabetic string it finds which is >=3
   characters */
	indptr = buildIndex(db,diclen);

	printf("Index entries: %d  \nSorting (this is slow)......\n",indptr);
	jqsort((int32_t)1,indptr);

	printf("Sorted\nWriting index file ....\n");
	fp = fopen(JDXname,"wb");
	if (fp==NULL )
	{
		printf("\nCannot open %s output file\n",JDXname);
		exit(1);
	}
	jindex[0] = diclen+INDEX_VERSION; /* prepend the index file size + version # */
	fwrite(jindex,sizeof(int32_t),indptr+1,fp);
	fclose(fp);

	return 0;
}

/*=========function to parse the dict file and fill the jindex global with the index====*/
/*=========returns the size of the index file                                       ====*/
/*
	A bit of explanation on what this thing generates is probably in order.
	Essentially, it fills jindex with a large number of numbers... each number
	being an offset to a byte location inside of the dictionary file. Starting
	at position index 1 (second pos)
	In other words... feeding this thing the dict file
	"Llama X1\nJT Fred Flintstone X"
	would generate: {<unmodified>,0,6,12,17}.
	"X" is skipped because it is only 1 byte long.
	"JT" is skipped because it is only two bytes long, the J is regular ascii
		(<127), and the T is not a digit. If any of those were different, (it
		was longer than 2 bytes, was an euc (kana or kanji) character, or T was
		a digit) it would be included in the index.
*/

/*First... an ugly #define to make our code a bit more readable*/
#define INDEX_OVERFLOW_CHECK(x) {if(x > indlen/sizeof(int32_t)) { \
	printf("Index table overflow. Dictionary too large?\n"); exit(1); } }

uint32_t buildIndex(unsigned char *dict, uint32_t dictLength) {
	int nowReadingWord = FALSE; /*Boolean to track if we're mid-word in the dict */
	uint32_t currentDictCharacter;   /*Current character index in the dict */
	unsigned char c;				/*the current reading character*/
	unsigned char currstr[TOKENLIM]; /* String that we're currently getting */
	int currstrIndex = 0;
	uint32_t indptr = 1;        /* next 'slot' in the index to fill */
	int saving = FALSE;	/*is what we are doing right now slated for salvation?*/

	for (currentDictCharacter =0; currentDictCharacter < dictLength;
							currentDictCharacter++)
	{
		c = dict[currentDictCharacter]; /* Fetch the next character */

		if(!nowReadingWord) /*if we are NOT in the middle of reading a word */
		{
			if (alphaoreuc(c) || c == SPTAG) /* if character or priority entry */
			{
				nowReadingWord = TRUE;  /* Mark that we're mid word */
				jindex[indptr] = currentDictCharacter;
					/* copy the location of this character to our index structure */
				currstrIndex = 1;
					/*mark the next position in the string to copy a char into */
				currstr[0] = c;
					/*set the current string to be equal to this character so far */
				currstr[1] = '\0';
				saving = TRUE;
			}
		} else {		/*If we're in the middle of parsing a word atm */

			/*if it's alphanumeric or - or . copy it and increment where the
			  next one goes */
			if ((alphaoreuc(c))||(c == '-')||(c == '.')||((c >= '0') && (c<='9')))
			{
				currstr[currstrIndex] = c;
				if(currstrIndex < TOKENLIM-1)
					currstrIndex++;
			}
			else /* We were reading a word... and we just encountered the
					  end of the word */
			{
				currstr[currstrIndex] = '\0'; /*null terminate the string */
				nowReadingWord = FALSE;

				/*Don't save single or dual character items where the
				  first item is ascii */
				if ((strlen((const char*)currstr) <= 2) && (currstr[0] < 127))
					saving = FALSE;
				/*EXCEPT: Save anything that's two character where the second
				  is a number
					Note that this might catch single 2-byte kanji as well...
					but it might not*/
				if ((strlen((const char*)currstr) == 2) && (currstr[1] <= '9'))
					saving = TRUE;

				/* This is a latin-character string, either longer than 2 bytes
					or having an ascii digit for a second byte */
				if (saving && (currstr[0] < 127))
				{
					indptr++;
					INDEX_OVERFLOW_CHECK(indptr);

					/* If this is non-Japanese, and has a 'SPTAGn' tag, generate
						two indices */
					if ( currstr[0] == SPTAG)
					{
						/*make a separate entry pointing to
							the non-SPTAG'd entry (the next byte)*/
						jindex[indptr] = jindex[indptr-1]+1;
							/*overwrite the SPTAG marker*/
						strcpy((char*)currstr,(char*)(currstr+1));
						indptr++;
						INDEX_OVERFLOW_CHECK(indptr);
					}
				}

				/*For strings that start with non latin characters*/
				if (saving && (currstr[0] > 127))
				{
					uint32_t i;
					uint32_t possav = jindex[indptr]; /*Save the current marker*/
					indptr++;
					INDEX_OVERFLOW_CHECK(indptr);

					/* generate index for *every* kanji in key */
					i = 2;
					/*if this is a three byte kanji, ignore the 0x8f marker */
					if (currstr[0] == 0x8f)
						i++;
					/*step through... two by two*/
					for ( ;  i < strlen((const char*)currstr);  i+=2)
					{
						if((currstr[i] >= 0xb0) || (currstr[i] == 0x8f))
						{
							/*Add in a specific reference to the kanji*/
							jindex[indptr] = possav+i;
							indptr++;
							INDEX_OVERFLOW_CHECK(indptr);
						}
						/*again the check if it's a three byte kanji*/
						if(currstr[i] == 0x8f)
							i++;
					}
				}
			}
		}
	}
	indptr--; /*correct for the overshoot */
	return indptr;
}

/*===function to read the dictionary files into array, returning filesize===*/
/*Note: We leave a blank byte in the first byte of the returned dictionary, and
  allocate an extra 99 bytes at the end */
unsigned char*
readDictionary(const char* dictName,uint32_t *filesize) {
  FILE *fp;
  struct stat buf;
  unsigned char *memDictionary;
  int nodread;

  if(stat(dictName, &buf) != 0) /* if the dict file doesn't exist */
  {
	 perror(NULL);
	 printf("Cannot stat: %s \n",dictName);
	 exit(1);
  }

  *filesize = buf.st_size; /*file size in bytes*/

  puts ("\nLoading Dictionary file.  Please wait.....\n");
  fp=fopen(dictName,"rb");
  if (fp==NULL )
  {
	printf("\nCannot open dictionary file\n");
	exit(1);
  }
  /*Allocate the database index 100 bytes larger than the dict filesize*/
  memDictionary=(unsigned char*)malloc((*filesize+100)*sizeof(unsigned char));
  if(memDictionary == NULL)
  {
      fprintf(stderr,"malloc() for dictionary failed.\n");
      fclose(fp);
      exit(1);
  }

  nodread = (*filesize)/1024; /*number of kilobytes in the file */
  /*reads 1024 x nodread bytes from fp, storing in memDictionary at offset 1*/
  fread((unsigned char *)memDictionary+1, 1024, nodread, fp);
  nodread = (*filesize) % 1024; /* "leftover" bytes after the previous read */
  /*reads the remaining bytes from fp... for what filesystem is this split-read needed?*/
  fread((unsigned char *)(memDictionary+((*filesize)/1024)*1024)+1, nodread,1, fp);
  fclose(fp);

  return memDictionary;
}

/*======function to sort jindex table====================*/
/*see the index generator for information about what jindex contains
  This simply sorts that output according to the data in the dictionary*/
void jqsort(int32_t lhs, int32_t rhs)
{
	int32_t i,last,midp;
	uint32_t temp;

	if (lhs >= rhs) return;

	midp = (lhs+rhs)/2; /* calculate the midpoint */

	/*Swap (midp,lhs) */
	temp = jindex[lhs];
	jindex[lhs] = jindex[midp];
	jindex[midp] = temp;

	last = lhs;
	for (i = lhs+1;i <= rhs; i++)
		{
			if (Kstrcmp(jindex[i],jindex[lhs]) < 0)
			{
				/* Swap(++last,i);*/
				last++;
				temp = jindex[i];
				jindex[i] = jindex[last];
				jindex[last] = temp;
			}
		}

/*	Swap (lhs,last);*/
	temp = jindex[lhs];
	jindex[lhs] = jindex[last];
	jindex[last] = temp;

	jqsort(lhs,last-1);
	jqsort(last+1,rhs);
}

/*=====string comparison used by jqsort==========================*/
int Kstrcmp(uint32_t lhs, uint32_t rhs)
{
	int i,c1 = 0, c2 = 0;
/* effectively does a strnicmp on two "strings" within the dictionary,
   except it will make katakana and hirgana match (EUC A4 & A5) */

	for (i = 0; i<20 ; i++) /*Compare up to 20 chars*/
	{
		c1 = db[lhs+i];
		c2 = db[rhs+i];

		if ((i % 2) == 0) /*If we're reading the first byte*/
		{
			if (c1 == 0xA5) /*Change hiragana to katakana for */
				c1 = 0xA4;   /*The purposes of this comparison */
			if (c2 == 0xA5)
				c2 = 0xA4;
		}

		/*If this is ascii, remove the difference between capitals and small*/
		if ((c1 >= 'A') && (c1 <= 'Z')) c1 |= 0x20;
		if ((c2 >= 'A') && (c2 <= 'Z')) c2 |= 0x20;

		if (c1 != c2 ) break;
	}
	return(c1-c2);
}

/*=======function to test a character for alpha or kana/kanji====*/
int alphaoreuc(unsigned char x)
{
	int c;

	c = x & 0xff;
	if(((c >= 65) && (c <= 90)) || ((c >= 97) && (c <= 122)))
		/*ASCII alphabet*/
	{
		return (TRUE);
	}
	if ((c >= '0') && (c <= '9'))
		/*digits*/
	{
		return(TRUE);
	}
	if ((c & 0x80) > 0)
		/*EUC kanji/kana*/
	{
		return(TRUE);
	}
	return (FALSE);
}

