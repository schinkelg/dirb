/*
 * DIRB
 *
 * calculanec.c - Obtiene el codigo NEC
 *
 */

#include "dirb.h"
#include "http_codes.h"


int get_necs(const char *direccion) {
  unsigned int not_found_count=0;
  struct words *exts_current;

  if(!options.silent_mode) printf("*** Calculating NOT_FOUND code...\n");

  exts_current=exts_base;

  for(unsigned int exts_pos=0;exts_pos<exts_num;exts_pos++) {
    nec[exts_pos] = calcula_nec(direccion);
    exts_current = exts_current->next;
    not_found_count++;
    }

  return not_found_count;
}


struct result *calcula_nec(const char *direccion) {
  char *rand_url1;
  char *rand_url2;
  char *url;
  const char *random_base_url1 = "randomfile1";
  const char *random_base_url2 = "frand2";
  struct result *mynec;
  struct result nec1, nec2;

  mynec = calloc(1, sizeof(struct result));
  memset(&nec1, 0,  sizeof(struct result));
  memset(&nec2, 0,  sizeof(struct result));

  rand_url1 = mkstr(random_base_url1, exts_current->word);
  url = mkstr(direccion, rand_url1);

  nec1 = get_url(url);

  switch(nec1.codigo_http) {

    case 301:
    case 302:
      if(options.finetuning==1) location_clean(nec1.location, rand_url1);
      break;

    default:
      if(options.finetuning==1) nec1.body_size=nec1.body_words;
      break;
    }

  rand_url2 = mkstr(random_base_url2, exts_current->word);
  free(url);
  url = mkstr(direccion, rand_url2);

  nec2 = get_url(url);

  if(nec1.codigo_http==nec2.codigo_http) {
    switch(nec2.codigo_http) {
      case 200:
        if(options.finetuning==1) nec2.body_size=nec2.body_words;

        if(nec1.body_size!=nec2.body_size) {
          IMPRIME("(!) WARNING: NOT_FOUND[%s] page not stable, unable to determine the correct URLs {200}.\n", exts_current->word);
          IMPRIME("    (Try using FineTunning: '-f')\n");
          if(options.exitonwarn) { next_dir=1; }
          } else {
          mynec->body_size=nec1.body_size;
          }
        break;

      case 301:
      case 302:
        if(options.finetuning==1) {
          location_clean(nec2.location, rand_url2);
          }
        if(strcmp(nec1.location, nec2.location)!=0) {
          printf("(!) WARNING: NOT_FOUND[%s] not stable, unable to determine correct URLs {30X}.\n", exts_current->word);
          printf("Not equal: %s and %s\n", nec1.location, nec2.location);
          if(options.finetuning!=1) printf("    (Try using FineTunning: '-f')\n");
          if(options.exitonwarn) { next_dir=1; }
          } else {
          strcpy(mynec->location, nec1.location);
          }
        break;
      default:
        break;
    }
  } else {
    IMPRIME("(!) WARNING: NOT_FOUND[%s] not stable, unable to determine the correct URLs {%d,%d}.\n", exts_current->word, nec1.codigo_http, nec2.codigo_http);
    IMPRIME("    (Server is returning random responses)\n");
    if(options.exitonwarn) { next_dir=1; }
  }

  mynec->codigo_http=nec1.codigo_http;
  mynec->body_size=nec1.body_size;
  if (nec1.server != NULL) {
    strcpy(mynec->server, nec1.server);
  }

  free(url);
  free(rand_url1);
  free(rand_url2);
  return mynec;
}
