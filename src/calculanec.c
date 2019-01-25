/*
 * DIRB
 *
 * calculanec.c - Obtiene el codigo NEC
 *
 */

#include "dirb.h"
#include "http_codes.h"


/*
 * GET_NECS: Obtienes los NECs correspondientes a cada extension
 *
 */

int get_necs(const char *direccion) {
  int exts_pos=0;
  int necs=0;

  if(!options.silent_mode) printf("*** Calculating NOT_FOUND code...\r");
  fflush(stdout);

  // Inicializamos
  exts_current=exts_base;

  // Bucle
  for(exts_pos=0;exts_pos<exts_num;exts_pos++) {

    nec[exts_pos]=calcula_nec(direccion);

    //if(options.debuging>1) printf("[+] calcula_nec() NEC[%s]: %d -> %s\n", exts_current->word, nec[exts_pos]->codigo_http, code2string(code2message, nec[exts_pos]->codigo_http));

    if(options.debuging>1) printf("[+] calcula_nec() (Size: %d - Location: '%s')\n", nec[exts_pos]->body_size, nec[exts_pos]->location);

    if(options.debuging>1) printf("[+] calcula_nec() SERVER_BANNER[%s]: %s\n", exts_current->word, nec[exts_pos]->server);

    exts_current=exts_current->siguiente;

    necs++;

    }

  exts_current=exts_base;

  if(!options.silent_mode) printf("                                                                               \r");
  fflush(stdout);

  return necs;
}


/*
 * CALCULA_NEC: Obtiene el codigo de pagina no existente
 *
 */

struct result *calcula_nec(const char *direccion) {
  struct result nec1, nec2;
  struct result *mynec;
  char *url;
  char *random_base_url1 = "randomfile1";
  char *random_base_url2 = "frand2";
  char *rand_url1;
  char *rand_url2;
  unsigned int length;

  // Inicializamos
  mynec=malloc(sizeof(struct result));
  memset(mynec, 0, sizeof(struct result));
  memset(&nec1, 0, sizeof(struct result));
  memset(&nec2, 0, sizeof(struct result));

  // Calculo del primer NEC
  length = strlen(random_base_url1)+strlen(exts_current->word)+1;
  rand_url1 = malloc(length);
  snprintf(rand_url1, length, "%s%s", random_base_url1, exts_current->word);

  length = strlen(direccion)+strlen(rand_url1)+1;
  url = malloc(length);
  snprintf(url, length, "%s%s", direccion, rand_url1);

  nec1=get_url(url);
  free(url);

  if(options.debuging>2) printf("[++] calcula_nec() NEC1: %d\n", nec1.codigo_http);

  switch(nec1.codigo_http) {

    case 200:
      if(options.finetunning==1) nec1.body_size=nec1.body_words;
      break;

    case 301:
    case 302:
      if(options.finetunning==1) {
      location_clean(nec1.location, rand_url1);
      }

      break;

    default:
      if(options.finetunning==1) nec1.body_size=nec1.body_words;
      break;

    }
    free(rand_url1);

  // Calculo del segundo NEC
  length = strlen(random_base_url2)+strlen(exts_current->word)+1;
  rand_url2 = malloc(length);
  snprintf(rand_url2, length, "%s%s", random_base_url2, exts_current->word);

  length = strlen(direccion)+strlen(rand_url2)+1;
  url = malloc(length);
  snprintf(url, length, "%s%s", direccion, rand_url2);

  nec2=get_url(url);
  free(url);

  if(options.debuging>2) printf("[++] calcula_nec() NEC2: %d - %d\n", nec2.codigo_http, nec2.body_size);

  // Comparamos
  if(nec1.codigo_http==nec2.codigo_http) {

    switch(nec2.codigo_http) {

      case 200:
        if(options.finetunning==1) nec2.body_size=nec2.body_words;

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
        if(options.finetunning==1) {
          location_clean(nec2.location, rand_url2);
          }

        if(strncmp(nec1.location, nec2.location, STRING_SIZE-1)!=0) {
          printf("(!) WARNING: NOT_FOUND[%s] not stable, unable to determine correct URLs {30X}.\n", exts_current->word);
          printf("Not equal: %s and %s\n", nec1.location, nec2.location);
          if(options.finetunning!=1) printf("    (Try using FineTunning: '-f')\n");
          if(options.exitonwarn) { next_dir=1; }
          } else {
          strncpy(mynec->location, nec1.location, STRING_SIZE);
          }
        break;

      default:
        mynec->body_size=nec1.body_size;
        break;

    }

  } else {
    IMPRIME("(!) WARNING: NOT_FOUND[%s] not stable, unable to determine the correct URLs {%d,%d}.\n", exts_current->word, nec1.codigo_http, nec2.codigo_http);
    IMPRIME("    (Server is returning random responses)\n");
    if(options.exitonwarn) { next_dir=1; }
  }

  free(rand_url2);
  mynec->codigo_http=nec1.codigo_http;
  mynec->body_size=nec1.body_size;
  strncpy(mynec->server, nec1.server, STRING_SIZE);

  if(options.debuging>2) printf("[++] calcula_nec() NEC: %d - %d\n", mynec->codigo_http, mynec->body_size);

  return mynec;
}
