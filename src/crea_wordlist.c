/*
 * DIRB
 *
 * crea_wordlist.c - Crea la lista de palabras a probar
 *
 */

#include <stdio.h>
#include "dirb.h"


/*
 * CREA_WORDLIST: Crea la lista de palabras a probar a partir de un fichero
 *
 */

struct words *crea_wordlist(const char *filename) {
  FILE *file;
  struct words *wordlist;
  struct words *base;
  char cbuffer[256];

  wordlist=(struct words *)calloc(1, sizeof(struct words));
  base = wordlist;

  if((file=fopen(filename, "r"))==0) {
    IMPRIME("\n(!) FATAL: Error opening wordlist file: %s\n", filename);
    exit(-1);
    }

  while(!feof(file)) {
    if(fgets(cbuffer, 255, file)==0) break;

    limpia_url(cbuffer);

    if(cbuffer[0]=='#') continue;
    if(strlen(cbuffer)==0) continue;

    wordlist->word = malloc(strlen(cbuffer)+1);
    strcpy(wordlist->word, cbuffer);
    wordlist->next=(struct words *)calloc(1, sizeof(struct words));
    wordlist=wordlist->next;
    wordlist->word = "";
    }

  fclose(file);
  return base;
}

/*
 * COUNT_WORDS: Count the words in the list.
 *
 */

int count_words(const struct words *list) {
  unsigned int count=0;
  struct words *ptr= (struct words*)list;

  while(0 != ptr->next) {
    count++;
    ptr=ptr->next;
  }

  return count;
}
